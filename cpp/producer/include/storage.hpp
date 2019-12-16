#ifndef STORAGE_HPP_
#define STORAGE_HPP_

#include <vector>
#include <chrono>

class Storage
{
public:
    void store(int value);
    void store_batch(const std::vector<int>& values);

    std::vector<int> get(std::chrono::seconds& sec) const;

};

#endif //STORAGE_HPP_