#pragma once
#include <functional>

namespace LCDS
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
	template<typename T>
	class BinaryTree
	{
		unsigned int m_NodeCount;
		BinaryTreeNode<T>* m_Root;

		std::function<int(T& a, T& b)> m_ComparisonFunc = [](T& a, T& b) { return a - b; };

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

		// Returns the deepest, left-most node of the input node
		BinaryTreeNode<T>* GetMinValueNode(BinaryTreeNode<T>* node)
		{
			while (node && node->Left)
				node = node->Left;
			return node;
		}

		// Returns the deepest, right-most node of the input node
		BinaryTreeNode<T>* GetMaxValueNode(BinaryTreeNode<T>* node)
		{
			while (node && node->Right)
				node = node->Right;
			return node;
		}

		// Returns how deep the tree goes from this node
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
		// Insert the value under the input node.
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
			{
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

		// Searches for value in entire tree, then removes it
		void Remove(T value) { m_Root = Remove(m_Root, value); }
#pragma endregion

#pragma region Search
		// Search under input node, using selector to find value
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

		// Searches through input node for value
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

		// Searches through entire tree for value, using selector
		BinaryTreeNode<T>* Search(std::function<bool(T&)> selector) { return Search(m_Root, selector); }

		// Searches through entire tree for value
		BinaryTreeNode<T>* Search(T value) { return Search(m_Root, value); }

		// Searches for parent of value node, using selector
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
		BinaryTreeNode<T>* SearchForParent(std::function<bool(T&)> selector) { return SearchForParent(m_Root, selector); }

		// Search for parent of value node
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

		BinaryTreeNode<T>* SearchForParent(BinaryTreeNode<T>* node, BinaryTreeNode<T>* value) { return SearchForParent(node, value->Value); }
#pragma endregion

		bool Contains(T value) { return Search(m_Root, value) != nullptr; }

		void Clear()
		{
			while (m_Root)
				m_Root = Remove(m_Root, m_Root->Value);
		}

		void SetComparisonFunction(std::function<int(T&, T&)> comparisonFunc) { m_ComparisonFunc = comparisonFunc; }
	};
}