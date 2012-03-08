// Read for more info: http://sourceware.org/newlib/libc.html#Stubs

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include <misc.h>
#include <core_cm3.h>

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status)
{
  _write(STDERR_FILENO, "exit", 4);
  do {} while (1);
}

int _close(int file)
{
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}

int _fork(void)
{
  errno = EAGAIN;
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _getpid(void)
{
  return 1;
}

int _isatty(int file)
{
  switch (file)
  {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
      return 1;

    default:
    {
      errno = EBADF;
      return 0;
    }
  }
}

int _kill(int pid, int sig)
{
  errno = EINVAL;
  return (-1);
}

int _link(char *old, char *new)
{
  errno = EMLINK;
  return -1;
}

int _lseek(int fildes, int offset, int whence)
{
  return -1;
}

caddr_t _sbrk(int incr)
{
  extern char _ebss; // Defined by the linker
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0)
    heap_end = &_ebss;

  prev_heap_end = heap_end;

  char * stack = (char*) __get_MSP();

  if (heap_end + incr >  stack)
  {
    _write(STDERR_FILENO, "Heap and stack collision\n", 25);
    errno = ENOMEM;
    return  (caddr_t) -1;
  }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _read(int file, char *ptr, int len)
{
  errno = EBADF;
  return -1;
}

int _stat(const char *filepath, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

clock_t _times(struct tms *buf)
{
  return -1;
}

int _unlink(char *name)
{
  errno = ENOENT;
  return -1;
}

int _wait(int *status)
{
  errno = ECHILD;
  return -1;
}

int _write(int file, char *ptr, int len)
{
  errno = EBADF;
  return -1;
}
