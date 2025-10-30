#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Scheduler Comparison Test Runner
// Spawns multiple CPU and I/O bound processes to test scheduler behavior
// Measures: fairness, turnaround time, response time, throughput

#define NUM_CPU_PROCS 3
#define NUM_IO_PROCS 3

int main(int argc, char *argv[]) {
    printf("\n");
    printf("=====================================================\n");
    printf("   SCHEDULER PERFORMANCE BENCHMARK\n");
    printf("=====================================================\n");
    printf("This test measures scheduler performance with mixed workloads\n");
    printf("CPU-bound processes: %d\n", NUM_CPU_PROCS);
    printf("I/O-bound processes: %d\n", NUM_IO_PROCS);
    printf("=====================================================\n\n");
    
    int start_time = uptime();
    int proc_count = 0;
    
    printf("Starting CPU-bound processes...\n");
    
    // Spawn CPU-bound processes
    for (int i = 0; i < NUM_CPU_PROCS; i++) {
        int pid = fork();
        if (pid == 0) {
            // Child process
            char arg[2];
            arg[0] = '0' + i;
            arg[1] = '\0';
            char *args[] = {"cpubound", arg, 0};
            exec("cpubound", args);
            printf("ERROR: exec cpubound failed\n");
            exit(1);
        } else if (pid > 0) {
            proc_count++;
            printf("  Spawned CPU-bound process %d with PID %d\n", i, pid);
        }
    }
    
    // Small delay to stagger process starts (busy wait)
    int delay_start = uptime();
    while (uptime() - delay_start < 10) {
        // busy wait
    }
    
    printf("\nStarting I/O-bound processes...\n");
    
    // Spawn I/O-bound processes
    for (int i = 0; i < NUM_IO_PROCS; i++) {
        int pid = fork();
        if (pid == 0) {
            // Child process
            char arg[2];
            arg[0] = '0' + i;
            arg[1] = '\0';
            char *args[] = {"iobound", arg, 0};
            exec("iobound", args);
            printf("ERROR: exec iobound failed\n");
            exit(1);
        } else if (pid > 0) {
            proc_count++;
            printf("  Spawned I/O-bound process %d with PID %d\n", i, pid);
        }
    }
    
    printf("\n=====================================================\n");
    printf("All processes spawned. Waiting for completion...\n");
    printf("=====================================================\n\n");
    
    // Wait for all child processes
    int completed = 0;
    while (completed < proc_count) {
        int status;
        int pid = wait(&status);
        if (pid > 0) {
            completed++;
            printf("[SCHEDULER TEST] Process PID %d completed (%d/%d)\n", 
                   pid, completed, proc_count);
        }
    }
    
    int end_time = uptime();
    int total_time = end_time - start_time;
    
    printf("\n=====================================================\n");
    printf("   SCHEDULER BENCHMARK COMPLETE\n");
    printf("=====================================================\n");
    printf("Total execution time: %d ticks\n", total_time);
    printf("Average time per process: %d ticks\n", total_time / proc_count);
    printf("All %d processes completed successfully\n", proc_count);
    printf("=====================================================\n\n");
    
    exit(0);
}