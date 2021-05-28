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
		uint32_t m_NodeCount;
		DoubleLinkedListNode<T>* m_Head;
		DoubleLinkedListNode<T>* m_Tail;

		static bool DefaultComparitor(T& a, T& b) { return a >= b; }

	public:
		// --- CONSTRUCTORS --- //

		DoubleLinkedList() : m_NodeCount(0), m_Head(nullptr), m_Tail(nullptr) { }

		// Copy array into linked list
		DoubleLinkedList(T values[], uint32_t count) : DoubleLinkedList<T>()
		{
			// Add all values as nodes, in order
			for (uint32_t i = 0; i < count; i++)
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
			
			while(m_NodeCount > 0)
				RemoveAt(0);
		}

		// --- GETTERS --- //
		bool IsEmpty() const { return m_Head == nullptr; }
		DoubleLinkedListNode<T>* GetHead() const { return m_Head; }
		DoubleLinkedListNode<T>* GetTail() const { return m_Tail; }
		DoubleLinkedListNode<T>* GetNode(const uint32_t index)
		{
			uint32_t traversed = 0;
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

		uint32_t Size() const { return m_NodeCount; }

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
			currentNode->Previous->Next = newNode;
			currentNode->Previous = newNode;

			m_NodeCount++;
			return newNode;
		}

		// Deletes node from list
		bool RemoveAt(uint32_t index)
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
		bool Remove(DoubleLinkedListNode<T>* node)
		{
			if(!node)
				return false;
			if(node->Previous)
				node->Previous->Next = node->Next;
			if(node->Next)
				node->Next->Previous = node->Previous;

			delete node;
			m_NodeCount--;
			
			return true; // Successfully removed node
		}

		void Sort(std::function<bool(T&, T&)> comparitor = nullptr)
		{
			T** values = new T*[Size()];
			
			auto node = GetHead();
			int nodeIndex = 0;
			while(node)
			{
				values[nodeIndex++] = &node->Value;
				node = node->Next;
			}

			Sorting::QuickSort(values, 0, Size() - 1, comparitor);

			delete[] values;
		}

		// --- OPERATORS --- //
		DoubleLinkedListNode<T>* operator [](uint32_t index) const { return GetNode(index); }
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
