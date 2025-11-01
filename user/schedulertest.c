/*
 * schedulertest.c
 * 
 * Spawns multiple CPU-bound and I/O-bound processes simultaneously to
 * test scheduler behavior under mixed workload conditions. Measures overall
 * system throughput and process completion times.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_CPU_PROCS 3
#define NUM_IO_PROCS 3

int main(int argc, char *argv[]) {
    printf("        SCHEDULER BENCHMARK TEST\n");
    printf("Mixed workload test with:\n");
    printf("  - %d CPU-bound processes\n", NUM_CPU_PROCS);
    printf("  - %d I/O-bound processes\n", NUM_IO_PROCS);
    
    int start_time = uptime();
    int total_procs = 0;
    
    printf("Launching CPU-bound processes...\n");
    for (int i = 0; i < NUM_CPU_PROCS; i++) {
        int pid = fork();
        if (pid == 0) {
            char arg[2];
            arg[0] = '0' + i;
            arg[1] = '\0';
            char *args[] = {"cpubound", arg, 0};
            exec("cpubound", args);
            printf("Failed to execute cpubound\n");
            exit(1);
        } else if (pid > 0) {
            total_procs++;
            printf("  [CPU-%d] Started with PID %d\n", i, pid);
        }
    }
    
    int delay = uptime();
    while (uptime() - delay < 10) {
    }
    
    printf("\nLaunching I/O-bound processes...\n");
    for (int i = 0; i < NUM_IO_PROCS; i++) {
        int pid = fork();
        if (pid == 0) {
            char arg[2];
            arg[0] = '0' + i;
            arg[1] = '\0';
            char *args[] = {"iobound", arg, 0};
            exec("iobound", args);
            printf("Failed to execute iobound\n");
            exit(1);
        } else if (pid > 0) {
            total_procs++;
            printf("  [I/O-%d] Started with PID %d\n", i, pid);
        }
    }
    
    printf("All %d processes launched. Monitoring...\n", total_procs);
    
    int finished = 0;
    while (finished < total_procs) {
        int status;
        int pid = wait(&status);
        if (pid > 0) {
            finished++;
            int elapsed = uptime() - start_time;
            printf("[%d/%d] Process %d finished at %d ticks\n", 
                   finished, total_procs, pid, elapsed);
        }
    }
    
    int end_time = uptime();
    int duration = end_time - start_time;
    
    printf("        BENCHMARK COMPLETE\n");
    printf("Total time: %d ticks\n", duration);
    printf("Avg per process: %d ticks\n", duration / total_procs);
    printf("Throughput: %d processes completed\n", total_procs);
    
    exit(0);
}