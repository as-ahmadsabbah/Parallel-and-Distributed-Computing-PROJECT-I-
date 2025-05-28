'''

import subprocess
import time
import os
import csv
import matplotlib.pyplot as plt

# التجارب: عدد الاستعلامات المختلفة - أكبر وأوسع
test_cases = [1000, 5000, 10000, 20000, 50000, 100000]

# مجلد المشروع
project_dir = "/home/kasper/project prallel"

# توليد ملفات الإدخال مع قيم أكبر للنطاقات
def generate_input_file(filename, num_queries):
    L_min = 1_000_000
    range_size = 100_000
    with open(filename, 'w') as f:
        f.write(f"{num_queries}\n")
        for i in range(num_queries):
            L = L_min + i * 10  # تزايُد بسيط لتفادي التداخل الكبير
            R = L + range_size
            f.write(f"{L} {R}\n")

# تشغيل البرنامج وقياس الزمن
def run_cpp_binary(binary, input_file):
    start = time.perf_counter()
    result = subprocess.run(f"./{binary}", stdin=open(input_file), capture_output=True, text=True)
    end = time.perf_counter()
    duration_ms = (end - start) * 1000
    return duration_ms, result.stdout.strip()

# المسار الرئيسي
def main():
    os.chdir(project_dir)

    print("Compiling C++ files...")
    subprocess.run("g++ seq.cpp -o seq", shell=True, check=True)
    subprocess.run("g++ par.cpp -o par -pthread", shell=True, check=True)

    csv_path = os.path.join(project_dir, "benchmark_results.csv")
    with open(csv_path, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Queries", "Sequential Time (ms)", "Parallel Time (ms)", "Speedup", "Output Match"])

        seq_times = []
        par_times = []

        for i, q in enumerate(test_cases):
            print(f"\nRunning test case {i+1} with {q} queries...")
            input_file = os.path.join(project_dir, f"input{i+1}.txt")
            generate_input_file(input_file, q)

            seq_time, seq_output = run_cpp_binary("seq", input_file)
            par_time, par_output = run_cpp_binary("par", input_file)

            # حفظ المخرجات للمقارنة
            with open(os.path.join(project_dir, f"output_seq_{q}.txt"), "w") as f1:
                f1.write(seq_output)
            with open(os.path.join(project_dir, f"output_par_{q}.txt"), "w") as f2:
                f2.write(par_output)

            match = "YES" if seq_output == par_output else "NO"
            speedup = round(seq_time / par_time, 2) if par_time != 0 else 0

            print(f"Sequential: {seq_time:.2f} ms | Parallel: {par_time:.2f} ms | Speedup: {speedup} | Match: {match}")
            writer.writerow([q, round(seq_time, 2), round(par_time, 2), speedup, match])

            seq_times.append(seq_time)
            par_times.append(par_time)

    print("\nBenchmark complete! Results saved to", csv_path)

    # رسم بياني للمقارنة
    plt.figure(figsize=(10,6))
    plt.plot(test_cases, seq_times, marker='o', label='Sequential')
    plt.plot(test_cases, par_times, marker='o', label='Parallel')
    plt.xlabel('Number of Queries')
    plt.ylabel('Execution Time (ms)')
    plt.title('Benchmark: Sequential vs Parallel Execution Time')
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(project_dir, 'benchmark_plot.png'))
    plt.show()

if __name__ == "__main__":
    main()
'''
import subprocess
import time
import matplotlib.pyplot as plt

input_files = [f"input{i}.txt" for i in range(1, 7)]

def run_cpp_binary(binary, input_file):
    start = time.perf_counter()
    result = subprocess.run(f"./{binary}", stdin=open(input_file), capture_output=True, text=True)
    end = time.perf_counter()
    duration_ms = (end - start) * 1000
    return duration_ms, result.stdout.strip()

def count_queries(input_file):
    with open(input_file) as f:
        return int(f.readline())

def main():
    print("Compiling C++ files...")
    subprocess.run("g++ 'seq.cpp' -o seq", shell=True, check=True)
    subprocess.run("g++ 'par.cpp' -o par -pthread", shell=True, check=True)

    print("\nRunning benchmarks...\n")
    print(f"{'File':<12} {'Queries':<10} {'Seq Time (ms)':<15} {'Par Time (ms)':<15} {'Speedup':<10} {'Match'}")

    filenames = []
    queries_list = []
    seq_times = []
    par_times = []
    speedups = []

    for input_file in input_files:
        num_queries = count_queries(input_file)

        seq_time, seq_output = run_cpp_binary("seq", input_file)
        par_time, par_output = run_cpp_binary("par", input_file)

        match = "YES" if seq_output == par_output else "NO"
        speedup = round(seq_time / par_time, 2) if par_time > 0 else 0

        print(f"{input_file:<12} {num_queries:<10} {seq_time:<15.2f} {par_time:<15.2f} {speedup:<10} {match}")

        # حفظ البيانات للرسم
        filenames.append(input_file)
        queries_list.append(num_queries)
        seq_times.append(seq_time)
        par_times.append(par_time)
        speedups.append(speedup)

    print("\nBenchmarking done.")

    # رسم الرسم البياني
    x = range(len(filenames))
    plt.figure(figsize=(10, 6))
    plt.bar(x, seq_times, width=0.4, label='Sequential', align='center')
    plt.bar([i + 0.4 for i in x], par_times, width=0.4, label='Parallel', align='center')
    plt.xticks([i + 0.2 for i in x], filenames)
    plt.ylabel("Time (ms)")
    plt.title("Sequential vs Parallel Execution Time")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
