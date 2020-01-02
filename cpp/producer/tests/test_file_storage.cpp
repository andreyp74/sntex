#include "gtest/gtest.h"

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include "file_storage.hpp"

class FileStorageTest : public testing::Test {
protected:

	FileStorageTest()
		: file_storage("./.local")
	{
	}

	void SetUp() override
	{
		std::filesystem::create_directory("./.local");
		std::srand((unsigned int)std::time(nullptr));
	}

	void TearDown() override
	{
		//std::filesystem::remove("./.local");
	}

	FileStorage file_storage;
};

TEST_F(FileStorageTest, empty)
{
	int64_t tp = std::chrono::system_clock::now().time_since_epoch().count();

	{
		std::vector<value_type> vec;
		file_storage.get_data(tp, tp, vec);
		ASSERT_TRUE(vec.empty());
	}

	{
		std::vector<value_type> vec;
		file_storage.get_data(0, tp, vec);
		ASSERT_TRUE(vec.empty());
	}
}

TEST_F(FileStorageTest, put_get)
{
	int64_t tp1 = std::chrono::system_clock::now().time_since_epoch().count();
	auto v1 = std::rand();
	int64_t tp2 = std::chrono::system_clock::now().time_since_epoch().count();
	auto v2 = std::rand();
	int64_t tp3 = std::chrono::system_clock::now().time_since_epoch().count();
	auto v3 = std::rand();

	data_map_type m;
	m.insert({ tp1, v1 });
	m.insert({ tp2, v2 });
	m.insert({ tp3, v3 });
	file_storage.put_data(std::move(m));

	{
		std::vector<value_type> vec;
		file_storage.get_data(tp1, tp3, vec);
		ASSERT_EQ(vec.size(), 3U);
		EXPECT_EQ(vec[0], v1);
		EXPECT_EQ(vec[1], v2);
		EXPECT_EQ(vec[2], v3);
	}
}