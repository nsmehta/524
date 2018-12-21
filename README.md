# Alv Merge:

## CSE 524 Project

The project can be run by using the following command:

1. `g++ -o main test.cpp seekgzip.c -std=c++11 -lboost_iostreams -lz -lbz2`
2. `./main`

The test.cpp file requires filepath of both the mouse matrices to be specified in its main function.
For 2 mouse matrices with the names:
1. /home/name/mouse_1_quants_mat.gz
2. /home/name/mouse_2_quants_mat.gz

Specify the filepaths as:
1. /home/name/mouse_1_quants_mat
2. /home/name/mouse_2_quants_mat

In the method calls:
`read_quants_bin("/home/name/mouse_1_quants_mat", "/home/name/mouse_2_quants_mat");`
`search_quants_bin(1, "ATCCACCAGCGTGAAC", "ENSMUSG00000081566.1", "/home/name/mouse_1_quants_mat", "/home/name/mouse_2_quants_mat");`

The method read_quant_bin reads both the mouse matrices, uniquely combines them and generates a combined gzip file.
The method search_quants_bin generates the index file from the generated gzip file of read_quants_bin method and then searches for the cell specifies by its row ID and column ID and prints the result.
