#include <catch.hpp>
#include <string>
#include <DataStructures/HashTable.hpp>

using namespace std;
using namespace LCDS;

static vector<pair<string, int>> values =
{
	{ "Lewis",  21	},
	{ "Klemze", 42	},
	{ "Squid",  361 }
};

unsigned int StringHash(string& input)
{
	unsigned int hash = 0;
	for (size_t i = 0; i < input.size(); i++)
		hash = (hash * 1313) + input[i];

	return hash;
}

TEST_CASE("HashTable can be created from string keys and int values", "[hash-table]")
{
	HashTable<string, int> table(
		values,
		10, // Bucket count
		StringHash
	);

	REQUIRE(table.Size() == values.size());

	SECTION("Adding element")
	{
		pair<string, int> valueToAdd = { "Life", 42 };
		table.Add(valueToAdd);

		REQUIRE(table.Size() == (values.size() + 1));
	}

	SECTION("Finding element")
	{
		auto desiredFound = values[1];
		int foundValue = table.Find(desiredFound.first);

		REQUIRE(foundValue == desiredFound.second);
	}

	SECTION("Removing element")
	{
		table.Remove(values[2].first);

		REQUIRE(table.Size() == (values.size() - 1));
	}

	SECTION("Duplicate handling")
	{
		table.Add(values[1]);
	}
}

TEST_CASE("Elements are hashed")
{

}