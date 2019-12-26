#ifndef STORAGE_HPP_
#define STORAGE_HPP_

#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <shared_mutex>

class Storage
{
public:
	Storage() = default;

    void put_data(int64_t time_point, int value)
    {   
        std::unique_lock<std::shared_mutex> lock(mtx);
        data.insert(std::make_pair(time_point, value));
    }

    std::vector<int> get_data(int64_t start_time, int64_t end_time) const
    {
		std::vector<int> result;
		data_map_type::const_iterator it_start, it_end;
        {
            std::shared_lock<std::shared_mutex> lock(mtx, std::defer_lock);

            while(! lock.try_lock())
                std::this_thread::yield();

			if (!start_time)
				it_start = data.begin();
			else
			{
				auto start_erange = data.equal_range(start_time);
				it_start = start_erange.first;
				if (std::distance(start_erange.first, start_erange.second) == 0)
				{
					//TODO get_persist_data(start_time, start_erange.second->first)
				}
			}

			if (!end_time)
				it_end = data.end();
			else
			{
				auto end_erange = data.equal_range(end_time);
				it_end = end_erange.second;
			}

			for (auto it = it_start; it != it_end; ++it)
				result.push_back(it->second);
        }

        return result;
    }

    //TODO persist_data
    //TODO get from mem & persist storage

    void purge()
    {
        std::unique_lock<std::shared_mutex> lock(mtx);
        data.clear();
    }

private:
    Storage(Storage&) = delete;
    Storage& operator=(Storage&) = delete;

private:

    using data_map_type = std::multimap<int64_t, int>;
    data_map_type data;
    mutable std::shared_mutex mtx;
};

#endif //STORAGE_HPP_