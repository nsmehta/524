#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <stdint.h>
#include <map>
#include <sstream>
#include "seekgzip.h"


// g++ test.cpp -std=c++11 -lboost_iostreams -lz -lbz2


int read_quants_bin(std::string filepath1, std::string filepath2) {

	std::string line;
	std::vector<std::string> rows1;
	std::vector<std::string> cols1;
	std::vector<std::string> rows2;
	std::vector<std::string> cols2;
	

	std::ifstream cb_file1(filepath1 + "_rows.txt");
	std::ifstream cb_file2(filepath2 + "_rows.txt");

	if(!cb_file1 or !cb_file2) {
		std::cout << "Error opening quants_mat_rows file" << std::endl;
		return -1;
	}

	while(std::getline(cb_file1, line)) {
		rows1.push_back(line);
	}


	std::ifstream gene_file1(filepath1 + "_cols.txt");
	std::ifstream gene_file2(filepath2 + "_cols.txt");


	if(!gene_file1 or !gene_file2) {
		std::cout << "Error opening quants_mat_cols file" << std::endl;
		return -1;
	}

	while(std::getline(gene_file1, line)) {
		cols1.push_back(line);
	}



	int row_size1 = rows1.size();
	int col_size1 = cols1.size();
	int row_size2 = rows2.size();
	int col_size2 = cols2.size();


	// store all the vector values in a map
	std::map<std::string, int> row_map1;
	std::map<std::string, int> col_map1;
	std::map<std::string, int> row_map2;
	std::map<std::string, int> col_map2;


	for(auto i : rows1) {
		row_map1[i] = 1;
	}

	for(auto i : cols1) {
		col_map1[i] = 1;
	}

	// read cb_file2 and check for duplicates with cb_file1
	while(std::getline(cb_file2, line)) {

		std::map<std::string,int>::iterator i = row_map1.find(line);

		if (i == row_map1.end()) {
			/* Not found */
			row_map1[line] = 1;
			rows1.push_back(line);
		}
		else {
			/* Found, i->first is f, i->second is ++-- */
			rows1.push_back(line + "_" + std::to_string(i->second));
			i->second++;
		}
		
	}

	// read gene_file2 and check for duplicates with gene_file1
	while(std::getline(gene_file2, line)) {

		std::map<std::string,int>::iterator i = col_map1.find(line);

		if (i == col_map1.end()) {
			/* Not found */
			col_map1[line] = 1;
			cols1.push_back(line);
		}
		else {
			/* Found, i->first is f, i->second is ++-- */
			cols1.push_back(line + "_" + std::to_string(i->second));
			i->second++;
		}
		
	}


	std::ofstream cb_file("rows_total.txt");
  for (const auto &e : rows1) cb_file << e << "\n";
	cb_file.close();

	std::ofstream gene_file("cols_total.txt");
  for (const auto &e : cols1) gene_file << e << "\n";
	gene_file.close();

	

  std::cout << "in read quants bin" << '\n';
  std::cout << "filepath1 = " << filepath1 << '\n';
  std::ifstream infile1(filepath1 + ".gz", std::ios_base::in | std::ios_base::binary);
  std::ifstream infile2(filepath2 + ".gz", std::ios_base::in | std::ios_base::binary);
	std::ofstream outfile("out_total.gz", std::ios_base::out | std::ios_base::binary);
	std::ofstream outpointers("out_pointers.txt", std::ios_base::out | std::ios::trunc | std::ios_base::binary);




	double sample1[col_size1];
	double sample2[col_size2];

	char *input;

  try {
      boost::iostreams::filtering_istream in1;
      in1.push(boost::iostreams::gzip_decompressor());
      in1.push(infile1);

			boost::iostreams::filtering_ostream out;
      out.push(boost::iostreams::gzip_compressor());
      out.push(outfile);

			double length = 0.0;			
			outpointers.write(reinterpret_cast<const char *>(&length), sizeof(length));


			std::stringstream decompressed;
			int result = 0;
			while(result < row_map1.size()) {
				in1.read((char *)&sample1, sizeof(sample1));
				//std::cout << "sample = " << sample[col_size1];
				out.write((char *)&sample1, sizeof(sample1));
				// outfile2.write((char *)&sample1, sizeof(sample1));

				result++;
			}
			length = outfile.tellp();
			std::cout << "length = " << length << std::endl;

			outpointers.write(reinterpret_cast<const char *>(&length), sizeof(length));

      boost::iostreams::filtering_istream in2;
      in2.push(boost::iostreams::gzip_decompressor());
      in2.push(infile2);


			result = 0;
			while(result < row_map2.size()) {

				in2.read((char *)&sample2, sizeof(sample2));
				out.write((char *)&sample2, sizeof(sample2));

				result++;
			}

			outpointers.close();

			

  }
  catch(const boost::iostreams::gzip_error& e) {
       std::cout << e.what() << '\n';
  }
	infile1.close();
	infile2.close();
	outfile.close();

  return 0;
}


int search_quants_bin(int mouse_id, std::string row_id, std::string col_id, std::string filepath1, std::string filepath2) {

	int ret = seekgzip_build("out_total.gz");


	if (ret != 0) {
		std::cout << "Error building index file!" << std::endl;
		return 1;
  }

	std::cout << "ret = " << ret << std::endl;
  std::ifstream r_file ("out_total.gz", std::ios::binary);


	std::ifstream infile("out_total.gz", std::ios_base::in | std::ios_base::binary);
	std::ifstream inpointers("out_pointers.txt", std::ios_base::in | std::ios_base::binary);

	
	std::string line;
	std::ifstream cb_file;
	std::ifstream gene_file;
	
	
	if(mouse_id == 1) {
		cb_file.open(filepath1 + "_rows.txt");
		gene_file.open(filepath1 + "_cols.txt");
	}
	else {
		cb_file.open(filepath2 + "_rows.txt");
		gene_file.open(filepath2 + "_cols.txt");
	}


	if(!cb_file) {
		std::cout << "Error opening quants_mat_rows file" << std::endl;
		return -1;
	}

	if(!gene_file) {
		std::cout << "Error opening quants_mat_rows file" << std::endl;
		return -1;
	}

	int row_count = 0;
	int row_flag = 0;
	int col_count = 0;
	int col_flag = 0;
	int num_cols = 0;

	while(std::getline(cb_file, line)) {
		if(line == row_id) {
			row_flag = 1;
			break;
		}
		//if(row_flag == 0) {
			row_count++;
		//}
		//num_rows++;
	}

	if(row_flag == 0) {
		std::cout << "Row ID not found!" << std::endl;
		return -1;
	}

	while(std::getline(gene_file, line)) {
		if(line == col_id) {
			col_flag = 1;
			//break;
		}
		if(col_flag == 0) {
			col_count++;
		}
		num_cols++;
	}

	if(col_flag == 0) {
		std::cout << "Col ID not found!" << std::endl;
		return -1;
	}


	
	std::cout << "row_count = " << row_count << " col_count = " <<  col_count << " num_cols = " << num_cols << std::endl;

	std::streampos fileSize;
  double *fileBuffer;
  size_t sizeOfBuffer;

	inpointers.seekg (0, std::ios::end);
	int length = inpointers.tellg();
	inpointers.seekg (0, std::ios::beg);

  char *buffer = new char[length];


	sizeOfBuffer = length / sizeof(double);

  fileBuffer = new double[2];
	std::cout << "size = " << sizeOfBuffer << std::endl;

  inpointers.read(reinterpret_cast<char*>(fileBuffer), length);

  // Convert the double array into vector
  std::vector<double> *data = new std::vector<double>(fileBuffer, fileBuffer + sizeOfBuffer);

	std::cout << "data->size() = " << data->size() << std::endl;

	if (mouse_id > data->size()) {
		std::cout << "mouse id not found!" << std::endl;
		return -1;
	}



	int count = 0;
	double offset = 0;


	for (const auto& str: *data) {
			if(count == mouse_id - 1) {
				std::cout << "str = " << str << " \n";
				offset = (double)str;
				break;
			}
			count++;
	}

	std::cout << "offset = " <<  ((row_count) * sizeof(double) * num_cols) + (col_count * sizeof(double)) << std::endl;
	offset += ((row_count) * sizeof(double) * num_cols) + (col_count * sizeof(double));

	std::cout << "offset = " << offset << std::endl;

	seekgzip_t* zs = seekgzip_open("out_total.gz", NULL);
	if (zs == NULL) {
		  fprintf(stderr, "ERROR: Failed to open the index file.\n");
		  return 1;
	}

	off_t begin = offset;
	seekgzip_seek(zs, begin);
	double result;


	int read = seekgzip_read(zs, &result, sizeof(result));

	if (0 < read) {
		std::cout << "result = " << result << std::endl;
  }
	else {
		std::cout << "Failed to locate the data.\n";
		return 1;	
	
	}




	return -1;


}


int main(int argc, char const *argv[]) {
  read_quants_bin("/home/nikhil/Downloads/mouse_matrices_alv_merge_updated/E18_20160930_Neurons_Sample_24_quants_mat", "/home/nikhil/Downloads/mouse_matrices_alv_merge_updated/E18_20160930_Neurons_Sample_22_quants_mat");

	search_quants_bin(1, "ATCCACCAGCGTGAAC", "ENSMUSG00000081566.1", "/home/nikhil/Downloads/mouse_matrices_alv_merge_updated/E18_20160930_Neurons_Sample_24_quants_mat", "/home/nikhil/Downloads/mouse_matrices_alv_merge_updated/E18_20160930_Neurons_Sample_22_quants_mat");
	//testing();
  return 0;
}
