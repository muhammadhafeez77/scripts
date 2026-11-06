# afd.gdbinit - AFD-style full-screen register/flags/stack/memory view for gdb (x86-64)
#
# Use:   gdb -q -x afd.gdbinit ./qw
#        (gdb) starti
#        (gdb) s          <- step into (Enter repeats)
#        (gdb) n          <- step over (ProcStep)
#
# Needs a terminal of roughly 90 columns x 34 rows.

set confirm off
set pagination off
set height 0
set width 0
set disassembly-flavor intel
set prompt CMD > 

python
import gdb

MASK  = 0xFFFFFFFFFFFFFFFF
GREEN = "\033[92m"
WHITE = "\033[97m"
REV   = "\033[7m"
RST   = "\033[0m"
CLEAR = "\033[H\033[2J\033[3J"

clear_screen = True
prev  = {}
dumps = {"d1": "$rsi", "d2": "$rdi"}   # change with:  d1 <expr>   d2 <expr>

LEFT  = ["rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp"]
MID   = ["r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
RIGHT = ["rip", "cs", "ss", "ds", "es", "fs", "gs", "fs_base"]
SEG   = ("cs", "ss", "ds", "es", "fs", "gs")
LABEL = {"fs_base": "FSB"}
FLAGS = [("OF", 11), ("DF", 10), ("IF", 9), ("SF", 7),
         ("ZF", 6), ("AF", 4), ("PF", 2), ("CF", 0)]


def rv(name):
    try:
        return int(gdb.parse_and_eval("$" + name)) & MASK
    except Exception:
        return None


def rd(addr, n):
    try:
        return bytes(gdb.selected_inferior().read_memory(addr, n))
    except Exception:
        return None


def cell(name, cur):
    v = cur.get(name)
    w = 4 if name in SEG else 16
    txt = "?" * w if v is None else f"{v:0{w}X}"
    p = prev.get(name)
    if p is not None and v is not None and p != v:
        txt = REV + txt + RST + GREEN
    lab = LABEL.get(name, name.upper())
    return f"{WHITE}{lab:<3}{GREEN} {txt}"


def reg_block(cur):
    out = []
    for a, b, c in zip(LEFT, MID, RIGHT):
        out.append(cell(a, cur) + "  " + cell(b, cur) + "  " + cell(c, cur))
    return out


def flag_block(cur):
    ef = cur.get("eflags") or 0
    pe = prev.get("eflags")
    hdr = "".join(f"{n:>3}" for n, _ in FLAGS)
    vals = ""
    for n, bit in FLAGS:
        b = (ef >> bit) & 1
        s = f"{b:>3}"
        if pe is not None and ((pe >> bit) & 1) != b:
            s = "  " + REV + str(b) + RST + GREEN
        vals += s
    return [f"{WHITE}Flags{GREEN} {ef & 0xFFFF:04X}   {WHITE}{hdr}{GREEN}",
            " " * 13 + vals]


def disasm_lines(pc, n=8):
    out = []
    try:
        arch = gdb.selected_frame().architecture()
        for i in arch.disassemble(pc, count=n):
            raw = rd(i["addr"], i["length"]) or b""
            hx = " ".join(f"{x:02X}" for x in raw)
            out.append(f"{i['addr']:016X} {hx:<20.20} {i['asm']}")
    except Exception as e:
        out.append(f"<cannot disassemble: {e}>")
    return out


def stack_lines(sp, n=8):
    out = []
    for k in range(n):
        b = rd(sp + 8 * k, 8)
        if b is None:
            out.append(f"+{8*k:<3X} ????????????????")
        else:
            out.append(f"+{8*k:<3X} {int.from_bytes(b, 'little'):016X}")
    return out


def dump_lines(label, expr, rows=4):
    try:
        addr = int(gdb.parse_and_eval(expr)) & MASK
    except Exception:
        return [f"{WHITE}{label}{GREEN} [{expr}]  <bad expression>"]
    out = [f"{WHITE}{label}{GREEN} [{expr}]"]
    for r in range(rows):
        a = addr + 16 * r
        b = rd(a, 16)
        if b is None:
            out.append(f"{a:016X}  <unreadable>")
            continue
        hx = " ".join(f"{x:02X}" for x in b[:8]) + "  " + \
             " ".join(f"{x:02X}" for x in b[8:])
        asc = "".join(chr(x) if 32 <= x < 127 else "." for x in b)
        out.append(f"{a:016X}  {hx}  {asc}")
    return out


def show(*_):
    global prev
    try:
        gdb.selected_frame()
    except Exception:
        return
    names = LEFT + MID + RIGHT + ["eflags"]
    cur = {n: rv(n) for n in names}
    pc, sp = cur.get("rip") or 0, cur.get("rsp") or 0

    L = []
    L += reg_block(cur)
    L += flag_block(cur)
    L.append("")

    code = disasm_lines(pc)
    stk = stack_lines(sp)
    L.append(f"{WHITE}{'Code':<64}  Stack{GREEN}")
    for i in range(8):
        lt = (code[i] if i < len(code) else "")[:64].ljust(64)
        rt = stk[i] if i < len(stk) else ""
        if i == 0:
            L.append(f"{REV}{lt}{RST}{GREEN}  {rt}")
        else:
            L.append(f"{lt}  {rt}")
    L.append("")
    L += dump_lines("D1", dumps["d1"])
    L += dump_lines("D2", dumps["d2"])
    L.append("")
    L.append(f"{RST}{REV} s Step  n ProcStep  c Cont  d1/d2 <expr> Dump  "
             f"afd Redraw  afdclear Toggle {RST}")

    buf = (CLEAR if clear_screen else "") + GREEN + \
          ("\n" + GREEN).join(L) + RST
    gdb.write(buf + "\n")
    prev = dict(cur)


class Afd(gdb.Command):
    """Redraw the AFD-style screen."""
    def __init__(self):
        super().__init__("afd", gdb.COMMAND_USER)
    def invoke(self, arg, tty):
        show()


class AfdClear(gdb.Command):
    """Toggle clearing the screen before each redraw."""
    def __init__(self):
        super().__init__("afdclear", gdb.COMMAND_USER)
    def invoke(self, arg, tty):
        global clear_screen
        clear_screen = not clear_screen
        gdb.write(f"clear screen: {clear_screen}\n")


class Dump(gdb.Command):
    """d1 <expr> / d2 <expr>: set the address expression of a dump pane."""
    def __init__(self, name):
        super().__init__(name, gdb.COMMAND_USER)
        self.n = name
    def invoke(self, arg, tty):
        if arg.strip():
            dumps[self.n] = arg.strip()
        show()


Afd()
AfdClear()
Dump("d1")
Dump("d2")
end

# redraw the screen on every stop (runs before gdb prints its own location line)
define hook-stop
  python show()
end

# AFD-like keys. F1 = Step, F2 = ProcStep
define s
  stepi
end
define n
  nexti
end
