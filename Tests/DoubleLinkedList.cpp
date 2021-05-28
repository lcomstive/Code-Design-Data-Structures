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

static const uint32_t TestArraySize = 5;

TEST_CASE("Double linked list can be created from int array", "[double-linked-list]")
{
	DoubleLinkedList linkedList(TestArray, TestArraySize);
	REQUIRE(linkedList.Size() == TestArraySize);

	SECTION("Adding element at start")
	{
		int addElementAtStart = 20;
		linkedList.Prepend(addElementAtStart);

		REQUIRE(linkedList.GetHead()->Value == addElementAtStart);
		REQUIRE(linkedList.Size() == (TestArraySize + 1));
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

	SECTION("Sort using default comparitor")
	{
		linkedList.Sort();

		for(int i = 0; i < TestArraySize; i++)
			REQUIRE(linkedList.GetNode(i)->Value == TestArraySorted[i]);
	}
}