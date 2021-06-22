/*
 *
 * AIE Introduction to C++
 * Data Structures
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright.
 *
 */

#pragma once
#include <memory>
#include <utility>
#include <iostream>
#include <functional>

#include "QuickSort.hpp"

namespace LCDS
{
	// --- DOUBLE LINKED LIST NODE --- //
	template<typename T>
	struct DoubleLinkedListNode
	{
		T Value;
		DoubleLinkedListNode<T>* Next;
		DoubleLinkedListNode<T>* Previous;

		DoubleLinkedListNode(
			const T& value,
			DoubleLinkedListNode<T>* previous = nullptr,
			DoubleLinkedListNode<T>* next = nullptr
		) : Next(next), Previous(previous), Value(std::move(value)) { }
	};

	// ----- DOUBLE LINKED LIST ----- //
	template<typename T>
	class DoubleLinkedList
	{
		unsigned int m_NodeCount;
		DoubleLinkedListNode<T>* m_Head;
		DoubleLinkedListNode<T>* m_Tail;

		static bool DefaultComparitor(T& a, T& b) { return a >= b; }

	public:
		// --- CONSTRUCTORS --- //

		DoubleLinkedList() : m_NodeCount(0), m_Head(nullptr), m_Tail(nullptr) { }

		// Copy array into linked list
		DoubleLinkedList(T values[], unsigned int count) : DoubleLinkedList<T>()
		{
			// Add all values as nodes, in order
			for (unsigned int i = 0; i < count; i++)
				Add(values[i]);
		}

		// Copy constructor
		DoubleLinkedList(const DoubleLinkedList<T>&& other)
		{
			auto currentNode = other.m_Head;
			while (currentNode != nullptr)
			{
				Add(currentNode.Value);
				currentNode = currentNode->Next;
			}
		}

		~DoubleLinkedList()
		{
			if(IsEmpty())
				return;
			
			while (m_NodeCount > 0)
				RemoveAt(0);
		}

		// --- GETTERS --- //
		bool IsEmpty() const { return m_Head == nullptr; }
		DoubleLinkedListNode<T>* GetHead() const { return m_Head; }
		DoubleLinkedListNode<T>* GetTail() const { return m_Tail; }
		DoubleLinkedListNode<T>* GetNode(const uint32_t index)
		{
			unsigned int traversed = 0;
			auto currentNode = GetHead();
			while (traversed < index)
			{
				if (currentNode->Next == nullptr)
					return nullptr;

				currentNode = currentNode->Next;
				traversed++;
			}
			return currentNode;
		}

		unsigned int Size() const { return m_NodeCount; }

		// --- NODE OPERATIONS --- //

		// Appends value at end of list
		DoubleLinkedListNode<T>* Add(const T& value)
		{
			if (m_Head == nullptr)
			{
				m_Head = new DoubleLinkedListNode<T>(value);
				m_Tail = m_Head;
				m_NodeCount++;
				return m_Head;
			}

			auto node = new DoubleLinkedListNode<T>(value, m_Tail);
			m_Tail->Next = node;
			m_Tail = node;
			m_NodeCount++;
			return m_Tail;
		}

		// Inserts value at beginning of list
		DoubleLinkedListNode<T>* Prepend(const T& value)
		{
			if (m_Head == nullptr)
			{
				m_Head = new DoubleLinkedListNode<T>(value);
				m_Tail = m_Head;
				return m_Head;
			}

			auto node = new DoubleLinkedListNode<T>(value, nullptr, m_Head);
			m_Head->Previous = node;
			m_Head = node;
			m_NodeCount++;
			return m_Head;
		}

		// Inserts value at index
		DoubleLinkedListNode<T>* Insert(const T& value, uint32_t index)
		{
			auto currentNode = GetNode(index);
			if (currentNode == nullptr)
				return Add(value); // Index exceeds length of list, add to end

			auto newNode = new DoubleLinkedListNode<T>(value, currentNode->Previous, currentNode);
			if(currentNode->Previous)
				currentNode->Previous->Next = newNode;
			currentNode->Previous = newNode;

			if (index == 0)
				m_Head = newNode;

			m_NodeCount++;
			return newNode;
		}

		bool Move(uint32_t index, int amount)
		{
			if (amount == 0)
				return true;
			bool forward = amount < 0;
			if (amount < 0)
				amount = -amount;
			bool success = true;
			while (amount > 0 && success)
			{
				success = Shift(index, forward);
				amount--;

				index += forward ? -1 : 1;
			}
			return success;
		}

		bool Shift(uint32_t index, bool forward = true)
		{
			uint32_t newIndex = index + (forward ? -1 : 1);
			auto a = GetNode(index > newIndex ? newIndex : index);
			auto b = GetNode(index > newIndex ? index : newIndex);
			if (!a || !b)
				return false; // Index out of range

			// Ensure heads and tails are correct
			if (a == m_Head)
				m_Head = b;
			else if (b == m_Head)
				m_Head = a;
			if(a == m_Tail)
				m_Tail = b;
			else if (b == m_Tail)
				m_Tail = a;

			if (a->Previous)
				a->Previous->Next = b;
			if (b->Next)
				b->Next->Previous = b->Previous;

			// Le olde swapsies
			a->Next = b->Next;
			b->Previous = a->Previous;
			a->Previous = b;
			b->Next = a;

			return true;
		}

		// Deletes node from list
		bool RemoveAt(unsigned int index)
		{
			auto node = GetNode(index);
			if (node == nullptr)
				return false; // Failed to find node
			if (node->Previous) // Set previous node's `Next` to this node's `Next`
				node->Previous->Next = node->Next;
			if(node->Next)
				node->Next->Previous = node->Previous;

			// Check for start or end of list
			if(index == 0)
				m_Head = node->Next;
			if(index == m_NodeCount - 1)
				m_Tail = node->Previous;

			delete node;
			m_NodeCount--;

			return true; // Successfully removed node
		}

		// Deletes node from list
		// Deletes node from list
		bool Remove(DoubleLinkedListNode<T>* node)
		{
			if (!node)
				return false;
			if (node->Previous)
				node->Previous->Next = node->Next;
			if (node->Next)
				node->Next->Previous = node->Previous;

			delete node;
			m_NodeCount--;

			return true; // Successfully removed node
		}

		void Sort(std::function<bool(T&, T&)> comparitor = nullptr)
		{
			T** data = Data();
			Sorting::QuickSort(data, 0, Size() - 1, comparitor);

			delete[] data;
		}

		DoubleLinkedListNode<T>* Find(std::function<bool(T&)> selector)
		{
			auto node = GetHead();

			while (node)
			{
				if (selector(node->Value))
					return node;
				node = node->Next;
			}

			return nullptr;
		}
		
		int IndexOf(std::function<bool(T&)> selector)
		{
			auto node = GetHead();
			int index = 0;

			while (node)
			{
				if (selector(node->Value))
					return index;
				node = node->Next;
				index++;
			}

			return -1;
		}

		// Removes all nodes from list
		void Clear()
		{
			while(!IsEmpty())
				RemoveAt(0);
		}

		// Creates a new dynamic array of references to each value.
		// Calling function needs to delete the return value!
		T** Data()
		{
			T** values = new T*[Size()];
			
			auto node = GetHead();
			int nodeIndex = 0;
			while(node)
			{
				values[nodeIndex++] = &node->Value;
				node = node->Next;
			}

			return values;
		}

		// --- OPERATORS --- //
		DoubleLinkedListNode<T>* operator [](unsigned int index) const { return GetNode(index); }
	};

	template<typename T>
	std::ostream& operator <<(std::ostream& stream, const DoubleLinkedList<T>& list)
	{
		DoubleLinkedListNode<T>* node = list.GetHead();
		while (node != nullptr)
		{
			if (node->Previous)
				stream << " -> ";
			stream << node->Value;
			node = node->Next;
		}
		return stream;
	}
}
