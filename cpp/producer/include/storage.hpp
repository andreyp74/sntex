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
    void store(int value)
    {   
        auto start = std::chrono::system_clock::now();
        std::time_t time_point = std::chrono::system_clock::to_time_t(start);

        std::unique_lock lock(mtx);
        data.insert(std::make_pair(time_point, value));
    }

    std::vector<int> get_data(std::time_t start_time, std::time_t end_time) const
    {
        std::vector<int> result;

        {
            std::shared_lock lock(mtx, std::defer_lock);

            while(! lock.try_lock())
                std::this_thread::yield();

            auto start = data.upper_bound(start_time);
            auto end = data.lower_bound(end_time);

            for (auto it = start; it != end; ++it)
                result.push_back(it->second);
        }

        return result;
    }

    //persist_data

private:
    Storage(Storage&) = delete;
    Storage& operator=(Storage&) = delete;

private:
    using data_map_type = std::map<std::time_t, int>;
    data_map_type data;
    mutable std::shared_mutex mtx;
};

#endif //STORAGE_HPP_