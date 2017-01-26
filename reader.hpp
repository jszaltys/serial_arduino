#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Reader
{
public:
	void read_data(const char* name)
	{
		string line, tmp;
		ifstream my_file(name);
		unsigned int counter = 0;
		if (my_file.is_open())
		{
			while (getline(my_file, line))
			{
				data.push_back(vector<char>());
				for (auto &r : line)
				{
					if (r == ' ')
					{
						data[counter].push_back((char)atoi(tmp.c_str()));
						tmp.clear();
					}
					else
						tmp += r;
				}
				data[counter].push_back((char)atoi(tmp.c_str()));
				tmp.clear();
				++counter;
			}
		}
		my_file.close();
	}
	template <size_t rows, size_t cols = 0>
	void write_data(const char(&data)[rows],const char* name)
	{
		ofstream my_file(name);
		if (my_file.is_open())
		{
			for (unsigned int j = 0; j < rows; ++j)
			{
				if (j == 15)my_file << (int)data[j];
				else my_file <<  (int)data[j] << " ";
			}
			my_file << endl;
		}
		my_file.close();
	}
	template <size_t rows, size_t cols>
	void write_data(const char(&data)[rows][cols], const char* name)
	{
		ofstream my_file(name);
		if (my_file.is_open())
		{
			for (unsigned int i = 0; i < rows; ++i)
			{
				for (unsigned int j = 0; j < cols; ++j)
				{
					if (j == 15)my_file << (char)data[i][j];
					else my_file << (char)data[i][j] << " ";
				}
				my_file << endl;
			}	
		}
		my_file.close();
	}

	vector<vector<char>> data;
};