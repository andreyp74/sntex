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
    Generator(std::shared_ptr<Storage> storage, 
              int generation_period_ms=20):
              
        storage(storage),
        generation_period_ms(generation_period_ms),
        done(false)
    {
        std::srand((unsigned int)std::time(nullptr));
    }

    ~Generator()
    {
        stop();
    }

    void start()
    {
        gen_thread = std::thread(&Generator::run, this);
    }

    void stop()
    {
        done = true;
        if (gen_thread.joinable())
            gen_thread.join();
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

private:

    int get_next()
    {
        return std::rand();
    }

    Generator(Generator&) = delete;
    Generator& operator=(Generator&) = delete;

    std::atomic<bool> done;
    int generation_period_ms;
    std::thread gen_thread;
    std::shared_ptr<Storage> storage;
};

#endif //GENERATOR_HPP_