/*
 * cpubound.c
 * 
 * CPU-intensive benchmark that performs heavy mathematical computations.
 * Uses fibonacci calculations and factorial operations to maximize CPU usage
 * without any I/O operations. Designed to test scheduler behavior with
 * compute-heavy workloads that never voluntarily yield the processor.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int fibonacci(int n) {
    if (n <= 1) return n;
    
    int a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
        
        if (c < 0) {
            c = c % 10007;
            b = c;
        }
    }
    return b;
}

int factorial_mod(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result = (result * i) % 9973;
    }
    return result;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int main(int argc, char *argv[]) {
    int pid = getpid();
    int start_time = uptime();
    
    printf("CPU-bound process started (PID: %d)\n", pid);
    
    int fib_sum = 0;
    int fact_sum = 0;
    int gcd_sum = 0;
    
    for (int i = 1; i <= 15000; i++) {
        int fib_val = fibonacci(i % 30);
        fib_sum = (fib_sum + fib_val) % 100000;
        
        int fact_val = factorial_mod(i % 15);
        fact_sum = (fact_sum + fact_val) % 100000;
        
        int gcd_val = gcd(i * 17, i * 13);
        gcd_sum = (gcd_sum + gcd_val) % 100000;
        
        for (int j = 1; j <= 100; j++) {
            int dummy = (i * j) % 997;
            fib_sum = (fib_sum + dummy) % 100000;
        }
    }
    
    int end_time = uptime();
    int total_time = end_time - start_time;
    
    printf("\nCPU-bound process completed (PID: %d)\n", pid);
    printf("Fibonacci sum: %d\n", fib_sum);
    printf("Factorial sum: %d\n", fact_sum);
    printf("GCD sum: %d\n", gcd_sum);
    printf("Execution time: %d ticks\n", total_time);
    
    struct procinfo info;
    int ret = getprocinfo(pid, &info);

    if (ret >= 0) {
        printf("CPU ticks used: %d\n", info.cpu_ticks);
        printf("Times scheduled: %d\n", info.num_schedules);
        if (info.num_schedules > 0) {
            /* tick duration is set in kernel/trap.c via w_stimecmp(r_time() + 1000000);
             * that interval corresponds to ~100 ms per tick in this build, so
             * convert ticks -> ms using TICK_MS = 100.
             */
            const int TICK_MS = 100;
            int avg_ms_per_sched = (info.cpu_ticks * TICK_MS) / info.num_schedules;
            int cpu_time_ms = info.cpu_ticks * TICK_MS;
            printf("CPU time (from cpu_ticks): %d ms\n", cpu_time_ms);
            printf("Avg ms per schedule: %d ms\n", avg_ms_per_sched);
        }
    } else {
        printf("getprocinfo failed (ret=%d)\n", ret);
    }
    
    exit(0);
}