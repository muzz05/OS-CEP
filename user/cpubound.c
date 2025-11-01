

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
    

    struct procinfo info;
    getprocinfo(pid, &info);
    
    if (argc > 2) {
        int pipe_fd = 0;
        for (int i = 0; argv[2][i] != '\0'; i++) {
            pipe_fd = pipe_fd * 10 + (argv[2][i] - '0');
        }
        
        struct proc_stats {
            int pid;
            int cpu_ticks;
            int num_schedules;
            int is_io;
        } stats;
        
        stats.pid = pid;
        stats.cpu_ticks = info.cpu_ticks;
        stats.num_schedules = info.num_schedules;
        stats.is_io = 0;
        
        write(pipe_fd, &stats, sizeof(stats));
        close(pipe_fd);
    }
    
    exit(0);
}