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
		unsigned int m_NodeCount;		 // Keep track of nodes created & deleted
		DoubleLinkedListNode<T>* m_Head; // Always points to first node
		DoubleLinkedListNode<T>* m_Tail; // Always points to last node

		// When no function is given for comparing values for sorting, this one is used
		static bool DefaultComparitor(T& leftElement, T& rightElement) { return leftElement >= rightElement; }

	public:
		// --- CONSTRUCTORS --- //

		// Default
		DoubleLinkedList() : m_NodeCount(0), m_Head(nullptr), m_Tail(nullptr) { }

		// Copy array into linked list
		DoubleLinkedList(T values[], unsigned int count) : DoubleLinkedList<T>()
		{
			// Add all values as nodes, in order
			for (unsigned int i = 0; i < count; i++)
				Add(values[i]);
		}

		// Copy constructor
		// TODO: Test this
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
				return; // No nodes to delete
			Clear(); // Remove & delete all nodes
		}

		// --- GETTERS --- //

		unsigned int Size() const { return m_NodeCount; }
		bool IsEmpty() const { return Size() == 0; }

		// Get first node
		DoubleLinkedListNode<T>* GetHead() const { return m_Head; }
		// Get last node
		DoubleLinkedListNode<T>* GetTail() const { return m_Tail; }

		// Get node at index location
		DoubleLinkedListNode<T>* GetNode(const unsigned int index)
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

		// --- NODE OPERATIONS --- //

		// Append value at end of list
		DoubleLinkedListNode<T>* Add(const T& value)
		{
			if (m_Head == nullptr)
			{	// No other nodes in list
				m_Head = m_Tail = new DoubleLinkedListNode<T>(value);
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
				return Add(value); // Sets head, tail and increments count

			// Create node, set as head
			auto node = new DoubleLinkedListNode<T>(value, /* Previous: */ nullptr, /* Next: */ m_Head);
			m_Head->Previous = node;
			m_Head = node;
			m_NodeCount++;
			return m_Head;
		}

		// Inserts value at index
		DoubleLinkedListNode<T>* Insert(const T& value, unsigned int index)
		{
			auto currentNode = GetNode(index);
			if (currentNode == nullptr)
				return Add(value); // Index exceeds length of list, add to end

			auto newNode = new DoubleLinkedListNode<T>(value, /* Previous: */ currentNode->Previous, /* Next: */ currentNode);

			// Inform surrounding nodes of insertion
			if(currentNode->Previous)
				currentNode->Previous->Next = newNode;
			currentNode->Previous = newNode;

			if (index == 0) // Insertion was at start of list
				m_Head = newNode;

			m_NodeCount++;
			return newNode;
		}

		// Shifts node at index position by input amount
		bool Move(unsigned int index, int amount)
		{
			if (amount == 0)
				return true; // No movement amount

			// 'Right' in this case is towards end of the list
			bool shouldShiftRight = amount < 0;
			if (amount < 0)
				amount = -amount; // Ensure amount is made positive for decrementing in below while loop

			bool success = true; // Keep track of success
			while (amount > 0 && success) // Stop if error occurs or desired number of shifts achieved
			{
				success = Shift(index, shouldShiftRight); // Modify list

				amount--;
				index += shouldShiftRight ? -1 : 1; // Move index same direction as element
			}
			return success;
		}

		// Shifts an element at index
		bool Shift(unsigned int index, bool shouldShiftRight = true)
		{
			unsigned int newIndex = index + (shouldShiftRight ? -1 : 1);
			auto leftElement  = GetNode(index > newIndex ? newIndex : index); // Get right-most node
			auto rightElement = GetNode(index > newIndex ? index : newIndex); // Get left-most node

			if (!leftElement || !rightElement)
				return false; // Index out of range

			// Ensure heads and tails are corrected
			if (leftElement == m_Head)
				m_Head = rightElement;
			if (rightElement == m_Tail)
				m_Tail = leftElement;

			// Inform previous and next nodes of the swap
			if (leftElement->Previous)
				leftElement->Previous->Next = rightElement;
			if (rightElement->Next)
				rightElement->Next->Previous = rightElement->Previous;

			// Le olde swapsies
			leftElement->Next = rightElement->Next;
			rightElement->Previous = leftElement->Previous;
			leftElement->Previous = rightElement;
			rightElement->Next = leftElement;

			return true; // Success
		}

		// Deletes node from list
		bool RemoveAt(unsigned int index)
		{
			auto node = GetNode(index);
			if (node == nullptr)
				return false; // Failed to find node

			return Remove(node);
		}

		// Deletes node from list
		bool Remove(DoubleLinkedListNode<T>* node)
		{
			if (!node) // Invalid input
				return false;

			// Link surrounding nodes
			if (node->Previous)
				node->Previous->Next = node->Next;
			if(node->Next)
				node->Next->Previous = node->Previous;

			// Check for removing node that is start or end of list
			if(node == m_Head)
				m_Head = node->Next;
			if(node == m_Tail)
				m_Tail = node->Previous;

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

		// Finds the first item using a function to determine if value is sought after.
		DoubleLinkedListNode<T>* Find(std::function<bool(T&)> selector)
		{
			// Loop through entire list, testing selector on each
			auto node = GetHead();
			while (node)
			{
				if (selector(node->Value))
					return node;
				node = node->Next;
			}
			return nullptr; // None found
		}

		// Finds the location of the first value that the selector function returns true
		int IndexOf(std::function<bool(T&)> selector)
		{
			auto node = GetHead();
			int index = 0;

			// Loop through entire list, testing selector on each
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
		DoubleLinkedListNode<T>* operator [](unsigned int index) const { return GetNode(index); }
	};

	// Stream operator, for when printing entire list to console
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
