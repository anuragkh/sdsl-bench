#ifndef BENCH_H_
#define BENCH_H_

#include <sdsl/suffix_arrays.hpp>

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void) {
  unsigned long long int x;
  __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
  return x;
}
#elif defined(__x86_64__)

static __inline__ unsigned long long rdtsc(void) {
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ((unsigned long long) lo) | (((unsigned long long) hi) << 32);
}

#elif defined(__powerpc__)

static __inline__ unsigned long long rdtsc(void) {
  unsigned long long int result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
      "0:                  \n"
      "\tmftbu   %0           \n"
      "\tmftb    %1           \n"
      "\tmftbu   %2           \n"
      "\tcmpw    %2,%0        \n"
      "\tbne     0b         \n"
      : "=r"(upper),"=r"(lower),"=r"(tmp)
  );
  result = upper;
  result = result<<32;
  result = result|lower;

  return(result);
}

#else

#error "No tick counter is available!"

#endif

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

  void BenchmarkExtractTicks();
  void BenchmarkCountTicks();
  void BenchmarkSearchTicks();

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
