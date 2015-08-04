#include "bench.h"

#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <vector>

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
}

void Benchmark::BenchmarkCount() {
  timestamp_t t0, t1, tdiff;

  fprintf(stderr, "Warming up...\n");
  uint64_t sum = 0;
  for (int i = 0; i < 100; i++) {
    const char *query = queries[i].c_str();
    auto c = count(csa, query);
    sum += c;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking count latency...\n");
  // Benchmarking count latency
  std::ofstream f_c((std::string("latency_count_") + input_file_).c_str());
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
  for (int i = 0; i < 100; i++) {
    const char *query = queries[i].c_str();
    auto locs = locate(csa, query);
    sum += locs.size();
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  // Benchmarking search latency
  fprintf(stderr, "Benchmarking search latency...\n");
  std::ofstream f_l((std::string("latency_search_") + input_file_).c_str());
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
    uint64_t pos = rand() % csa.size();
    auto val = extract(csa, pos, pos + 1000);
    sum += val.length();
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking extract latency...\n");
  // Benchmarking extract latency
  std::ofstream f_e((std::string("latency_extract_") + input_file_).c_str());
  for (int i = 0; i < 100000; i++) {
    uint64_t pos = rand() % csa.size();
    t0 = get_timestamp();
    auto val = extract(csa, pos, pos + 1000);
    t1 = get_timestamp();
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
    auto val = csa.psi[i];
    sum += val;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking LookupNPA latency...\n");
  // Benchmarking extract latency
  std::ofstream f_n((std::string("latency_npa_") + input_file_).c_str());
  for (int i = 0; i < 100000; i++) {
    uint64_t index = rand() % csa.size();
    t0 = get_timestamp();
    auto val = csa.psi[i];
    t1 = get_timestamp();
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
    auto val = csa.isa[i];
    sum += val;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking LookupNPA latency...\n");
  // Benchmarking extract latency
  std::ofstream f_i((std::string("latency_isa_") + input_file_).c_str());
  for (int i = 0; i < 100000; i++) {
    uint64_t index = rand() % csa.size();
    t0 = get_timestamp();
    auto val = csa.isa[i];
    t1 = get_timestamp();
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
    auto val = csa[i];
    sum += val;
  }
  fprintf(stderr, "Warmup complete! Checksum = %llu\n", sum);

  fprintf(stderr, "Benchmarking LookupNPA latency...\n");
  // Benchmarking extract latency
  std::ofstream f_s((std::string("latency_sa_") + input_file_).c_str());
  for (int i = 0; i < 100000; i++) {
    uint64_t index = rand() % csa.size();
    t0 = get_timestamp();
    auto val = csa.bwt[i];
    t1 = get_timestamp();
    tdiff = t1 - t0;
    f_s << val << "\t" << tdiff << "\n";
  }
  f_s.close();
  fprintf(stderr, "Benchmark complete!\n");
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s [inputfile] [queryfile]\n", argv[0]);
    return 0;
  }

  std::string input_file = argv[1];
  std::string query_file = argv[2];

  Benchmark bench(input_file, query_file);

  bench.BenchmarkLookupNPA();
  bench.BenchmarkLookupISA();
  bench.BenchmarkLookupSA();

  bench.BenchmarkExtract();
  bench.BenchmarkCount();
  bench.BenchmarkSearch();

  return 0;
}
