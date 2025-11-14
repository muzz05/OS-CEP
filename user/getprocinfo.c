#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  struct procinfo info;

  if (argc < 2) {
    pid = getpid();
  } else {
    pid = atoi(argv[1]);
  }

  int ret = getprocinfo(pid, &info);
  if (ret < 0) {
    printf("getprocinfo failed for pid %d (ret=%d)\n", pid, ret);
    exit(1);
  }

  printf("proc %d: cpu_ticks=%d, num_schedules=%d\n",
         pid, info.cpu_ticks, info.num_schedules);

  exit(0);
}
