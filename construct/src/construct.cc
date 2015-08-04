#include <iostream>

#include <sdsl/suffix_arrays.hpp>

using namespace sdsl;

int main(int argc, char **argv) {
	if(argc < 4) {
		fprintf(stderr, "Usage: %s [csa-type] [input-file] [output-file]\n", argv[0]);
		return -1;
	}
	int csa_type = atoi(argv[1]);
	if(csa_type == 0) {
		csa_wt<wt_huff<>, 32, 32> csa;
		construct(csa, argv[2], 1);
		fprintf(stderr, "Size = %f\n", size_in_mega_bytes(csa));
		store_to_file(csa, argv[3]);
	} else if(csa_type == 1) {
		csa_sada<enc_vector<>, 32, 32> csa;
		construct(csa, argv[2], 1);
		fprintf(stderr, "Size = %f\n", size_in_mega_bytes(csa));
		store_to_file(csa, argv[3]);
	} else {
		csa_bitcompressed<> csa;
		construct(csa, argv[2], 1);
		fprintf(stderr, "Size = %f\n", size_in_mega_bytes(csa));
		store_to_file(csa, argv[3]);
	}

	
	return 0;
}
