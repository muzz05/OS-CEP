#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// CPU-Bound Benchmark for Scheduler Comparison
// Pure computational workload - tests how schedulers handle CPU-intensive processes
// Key metrics: turnaround time, waiting time, CPU burst behavior

#define COMPUTE_ITERATIONS 15000
#define NUM_PROCESSES 4

// Perform intensive computation (matrix-like calculations)
int heavy_computation(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result = (result * i) % 997;  // Prime modulo to keep numbers manageable
        
        // Additional work to increase CPU time
        for (int j = 1; j < 10; j++) {
            result = (result + j * i) % 997;
        }
    }
    return result;
}

// Calculate perfect numbers (CPU intensive divisor checking)
int is_perfect(int n) {
    int sum = 1;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            sum += i;
            if (i * i != n) {
                sum += n / i;
            }
        }
    }
    return (sum == n && n != 1);
}

int main(int argc, char *argv[]) {
    int pid = getpid();
    int process_num = 0;
    
    if (argc > 1) {
        process_num = atoi(argv[1]);
    }
    
    int start_time = uptime();
    int computation_result = 0;
    int perfect_count = 0;
    
    // Pure CPU work - no I/O, no blocking
    for (int i = 1; i <= COMPUTE_ITERATIONS; i++) {
        // Heavy computation
        computation_result += heavy_computation(i % 100);
        
        // Check for perfect numbers (divisor intensive)
        if (i % 50 == 0 && is_perfect(i)) {
            perfect_count++;
        }
        
    }
    
    int end_time = uptime();
    int turnaround_time = end_time - start_time;
    
    // Final metrics
    printf("\n========== CPU-BOUND Process %d Results ==========\n", process_num);
    printf("PID: %d\n", pid);
    printf("Computation result: %d\n", computation_result);
    printf("Perfect numbers found: %d\n", perfect_count);
    printf("Start time: %d ticks\n", start_time);
    printf("End time: %d ticks\n", end_time);
    printf("Turnaround time: %d ticks\n", turnaround_time);
    printf("Average time per iteration: %d ticks\n", 
           turnaround_time / COMPUTE_ITERATIONS);
    printf("Status: COMPLETED\n");
    printf("==================================================\n\n");
    
    exit(0);
}