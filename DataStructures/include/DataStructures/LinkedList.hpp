/*
 *
 * AIE Code Design & Data Structures
 * Data Structures
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright information.
 *
 */
#pragma once
#include <memory>
#include <utility>
#include <iostream>
#include <functional>

#include "QuickSort.hpp"

namespace LCDS // Lewis Comstive's Data Structures
{
	// --- LINKED LIST NODE --- //
	template<typename T>
	struct LinkedListNode
	{
		T Value;
		LinkedListNode<T>* Next;

		LinkedListNode(
			const T& value,
			LinkedListNode<T>* next = nullptr
		) : Next(next), Value(std::move(value)) { }
	};

	// ----- LINKED LIST ----- //
	template<typename T>
	class LinkedList
	{
		unsigned int m_NodeCount;  // Keep track of nodes created & deleted
		LinkedListNode<T>* m_Head; // Always points to first node
		LinkedListNode<T>* m_Tail; // Always points to last node

		// When no function is given for comparing values for sorting, this one is used
		static bool DefaultComparitor(T& a, T& b) { return a >= b; }

	public:
		// --- CONSTRUCTORS --- //

		// Default
		LinkedList() : m_NodeCount(0), m_Head(nullptr), m_Tail(nullptr) { }

		// Copy array into linked list
		LinkedList(T values[], unsigned int count) : LinkedList<T>()
		{
			// Add all values as nodes, in order
			for (unsigned int i = 0; i < count; i++)
				Add(values[i]);
		}

		// Copy constructor
		LinkedList(const LinkedList<T>&& other)
		{
			auto currentNode = other.m_Head;
			while (currentNode != nullptr)
			{
				Add(currentNode.Value);
				currentNode = currentNode->Next;
			}
		}

		~LinkedList()
		{
			if(IsEmpty())
				return;
			
			while(m_NodeCount > 0)
				RemoveAt(0);
		}

		// --- GETTERS --- //

		bool IsEmpty() const { return m_Head == nullptr; }

		// Get first node
		LinkedListNode<T>* GetHead() const { return m_Head; }

		// Get last node
		LinkedListNode<T>* GetTail() const { return m_Tail; }

		// Get node at index location
		LinkedListNode<T>* GetNode(const unsigned int index)
		{
			if(index >= Size())
				return nullptr; // Out of bounds

			unsigned int traversed = 0;
			auto currentNode = GetHead();
			while (traversed < index)
			{
				if (currentNode->Next == nullptr)
					return nullptr; // End of list, or malformed?

				currentNode = currentNode->Next;
				traversed++;
			}
			return currentNode;
		}

		unsigned int Size() const { return m_NodeCount; }

		// --- NODE OPERATIONS --- //

		// Append value at end of list
		LinkedListNode<T>* Add(const T& value)
		{
			if (m_Head == nullptr)
			{	// No other nodes in list
				m_Head = new LinkedListNode<T>(value);
				m_Tail = m_Head;
				m_NodeCount++;
				return m_Head;
			}

			auto node = new LinkedListNode<T>(value);
			m_Tail->Next = node;
			m_Tail = node;
			m_NodeCount++;
			return m_Tail;
		}

		LinkedListNode<T>* FindPrevious(LinkedListNode<T>* node)
		{
			if (!node || node == m_Head)
				return nullptr;
			auto currentNode = m_Head;
			while (currentNode)
			{
				if (currentNode->Next == node)
					return currentNode;
				currentNode = currentNode->Next;
			}
			return currentNode;
		}

		// Get location of node in list
		unsigned int GetIndex(LinkedListNode<T>* node)
		{
			unsigned int traversed = 0;
			auto currentNode = GetHead();
			while (currentNode && currentNode != node)
			{
				traversed++;
				currentNode = currentNode->Next;
			}
			return traversed;
		}

		// Inserts value at beginning of list
		LinkedListNode<T>* Prepend(const T& value)
		{
			if (m_Head == nullptr)
			{
				m_Head = new LinkedListNode<T>(value);
				m_Tail = m_Head;
				return m_Head;
			}

			auto node = new LinkedListNode<T>(value, m_Head);
			m_Head = node;
			m_NodeCount++;
			return m_Head;
		}

		// Inserts value at index
		LinkedListNode<T>* Insert(const T& value, unsigned int index)
		{
			auto currentNode = GetNode(index);
			if (currentNode == nullptr)
				return Add(value); // Index exceeds length of list, add to end
			auto previousNode = FindPrevious(currentNode);

			auto newNode = new LinkedListNode<T>(value, currentNode);

			if (previousNode)
				previousNode->Next = newNode;
			newNode->Next = currentNode;

			m_NodeCount++;
			return newNode;
		}

		// Deletes node from list
		bool RemoveAt(unsigned int index)
		{
			auto previousNode = index > 0 ? GetNode(index - 1) : nullptr;
			auto node = GetNode(index);
			if (node == nullptr)
				return false; // Failed to find node
			if (previousNode) // Set previous node's `Next` to this node's `Next`
				previousNode->Next = node->Next;

			// Check for start or end of list
			if(index == 0)
				m_Head = node->Next;
			if(index == m_NodeCount - 1)
				m_Tail = previousNode;

			delete node;
			m_NodeCount--;

			return true; // Successfully removed node
		}

		// Deletes node from list
		bool Remove(LinkedListNode<T>* node)
		{
			if(!node)
				return false;
			auto previousNode = FindPrevious(node);
			if (previousNode)
				previousNode->Next = node->Next;

			delete node;
			m_NodeCount--;
			
			return true; // Successfully removed node
		}

		// Sorts the list
		// 	(optional function for comparison type, which should return true if elements are to be swapped)
		void Sort(std::function<bool(T&, T&)> comparitor = nullptr)
		{
			T** data = Data();
			Sorting::QuickSort(data, 0, Size() - 1, comparitor);

			delete[] data;
		}

		// Creates a new dynamic array consisting of references to each value.
		// RETURNED ARRAY SHOULD BE DELETED BY ACQUIRER!
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
		LinkedListNode<T>* operator [](unsigned int index) const { return GetNode(index); }
	};

	// Stream operator, for when printing entire list to console
	template<typename T>
	std::ostream& operator <<(std::ostream& stream, const LinkedList<T>& list)
	{
		LinkedListNode<T>* node = list.GetHead();
		stream << node->Value;
		while (node != nullptr)
		{
			stream << " -> " << node->Value;
			node = node->Next;
		}
		return stream;
	}
}
