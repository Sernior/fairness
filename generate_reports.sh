#!/bin/bash

cmake . -B build -G Ninja -DLIB_FAIRNESS_COMPILE_TESTS=ON -DLIB_FAIRNESS_COMPILE_BENCHMARKS=ON -DLIB_FAIRNESS_COMPILE_BENCHMARKS_AS_SINGLE_FILE=ON
cmake --build build

mkdir -p reports

sudo perf record -o reports/priority_mutex_benchmark.data ./build/benchmarks/priority_mutex_benchmark
sudo perf c2c record -o reports/priority_mutex_benchmark_c2c.data ./build/benchmarks/priority_mutex_benchmark

sudo perf record -o reports/spinlock_priority_mutex_benchmark.data ./build/benchmarks/spinlock_priority_mutex_benchmark
sudo perf c2c record -o reports/spinlock_priority_mutex_benchmark_c2c.data ./build/benchmarks/spinlock_priority_mutex_benchmark

sudo perf record -o reports/standard_mutex_benchmark.data ./build/benchmarks/standard_mutex_benchmark
sudo perf c2c record -o reports/standard_mutex_benchmark_c2c.data ./build/benchmarks/standard_mutex_benchmark
