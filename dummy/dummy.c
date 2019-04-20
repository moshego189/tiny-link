#define BREAK __asm__ volatile("int $3;\n");

// Write syscall
// =============
static long _write(long fd, const char* buf, unsigned long len) {

  register long ret asm ("rax");
  register long _fd asm ("rdi") = fd;
  register const char* _buf asm ("rsi") = buf;
  register unsigned long _len asm ("rdx") = len;
  register int sys_write asm ("rax") = 1;
  asm volatile (
      "syscall;"
      : "=r" (ret)
      : "r" (_fd), "r" (_buf), "r" (_len), "r" (sys_write)
      :
  );
  return ret;
}


// exit syscall
// =============
static long __exit(int errcode) {

  register long ret asm ("rax");
  register int _errcode asm ("rdi") = errcode;
  register int sys__exit asm ("rax") = 60;
  asm volatile (
      "syscall;"
      : "=r" (ret)
      : "r" (_errcode), "r" (sys__exit)
      :
  );
  return ret;
}

void _start()
{
     _write(0, "dummy\n", 7);
     __exit(123);
}

/*#include <sys/syscall.h>
#include <unistd.h>

void main()
{
     syscall(SYS_write, 0, "dummy\n", 7);
     syscall(SYS_exit_group, 123);
}*/
