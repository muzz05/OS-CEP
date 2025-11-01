#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define NUM_CPU_PROCS 3
#define NUM_IO_PROCS 3

// simple integer to string converter (xv6 has no sprintf)
void itoa(int n, char *str) {
  int i = 0, j;
  char temp[10];
  if (n == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }
  while (n > 0) {
    temp[i++] = '0' + (n % 10);
    n /= 10;
  }
  for (j = 0; j < i; j++)
    str[j] = temp[i - j - 1];
  str[i] = '\0';
}

int main(void) {
  printf("=== Scheduler Benchmark Test ===\n");

  // --- Run CPU-bound processes ---
  for (int i = 0; i < NUM_CPU_PROCS; i++) {
    int pid = fork();
    if (pid == 0) {
      char filename[16] = "cpu";
      char num[8];
      itoa(i, num);
      int len = strlen(filename);
      for (int j = 0; num[j]; j++) filename[len++] = num[j];
      filename[len++] = '.';
      filename[len++] = 't';
      filename[len++] = 'x';
      filename[len++] = 't';
      filename[len] = '\0';

      int fd = open(filename, O_CREATE | O_WRONLY);
      if (fd >= 0) {
        close(1); // close stdout
        dup(fd);  // redirect stdout to file
        close(fd);
      }

      char *args[] = {"cpubound", 0};
      exec("cpubound", args);
      exit(0);
    }
  }

  // --- Run IO-bound processes ---
  for (int i = 0; i < NUM_IO_PROCS; i++) {
    int pid = fork();
    if (pid == 0) {
      char filename[16] = "io";
      char num[8];
      itoa(i, num);
      int len = strlen(filename);
      for (int j = 0; num[j]; j++) filename[len++] = num[j];
      filename[len++] = '.';
      filename[len++] = 't';
      filename[len++] = 'x';
      filename[len++] = 't';
      filename[len] = '\0';

      int fd = open(filename, O_CREATE | O_WRONLY);
      if (fd >= 0) {
        close(1);
        dup(fd);
        close(fd);
      }

      char *args[] = {"iobound", 0};
      exec("iobound", args);
      exit(0);
    }
  }

  // Wait for all to complete
  for (int i = 0; i < NUM_CPU_PROCS + NUM_IO_PROCS; i++) {
    wait(0);
  }

  printf("All processes finished.\n");

  // --- Display collected outputs ---
  char buf[512];
  for (int i = 0; i < NUM_CPU_PROCS; i++) {
    printf("\n--- CPU%d Output ---\n", i);

    char filename[16] = "cpu";
    char num[8];
    itoa(i, num);
    int len = strlen(filename);
    for (int j = 0; num[j]; j++) filename[len++] = num[j];
    filename[len++] = '.';
    filename[len++] = 't';
    filename[len++] = 'x';
    filename[len++] = 't';
    filename[len] = '\0';

    int fd = open(filename, O_RDONLY);
    if (fd < 0) continue;
    int n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) write(1, buf, n);
    close(fd);
  }

  for (int i = 0; i < NUM_IO_PROCS; i++) {
    printf("\n--- IO%d Output ---\n", i);

    char filename[16] = "io";
    char num[8];
    itoa(i, num);
    int len = strlen(filename);
    for (int j = 0; num[j]; j++) filename[len++] = num[j];
    filename[len++] = '.';
    filename[len++] = 't';
    filename[len++] = 'x';
    filename[len++] = 't';
    filename[len] = '\0';

    int fd = open(filename, O_RDONLY);
    if (fd < 0) continue;
    int n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) write(1, buf, n);
    close(fd);
  }

  exit(0);
}
