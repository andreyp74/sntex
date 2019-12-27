#ifndef STORAGE_HPP_
#define STORAGE_HPP_

#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>


class Storage
{
public:
	Storage() :
		persist_done(false)
    {
    }

	~Storage()
	{
		stop();
	}

	void start()
	{
		persist_thread = std::thread(&Storage::persist_run, this);
	}

	void stop()
	{
		persist_done = true;
		if (persist_thread.joinable())
			persist_thread.join();
	}

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
					int64_t end_time1 = 0;
					if (start_erange.second != data.end())
						end_time1 = start_erange.second->first;
					result = std::move(get_persistent_data(start_time, end_time1));
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

    void purge()
    {
        std::unique_lock<std::shared_mutex> lock(mtx);
        data.clear();
    }

protected:
	void persist_data()
	{
		
	}

	std::vector<int> get_persistent_data(int64_t start_time, int64_t end_time) const
	{
		std::vector<int> result;
		return result;
	}

	void persist_run()
	{
		while (!persist_done)
		{
			persist_data();
			std::this_thread::sleep_for(std::chrono::seconds(20));
		}
	}

	void purge_persistent_data()
	{

	}

private:
    Storage(Storage&) = delete;
    Storage& operator=(Storage&) = delete;

private:

    using data_map_type = std::multimap<int64_t, int>;
    data_map_type data;
    mutable std::shared_mutex mtx;
    std::thread persist_thread;
    std::atomic<bool> persist_done;
};

#endif //STORAGE_HPP_