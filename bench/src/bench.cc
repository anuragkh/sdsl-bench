#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <sdsl/suffix_arrays.hpp>

using namespace sdsl;
using namespace std;

typedef uint64_t timestamp_t;

vector<string> queries;
vector<uint64_t> positions;
vector<uint64_t> lengths;

/* Returns current timestamp */
static timestamp_t get_timestamp() {
    
    struct timeval now;
    gettimeofday (&now, NULL);
    
    return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

void read_queries(char *queryfile) {
    ifstream inputfile(queryfile);
    if(!inputfile.is_open()) {
        fprintf(stderr, "Error: Query file [ %s ] may be missing.\n", queryfile);
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

void read_positions_lengths(char *poslenfile) {
    ifstream inputfile(poslenfile);
    if(!inputfile.is_open()) {
        fprintf(stderr, "Error: Positions/Lengths file [ %s ] may be missing.\n", poslenfile);
        exit(0);
    }
    std::string line, pos, len;

    while (getline(inputfile, line)) {
        // Extract key and value
        int split_index = line.find_first_of('\t');
        pos = line.substr(0, split_index);
        len = line.substr(split_index + 1);
        positions.push_back(atol(pos.c_str()));
        lengths.push_back(atol(len.c_str()));
    }
    inputfile.close();
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [inputfile] [queryfile] [poslenfile]\n", argv[0]);
        return 0;
    }

    csa_sada<enc_vector<>, 32, 32> csa;
    char *inputfile = argv[1];
    char *queryfile = argv[2];
    char *poslenfile = argv[3];
    read_queries(queryfile);

    fprintf(stderr, "Loading csa from file...\n");
    load_from_file(csa, inputfile);

    timestamp_t t0, t1, tdiff;

    fprintf(stderr, "Benchmarking count latency...\n");
    
    // Benchmarking count latency
    ofstream f_c((string("latency_count_") + inputfile).c_str());
    for(int i = 0; i < queries.size(); i++) {
        const char *query = queries[i].c_str();
        t0 = get_timestamp();
        auto c = count(csa, query);
        t1 = get_timestamp();
        tdiff = t1 - t0;
        f_c << c << "\t" << tdiff << "\n";
    }
    f_c.close();

    fprintf(stderr, "Benchmarking locate latency...\n");
    
    ofstream f_l((string("latency_locate_") + inputfile).c_str());
    for(int i = 0; i < queries.size(); i++) {
        const char *query = queries[i].c_str();
        t0 = get_timestamp();
        auto locs = locate(csa, query);
        t1 = get_timestamp();
        tdiff = t1 - t0;
        f_l << locs.size() << "\t" << tdiff << "\n";
    }
    f_l.close();

    fprintf(stderr, "Benchmarking extract latency...\n");
    ofstream f_e((string("latency_extract_") + inputfile).c_str());
    for(int i = 0; i < positions.size(); i++) {
        uint64_t pos = rand() % csa.size();
        t0 = get_timestamp();
        auto val = extract(csa, pos, pos + 1000);
        t1 = get_timestamp();
        tdiff = t1 - t0;
        f_e << val.length() << "\t" << tdiff << "\n";
    }
    f_e.close();

    return 0;
}
