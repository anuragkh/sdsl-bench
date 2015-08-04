#ifndef BENCH_H_
#define BENCH_H_

#include <sdsl/suffix_arrays.hpp>

class Benchmark {
 public:
  typedef uint64_t timestamp_t;

  Benchmark(std::string& input_file, std::string& query_file);

  void BenchmarkLookupNPA();
  void BenchmarkLookupSA();
  void BenchmarkLookupISA();

  void BenchmarkExtract();
  void BenchmarkCount();
  void BenchmarkSearch();

 private:
  void ReadQueries(std::string& query_file);

  /* Returns current timestamp */
  static timestamp_t get_timestamp() {

    struct timeval now;
    gettimeofday(&now, NULL);

    return now.tv_usec + (timestamp_t) now.tv_sec * 1000000;
  }

  std::string input_file_;
  std::string query_file_;
  std::vector<std::string> queries;
  sdsl::csa_sada<sdsl::enc_vector<>, 32, 32> csa;
};

#endif // BENCH_H_
