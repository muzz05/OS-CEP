/*
 * iobound.c (xv6-compatible fixed version)
 *
 * Adds short busy-wait delay before exit to ensure
 * pipe write completes and parent can read stats.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[])
{
    int pid = getpid();

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

    struct procinfo info;
    getprocinfo(pid, &info);

    if (argc > 2)
    {
        int pipe_fd = 0;
        for (int i = 0; argv[2][i] != '\0'; i++)
        {
            pipe_fd = pipe_fd * 10 + (argv[2][i] - '0');
        }

        struct proc_stats
        {
            int pid;
            int cpu_ticks;
            int num_schedules;
            int is_io;
        } stats;

        stats.pid = pid;
        stats.cpu_ticks = info.cpu_ticks;
        stats.num_schedules = info.num_schedules;
        stats.is_io = 1;

        write(pipe_fd, &stats, sizeof(stats));

        // FIX: small busy-wait delay to let parent read
        for (volatile int i = 0; i < 10000000; i++) {
            ; // do nothing — just delay
        }

        close(pipe_fd);
    }

    exit(0);
}
