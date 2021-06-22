#include <catch.hpp>
#include <DataStructures/DoubleLinkedList.hpp>

using namespace LCDS;

static int TestArray[] =
{
	10,
	12,
	8,
	22,
	15
};

static int TestArraySorted[] =
{
	8,
	10,
	12,
	15,
	22
};

static const unsigned int TestArraySize = 5;

TEST_CASE("Double linked list can be created from int array", "[double-linked-list]")
{
	DoubleLinkedList<int> linkedList(TestArray, TestArraySize);
	REQUIRE(linkedList.Size() == TestArraySize);

	SECTION("Add element at start")
	{
		int addElementAtStart = 20;
		linkedList.Prepend(addElementAtStart);

		REQUIRE(linkedList.GetHead()->Value == addElementAtStart);
		REQUIRE(linkedList.Size() == (TestArraySize + 1));
	}

	SECTION("Delete element at front")
	{
		linkedList.RemoveAt(0);

		REQUIRE(linkedList.Size() == (TestArraySize - 1));
	}

	SECTION("Adding element at end")
	{
		int elementToAdd = 57;
		linkedList.Add(elementToAdd);

		REQUIRE(linkedList.Size() == (TestArraySize + 1));
		REQUIRE(linkedList.GetTail()->Value == elementToAdd);
	}

	SECTION("Delete element at end")
	{
		linkedList.RemoveAt(linkedList.Size() - 1); // Last element

		REQUIRE(linkedList.Size() == (TestArraySize - 1));
		REQUIRE(linkedList.GetTail()->Value == TestArray[TestArraySize - 2]);
	}

	SECTION("Remove second-from-last element by pointer")
	{
		auto secondFromEndNode = linkedList.GetTail()->Previous;
		REQUIRE(secondFromEndNode->Value == TestArray[TestArraySize - 2]);

		linkedList.Remove(secondFromEndNode);

		REQUIRE(linkedList.Size() == TestArraySize - 1);
		REQUIRE(linkedList.GetTail()->Previous->Value == TestArray[TestArraySize - 3]);
	}

	SECTION("Remove third element by index")
	{
		int index = 2;
		auto thirdElement = linkedList.GetNode(index);
		REQUIRE(thirdElement->Value == TestArray[index]);

		linkedList.RemoveAt(index);

		REQUIRE(linkedList.Size() == TestArraySize - 1);
		REQUIRE(linkedList.GetNode(index)->Value != TestArray[index]);
	}

	SECTION("Clear list")
	{
		linkedList.Clear();
		REQUIRE(linkedList.IsEmpty());
		REQUIRE(linkedList.Size() == 0);
	}

	SECTION("Sort using default comparitor (quicksort)")
	{
		linkedList.Sort();

		for (int i = 0; i < TestArraySize; i++)
			REQUIRE(linkedList.GetNode(i)->Value == TestArraySorted[i]);
	}

	SECTION("Moving node up")
	{
		int beforeIndex = 2; // 8
		int afterIndex = 1;

		auto node = linkedList.GetNode(beforeIndex);
		linkedList.Shift(beforeIndex);

		int foundIndex = linkedList.IndexOf([=](int& x) { return x == node->Value; });
		REQUIRE(foundIndex == afterIndex);
	}
	
	SECTION("Moving node down")
	{
		int beforeIndex = 2; // 8
		int afterIndex = 3;

		auto node = linkedList.GetNode(beforeIndex);
		linkedList.Shift(beforeIndex, false);

		int foundIndex = linkedList.IndexOf([=](int& x) { return x == node->Value; });
		REQUIRE(foundIndex == afterIndex);
	}
	
	SECTION("Moving node up two")
	{
		int beforeIndex = 4; // 15
		int afterIndex = 2;

		auto node = linkedList.GetNode(beforeIndex);
		linkedList.Move(beforeIndex, -2);

		int foundIndex = linkedList.IndexOf([=](int& x) { return x == node->Value; });
		REQUIRE(foundIndex == afterIndex);
	}

	SECTION("Moving node down three")
	{
		int beforeIndex = 1; // 12
		int afterIndex = 4;

		auto node = linkedList.GetNode(beforeIndex);
		linkedList.Move(beforeIndex, afterIndex - beforeIndex);

		int foundIndex = linkedList.IndexOf([=](int& x) { return x == node->Value; });
		REQUIRE(foundIndex == afterIndex);
	}
}