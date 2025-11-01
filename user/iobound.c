/*
 * iobound.c
 * 
 * I/O-intensive benchmark that performs frequent read and write operations.
 * Creates temporary files and repeatedly performs small I/O operations to
 * maximize context switches and blocking time. Minimal CPU computation between
 * I/O calls to simulate realistic I/O-bound behavior like database operations
 * or network services.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    int pid = getpid();
    int start_time = uptime();
    
    printf("I/O-bound process started (PID: %d)\n", pid);
    
    char filename[20];
    char *prefix = "iofile_";
    int prefix_len = 7;
    for (int i = 0; i < prefix_len; i++) {
        filename[i] = prefix[i];
    }
    
    int temp = pid;
    int digits = 0;
    int temp_copy = temp;
    while (temp_copy > 0) {
        temp_copy /= 10;
        digits++;
    }
    
    for (int i = digits - 1; i >= 0; i--) {
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
    
    for (int iter = 0; iter < 250; iter++) {
        int fd = open(filename, O_CREATE | O_WRONLY);
        if (fd < 0) {
            printf("Failed to open file\n");
            exit(1);
        }
        
        for (int i = 0; i < 64; i++) {
            buffer[i] = 'A' + ((iter + i) % 26);
        }
        
        int written = write(fd, buffer, 64);
        bytes_transferred += written;
        total_operations++;
        close(fd);
        
        if (iter % 2 == 0) {
            fd = open(filename, O_RDONLY);
            if (fd >= 0) {
                char read_buf[32];
                int n = read(fd, read_buf, 32);
                bytes_transferred += n;
                total_operations++;
                close(fd);
            }
        }
        
        if (iter > 0 && iter % 50 == 0) {
            int current = uptime();
            printf("Progress: %d operations, %d ticks elapsed\n", 
                   total_operations, current - start_time);
        }
    }
    
    int end_time = uptime();
    int total_time = end_time - start_time;
    
    printf("\nI/O-bound process completed (PID: %d)\n", pid);
    printf("Total I/O operations: %d\n", total_operations);
    printf("Bytes transferred: %d\n", bytes_transferred);
    printf("Execution time: %d ticks\n", total_time);

    struct procinfo info;
    int ret = getprocinfo(pid, &info);
    if (ret >= 0) {
        printf("procinfo: cpu_ticks=%d, num_schedules=%d\n",
               info.cpu_ticks, info.num_schedules);
        if (info.num_schedules > 0) {
            const int TICK_MS = 100; /* see trap.c timer interval */
            int cpu_time_ms = info.cpu_ticks * TICK_MS;
            int avg_ms = (info.cpu_ticks * TICK_MS) / info.num_schedules;
            printf("CPU time (from cpu_ticks): %d ms\n", cpu_time_ms);
            printf("Avg ms per schedule: %d ms\n", avg_ms);
        }
    } else {
        printf("getprocinfo failed for PID %d (ret=%d)\n", pid, ret);
    }
    
    exit(0);
}