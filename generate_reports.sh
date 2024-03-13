#!/bin/bash

if [ -d "reports" ]; then
    rm -rf "reports"
fi

 mkdir -p "reports"

cmake . -B build -G Ninja -DLIB_FAIRNESS_COMPILE_TESTS=ON -DLIB_FAIRNESS_COMPILE_BENCHMARKS=ON -DLIB_FAIRNESS_COMPILE_BENCHMARKS_AS_SINGLE_FILE=ON >> reports/outputs.txt

cmake --build build >> reports/outputs.txt

record_performance() {

    local threads=$1

    mkdir -p "reports/${threads}threads"

    sudo perf record -o "reports/${threads}threads/priority_mutex_benchmark_${threads}.data" ./build/benchmarks/priority_mutex_benchmark_${threads}
    sudo perf c2c record -o "reports/${threads}threads/priority_mutex_benchmark_${threads}_c2c.data" ./build/benchmarks/priority_mutex_benchmark_${threads}
    sudo perf record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_${threads}.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_${threads}
    sudo perf c2c record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_${threads}_c2c.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_${threads}
    sudo perf record -o "reports/${threads}threads/standard_mutex_benchmark_${threads}.data" ./build/benchmarks/standard_mutex_benchmark_${threads}
    sudo perf c2c record -o "reports/${threads}threads/standard_mutex_benchmark_${threads}_c2c.data" ./build/benchmarks/standard_mutex_benchmark_${threads}

    sudo perf record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_long_${threads}.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_long_${threads}
    sudo perf c2c record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_long_${threads}_c2c.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_long_${threads}
    sudo perf record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_long_${threads}.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_long_${threads}
    sudo perf c2c record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_long_${threads}_c2c.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_long_${threads}
    sudo perf record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_long_${threads}.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_long_${threads}
    sudo perf c2c record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_long_${threads}_c2c.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_long_${threads}

    sudo perf record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}
    sudo perf c2c record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}_c2c.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}
    sudo perf record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}
    sudo perf c2c record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}_c2c.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_gaming_${threads}
    sudo perf record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_gaming_${threads}.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_gaming_${threads}
    sudo perf c2c record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_gaming_${threads}_c2c.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_gaming_${threads}

    sudo perf record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_audio_${threads}.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_audio_${threads}
    sudo perf c2c record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_audio_${threads}_c2c.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_audio_${threads}
    sudo perf record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_audio_${threads}.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_audio_${threads}
    sudo perf c2c record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_audio_${threads}_c2c.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_audio_${threads}
    sudo perf record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_audio_${threads}.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_audio_${threads}
    sudo perf c2c record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_audio_${threads}_c2c.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_audio_${threads}

    sudo perf record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_fast_${threads}.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_fast_${threads}
    sudo perf c2c record -o "reports/${threads}threads/priority_mutex_benchmark_pipeline_benchmark_fast_${threads}_c2c.data" ./build/benchmarks/priority_mutex_benchmark_pipeline_benchmark_fast_${threads}
    sudo perf record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_fast_${threads}.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_fast_${threads}
    sudo perf c2c record -o "reports/${threads}threads/spinlock_priority_mutex_benchmark_pipeline_benchmark_fast_${threads}_c2c.data" ./build/benchmarks/spinlock_priority_mutex_benchmark_pipeline_benchmark_fast_${threads}
    sudo perf record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_fast_${threads}.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_fast_${threads}
    sudo perf c2c record -o "reports/${threads}threads/standard_mutex_benchmark_pipeline_benchmark_fast_${threads}_c2c.data" ./build/benchmarks/standard_mutex_benchmark_pipeline_benchmark_fast_${threads}
}

record_performance 8 >> reports/outputs.txt
record_performance 16 >> reports/outputs.txt
record_performance 32 >> reports/outputs.txt
record_performance 64 >> reports/outputs.txt
record_performance 128 >> reports/outputs.txt
