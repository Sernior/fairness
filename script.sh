#!/bin/bash

cmake . -B build -G Ninja -DLIB_FAIRNESS_COMPILE_TESTS=ON -DLIB_FAIRNESS_COMPILE_BENCHMARKS=ON
cmake --build build

mkdir -p reports

perf record -o reports/priority_mutex_benchmark ./build/benchmarks/priority_mutex_benchmark
perf report -i reports/priority_mutex_benchmark

perf record -o reports/spinlock_priority_mutex_benchmark ./build/benchmarks/spinlock_priority_mutex_benchmark
perf report -i reports/spinlock_priority_mutex_benchmark

perf record -o reports/standard_mutex_benchmark ./build/benchmarks/standard_mutex_benchmark
perf report -i reports/standard_mutex_benchmark
