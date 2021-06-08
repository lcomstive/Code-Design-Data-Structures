#pragma once

namespace LCDS
{
	template<typename T>
	struct BinaryTreeNode
	{
		T Value;
		BinaryTreeNode* Left;
		BinaryTreeNode* Right;

		BinaryTreeNode(
			T& value,
			BinaryTreeNode* left = nullptr,
			BinaryTreeNode* right = nullptr
		) : Value(std::move(value)), Left(left), Right(right) { }
	};

	template<typename T>
	class BinaryTree
	{
		unsigned int m_NodeCount;
		BinaryTreeNode<T>* m_Root;

		BinaryTreeNode<T>* Insert(BinaryTreeNode<T>* current, T& value)
		{
			if (!current)
			{
				m_NodeCount++;
				return new BinaryTreeNode<T>(value);
			}

			if (value < current->Value)
				current->Left = Insert(current->Left, value);
			else if (value > current->Value)
				current->Right = Insert(current->Right, value);

			return current;
		}

		BinaryTreeNode<T>* Search(BinaryTreeNode<T>* current, T& value)
		{
			if (!current) return nullptr;

			if (current->Value == value)
				return current;

			return value < current->Value ?
				Search(current->Left, value) :
				Search(current->Right, value);
		}

		BinaryTreeNode<T>* GetMinValueNode(BinaryTreeNode<T>* current)
		{
			while (current && current->Left)
				current = current->Left;
			return current;
		}

		BinaryTreeNode<T>* Remove(BinaryTreeNode<T>* current, T& value)
		{
			if (!current)
				return nullptr;

			if (current->Value != value)
			{
				if (value < current->Value)
					current->Left = Remove(current->Left, value);
				else
					current->Right = Remove(current->Right, value);
				return current;
			}

			// 'current' is the node to be deleted

			// Node with one or no children
			if (!current->Left || !current->Right) // exclusive or operation (XOR ^)
			{
				auto temp = current->Left ? current->Left : current->Right;
				delete current;
				m_NodeCount--;
				return temp;
			}

			// Node with two children
			auto temp = GetMinValueNode(current->Right);

			current->Value = temp->Value;
			current->Right = Remove(current->Right, current->Value);

			return current;
		}

	public:
		// --- CONSTRUCTORS --- //
		BinaryTree() : m_NodeCount(0), m_Root(nullptr) { }

		BinaryTree(T values[], unsigned int count) : BinaryTree<T>()
		{
			for (unsigned int i = 0; i < count; i++)
				Insert(values[i]);
		}

		~BinaryTree() { Clear(); }

		unsigned int Size() const { return m_NodeCount; }

		// --- GETTERS --- //
		bool IsEmpty() const { return m_Root == nullptr; }
		BinaryTreeNode<T>* GetRoot() const { return m_Root; }

		// --- OPERATIONS --- //
		void Insert(T value)
		{
			if (!m_Root)
			{
				m_NodeCount++;
				m_Root = new BinaryTreeNode<T>(value);
			}
			else
				Insert(m_Root, value); // Recursively insert
		}

		void Remove(T value) { Remove(m_Root, value); }

		bool Contains(T value) { return Search(m_Root, value) != nullptr; }

		BinaryTreeNode<T>* Search(T value) { return Search(m_Root, value); }

		void Clear() { Remove(m_Root, m_Root->Value); }
	};
}