
#include "gtest/gtest.h"

#include <cstdlib>
#include <iostream>
#include "storage.hpp"

class StorageTest : public testing::Test {
protected:

    void SetUp() override
    {
        storage.purge();
        std::srand((unsigned int)std::time(nullptr));
    }

    void TearDown() override
    {
    }
    
    Storage storage;
};

TEST_F(StorageTest, empty)
{
	int64_t tp = std::chrono::system_clock::now().time_since_epoch().count();

	{
		auto vec = storage.get_data(tp, tp);
		ASSERT_TRUE(vec.empty());
	}

	{
		auto vec = storage.get_data(0, tp);
		ASSERT_TRUE(vec.empty());
	}
}

TEST_F(StorageTest, put_get)
{
    int64_t tp1 = std::chrono::system_clock::now().time_since_epoch().count();
    auto v1 = std::rand();
    storage.put_data(tp1, v1);

    {
        auto vec = storage.get_data(tp1, tp1);
        ASSERT_EQ(vec.size(), 1U);
        EXPECT_EQ(vec[0], v1);
    }

    int64_t tp2 = std::chrono::system_clock::now().time_since_epoch().count();
    auto v2 = std::rand();
    storage.put_data(tp2, v2);

    {
        auto vec = storage.get_data(tp1, tp2);
        ASSERT_EQ(vec.size(), 2U);
        EXPECT_EQ(vec[0], v1);
        EXPECT_EQ(vec[1], v2);
    }

    int64_t tp3 = std::chrono::system_clock::now().time_since_epoch().count();
    auto v3 = std::rand();
    storage.put_data(tp3, v3);

    {
        auto vec = storage.get_data(tp1, tp3);
        ASSERT_EQ(vec.size(), 3U);
        EXPECT_EQ(vec[0], v1);
        EXPECT_EQ(vec[1], v2);
        EXPECT_EQ(vec[2], v3);
    }
}

TEST_F(StorageTest, put_get_ranges)
{
	int64_t tp_end = std::chrono::system_clock::now().time_since_epoch().count();
	int64_t tp1 = tp_end - 1000000000LL;
	int64_t tp2 = tp_end - 750000000LL;
	int64_t tp3 = tp_end - 500000000LL;

	auto v1 = std::rand();
	storage.put_data(tp1, v1);

	auto v2 = std::rand();
	storage.put_data(tp2, v2);

	auto v3 = std::rand();
	storage.put_data(tp3, v3);

	auto v4 = std::rand();
	storage.put_data(tp_end, v4);

	{
		auto vec = storage.get_data(tp1, tp_end);
		ASSERT_EQ(vec.size(), 4U);
		EXPECT_EQ(vec[0], v1);
		EXPECT_EQ(vec[1], v2);
		EXPECT_EQ(vec[2], v3);
		EXPECT_EQ(vec[3], v4);
	}
	{
		auto vec = storage.get_data(tp1, tp2);
		ASSERT_EQ(vec.size(), 2U);
		EXPECT_EQ(vec[0], v1);
		EXPECT_EQ(vec[1], v2);
	}
	{
		auto vec = storage.get_data(tp2, tp_end);
		EXPECT_EQ(vec.size(), 3U);
		EXPECT_EQ(vec[0], v2);
		EXPECT_EQ(vec[1], v3);
		EXPECT_EQ(vec[2], v4);
	}
	{
		auto vec = storage.get_data(tp2, tp3);
		ASSERT_EQ(vec.size(), 2U);
		EXPECT_EQ(vec[0], v2);
		EXPECT_EQ(vec[1], v3);
	}
	{
		auto vec = storage.get_data(tp2 - 100000000, tp3 + 100000000);
		ASSERT_EQ(vec.size(), 2U);
		EXPECT_EQ(vec[0], v2);
		EXPECT_EQ(vec[1], v3);
	}
}

TEST_F(StorageTest, get_out_of_range)
{
	int64_t tp1 = std::chrono::system_clock::now().time_since_epoch().count();
	auto v1 = std::rand();
	storage.put_data(tp1, v1);

	int64_t tp2 = std::chrono::system_clock::now().time_since_epoch().count();
	auto v2 = std::rand();
	storage.put_data(tp2, v2);


	{
		auto vec = storage.get_data(tp1 - 1000, tp1 - 1000);
		ASSERT_TRUE(vec.empty());
	}
	{
		auto vec = storage.get_data(0, tp1 - 1000);
		ASSERT_TRUE(vec.empty());
	}
	{
		auto vec = storage.get_data(tp2 + 1000, tp2 + 1000);
		ASSERT_TRUE(vec.empty());
	}
	{
		auto vec = storage.get_data(tp1 - 1000, tp2);
		ASSERT_EQ(vec.size(), 2U);
		EXPECT_EQ(vec[0], v1);
		EXPECT_EQ(vec[1], v2);
	}
	{
		auto vec = storage.get_data(tp2, tp2 + 1000);
		ASSERT_EQ(vec.size(), 1U);
		EXPECT_EQ(vec[0], v2);
	}
}

TEST_F(StorageTest, equal_range)
{
	int64_t tp = std::chrono::system_clock::now().time_since_epoch().count();
	auto v1 = std::rand();
	storage.put_data(tp, v1);

	auto v2 = std::rand();
	storage.put_data(tp, v2);

	auto v3 = std::rand();
	storage.put_data(tp, v3);

	{
		auto vec = storage.get_data(tp, tp);
		ASSERT_EQ(vec.size(), 3U);
		EXPECT_EQ(vec[0], v1);
		EXPECT_EQ(vec[1], v2);
		EXPECT_EQ(vec[2], v3);
	}
}



   