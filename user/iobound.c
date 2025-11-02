#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

extern int uptime(void);

int main(int argc, char *argv[])
{
    int pid = getpid();
    int start_time = uptime();

    printf("[I/O] Process started (PID: %d) at time: %d ticks\n", pid, start_time);

    char filename[20];
    char *prefix = "iofile_";
    int prefix_len = 7;
    for (int i = 0; i < prefix_len; i++)
    {
        filename[i] = prefix[i];
    }

    int temp = pid;
    int digits = 0;
    int temp_copy = temp;
    while (temp_copy > 0)
    {
        temp_copy /= 10;
        digits++;
    }

    for (int i = digits - 1; i >= 0; i--)
    {
        filename[prefix_len + i] = '0' + (temp % 10);
        temp /= 10;
    }
    filename[prefix_len + digits] = '.';
    filename[prefix_len + digits + 1] = 't';
    filename[prefix_len + digits + 2] = 'x';
    filename[prefix_len + digits + 3] = 't';
    filename[prefix_len + digits + 4] = '\0';

    int total_operations = 0;
    int bytes_transferred = 0;
    char buffer[64];

    for (int iter = 0; iter < 250; iter++)
    {
        int fd = open(filename, O_CREATE | O_WRONLY);
        if (fd < 0)
        {
            printf("Failed to open file\n");
            exit(1);
        }

        for (int i = 0; i < 64; i++)
        {
            buffer[i] = 'A' + ((iter + i) % 26);
        }

        int written = write(fd, buffer, 64);
        bytes_transferred += written;
        total_operations++;
        close(fd);

        if (iter % 2 == 0)
        {
            fd = open(filename, O_RDONLY);
            if (fd >= 0)
            {
                char read_buf[32];
                int n = read(fd, read_buf, 32);
                bytes_transferred += n;
                total_operations++;
                close(fd);
            }
        }
    }

    int completion_time = uptime();
    printf("[I/O] Process completed (PID: %d) at time: %d ticks\n", pid, completion_time);

    struct procinfo info;
    int ret = getprocinfo(pid, &info);
    if (ret >= 0)
    {
        printf("CPU ticks used: %d\n", info.cpu_ticks);
        printf("Times scheduled: %d\n", info.num_schedules);
        int turnaround_time = completion_time - start_time;
        printf("Turnaround time: %d ticks\n", turnaround_time);
    }
    else
    {
        printf("getprocinfo failed for PID %d (ret=%d)\n", pid, ret);
    }

    exit(0);
}