#include <string>
#include <vector>
#include <iostream>
#include <catch.hpp>
#include <DataStructures/BinaryTree.hpp>

#define CONSOLE_UTF8_SUPPORT
#include "Console.hpp"

using namespace std;
using namespace LCDS;

// --- INT ARRAY --- //
static int TestArray[] =
{
	10,
	12,
	8,
	22,
	15
};
unsigned int TestArraySize = 5;

TEST_CASE("Binary tree can be created from int array", "[binary-tree]")
{
	BinaryTree<int> tree(TestArray, TestArraySize);

	REQUIRE(tree.Size() == TestArraySize);
	REQUIRE(tree.Contains(TestArray[2]));

	SECTION("Adding element")
	{
		int elementToAdd = 20;
		tree.Insert(elementToAdd);

		SECTION("Searching for element")
		{
			auto node = tree.Search(elementToAdd);
			REQUIRE(node->Value == elementToAdd);
		}

		REQUIRE(tree.Size() == (TestArraySize + 1));
	}

	SECTION("Adding many elements")
	{
		tree.Insert(16);
		tree.Insert(9);
		tree.Insert(11);

		REQUIRE(tree.Size() == (TestArraySize + 3));

		SECTION("Removing element")
		{
			tree.Remove(11);

			REQUIRE(tree.Size() == (TestArraySize + 2));
		}
	}

	SECTION("Duplicate entries are not added")
	{
		for(unsigned int i = 0; i < TestArraySize; i++)
			tree.Insert(TestArray[i]);
		REQUIRE(tree.Size() == TestArraySize);
	}
}

// --- NON-STANDARD DATA TYPE --- //
struct Folder
{
	string Name;

	vector<string> Files;

	Folder() : Name(), Files() { }
	Folder(string name) : Name(name), Files() { }
	Folder(string name, vector<string> files) : Name(name), Files(files) { }

	bool operator ==(const Folder& other) { return Name.compare(other.Name) == 0; }
	bool operator !=(const Folder& other) { return Name.compare(other.Name) != 0; }

	bool operator >(const Folder& other) { return Name > other.Name; }
	bool operator <(const Folder& other) { return Name < other.Name; }
};

vector<Folder> FileSystem =
{
	Folder("Home", { "ur_mum.txt", "passwords.txt" }),
	Folder("Desktop", { "LoL", "Valorant", "Chrome" }),
	Folder("Definitely Homework", { "Samsung Sam", "Gawr Gura" }),
	Folder("Actually Homework", { "crying_for_hours.mp4" })
};

void PrintFolders(BinaryTreeNode<Folder>* node, int depth = 0)
{
	Console::Initialise(); // Initialize if not already

	string prefix;
	for (int i = 0; i < depth; i++)
		prefix += "  ";
	Console::Write(prefix + "L ", ConsoleColour::Green);
	Console::WriteLine(node->Value.Name + "/", ConsoleColour::White, ConsoleColour::DarkGreen);

	prefix += "  ";
	for (auto file : node->Value.Files)
		Console::WriteLine(prefix + "| " + file, ConsoleColour::Green);

	if(node->Left)
		PrintFolders(node->Left, depth + 1);
	if (node->Right)
		PrintFolders(node->Right, depth + 1);
}

void PrintFolders(BinaryTree<Folder>& tree) { PrintFolders(tree.GetRoot(), 0); }

TEST_CASE("Binary tree can be created from non-standard struct", "[binary-tree]")
{
	BinaryTree<Folder> tree(FileSystem.data(), (unsigned int)FileSystem.size());

	REQUIRE(tree.Size() == FileSystem.size());
	REQUIRE(tree.Contains(FileSystem[2]));

	SECTION("Adding element")
	{
		Folder elementToAdd("Memes", { "monke.png", "monkee.wav", "THE GAME" });
		tree.Insert(elementToAdd);

		SECTION("Searching for element")
		{
			auto node = tree.Search(elementToAdd);
			REQUIRE(node->Value.Files[0].compare(elementToAdd.Files[0]) == 0);
		}

		REQUIRE(tree.Size() == (FileSystem.size() + 1));
	}

	SECTION("Adding many elements")
	{
		Folder receipts("Receipts", { "Overpriced Mayonnaise", "Ugandan Knuckles NFT" });

		tree.Insert(Folder("Programmy Things", { "Unity", "Unreal Engine" }));
		tree.Insert(receipts);
		tree.Insert(Folder("Music", { "Sweet Home Alabama", "Never Gonna Give You Up" }));

		REQUIRE(tree.Size() == (FileSystem.size() + 3));

		PrintFolders(tree);


		SECTION("Removing element")
		{
			tree.Remove(receipts);

			REQUIRE(tree.Size() == (FileSystem.size() + 2));
		}
	}

	SECTION("Duplicate entries are not added")
	{
		for(auto& folder : FileSystem)
			tree.Insert(folder);
		REQUIRE(tree.Size() == FileSystem.size());
	}
}