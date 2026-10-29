
static long sys(long n, long a, long b, long c)
{
  long r;
  __asm__ volatile("syscall"
                   : "=a"(r)
                   : "a"(n), "D"(a), "S"(b), "d"(c)
                   : "rcx", "r11", "memory");
  return r;
}
static void out(const char *s, long n) { sys(1, 1, (long)s, n); }
static void str(const char *s)
{
  long n = 0;
  while (s[n])
    n++;
  out(s, n);
}

static void num(unsigned long v)
{
  char b[24];
  int i = 24;
  do
  {
    b[--i] = '0' + v % 10;
    v /= 10;
  } while (v);
  out(b + i, 24 - i);
}

static void flt(double d)
{ /* non-negative, 6 decimals */
  unsigned long ip = (unsigned long)d;
  double frac = d - ip;
  unsigned long f = (unsigned long)(frac * 1000000 + 0.5);
  if (f >= 1000000)
  {
    ip++;
    f -= 1000000;
  }
  num(ip);
  out(".", 1);
  char b[6];
  for (int i = 5; i >= 0; i--)
  {
    b[i] = '0' + f % 10;
    f /= 10;
  }
  out(b, 6);
}

void _start(void)
{
  str("Hello World ");
  num(10);
  out(" ", 1);
  out("a", 1);
  out(" ", 1);
  flt(3.5);
  sys(60, 0, 0, 0);
  __builtin_unreachable();
}
