#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

uint32_t collatzSteps(uint64_t value) {
    uint32_t steps = 0;

    while (value != 1) {
        if ((value & 1ull) == 0) {
            value /= 2;
        } else {
            value = value * 3 + 1;
        }
        ++steps;
    }

    return steps;
}

struct RunResult {
    uint32_t limit;
    int threadsCount;
    uint64_t totalSteps;
    long double averageSteps;
    double executionTime;
    long double timePerNumber;
};

RunResult runCollatz(uint32_t limit, int threadsCount) {
    std::vector<uint32_t> numbers(limit);
    std::iota(numbers.begin(), numbers.end(), 1u);

    std::atomic<uint32_t> nextIndex{0};
    std::vector<uint64_t> localSums(static_cast<size_t>(threadsCount), 0);
    const uint32_t chunkSize = 1024;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int t = 0; t < threadsCount; ++t) {
        threads.emplace_back([&, t]() {
            uint64_t localTotal = 0;

            while (true) {
                uint32_t begin = nextIndex.fetch_add(chunkSize);
                if (begin >= limit) {
                    break;
                }

                uint32_t end = begin + chunkSize;
                if (end > limit) {
                    end = limit;
                }

                for (uint32_t i = begin; i < end; ++i) {
                    localTotal += collatzSteps(numbers[i]);
                }
            }

            localSums[static_cast<size_t>(t)] = localTotal;
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(finish - start).count();

    uint64_t totalSteps = 0;
    for (uint64_t value : localSums) {
        totalSteps += value;
    }

    long double average = static_cast<long double>(totalSteps) / static_cast<long double>(limit);
    long double timePerNumber = static_cast<long double>(seconds) / static_cast<long double>(limit);

    return {limit, threadsCount, totalSteps, average, seconds, timePerNumber};
}

int main(int argc, char* argv[]) {
    bool fullMode = false;
    uint32_t limit = 100000;
    int threadsCount = static_cast<int>(std::thread::hardware_concurrency());
    if (threadsCount <= 0) {
        threadsCount = 4;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--full") {
            fullMode = true;
            limit = 10000000;
        } else if (arg == "--quick") {
            fullMode = false;
            limit = 100000;
        } else if (arg == "--threads" && i + 1 < argc) {
            threadsCount = std::stoi(argv[++i]);
        } else if (arg == "--limit" && i + 1 < argc) {
            limit = static_cast<uint32_t>(std::stoul(argv[++i]));
        }
    }

    if (threadsCount < 1) {
        std::cerr << "Threads count must be positive\n";
        return 1;
    }

    std::filesystem::create_directories("results");
    std::ofstream csv("results/collatz_parallel_results.csv");
    if (!csv) {
        std::cerr << "Cannot open results/collatz_parallel_results.csv\n";
        return 1;
    }

    RunResult result = runCollatz(limit, threadsCount);

    csv << "limit,threads,total_steps,average_steps,execution_time_seconds,time_per_number\n";
    csv << std::setprecision(17)
        << result.limit << ','
        << result.threadsCount << ','
        << result.totalSteps << ','
        << static_cast<double>(result.averageSteps) << ','
        << result.executionTime << ','
        << static_cast<double>(result.timePerNumber) << '\n';

    std::cout << std::setprecision(17)
              << "limit=" << result.limit << '\n'
              << "threads=" << result.threadsCount << '\n'
              << "total_steps=" << result.totalSteps << '\n'
              << "average_steps=" << static_cast<double>(result.averageSteps) << '\n'
              << "execution_time_seconds=" << result.executionTime << '\n'
              << "time_per_number=" << static_cast<double>(result.timePerNumber) << '\n';

    return fullMode ? 0 : 0;
}
