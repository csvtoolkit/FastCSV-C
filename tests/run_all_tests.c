#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    const char *name;
    const char *executable;
} TestSuite;

TestSuite test_suites[] = {
    {"Arena Tests", "./test_arena"},
    {"CSV Config Tests", "./test_csv_config"},
    {"CSV Utils Tests", "./test_csv_utils"},
    {"CSV Parser Tests", "./test_csv_parser"},
    {"CSV Writer Tests", "./test_csv_writer"},
    {"CSV Reader Tests", "./test_csv_reader"}
};

int run_test_suite(const TestSuite *suite) {
    printf("\n🧪 Running %s...\n", suite->name);
    printf("=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    
    pid_t pid = fork();
    if (pid == 0) {
        execl(suite->executable, suite->executable, NULL);
        perror("execl failed");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("✅ %s PASSED\n", suite->name);
            return 0;
        } else {
            printf("❌ %s FAILED\n", suite->name);
            return 1;
        }
    } else {
        perror("fork failed");
        return 1;
    }
}

int main() {
    printf("🚀 CSV Library Test Suite Runner\n");
    printf("================================\n");
    
    int total_suites = sizeof(test_suites) / sizeof(test_suites[0]);
    int passed = 0;
    int failed = 0;
    
    for (int i = 0; i < total_suites; i++) {
        if (run_test_suite(&test_suites[i]) == 0) {
            passed++;
        } else {
            failed++;
        }
    }
    
    printf("\n" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    printf("📊 Test Results Summary\n");
    printf("=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    printf("Total Test Suites: %d\n", total_suites);
    printf("✅ Passed: %d\n", passed);
    printf("❌ Failed: %d\n", failed);
    
    if (failed == 0) {
        printf("\n🎉 All tests passed! Your CSV library is working correctly.\n");
        return 0;
    } else {
        printf("\n💥 Some tests failed. Please check the output above.\n");
        return 1;
    }
} 