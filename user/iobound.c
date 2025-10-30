#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// I/O-Bound Benchmark for Scheduler Comparison
// Frequent I/O operations with minimal CPU work
// Tests how schedulers handle blocking processes and I/O wait times

#define NUM_IO_OPERATIONS 300
#define WRITES_PER_FILE 100

int main(int argc, char *argv[]) {
    int pid = getpid();
    int process_num = 0;
    
    if (argc > 1) {
        process_num = atoi(argv[1]);
    }
    
    printf("[I/O-BOUND Process %d] PID: %d - Starting I/O-intensive workload\n", 
           process_num, pid);
    
    int start_time = uptime();
    
    // Create unique filename for this process
    char filename[32];
    filename[0] = 'i';
    filename[1] = 'o';
    filename[2] = 'f';
    filename[3] = 'i';
    filename[4] = 'l';
    filename[5] = 'e';
    filename[6] = '_';
    filename[7] = 'p';
    filename[8] = '0' + process_num;
    filename[9] = '.';
    filename[10] = 't';
    filename[11] = 'x';
    filename[12] = 't';
    filename[13] = '\0';
    
    int total_writes = 0;
    int total_reads = 0;
    int total_bytes = 0;
    int io_wait_time = 0;
    
    printf("[I/O-BOUND P%d] Writing to %s\n", process_num, filename);
    
    // Phase 1: Frequent small writes (high I/O blocking)
    for (int iter = 0; iter < NUM_IO_OPERATIONS; iter++) {
        int before_io = uptime();
        
        int fd = open(filename, O_CREATE | O_WRONLY);
        if (fd < 0) {
            printf("[I/O-BOUND P%d] ERROR: Cannot open file\n", process_num);
            exit(1);
        }
        
        // Write small chunks frequently (causes blocking)
        char buffer[64];
        for (int i = 0; i < 64; i++) {
            buffer[i] = 'A' + (iter % 26);
        }
        
        int bytes_written = write(fd, buffer, 64);
        if (bytes_written > 0) {
            total_bytes += bytes_written;
            total_writes++;
        }
        
        close(fd);
        
        int after_io = uptime();
        io_wait_time += (after_io - before_io);
        
        // Progress tracking
        if ((iter + 1) % 50 == 0) {
            int current_time = uptime();
            printf("[I/O-BOUND P%d] Progress: %d%% | I/O ops: %d | Time: %d ticks\n",
                   process_num, ((iter + 1) * 100) / NUM_IO_OPERATIONS, 
                   total_writes, current_time - start_time);
        }
    }
    
    printf("[I/O-BOUND P%d] Write phase complete. Starting read verification...\n", 
           process_num);
    
    // Phase 2: Read operations (more I/O blocking)
    for (int iter = 0; iter < 50; iter++) {
        int before_io = uptime();
        
        int fd = open(filename, O_RDONLY);
        if (fd >= 0) {
            char read_buffer[128];
            int n = read(fd, read_buffer, 128);
            if (n > 0) {
                total_reads++;
            }
            close(fd);
        }
        
        int after_io = uptime();
        io_wait_time += (after_io - before_io);
    }
    
    int end_time = uptime();
    int turnaround_time = end_time - start_time;
    int cpu_time = turnaround_time - io_wait_time;
    
    // Final metrics
    printf("\n========== I/O-BOUND Process %d Results ==========\n", process_num);
    printf("PID: %d\n", pid);
    printf("Total write operations: %d\n", total_writes);
    printf("Total read operations: %d\n", total_reads);
    printf("Total bytes written: %d\n", total_bytes);
    printf("Start time: %d ticks\n", start_time);
    printf("End time: %d ticks\n", end_time);
    printf("Turnaround time: %d ticks\n", turnaround_time);
    printf("I/O wait time: %d ticks\n", io_wait_time);
    printf("CPU time: %d ticks\n", cpu_time);
    printf("I/O ratio: %d%%\n", (io_wait_time * 100) / turnaround_time);
    printf("Average time per I/O: %d ticks\n", 
           io_wait_time / (total_writes + total_reads));
    printf("Status: COMPLETED\n");
    printf("==================================================\n\n");
    
    exit(0);
}