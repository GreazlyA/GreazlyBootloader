"""Run the built bootloader in a Cortex-M33 emulator (unicorn) against a
model of the BGM22 flash controller, through every install path and through
power cuts at any flash operation.

    py -3 -m pip install --user unicorn
    py -3 emu_test.py --bl <bootloader .hex/.bin> --builds <dir with GreazlyLoraFw_VER0_BUILDnnnn.hex>
                      [--old 2153 --new 2155] [--cuts 120] [--seed 1]

The flash model follows em_msc.c (series 2): ADDRB / WDATA / WRITECMD with
STATUS always ready. Programming ANDs into flash; ERASEPAGE sets 8 KB to
0xFF. A power cut at operation N leaves that operation half done (random
bits of a word programmed, or a page left with random contents), then the
device boots again with RAM content lost and flash kept.

Checks per boot: no CPU store to flash, no RAM write below 0x20005000 (the
app's NoInit data), no watchdog gap over ~2 s of 20 MHz cycles, and the app is
entered at its Reset_Handler with MSP and VTOR from its vector table.
"""

import argparse
import random
import struct
import sys
import zlib
from pathlib import Path

from unicorn import (Uc, UcError, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS,
                     UC_HOOK_CODE, UC_HOOK_MEM_WRITE, UC_HOOK_MEM_UNMAPPED)
from unicorn.arm_const import (UC_ARM_REG_SP, UC_ARM_REG_PC, UC_ARM_REG_MSP,
                               UC_CPU_ARM_CORTEX_M33)

FLASH_SIZE = 0x80000
PAGE = 0x2000
RAM_BASE, RAM_SIZE = 0x20000000, 0x8000
BL_RAM_BASE = 0x20005000
APP_ADDR, APP_SLOT = 0x2000, 0x26000
REBUILD_ADDR, REBUILD_SIZE = 0x28000, 0x18000
STORE_ADDR, STORE_SIZE = 0x40000, 0xE000
RECORD_ADDR = 0x4E000
BOOT_INFO_ADDR = 0x1FA0

INSTALL_MAGIC = 0x534E494C
ST_REQUESTED, ST_PATCHED, ST_INSTALLED, ST_COPY = 0x52455155, 0x50415443, 0x494E5354, 0x43500000
FAILED_TAG = 0xFA110000

MSC_BASES = (0x40030000, 0x50030000)
WDOG_BASES = (0x4A018000, 0x5A018000)
AIRCR = 0xE000ED0C
VTOR = 0xE000ED08

SLICE = 1_000_000                   # instructions per emu_start call
WDOG_MAX_GAP_INSN = 2 * 20_000_000  # ~2 s at 20 MHz; the watchdog is 8 s
BOOT_INSN_LIMIT = 150_000_000       # far more than an install needs


def crc32(b):
    return zlib.crc32(b) & 0xFFFFFFFF


def load_hex(path, base=0):
    mem = {}
    upper = 0
    for line in Path(path).read_text().splitlines():
        line = line.strip()
        if not line:
            continue
        rec = bytes.fromhex(line[1:])
        count, addr, rtype = rec[0], (rec[1] << 8) | rec[2], rec[3]
        data = rec[4:4 + count]
        if rtype == 0:
            for i, v in enumerate(data):
                mem[upper + addr + i] = v
        elif rtype == 1:
            break
        elif rtype == 2:
            upper = ((data[0] << 8) | data[1]) << 4
        elif rtype == 4:
            upper = ((data[0] << 8) | data[1]) << 16
    return mem


def image_from_hex(path):
    """App image from APP_ADDR to its last byte, gaps 0xFF (as on a device)."""
    mem = load_hex(path)
    end = max(mem) + 1
    return bytes(mem.get(a, 0xFF) for a in range(APP_ADDR, end))


class Device:
    """Flash contents survive power cuts; everything else is rebuilt per boot."""

    def __init__(self, bl_mem):
        self.flash = bytearray(b"\xFF" * FLASH_SIZE)
        for a, v in bl_mem.items():
            if a < FLASH_SIZE:
                self.flash[a] = v
        self.ops = 0                 # flash operations done (words programmed + pages erased)
        self.cut_at = None           # power cut during this operation number
        self.rng = random.Random(0)
        self.record_ops = []         # operation numbers that programmed the install record
        self.erase_ops = []          # operation numbers that erased a page

    # ---- flash helpers for test setup -------------------------------------------------
    def put(self, addr, data):
        self.flash[addr:addr + len(data)] = data

    def word(self, addr):
        return struct.unpack_from("<I", self.flash, addr)[0]


class Boot:
    """One power-on: runs until reset request, app entry, hang or power cut."""

    def __init__(self, dev, app_entry):
        self.dev = dev
        self.app_entry = app_entry
        self.result = None
        self.errors = []
        self.wdog_en = False
        self.insn = 0
        self.last_feed = 0
        self.max_gap = 0
        self.msc = {"addrb": 0, "wren": False}

        uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB | UC_MODE_MCLASS)
        uc.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M33)
        self.uc = uc
        uc.mem_map(0, FLASH_SIZE)
        uc.mem_write(0, bytes(dev.flash))
        uc.mem_map(RAM_BASE, RAM_SIZE)
        uc.mem_write(RAM_BASE, bytes(random.Random(dev.ops).getrandbits(8) for _ in range(RAM_SIZE)))
        uc.mem_map(0x0FE00000, 0x10000)                 # DEVINFO / chip config
        for base in (0x40000000, 0x50000000):            # peripherals (plain memory)
            for off in range(0, 0x100000, 0x10000):
                a = base + off
                if a not in MSC_BASES and not any(a == (m & ~0xFFFF) for m in MSC_BASES):
                    uc.mem_map(a, 0x10000)
        for m in MSC_BASES:
            uc.mmio_map(m, 0x4000, self.msc_read, None, self.msc_write, None)
            uc.mem_map(m + 0x4000, 0xC000)
        for w in WDOG_BASES:
            uc.mmio_map(w & ~0xFFFF, 0x10000, self.wdog_read, None, self.wdog_write, None)
        for base in (0x4A000000, 0x5A000000):
            for off in range(0x10000, 0x100000, 0x10000):
                if base + off not in [w & ~0xFFFF for w in WDOG_BASES]:
                    uc.mem_map(base + off, 0x10000)
            uc.mem_map(base, 0x10000)
        uc.mem_map(0xE0000000, 0x100000)                 # SCS

        uc.hook_add(UC_HOOK_MEM_WRITE, self.on_flash_store, begin=0, end=FLASH_SIZE - 1)
        uc.hook_add(UC_HOOK_MEM_WRITE, self.on_low_ram_store, begin=RAM_BASE, end=BL_RAM_BASE - 1)
        uc.hook_add(UC_HOOK_MEM_WRITE, self.on_scs_store, begin=0xE000ED00, end=0xE000ED10)
        uc.hook_add(UC_HOOK_CODE, self.on_app_entry, begin=app_entry & ~1, end=app_entry & ~1)
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, self.on_unmapped)

    # ---- emulation -----------------------------------------------------------------------
    def run(self):
        """Runs in slices of SLICE instructions; the slice count is the clock
        used for the watchdog gap (resolution: one slice)."""
        sp, pc = struct.unpack_from("<II", self.dev.flash, 0)
        self.uc.reg_write(UC_ARM_REG_SP, sp)
        while self.result is None and self.insn < BOOT_INSN_LIMIT:
            try:
                self.uc.emu_start(pc | 1, 0xFFFFFFFF, count=SLICE)
            except UcError as e:
                if self.result is None:
                    self.result = "crash"
                    self.errors.append(f"{e} at pc=0x{self.uc.reg_read(UC_ARM_REG_PC):X}")
            self.insn += SLICE
            if self.wdog_en:
                self.max_gap = max(self.max_gap, self.insn - self.last_feed)
            pc = self.uc.reg_read(UC_ARM_REG_PC)
        if self.result is None:
            self.result = "hang"
        return self.result

    def stop(self, result):
        if self.result is None:
            self.result = result
        self.uc.emu_stop()

    def on_unmapped(self, uc, access, addr, size, value, _):
        if 0x40000000 <= addr < 0x60000000:          # other peripheral: plain memory
            uc.mem_map(addr & ~0xFFFF, 0x10000)
            return True
        self.errors.append(f"unmapped access 0x{addr:X}")
        return False

    def on_flash_store(self, uc, access, addr, size, value, _):
        self.errors.append(f"CPU store to flash 0x{addr:X}")

    def on_low_ram_store(self, uc, access, addr, size, value, _):
        if len(self.errors) < 20:
            self.errors.append(f"RAM write below 0x{BL_RAM_BASE:X}: 0x{addr:X}")

    def on_scs_store(self, uc, access, addr, size, value, _):
        if addr == AIRCR and (value >> 16) == 0x05FA and value & 0x4:
            self.stop("reset")

    def on_app_entry(self, uc, addr, size, _):
        msp = uc.reg_read(UC_ARM_REG_MSP)
        vtor = struct.unpack("<I", uc.mem_read(VTOR, 4))[0]
        sp0 = struct.unpack_from("<I", self.dev.flash, APP_ADDR)[0]
        if msp != sp0:
            self.errors.append(f"app entered with MSP 0x{msp:X}, vector table has 0x{sp0:X}")
        if vtor != APP_ADDR:
            self.errors.append(f"app entered with VTOR 0x{vtor:X}")
        self.stop("app")

    # ---- flash controller model -----------------------------------------------------------
    def flash_op(self):
        """Count an operation; True if the power fails during it."""
        n = self.dev.ops
        self.dev.ops += 1
        return self.dev.cut_at is not None and n == self.dev.cut_at

    def program(self, addr, value):
        dev = self.dev
        if not (0 <= addr < FLASH_SIZE) or not self.msc["wren"]:
            self.errors.append(f"bad program 0x{addr:X}")
            return
        if addr < 0x2000:
            self.errors.append(f"bootloader page programmed at 0x{addr:X}")
        old = dev.word(addr)
        new = old & value
        if RECORD_ADDR <= addr < RECORD_ADDR + PAGE:
            dev.record_ops.append(dev.ops)
        if self.flash_op():
            new = old & (value | dev.rng.getrandbits(32))     # some bits programmed
            self.write_flash(addr, struct.pack("<I", new))
            self.stop("cut")
            return
        self.write_flash(addr, struct.pack("<I", new))

    def erase(self, addr):
        dev = self.dev
        addr &= ~(PAGE - 1)
        if not self.msc["wren"]:
            self.errors.append("erase without WREN")
            return
        if addr < 0x2000:
            self.errors.append("bootloader page erased")
        dev.erase_ops.append(dev.ops)
        if self.flash_op():
            self.write_flash(addr, bytes(dev.rng.getrandbits(8) for _ in range(PAGE)))
            self.stop("cut")
            return
        self.write_flash(addr, b"\xFF" * PAGE)

    def write_flash(self, addr, data):
        self.dev.flash[addr:addr + len(data)] = data
        self.uc.mem_write(addr, bytes(data))

    def msc_read(self, uc, off, size, _):
        reg = off & 0xFFF
        if reg == 0x1C:                               # STATUS: ready, never busy
            inv = 0 if self.msc["addrb"] < FLASH_SIZE else (1 << 2)
            return (1 << 3) | (1 << 27) | inv
        return 0

    def msc_write(self, uc, off, size, value, _):
        reg, kind = off & 0xFFF, off & 0x3000         # 0 plain, 0x1000 SET, 0x2000 CLR
        if reg == 0x0C:                               # WRITECTRL
            if kind == 0x1000 or (kind == 0 and value & 1):
                self.msc["wren"] = bool(value & 1) or self.msc["wren"]
            if kind == 0x2000 and value & 1:
                self.msc["wren"] = False
            if kind == 0 and not value & 1:
                self.msc["wren"] = False
        elif reg == 0x14 and kind == 0:               # ADDRB
            self.msc["addrb"] = value
        elif reg == 0x18 and kind == 0:               # WDATA
            self.program(self.msc["addrb"], value)
            self.msc["addrb"] += 4
        elif reg == 0x10 and kind in (0, 0x1000):     # WRITECMD
            if value & (1 << 1):
                self.erase(self.msc["addrb"])

    # ---- watchdog model --------------------------------------------------------------------
    def wdog_read(self, uc, off, size, _):
        reg = off & 0xFFF
        if reg == 0x04:
            return 1 if self.wdog_en else 0
        return 0                                       # STATUS, SYNCBUSY, LOCK: 0

    def wdog_write(self, uc, off, size, value, _):
        reg, kind = off & 0xFFF, off & 0x3000
        if reg == 0x04 and kind in (0, 0x1000) and value & 1:
            self.wdog_en = True
            self.last_feed = self.insn
        elif reg == 0x0C and value & 1 and self.wdog_en:
            self.last_feed = self.insn


# ---- scenario helpers --------------------------------------------------------------------------

def new_device(bl_mem, app):
    dev = Device(bl_mem)
    dev.put(APP_ADDR, app)
    return dev


def put_request(dev, lota, record_crc_ok=True):
    """What the phase-3 app will do: store the .lota, write the record."""
    dev.put(STORE_ADDR, lota)
    h = struct.unpack_from("<4sBBBBIIIIIIIIIBBHI8sI", lota)
    to_build, to_size, to_crc, header_crc = h[9], h[10], h[11], h[19]
    words = [INSTALL_MAGIC, STORE_ADDR, len(lota), to_build, to_size, to_crc, header_crc]
    rec_crc = crc32(struct.pack("<7I", *words))
    if not record_crc_ok:
        rec_crc ^= 1
    dev.put(RECORD_ADDR, struct.pack("<9I", *words, rec_crc, to_crc ^ ST_REQUESTED))
    return to_crc


def record_state(dev, to_crc):
    """Same rule as the bootloader: a mark is set once it is not erased (a
    power cut may leave it half programmed). exact_* say whether the value is
    the full to_crc32 ^ id."""
    w = lambda i: dev.word(RECORD_ADDR + 4 * i)
    erased = 0xFFFFFFFF
    return {
        "patched": w(9) != erased,
        "failed": None if w(10) == erased else w(10),
        "installed": w(11) != erased,
        "pages": sum(1 for n in range(19) if w(12 + n) != erased),
        "exact_installed": w(11) == to_crc ^ ST_INSTALLED,
    }


def boot_until_app(dev, max_boots=12, collect=None):
    """Power-on repeatedly (resets are reboots). Returns (result, boots)."""
    for boots in range(1, max_boots + 1):
        app_entry = struct.unpack_from("<I", dev.flash, APP_ADDR + 4)[0]
        b = Boot(dev, app_entry if app_entry != 0xFFFFFFFF else 0x2001)
        res = b.run()
        if collect is not None:
            collect.append(b)
        if b.errors:
            return "errors: " + "; ".join(b.errors[:5]), boots
        if res in ("app", "hang", "crash", "cut"):
            return res, boots
    return "too many boots", max_boots


def slot_matches(dev, image):
    n = len(image)
    return (bytes(dev.flash[APP_ADDR:APP_ADDR + n]) == image and
            all(v == 0xFF for v in dev.flash[APP_ADDR + n:APP_ADDR + APP_SLOT]))


class Report:
    def __init__(self):
        self.fails = 0

    def check(self, name, ok, detail=""):
        print(f"  {'PASS' if ok else 'FAIL'}  {name}{('  -- ' + detail) if detail else ''}")
        if not ok:
            self.fails += 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--bl", required=True, help="bootloader .hex")
    ap.add_argument("--builds", required=True, help="folder with GreazlyLoraFw_VER0_BUILDnnnn.hex")
    ap.add_argument("--lota-tool", default=str(Path(__file__).resolve().parents[2] / "GreazlyLora" / "FW" / "Lota"))
    ap.add_argument("--old", type=int, default=2153)
    ap.add_argument("--new", type=int, default=2155)
    ap.add_argument("--cuts", type=int, default=120, help="power-cut points per install type")
    ap.add_argument("--seed", type=int, default=1)
    args = ap.parse_args()

    sys.path.insert(0, args.lota_tool)
    import fw_lota

    bl_mem = load_hex(args.bl)
    bdir = Path(args.builds)
    img = lambda b: image_from_hex(bdir / f"GreazlyLoraFw_VER0_BUILD{b}.hex")
    old, new = img(args.old), img(args.new)
    print(f"bootloader {args.bl}: {max(a for a in bl_mem if a < 0x1FA0) + 1} B code, "
          f"boot info {bytes(bl_mem.get(BOOT_INFO_ADDR + i, 0xFF) for i in range(8)).hex()}")
    print(f"old build {args.old} ({len(old)} B), new build {args.new} ({len(new)} B)")
    delta = fw_lota.make_lota(fw_lota.TYPE_DELTA, new, old, args.old, args.new)
    full = fw_lota.make_lota(fw_lota.TYPE_FULL, new, None, 0, args.new)
    print(f"delta {len(delta)} B, full {len(full)} B")
    r = Report()

    print("normal start")
    dev = new_device(bl_mem, old)
    boots = []
    res, n = boot_until_app(dev, collect=boots)
    r.check("blank record: app starts in one boot", res == "app" and n == 1, res)
    r.check("watchdog untouched on normal start", not boots[-1].wdog_en)

    for name, lota in (("DELTA", delta), ("FULL", full)):
        print(f"{name} install {args.old} -> {args.new}")
        dev = new_device(bl_mem, old)
        to_crc = put_request(dev, lota)
        boots = []
        res, n = boot_until_app(dev, collect=boots)
        st = record_state(dev, to_crc)
        r.check("installs and starts the new app", res == "app" and slot_matches(dev, new), f"{res}, {n} boots, {st}")
        r.check("record: patched, 19 pages, installed, not failed",
                st["patched"] and st["pages"] == 19 and st["installed"] and st["failed"] is None, str(st))
        gap = max(b.max_gap for b in boots)
        r.check("watchdog fed often enough", gap < WDOG_MAX_GAP_INSN, f"max gap ~{gap:,} instructions")
        r.check("store left as received", bytes(dev.flash[STORE_ADDR:STORE_ADDR + len(lota)]) == lota)
        res, n = boot_until_app(dev)
        r.check("next boot starts the app directly", res == "app" and n == 1, res)

    print("rejected requests (old app must stay)")
    def rejected(name, lota, app, expect_step, expect_code=None, rec_ok=True):
        dev = new_device(bl_mem, app)
        to_crc = put_request(dev, lota, rec_ok)
        res, n = boot_until_app(dev)
        st = record_state(dev, to_crc)
        f = st["failed"]
        ok = res == "app" and slot_matches(dev, app)
        if expect_step is None:
            ok = ok and f is None and not st["patched"]
        else:
            ok = ok and f is not None and (f >> 8) & 0xFF == expect_step and (f & 0xFFFF0000) == FAILED_TAG
            if expect_code is not None:
                ok = ok and (f & 0xFF) == expect_code
        r.check(name, ok, f"{res}, boots {n}, failed={f if f is None else hex(f)}")

    bad = bytearray(delta); bad[100] ^= 0x40
    rejected("payload byte flipped -> failed check/PAYLOAD_CRC", bytes(bad), old, 2, 8)
    wrong_base = img(args.old - 1) if (bdir / f"GreazlyLoraFw_VER0_BUILD{args.old - 1}.hex").is_file() else new
    rejected("DELTA on the wrong base build -> failed check/BASE", delta, wrong_base, 2, 9)
    hw = bytearray(full); hw[8] = 2; hw[60:64] = struct.pack("<I", crc32(bytes(hw[:60])))
    rejected("other hw_id -> failed check/HW_ID", bytes(hw), old, 2, 4)
    rejected("record CRC wrong -> ignored", delta, old, None, rec_ok=False)
    big = bytearray(full); struct.pack_into("<I", big, 28, REBUILD_SIZE + 4); big[60:64] = struct.pack("<I", crc32(bytes(big[:60])))
    rejected("to_size larger than the rebuild area -> failed record", bytes(big), old, 1)

    print("legacy .ota")
    dev = new_device(bl_mem, old)
    ota = struct.pack(">8I", 0, len(new), crc32(new), 0, 0, 0, 0, 0) + new
    dev.put(REBUILD_ADDR, ota)
    res, n = boot_until_app(dev)
    r.check("legacy .ota copied, header page erased",
            res == "app" and slot_matches(dev, new) and all(v == 0xFF for v in dev.flash[REBUILD_ADDR:REBUILD_ADDR + PAGE]),
            f"{res}, {n} boots")

    print("no valid app")
    dev = Device(bl_mem)
    res, n = boot_until_app(dev)
    r.check("erased app slot: bootloader waits (no jump, no reset loop)", res == "hang", res)

    rng = random.Random(args.seed)
    for name, lota in (("DELTA", delta), ("FULL", full)):
        dev = new_device(bl_mem, old)
        to_crc = put_request(dev, lota)
        boot_until_app(dev)
        total = dev.ops
        # every install-record write and every page erase, plus random points
        targeted = set(dev.record_ops) | set(dev.erase_ops)
        points = sorted(targeted | {0, 1, total - 1, total - 2} |
                        {rng.randrange(total) for _ in range(args.cuts)})
        print(f"{name} power cuts: {len(points)} points over {total} flash operations "
              f"({len(dev.record_ops)} record writes, {len(dev.erase_ops)} erases)")
        bad_runs = []
        for cut in points:
            dev = new_device(bl_mem, old)
            to_crc = put_request(dev, lota)
            dev.cut_at = cut
            dev.rng = random.Random(cut)
            res, n = boot_until_app(dev)
            if res != "cut":
                bad_runs.append((cut, f"no cut ({res})"))
                continue
            dev.cut_at = None
            res, n = boot_until_app(dev)
            st = record_state(dev, to_crc)
            ok = res == "app" and slot_matches(dev, new) and st["installed"] and st["failed"] is None
            if not ok:
                bad_runs.append((cut, f"{res}, {st}, new={slot_matches(dev, new)}, old={slot_matches(dev, old)}"))
        r.check(f"{name}: every cut point ends with the new app installed", not bad_runs,
                "; ".join(f"cut {c}: {d}" for c, d in bad_runs[:5]))

    print(f"\n{'ALL PASSED' if r.fails == 0 else f'{r.fails} FAILED'}")
    sys.exit(1 if r.fails else 0)


if __name__ == "__main__":
    main()
