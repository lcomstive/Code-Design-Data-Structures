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
	78,
	101,
	90,
	88,
	89,
	40,
	50,
	45,
	30
};
unsigned int TestArraySize = 9;

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
	}

	SECTION("Duplicate entries are not added")
	{
		for(unsigned int i = 0; i < TestArraySize; i++)
			tree.Insert(TestArray[i]);
		REQUIRE(tree.Size() == TestArraySize);
	}
}

static int TestRemoveArray[] =
{
	10,
	20,
	30,
	40
};
unsigned int TestRemoveArraySize = 4;
TEST_CASE("Binary tree remove tests", "[binary-tree]")
{
	SECTION("Remove root node")
	{
		int values[] = { 10, 20, 30 };
		BinaryTree<int> tree(values, 3);

		REQUIRE(tree.Size() == 3);

		// Remove root
		tree.Remove(tree.GetRoot());

		auto root = tree.GetRoot();
		REQUIRE(root->Value == 20);
		REQUIRE(root->Right->Value == 30);
		REQUIRE(root->Left == nullptr);
	}

	SECTION("Remove middle node")
	{
		int values[] = { 20, 10, 30, 40 };
		BinaryTree<int> tree(values, 4);

		REQUIRE(tree.Size() == 4);

		// Remove middle element
		auto midElement = tree.Search(30);
		REQUIRE(midElement != nullptr);
		REQUIRE(midElement->Value == 30);
		tree.Remove(midElement);

		auto root = tree.GetRoot();
		REQUIRE(root->Value == 20);
		REQUIRE(root->Right->Value == 40);
		REQUIRE(root->Left->Value == 10);
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
};
inline int operator -(const Folder& a, const Folder& b) { return a.Name.compare(b.Name); }
inline bool operator ==(const Folder& a, const Folder& b) { return a.Name.compare(b.Name) == 0; }

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

	if (!node)
	{
		Console::WriteLine("EMPTY TREE", ConsoleColour::Red);
		return;
	}

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
	tree.SetComparisonFunction([](Folder& a, Folder& b) { return a - b; });

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
		tree.Insert(Folder("Programmy Things", { "Unity", "Unreal Engine" }));
		tree.Insert(Folder("Receipts", { "Overpriced Mayonnaise", "Ugandan Knuckles NFT" }));
		tree.Insert(Folder("Music", { "Sweet Home Alabama", "Never Gonna Give You Up" }));

		REQUIRE(tree.Size() == (FileSystem.size() + 3));
	}

	SECTION("Search tree by value")
	{
		Folder receipts("Receipts", { "Overpriced Mayonnaise", "Ugandan Knuckles NFT" });

		tree.Insert(receipts);
		auto receiptsNode = tree.Search(receipts);
		REQUIRE(receiptsNode->Value.Name.compare(receipts.Name) == 0);

		SECTION("Removing element by value")
		{
			tree.Remove(receipts);
			REQUIRE(tree.Size() == FileSystem.size());
		}

		SECTION("Removing element by node")
		{
			tree.Remove(receiptsNode);

			REQUIRE(tree.Size() == FileSystem.size());
			REQUIRE(tree.Search(receipts) == nullptr);
		}
	}

	SECTION("Search tree by selector")
	{
		Folder& searchFolder = FileSystem[(int)FileSystem.size() / 2];
		auto node = tree.Search([&](Folder& x) { return x == searchFolder; });

		REQUIRE(node != nullptr);
		REQUIRE(node->Value == searchFolder);
		REQUIRE(node->Value.Files.size() == searchFolder.Files.size());
	}
	
	SECTION("Clear tree")
	{
		tree.Clear();
		REQUIRE(tree.Size() == 0);
		REQUIRE(tree.GetRoot() == nullptr);
	}

	SECTION("Duplicate entries are not added")
	{
		for(auto& folder : FileSystem)
			tree.Insert(folder);
		REQUIRE(tree.Size() == FileSystem.size());
	}
}