#ifndef FILE_STORAGE_HPP_
#define FILE_STORAGE_HPP_

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <shared_mutex>

//#include <filesystem>

using key_type = int64_t;
using value_type = int;
using data_map_type = std::multimap<key_type, value_type>;

class FileStorage
{
public:

	FileStorage(const std::string& dir) :
		dir_(dir)
	{}

	void put_data(data_map_type&& data)
	{
		key_type start_point;
		std::string file_name;
		std::tie(start_point, file_name) = write(std::move(data));
		{
			std::unique_lock<std::mutex> lock(index_mtx);
			index.emplace(start_point, file_name);
		}
	}

	void get_data(key_type start_point, key_type end_point, std::vector<value_type>& result) const
	{
		std::vector<std::string> files;
		{
			std::unique_lock<std::mutex> lock(index_mtx);

			auto it_start = index.lower_bound(start_point);
			auto it_end = index.upper_bound(end_point);
			if (it_start != index.end() && it_start != index.begin() && it_start->first > start_point)
				--it_start;

			for (auto it = it_start; it != it_end; ++it)
			{
				files.push_back(it->second);
			}
		}

		for (auto& file_name : files)
		{
			data_map_type m = read(file_name);
			auto it_start = m.lower_bound(start_point);
			auto it_end = m.upper_bound(end_point);
			for (auto it = it_start; it != it_end; ++it)
			{
				result.push_back(it->second);
			}
		}
	}

private:

	std::pair<key_type, std::string> write(data_map_type&& data) const
	{
		auto start_point = data.begin()->first;
		auto end_point = data.rbegin()->first;

		std::string file_name = dir_ + std::to_string(start_point) + "_" + std::to_string(end_point) + ".dat";
		std::ofstream output(file_name, std::ios::binary | std::ios::trunc | std::ios::out);
		if (!output.is_open())
			throw std::runtime_error("Couldn't open file " + file_name + " for writing");

		for (auto const& kv : data)
		{
			output << kv.first << kv.second;
		}
		return { start_point, file_name };
	}

	data_map_type read(const std::string& file_name) const
	{
		data_map_type result;

		char key_buff[sizeof(key_type)];
		char value_buff[sizeof(value_type)];
		std::ifstream input(file_name, std::ios::binary | std::ios::in);
		if (!input.is_open())
			throw std::runtime_error("Couldn't open file " + file_name + " for reading");

		while (!input.eof())
		{
			input.read(key_buff, sizeof(key_type));
			input.read(value_buff, sizeof(value_type));
			result.emplace(atoll(key_buff), atoi(value_buff));
		}
		return result;
	}

private:
	std::string dir_;
	std::map<key_type, std::string> index;
	mutable std::mutex index_mtx;
};

#endif //FILE_STORAGE_HPP_
