#include <iostream>
#include <string>
#include <stdexcept>

#include "BenchmarkSuite.h"

void runTimeBenchmarks() {
    runCreationBenchmark();
    runOwnershipBenchmark();
    runAccessBenchmark();
    runArithmeticBenchmark();
    runMemorySpanBenchmark();
}

int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "       SMART POINTER BENCHMARKS\n";
    std::cout << "========================================\n";

    try {
        if (argc == 1) {
            runTimeBenchmarks();
            return 0;
        }

        std::string mode = argv[1];

        if (mode == "time" && argc == 2) {
            runTimeBenchmarks();
        }
        else if (mode == "memory" && argc == 4) {
            std::string type = argv[2];
            std::size_t count = std::stoull(argv[3]);

            runMemoryUsageBenchmark(type, count);
        }
        else {
            std::cerr
                    << "Usage:\n"
                    << "  smart_pointer_benchmarks time\n"
                    << "  smart_pointer_benchmarks memory raw 1000000\n"
                    << "  smart_pointer_benchmarks memory unique 1000000\n"
                    << "  smart_pointer_benchmarks memory shared 1000000\n"
                    << "  smart_pointer_benchmarks memory span 1000000\n";

            return 1;
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}