/*
 * schedulertest.c (Fixed)
 *
 * Prevents race conditions by mapping PIDs to pipes
 * and ensuring statistics are correctly read from each child.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_CPU_PROCS 3
#define NUM_IO_PROCS 3

struct proc_stats {
    int pid;
    int cpu_ticks;
    int num_schedules;
    int is_io;
};

int main(int argc, char *argv[])
{
    printf("        SCHEDULER BENCHMARK TEST\n");
    printf("Mixed workload test with:\n");
    printf("  - %d CPU-bound processes\n", NUM_CPU_PROCS);
    printf("  - %d I/O-bound processes\n", NUM_IO_PROCS);

    int start_time = uptime();
    int total_procs = 0;
    
    int pipes[NUM_CPU_PROCS + NUM_IO_PROCS][2];
    int pid_map[NUM_CPU_PROCS + NUM_IO_PROCS]; // FIX: PID to pipe index map

    printf("Launching CPU-bound processes...\n");
    for (int i = 0; i < NUM_CPU_PROCS; i++)
    {
        if (pipe(pipes[i]) < 0) {
            printf("Pipe creation failed\n");
            exit(1);
        }
        
        int pid = fork();
        if (pid == 0)
        {
            close(pipes[i][0]); // Close read end
            
            char arg[2];
            arg[0] = '0' + i;
            arg[1] = '\0';
            
            char pipe_arg[16];
            int fd = pipes[i][1];
            int len = 0;
            int temp = fd;
            if (temp == 0) {
                pipe_arg[len++] = '0';
            } else {
                char rev[16];
                int rev_len = 0;
                while (temp > 0) {
                    rev[rev_len++] = '0' + (temp % 10);
                    temp /= 10;
                }
                for (int j = rev_len - 1; j >= 0; j--) {
                    pipe_arg[len++] = rev[j];
                }
            }
            pipe_arg[len] = '\0';
            
            char *args[] = {"cpubound", arg, pipe_arg, 0};
            exec("cpubound", args);
            printf("Failed to execute cpubound\n");
            exit(1);
        }
        else if (pid > 0)
        {
            close(pipes[i][1]); // Close write end
            pid_map[i] = pid;   // FIX: store pid to pipe index
            total_procs++;
            printf("  [CPU-%d] Started with PID %d\n", i, pid);
        }
    }

    int delay = uptime();
    while (uptime() - delay < 10) { }

    printf("\n---------------------Launching I/O-bound processes...-------------------------\n");
    for (int i = 0; i < NUM_IO_PROCS; i++)
    {
        int pipe_idx = NUM_CPU_PROCS + i;
        if (pipe(pipes[pipe_idx]) < 0) {
            printf("Pipe creation failed\n");
            exit(1);
        }
        
        int pid = fork();
        if (pid == 0)
        {
            close(pipes[pipe_idx][0]); // Close read end
            
            char arg[2];
            arg[0] = '0' + i;
            arg[1] = '\0';
            
            char pipe_arg[16];
            int fd = pipes[pipe_idx][1];
            int len = 0;
            int temp = fd;
            if (temp == 0) {
                pipe_arg[len++] = '0';
            } else {
                char rev[16];
                int rev_len = 0;
                while (temp > 0) {
                    rev[rev_len++] = '0' + (temp % 10);
                    temp /= 10;
                }
                for (int j = rev_len - 1; j >= 0; j--) {
                    pipe_arg[len++] = rev[j];
                }
            }
            pipe_arg[len] = '\0';
            
            char *args[] = {"iobound", arg, pipe_arg, 0};
            exec("iobound", args);
            printf("Failed to execute iobound\n");
            exit(1);
        }
        else if (pid > 0)
        {
            close(pipes[pipe_idx][1]);
            pid_map[pipe_idx] = pid; // FIX: store pid mapping
            total_procs++;
            printf("  [I/O-%d] Started with PID %d\n", i, pid);
        }
    }

    printf("All %d processes launched. Monitoring...\n\n", total_procs);

    int finished = 0;
    while (finished < total_procs)
    {
        int status;
        int pid = wait(&status);
        if (pid > 0)
        {
            finished++;
            int elapsed = uptime() - start_time;

            // FIX: find exact pipe index by PID
            int pipe_idx = -1;
            for (int i = 0; i < total_procs; i++) {
                if (pid_map[i] == pid) {
                    pipe_idx = i;
                    break;
                }
            }

            struct proc_stats stats;
            int n = -1;

            if (pipe_idx >= 0) {
                n = read(pipes[pipe_idx][0], &stats, sizeof(stats));
                close(pipes[pipe_idx][0]);
            }

            if (n == sizeof(stats)) {
                char *proc_type = stats.is_io ? "I/O" : "CPU";
                printf("=== [%d/%d] Process %d (%s) finished at %d ticks ===\n",
                       finished, total_procs, pid, proc_type, elapsed);
                printf("    CPU ticks: %d | Schedules: %d\n",
                       stats.cpu_ticks, stats.num_schedules);
            } else {
                printf("=== [%d/%d] Process %d finished at %d ticks ===\n",
                       finished, total_procs, pid, elapsed);
                printf("    [Could not retrieve statistics]\n");
            }
            printf("\n");
        }
    }

    int end_time = uptime();
    int duration = end_time - start_time;

    printf("========================================\n");
    printf("        BENCHMARK COMPLETE\n");
    printf("========================================\n");
    printf("Total time: %d ticks\n", duration);
    printf("Avg per process: %d ticks\n", duration / total_procs);
    printf("Throughput: %d processes completed\n", total_procs);

    exit(0);
}
