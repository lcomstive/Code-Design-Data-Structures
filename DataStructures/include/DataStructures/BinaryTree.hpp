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
#include <functional>

namespace LCDS // Lewis Comstive's Data Structures
{
	// --- BINARY TREE NODE --- //
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

	// --- BINARY TREE --- //

	// Tree structure with each leaf node having a maximum of two child nodes
	template<typename T>
	class BinaryTree
	{
		unsigned int m_NodeCount;  // Keep track of created nodes
		BinaryTreeNode<T>* m_Root; // Always keep reference to first created node, all other nodes can be reached from here

		// When no function is given for comparing values for sorting, this one is used
		std::function<int(T& a, T& b)> m_ComparisonFunc = [](T& a, T& b) { return a - b; };

	public:
		// --- CONSTRUCTORS --- //

		// Default constructor
		BinaryTree() : m_NodeCount(0), m_Root(nullptr) { }

		// Initialise by array
		BinaryTree(T values[], unsigned int count) : BinaryTree<T>()
		{
			for (unsigned int i = 0; i < count; i++)
				Insert(values[i]);
		}

		~BinaryTree() { Clear(); }

		// --- GETTERS --- //

		bool IsEmpty() const { return Size() == 0; }
		unsigned int Size() const { return m_NodeCount; }

		// The top-most node
		BinaryTreeNode<T>* GetRoot() const { return m_Root; }

		// The deepest, left-most node of the input node (minimum value)
		BinaryTreeNode<T>* GetMinValueNode(BinaryTreeNode<T>* node)
		{
			while (node && node->Left)
				node = node->Left;
			return node;
		}

		// The deepest, right-most node of the input node (maximum value)
		BinaryTreeNode<T>* GetMaxValueNode(BinaryTreeNode<T>* node)
		{
			while (node && node->Right)
				node = node->Right;
			return node;
		}

		// How deep the tree goes from certain node
		int GetDepth(BinaryTreeNode<T>* node)
		{
			if (!node)
				return 0;
			int leftDepth = GetDepth(node->Left);
			int rightDepth = GetDepth(node->Right);
			if (leftDepth > rightDepth)
				return leftDepth + 1;
			return rightDepth + 1;
		}
		int GetDepth() { return GetDepth(m_Root); }

		// --- OPERATIONS --- //

#pragma region Insert
		// Insert the value underneath the input node.
		// Returns the input node (for recursive insertion), or a new node if node is null
		BinaryTreeNode<T>* Insert(BinaryTreeNode<T>* node, T& value)
		{
			if (!node)
			{
				m_NodeCount++;
				return new BinaryTreeNode<T>(value);
			}

			// Compare node value to insertion value
			int comparison = m_ComparisonFunc(value, node->Value);

			// Insert value either into appropriate child of tree
			if (comparison < 0)
				node->Left = Insert(node->Left, value);
			else if (comparison > 0)
				node->Right = Insert(node->Right, value);

			return node;
		}

		// Value is inserted relative to root node
		void Insert(T value)
		{
			if (!m_Root)
			{   // No values in tree, create one
				m_NodeCount++;
				m_Root = new BinaryTreeNode<T>(value);
			}
			else
				Insert(m_Root, value); // Recursively insert
		}
#pragma endregion

#pragma region Remove
		// Deletes the node and shifts appropriate nodes to fill.
		// Returns shifted node, or nullptr if node is nullptr
		BinaryTreeNode<T>* Remove(BinaryTreeNode<T>* node)
		{
			if (!node)
				return nullptr;

			// One or no children
			if (!node->Left || !node->Right)
			{
				auto parent = SearchForParent(m_Root, node);
				auto temp = node->Left ? node->Left : node->Right;

				if (parent)
				{
					int parentComparison = m_ComparisonFunc(parent->Value, node->Value);
					if (parentComparison < 0) // Parent is less
						parent->Right = temp;
					else
						parent->Left = temp;
				}

				delete node;
				m_NodeCount--;

				return temp;
			}

			auto temp = GetMinValueNode(node->Right);

			node->Value = temp->Value;
			node->Right = Remove(node->Right);

			return node;
		}

		// Searches for value under input node, then removes associated node.
		// Returns shifted node, or nullptr if node is nullptr
		BinaryTreeNode<T>* Remove(BinaryTreeNode<T>* current, T& value)
		{
			if (!current)
				return nullptr;
			return Remove(Search(current, value));
		}

		// Searches for value in entire tree and removes it
		void Remove(T value) { m_Root = Remove(m_Root, value); }
#pragma endregion

#pragma region Search
		// Recursively search under input node, using selector to find value
		BinaryTreeNode<T>* Search(BinaryTreeNode<T>* node, std::function<bool(T&)> selector)
		{
			if (!node)
				return nullptr;

			if (selector(node->Value))
				return node;

			// Check left and right branches
			auto temp = Search(node->Left, selector);
			if (!temp)
				temp = Search(node->Right, selector);
			return temp;
		}

		// Recursively searches through input node for value
		BinaryTreeNode<T>* Search(BinaryTreeNode<T>* node, T& value)
		{
			if (!node) return nullptr;

			int comparison = m_ComparisonFunc(node->Value, value);
			if (comparison == 0) // Same value
				return node;

			return comparison > 0 ?
				Search(node->Left, value) :
				Search(node->Right, value);
		}

		// Recursively searches through entire tree for value, using selector
		BinaryTreeNode<T>* Search(std::function<bool(T&)> selector) { return Search(m_Root, selector); }

		// Recursively searches through entire tree for value
		BinaryTreeNode<T>* Search(T value) { return Search(m_Root, value); }

		// Recursively searches for parent of value node, using selector
		// TODO: Test parent search
		BinaryTreeNode<T>* SearchForParent(BinaryTreeNode<T>* node, std::function<bool(T&)> selector)
		{
			if (!node)
				return nullptr;

			if (selector(node->Left->Value) || selector(node->Right->Value))
				return node;

			auto temp = SearchForParent(node->Left, selector);
			if (!temp)
				temp = SearchForParent(node->Right, selector);
			return temp;
		}
		// Recursively searches for parent of value, using selector
		BinaryTreeNode<T>* SearchForParent(std::function<bool(T&)> selector) { return SearchForParent(m_Root, selector); }

		// Recursively searches for parent of value
		BinaryTreeNode<T>* SearchForParent(BinaryTreeNode<T>* node, T& value)
		{
			if (!node)
				return nullptr;

			if ((node->Left && m_ComparisonFunc(node->Left->Value, value) == 0) ||
				(node->Right && m_ComparisonFunc(node->Right->Value, value) == 0))
				return node;

			return m_ComparisonFunc(value, node->Value) < 0 ?
				SearchForParent(node->Left, value) :
				SearchForParent(node->Right, value);
		}

		// Recursively searches for parent of value, relative to node
		BinaryTreeNode<T>* SearchForParent(BinaryTreeNode<T>* node, BinaryTreeNode<T>* value) { return SearchForParent(node, value->Value); }
#pragma endregion

		// Checks if value exists in tree
		bool Contains(T value) { return Search(m_Root, value) != nullptr; }

		// Removes all nodes, including root
		void Clear()
		{
			while (m_Root)
				m_Root = Remove(m_Root, m_Root->Value);
		}

		void SetComparisonFunction(std::function<int(T&, T&)> comparisonFunc) { m_ComparisonFunc = comparisonFunc; }
	};
}