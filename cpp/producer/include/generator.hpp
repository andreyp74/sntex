#ifndef GENERATOR_HPP_
#define GENERATOR_HPP_

#include <thread>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <memory>

#include "storage.hpp"

class Generator
{
public:
    Generator(std::shared_ptr<Storage> storage, int generation_period_ms=20):
        storage(storage),
        generation_period_ms(generation_period_ms),
        done(false),
        local_thread(&Generator::run, this)
    {
        std::srand(std::time(nullptr));
    }

    ~Generator()
    {
        stop();
    }

    void run()
    {
        while(!done)
        {
            int value = get_next();
            storage->store(value);

            std::this_thread::sleep_for(
                std::chrono::milliseconds(generation_period_ms));
        }
    }

    void stop()
    {
        done = true;
        local_thread.join();
    }

private:

    int get_next()
    {
        return std::rand();
    }

    std::atomic<bool> done;
    int generation_period_ms;
    std::thread local_thread;
    std::shared_ptr<Storage> storage;
};

#endif //GENERATOR_HPP_