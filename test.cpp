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

int testing()
{
  std::ifstream file("~/Downloads/alevin_matrices/mouse_1/quants_mat.gz", std::ios_base::in | std::ios_base::binary);
  try {
      boost::iostreams::filtering_istream in;
      in.push(boost::iostreams::gzip_decompressor());
      in.push(file);
      for(std::string str; std::getline(in, str); )
      {
          std::cout << "Processed line " << str << '\n';
      }
  }
  catch(const boost::iostreams::gzip_error& e) {
       std::cout << e.what() << '\n';
  }
	return 0;
}


int read_quants_bin(std::string filepath) {

	std::string line;
	std::vector<std::string> rows;
	std::vector<std::string> cols;
	

	std::ifstream cb_file("/home/nikhil/Downloads/alevin_matrices/mouse_1/quants_mat_rows.txt");

	if(!cb_file) {
		std::cout << "Error opening quants_mat_rows file" << std::endl;
		return -1;
	}

	while(std::getline(cb_file, line)) {
		rows.push_back(line);
	}

	std::ifstream gene_file("/home/nikhil/Downloads/alevin_matrices/mouse_1/quants_mat_cols.txt");

	if(!gene_file) {
		std::cout << "Error opening quants_mat_cols file" << std::endl;
		return -1;
	}

	while(std::getline(gene_file, line)) {
		cols.push_back(line);
	}

	int row_size = rows.size();
	int col_size = cols.size();

	// store all the vector values in a map
	std::map<std::string, int> row_map;
	std::map<std::string, int> col_map;

	for(auto i : rows) {
		row_map[i] = 1;
	}

	for(auto i : cols) {
		col_map[i] = 1;
	}
	
	std::cout << "row_map size = " << row_map.size() << std::endl;
	std::cout << "col_map size = " << col_map.size() << std::endl;




  std::cout << "in read quants bin" << '\n';
  std::cout << "filepath = " << filepath << '\n';
  std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);

	uint64_t arr[52325];
	char *input;

  try {
      boost::iostreams::filtering_istream in;
      in.push(boost::iostreams::gzip_decompressor());
      in.push(file);
			double sample[52325];
			std::stringstream decompressed;
			in.read((char *)&sample, sizeof(sample));
			//in.close();
			for(int i = 0; i < 200; i++) {
				std::cout << "sample[" << i << "] = " << sample[i] << std::endl;
			}
			return 1;

			boost::iostreams::copy(in, decompressed);
			std::cout << "sample = " << decompressed.str() << std::endl;
			return 1;

			/*file.read(input, sizeof(arr));
			std::cout << "processed: " << input;
			
      for(std::string str; std::getline(in, str); )
      {
          std::cout << "Processed line " << str << '\n';
      }
			std::cout << "\nin try\n";*/
  }
  catch(const boost::iostreams::gzip_error& e) {
       std::cout << e.what() << '\n';
  }
	file.close();
/*
		std::ifstream file("quants_mat.gz", std::ios_base::in | std::ios_base::binary);
    boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
    inbuf.push(boost::iostreams::gzip_decompressor());
    inbuf.push(file);
    //Convert streambuf to istream
    std::istream instream(&inbuf);
    //Iterate lines
    std::string line;
    while(std::getline(instream, line)) {
        std::cout << line << std::endl;
    }
	std::cout << "file close\n";
	file.close();
*/
  /*
  std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
  // std::cout << "file = " << file << '\n';
  boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
  inbuf.push(boost::iostreams::gzip_decompressor());
  inbuf.push(file);
  //Convert streambuf to istream
  std::istream instream(&inbuf);
  //Iterate lines
  std::string line;
  while(std::getline(instream, line)) {
      std::cout << line << std::endl;
  }
  //Cleanup
  file.close();
  */
  return 0;
}

int main(int argc, char const *argv[]) {
  read_quants_bin("/home/nikhil/Downloads/alevin_matrices/mouse_1/quants_mat.gz");
	//testing();
  return 0;
}
