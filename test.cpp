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
	std::vector<std::string> rows1;
	std::vector<std::string> cols1;
	std::vector<std::string> rows2;
	std::vector<std::string> cols2;
	

	std::ifstream cb_file1("/home/nikhil/Downloads/alevin_matrices/mouse_1/quants_mat_rows.txt");
	std::ifstream cb_file2("/home/nikhil/Downloads/alevin_matrices/mouse_2/quants_mat_rows.txt");

	if(!cb_file1 or !cb_file2) {
		std::cout << "Error opening quants_mat_rows file" << std::endl;
		return -1;
	}

	while(std::getline(cb_file1, line)) {
		rows1.push_back(line);
	}


	std::ifstream gene_file1("/home/nikhil/Downloads/alevin_matrices/mouse_1/quants_mat_cols.txt");
	std::ifstream gene_file2("/home/nikhil/Downloads/alevin_matrices/mouse_2/quants_mat_cols.txt");


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
		//return 1;
	
	/*for(auto i : rows2) {
		row_map2[i] = 1;
	}

	for(auto i : cols2) {
		col_map2[i] = 1;
	}*/

	

	std::cout << "row_map1 size = " << row_map1.size() << std::endl;
	std::cout << "col_map1 size = " << col_map1.size() << std::endl;

	std::cout << "row_map2 size = " << row_map2.size() << std::endl;
	std::cout << "col_map2 size = " << col_map2.size() << std::endl;


  std::cout << "in read quants bin" << '\n';
  std::cout << "filepath = " << filepath << '\n';
  std::ifstream infile1(filepath, std::ios_base::in | std::ios_base::binary);
  std::ifstream infile2("/home/nikhil/Downloads/alevin_matrices/mouse_2/quants_mat.gz", std::ios_base::in | std::ios_base::binary);
	std::ofstream outfile("out_total.gz", std::ios_base::out | std::ios_base::binary);

	double sample[col_size1 + col_size2];
	double sample1[col_size1];
	double sample2[col_size2];
	std::fill(sample, sample + col_size1 + col_size2, 0.0);

	char *input;

  try {
      boost::iostreams::filtering_istream in1;
      in1.push(boost::iostreams::gzip_decompressor());
      in1.push(infile1);

			boost::iostreams::filtering_ostream out;
      out.push(boost::iostreams::gzip_compressor());
      out.push(outfile);

			//double sample[52325];
			std::stringstream decompressed;
			int result = 0;
			while(result < row_map1.size()) {
				in1.read((char *)&sample, sizeof(sample1));
				//std::cout << "sample = " << sample[col_size1];
				out.write((char *)&sample, sizeof(sample));
				result++;
			}

			//std::cout << "sample = " << (&sample + col_size1);

      boost::iostreams::filtering_istream in2;
      in2.push(boost::iostreams::gzip_decompressor());
      in2.push(infile2);

			std::fill(sample, sample + col_size1 + col_size2, 0.0);
			result = 0;
			while(result < row_map2.size()) {
				in2.read((char *)(&sample[col_size1]), sizeof(sample2));
				out.write((char *)&sample, sizeof(sample));
				result++;
			}

			//in.close();
			/*for(int i = 0; i < 200; i++) {
				std::cout << "sample[" << i << "] = " << sample[i] << std::endl;
			}
			return 1;
			*/

			//boost::iostreams::copy(in, decompressed);
			//std::cout << "sample = " << decompressed.str() << std::endl;
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
	infile1.close();
	infile2.close();
	outfile.close();
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
