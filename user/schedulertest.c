#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void run(char *prog, char *outfile, int *arrival_time, int *pid_out)
{
  *arrival_time = uptime();

  int pid = fork();
  if (pid == 0)
  {
    int fd = open(outfile, O_WRONLY | O_CREATE | O_TRUNC);
    if (fd < 0)
    {
      printf("Failed to open %s\n", outfile);
      exit(1);
    }
    close(1);
    dup(fd);
    close(fd);

    char *argv[] = {prog, 0};
    exec(prog, argv);

    printf("exec %s failed\n", prog);
    exit(1);
  }
  else if (pid > 0)
  {
    *pid_out = pid;
  }
}

void print_file(char *filename)
{
  int fd = open(filename, O_RDONLY);
  if (fd < 0)
  {
    printf("Could not open %s for reading\n", filename);
    return;
  }

  char buf[512];
  int n;
  printf("\n========================= Output of %s =========================\n", filename);
  while ((n = read(fd, buf, sizeof(buf))) > 0)
  {
    write(1, buf, n);
  }
  close(fd);
}

int main(void)
{
  printf("Launching mixed workload test...\n");

  int arrival[4], pids[4];
  run("cpubound", "cpu1.txt", &arrival[0], &pids[0]);
  run("iobound", "io1.txt", &arrival[1], &pids[1]);
  run("cpubound", "cpu2.txt", &arrival[2], &pids[2]);
  run("iobound", "io2.txt", &arrival[3], &pids[3]);

  for (int i = 0; i < 4; i++)
    wait(0);

  printf("\n--- Process Summary ---\n");
  
  for (int i = 0; i < 4; i++)
  {
    printf("PID %d | Arrival: %d ticks\n", pids[i], arrival[i]);
  }

  printf("\nAll processes finished. Reading output files...\n");

  print_file("cpu1.txt");
  print_file("io1.txt");
  print_file("cpu2.txt");
  print_file("io2.txt");

  printf("\nAll done.\n");
  exit(0);
}
