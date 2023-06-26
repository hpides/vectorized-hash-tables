#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "gtest/gtest.h"
#include "hashmap/misc/stringkey.hpp"

namespace hashmap {

template <class HashmapType>
void InitializationTestImpl() {
  HashmapType hashmap(64, 0);
}

template <class HashmapType>
void LargeInitializationTestImpl() {
  HashmapType hashmap(1024 * 1024, 50);
}

template <class HashmapType>
void ContainsTestImpl() {
  HashmapType hashmap(64, 5);
  EXPECT_FALSE(hashmap.contains(42));
  EXPECT_FALSE(hashmap.contains(0));
  hashmap.insert(42, 1337);
  EXPECT_TRUE(hashmap.contains(42));
  EXPECT_FALSE(hashmap.contains(0));
}

template <class HashmapType>
void StringContainsTestImpl() {
  std::string inserted_str = "ThisIsFancy";
  std::string non_inserted_str = "ThisIsNotFancy";

  char* inserted = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  char* inserted_clone = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  char* not_inserted = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));

  std::strcpy(inserted, inserted_str.c_str());
  std::strcpy(inserted_clone, inserted_str.c_str());
  std::strcpy(not_inserted, non_inserted_str.c_str());

  HashmapType hashmap(64, 5);
  EXPECT_FALSE(hashmap.contains(StringKey(inserted)));
  EXPECT_FALSE(hashmap.contains(StringKey(not_inserted)));
  EXPECT_FALSE(hashmap.contains(StringKey(inserted_clone)));

  hashmap.insert(StringKey(inserted), 1337);

  EXPECT_TRUE(hashmap.contains(StringKey(inserted)));
  EXPECT_FALSE(hashmap.contains(StringKey(not_inserted)));
  EXPECT_TRUE(hashmap.contains(StringKey(inserted_clone)));

  std::free(reinterpret_cast<void*>(inserted));
  std::free(reinterpret_cast<void*>(inserted_clone));
  std::free(reinterpret_cast<void*>(not_inserted));
}

template <class HashmapType>
void InsertAndLookupTestImpl() {
  HashmapType hashmap(64, 5);
  EXPECT_FALSE(hashmap.contains(42));
  hashmap.insert(42, 1337);
  EXPECT_EQ(hashmap.lookup(42), 1337);
}

template <class HashmapType>
void StringInsertAndLookupTestImpl() {
  std::string inserted_str = "ThisIsFancy";

  char* inserted = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  char* inserted_clone = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));

  std::strcpy(inserted, inserted_str.c_str());
  std::strcpy(inserted_clone, inserted_str.c_str());

  HashmapType hashmap(64, 5);
  EXPECT_FALSE(hashmap.contains(StringKey(inserted)));
  EXPECT_FALSE(hashmap.contains(StringKey(inserted_clone)));

  hashmap.insert(StringKey(inserted), 1337);
  EXPECT_EQ(hashmap.lookup(StringKey(inserted)), 1337);
  EXPECT_EQ(hashmap.lookup(StringKey(inserted_clone)), 1337);

  std::free(reinterpret_cast<void*>(inserted));
  std::free(reinterpret_cast<void*>(inserted_clone));
}

template <class HashmapType>
void UpdateTestImpl() {
  HashmapType hashmap(64, 5);
  EXPECT_FALSE(hashmap.contains(42));
  hashmap.insert(42, 1337);
  EXPECT_EQ(hashmap.lookup(42), 1337);
  hashmap.insert(42, 4711);
  EXPECT_EQ(hashmap.lookup(42), 4711);
}

template <class HashmapType>
void StringUpdateTestImpl() {
  std::string inserted_str = "ThisIsFancy";

  char* inserted = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  char* inserted_clone = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));

  std::strcpy(inserted, inserted_str.c_str());
  std::strcpy(inserted_clone, inserted_str.c_str());

  HashmapType hashmap(64, 5);
  EXPECT_FALSE(hashmap.contains(StringKey(inserted)));
  EXPECT_FALSE(hashmap.contains(StringKey(inserted_clone)));
  hashmap.insert(StringKey(inserted), 1337);
  EXPECT_EQ(hashmap.lookup(StringKey(inserted)), 1337);
  EXPECT_EQ(hashmap.lookup(StringKey(inserted_clone)), 1337);
  hashmap.insert(StringKey(inserted), 4711);
  EXPECT_EQ(hashmap.lookup(StringKey(inserted)), 4711);
  EXPECT_EQ(hashmap.lookup(StringKey(inserted_clone)), 4711);

  std::free(reinterpret_cast<void*>(inserted));
  std::free(reinterpret_cast<void*>(inserted_clone));
}

template <class HashmapType>
void MultipleInsertsTestImpl() {
  HashmapType hashmap(64, 10);
  hashmap.insert(42, 1337);
  hashmap.insert(43, 1339);
  hashmap.insert(44, 1338);
  hashmap.insert(45, 1338);
  hashmap.insert(46, 1338);

  EXPECT_EQ(hashmap.lookup(42), 1337);
  EXPECT_EQ(hashmap.lookup(43), 1339);
  EXPECT_EQ(hashmap.lookup(44), 1338);
  EXPECT_EQ(hashmap.lookup(45), 1338);
  EXPECT_EQ(hashmap.lookup(46), 1338);
}

template <class HashmapType>
void StringMultipleInsertsTestImpl() {
  std::string str1 = "String1";
  char* char1 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char1, str1.c_str());

  std::string str2 = "String2";
  char* char2 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char2, str2.c_str());

  std::string str3 = "String3";
  char* char3 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char3, str3.c_str());

  std::string str4 = "String4";
  char* char4 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char4, str4.c_str());

  std::string str5 = "String5";
  char* char5 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char5, str5.c_str());

  HashmapType hashmap(64, 10);
  hashmap.insert(StringKey(char1), 1337);
  hashmap.insert(StringKey(char2), 1339);
  hashmap.insert(StringKey(char3), 1338);
  hashmap.insert(StringKey(char4), 1338);
  hashmap.insert(StringKey(char5), 1338);

  EXPECT_EQ(hashmap.lookup(StringKey(char1)), 1337);
  EXPECT_EQ(hashmap.lookup(StringKey(char2)), 1339);
  EXPECT_EQ(hashmap.lookup(StringKey(char3)), 1338);
  EXPECT_EQ(hashmap.lookup(StringKey(char4)), 1338);
  EXPECT_EQ(hashmap.lookup(StringKey(char5)), 1338);

  std::free(reinterpret_cast<void*>(char1));
  std::free(reinterpret_cast<void*>(char2));
  std::free(reinterpret_cast<void*>(char3));
  std::free(reinterpret_cast<void*>(char4));
  std::free(reinterpret_cast<void*>(char5));
}

template <class HashmapType>
void CrossBoundariesTestImpl() {
  HashmapType hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(45), 3);
}

template <class HashmapType>
void ContainsOnFullHashMapImpl() {
  HashmapType hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(45), 3);

  EXPECT_TRUE(hashmap.contains(42));
  EXPECT_TRUE(hashmap.contains(43));
  EXPECT_TRUE(hashmap.contains(44));
  EXPECT_TRUE(hashmap.contains(45));

  EXPECT_FALSE(hashmap.contains(46));
  EXPECT_FALSE(hashmap.contains(47));
}

template <class HashmapType>
void StringContainsOnFullHashMapImpl() {
  std::string str1 = "String1";
  char* char1 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char1, str1.c_str());

  std::string str2 = "String2";
  char* char2 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char2, str2.c_str());

  std::string str3 = "String3";
  char* char3 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char3, str3.c_str());

  std::string str4 = "String4";
  char* char4 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char4, str4.c_str());

  std::string str5 = "String5";
  char* char5 = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
  std::strcpy(char5, str5.c_str());

  HashmapType hashmap(4, 100);
  hashmap.insert(StringKey(char1), 0);
  hashmap.insert(StringKey(char2), 1);
  hashmap.insert(StringKey(char3), 2);
  hashmap.insert(StringKey(char4), 3);

  EXPECT_EQ(hashmap.lookup(StringKey(char1)), 0);
  EXPECT_EQ(hashmap.lookup(StringKey(char2)), 1);
  EXPECT_EQ(hashmap.lookup(StringKey(char3)), 2);
  EXPECT_EQ(hashmap.lookup(StringKey(char4)), 3);

  EXPECT_TRUE(hashmap.contains(StringKey(char1)));
  EXPECT_TRUE(hashmap.contains(StringKey(char2)));
  EXPECT_TRUE(hashmap.contains(StringKey(char3)));
  EXPECT_TRUE(hashmap.contains(StringKey(char4)));

  EXPECT_FALSE(hashmap.contains(StringKey(char5)));

  std::free(reinterpret_cast<void*>(char1));
  std::free(reinterpret_cast<void*>(char2));
  std::free(reinterpret_cast<void*>(char3));
  std::free(reinterpret_cast<void*>(char4));
  std::free(reinterpret_cast<void*>(char5));
}

template <class HashmapType>
void PointerUpdateTestImpl() {
  HashmapType hashmap(64, 7);
  uint64_t val1 = 1337;
  uint64_t val2 = 1338;

  hashmap.insert(42, &val1);
  hashmap.insert(43, &val2);

  EXPECT_EQ(hashmap.lookup(42), &val1);
  EXPECT_EQ(hashmap.lookup(43), &val2);

  hashmap.insert(42, &val2);
  EXPECT_EQ(hashmap.lookup(42), &val2);

  EXPECT_EQ(*hashmap.lookup(42), 1338);
  EXPECT_EQ(*hashmap.lookup(43), 1338);
}

template <class HashmapType>
void PointerExternalUpdateTestImpl() {
  HashmapType hashmap(64, 5);
  uint64_t val1 = 1337;
  uint64_t val2 = 1338;

  hashmap.insert(42, &val1);
  hashmap.insert(43, &val2);

  EXPECT_EQ(*hashmap.lookup(42), 1337);
  EXPECT_EQ(*hashmap.lookup(43), 1338);

  val1 = 4711;
  val2 = 4712;

  EXPECT_EQ(*hashmap.lookup(42), 4711);
  EXPECT_EQ(*hashmap.lookup(43), 4712);
}

}  // namespace hashmap
