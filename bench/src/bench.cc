#include "bench.h"

#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>

void Benchmark::ReadQueries(std::string& query_file) {
  std::ifstream inputfile(query_file);
  if (!inputfile.is_open()) {
    fprintf(stderr, "Error: Query file [ %s ] may be missing.\n", query_file.c_str());
    exit(0);
  }
  std::string line, bin, query;

  while (getline(inputfile, line)) {
    // Extract key and value
    int split_index = line.find_first_of('\t');
    bin = line.substr(0, split_index);
    query = line.substr(split_index + 1);
    queries.push_back(query);
  }
  inputfile.close();
}

Benchmark::Benchmark(std::string& input_file, std::string& query_file) {
  fprintf(stderr, "Loading csa from file...\n");
  load_from_file(csa, input_file);
  fprintf(stderr, "Done!\n");

  fprintf(stderr, "Loading queries...\n");
  ReadQueries(query_file);

  input_file_ = input_file;
  query_file_ = query_file;

  fprintf(stderr, "CSA size = %llu\n", csa.size());
}

void Benchmark::BenchmarkCount() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < std::min(queries.size(), 100UL); i++) {
    const char *query = queries[i].c_str();
    auto c = count(csa, query);
    sum += c;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking count latency...\n");
  // Benchmarking count latency
  std::ofstream f_c(input_file_ + std::string(".count"));
  for (int i = 0; i < queries.size(); i++) {
    const char *query = queries[i].c_str();
    t0 = get_timestamp();
    auto c = count(csa, query);
    t1 = get_timestamp();
    tdiff = t1 - t0;
    f_c << c << "\t" << tdiff << "\n";
  }
  f_c.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkSearch() {
  timestamp_t t0, t1, tdiff;

  // Warmup
  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < std::min(queries.size(), 100UL); i++) {
    const char *query = queries[i].c_str();
    auto locs = locate(csa, query);
    sum += locs.size();
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  // Benchmarking search latency
  fprintf(stderr, "Benchmarking search latency...\n");
  std::ofstream f_l(input_file_ + std::string(".search"));
  for (int i = 0; i < queries.size(); i++) {
    const char *query = queries[i].c_str();
    t0 = get_timestamp();
    auto locs = locate(csa, query);
    t1 = get_timestamp();
    tdiff = t1 - t0;
    f_l << locs.size() << "\t" << tdiff << "\n";
  }
  f_l.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkExtract() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < 1000; i++) {
    uint64_t pos = rand() % (csa.size() - 1000);
    auto val = extract(csa, pos, pos + 1000);
    sum += val.length();
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking extract latency...\n");
  // Benchmarking extract latency
  std::ofstream f_e(input_file_ + std::string(".extract"));
  for (int i = 0; i < 100000; i++) {
    uint64_t pos = rand() % (csa.size() - 1000);
    t0 = get_timestamp();
    auto val = extract(csa, pos, pos + 1000);
    t1 = get_timestamp();
    tdiff = t1 - t0;
    f_e << val.length() << "\t" << tdiff << "\n";
  }
  f_e.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkCountTicks() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < std::min(queries.size(), 100UL); i++) {
    const char *query = queries[i].c_str();
    auto c = count(csa, query);
    sum += c;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking count latency...\n");
  // Benchmarking count latency
  std::ofstream f_c(input_file_ + std::string(".count.ticks"));
  for (int i = 0; i < queries.size(); i++) {
    const char *query = queries[i].c_str();
    t0 = rdtsc();
    auto c = count(csa, query);
    t1 = rdtsc();
    tdiff = t1 - t0;
    f_c << c << "\t" << tdiff << "\n";
  }
  f_c.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkSearchTicks() {
  timestamp_t t0, t1, tdiff;

  // Warmup
  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < std::min(queries.size(), 100UL); i++) {
    const char *query = queries[i].c_str();
    auto locs = locate(csa, query);
    sum += locs.size();
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  // Benchmarking search latency
  fprintf(stderr, "Benchmarking search latency...\n");
  std::ofstream f_l(input_file_ + std::string(".search.ticks"));
  for (int i = 0; i < queries.size(); i++) {
    const char *query = queries[i].c_str();
    t0 = rdtsc();
    auto locs = locate(csa, query);
    t1 = rdtsc();
    tdiff = t1 - t0;
    f_l << locs.size() << "\t" << tdiff << "\n";
  }
  f_l.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkExtractTicks() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < 1000; i++) {
    uint64_t pos = rand() % (csa.size() - 1000);
    auto val = extract(csa, pos, pos + 1000);
    sum += val.length();
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking extract latency...\n");
  // Benchmarking extract latency
  std::ofstream f_e(input_file_ + std::string(".extract.ticks"));
  for (int i = 0; i < 100000; i++) {
    uint64_t pos = rand() % (csa.size() - 1000);
    t0 = rdtsc();
    auto val = extract(csa, pos, pos + 1000);
    t1 = rdtsc();
    tdiff = t1 - t0;
    f_e << val.length() << "\t" << tdiff << "\n";
  }
  f_e.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkLookupNPA() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < 1000; i++) {
    uint64_t index = rand() % csa.size();
    auto val = csa.psi[index];
    sum += val;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking LookupNPA latency...\n");
  // Benchmarking extract latency
  std::ofstream f_n(input_file_ + std::string(".npa"));
  for (int i = 0; i < 100000; i++) {
    uint64_t index = rand() % csa.size();
    t0 = rdtsc();
    auto val = csa.psi[index];
    t1 = rdtsc();
    tdiff = t1 - t0;
    f_n << val << "\t" << tdiff << "\n";
  }
  f_n.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkLookupISA() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < 1000; i++) {
    uint64_t index = rand() % csa.size();
    auto val = csa.isa[index];
    sum += val;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking LookupISA latency...\n");
  // Benchmarking extract latency
  std::ofstream f_i(input_file_ + std::string(".isa"));
  for (int i = 0; i < 100000; i++) {
    uint64_t index = rand() % csa.size();
    t0 = rdtsc();
    auto val = csa.isa[index];
    t1 = rdtsc();
    tdiff = t1 - t0;
    f_i << val << "\t" << tdiff << "\n";
  }
  f_i.close();
  fprintf(stderr, "Benchmark complete!\n");
}

void Benchmark::BenchmarkLookupSA() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < 1000; i++) {
    uint64_t index = rand() % csa.size();
    auto val = csa[index];
    sum += val;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking LookupSA latency...\n");
  // Benchmarking extract latency
  std::ofstream f_s(input_file_ + std::string(".sa"));
  for (int i = 0; i < 100000; i++) {
    uint64_t index = rand() % csa.size();
    t0 = rdtsc();
    auto val = csa[index];
    t1 = rdtsc();
    tdiff = t1 - t0;
    f_s << val << "\t" << tdiff << "\n";
  }
  f_s.close();
  fprintf(stderr, "Benchmark complete!\n");
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s [inputfile] [queryfile]\n", argv[0]);
    return -1;
  }

  std::string input_file = argv[1];
  std::string query_file = argv[2];

  Benchmark bench(input_file, query_file);

  bench.BenchmarkLookupNPA();
  bench.BenchmarkLookupISA();
  bench.BenchmarkLookupSA();

  bench.BenchmarkExtractTicks();
  bench.BenchmarkCountTicks();
  bench.BenchmarkSearchTicks();

  bench.BenchmarkExtract();
  bench.BenchmarkCount();
  bench.BenchmarkSearch();

  return 0;
}
