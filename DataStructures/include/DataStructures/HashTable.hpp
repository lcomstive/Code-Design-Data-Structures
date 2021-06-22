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
#include <vector>
#include <iostream>
#include <functional>

namespace LCDS // Lewis Comstive's Data Structures
{
	// Key-Value table, where each key is stored as a hashed value
	template<typename KeyType, typename T>
	class HashTable
	{
		// List of values and keys
		struct HashBucket
		{
			// Arrays
			T* Values;
			bool* Available;
			unsigned int* Keys; // Hashed keys

			unsigned int Size;

			bool expanded;

			HashBucket() : Values(nullptr), Keys(nullptr), Available(nullptr), Size(0), expanded(false) { }

			~HashBucket()
			{
				if (!Values)
					return;

				// Release memory
				delete[] Keys;
				delete[] Values;
				delete[] Available;

				Keys = nullptr;
				Values = nullptr;
				Available = nullptr;

				Size = 0;
			}

			// Increases size of arrays
			void Expand()
			{
				unsigned int newSize = Size == 0 ? 10 : (Size * 2);

				auto newValues = new T[newSize];
				auto newAvailable = new bool[newSize];
				auto newKeys = new unsigned int[newSize];

				if (Values)
				{	// If existing arrays, copy into new arrays
					memcpy(newValues, Values, sizeof(T) * Size);
					memcpy(newKeys, Keys, sizeof(unsigned int) * Size);
					memcpy(newAvailable, Available, sizeof(bool) * Size);

					// Release memory of old arrays
					delete[] Values;
					delete[] Keys;
					delete[] Available;
				}

				// Fill new portion of arrays with default values
				for (unsigned int i = Size; i < newSize; i++)
				{
					newKeys[i] = UINT_MAX;
					newValues[i] = T{};
					newAvailable[i] = true;
				}

				// Update variables
				Size = newSize;
				expanded = true;

				Keys = newKeys;
				Values = newValues;
				Available = newAvailable;
			}

			// Flags all keys/values as unused
			void Clear()
			{
				for (unsigned int i = 0; i < Size; i++)
					Available[i] = true;
			}

			void Add(unsigned int hash, T value)
			{
				if (!Values)
					Expand();

				// Loop through keys to check availability
				for (unsigned int i = 0; i < Size; i++)
				{
					if (Keys[i] == hash) return; 	// Duplicate key
					if (!Available[i])   continue; 	// Already occupied

					// Found availability
					Keys[i] = hash;
					Values[i] = value;
					Available[i] = false;
					break;
				}
			}

			// Flags index of key/value as available
			bool RemoveAt(unsigned int index)
			{
				if(index > Size)
					return false;
				Available[index] = true;
				return true;
			}

			// Flags key/value as available
			bool Remove(unsigned int hash)
			{
				int index = Find(hash);
				if (index < 0)
					return false; // Hash not found
				Available[index] = true;
				return true;
			}

			// Gets index of hashed key, or -1 if not found
			int Find(unsigned int hash)
			{
				if (!Values)
					return -1; // No keys/values in bucket
				for (unsigned int i = 0; i < Size; i++)
				{
					if (Available[i] || // No data associated
						Keys[i] != hash)
						continue;
					return i;
				}
				return -1;
			}
		};

		unsigned int m_ValueCount = 0;	// Track count of keys/values in table
		unsigned int m_BucketCount = 0;	// Track count of buckets created
		HashBucket** m_Buckets;			// Array of pointers to buckets (when not arrays, destructor caused exceptions?)

		// Function used when hashing keys
		std::function<unsigned int(KeyType&)> m_HashFunc;

		unsigned int DefaultHashFunc(KeyType& input) const
		{
			// Convert input to byte array
			char data[sizeof(KeyType)];
			memcpy(data, &input, sizeof(KeyType));

			unsigned int hash = 0;
			for (size_t i = 0; i < sizeof(KeyType); i++)
				hash += data[i];

			return hash;
		}

		// Convert key to hash
		unsigned int GetHash(KeyType& input) { return m_HashFunc ? m_HashFunc(input) : DefaultHashFunc(input); }

	public:
		static const unsigned int DefaultBucketCount = 25;

		// --- CONSTRUCTORS --- //

		// Default constructor
		HashTable(
			unsigned int bucketCount = DefaultBucketCount,
			std::function<unsigned int(KeyType&)> hashFunc = nullptr)
			: m_BucketCount(bucketCount), m_ValueCount(0)
		{
			m_HashFunc = hashFunc;

			// Create buckets
			// TODO: Expand buckets using function
			m_Buckets = new HashBucket*[m_BucketCount];
			for (unsigned int i = 0; i < m_BucketCount; i++)
				m_Buckets[i] = new HashBucket();
		}

		// Initialise with array of pairs
		HashTable(
			std::vector<std::pair<KeyType, T>> values,
			unsigned int bucketCount = DefaultBucketCount,
			std::function<unsigned int(KeyType&)> hashFunc = nullptr)
			: HashTable(bucketCount, hashFunc) // Call default constructor
		{

			for (auto& pair : values)
				Add(pair.first, pair.second);
		}

		~HashTable()
		{
			if (!m_Buckets)
				return; // Already destroyed

			for (unsigned int i = 0; i < m_BucketCount; i++)
				delete m_Buckets[i];
			delete[] m_Buckets;

			m_ValueCount = 0;
			m_BucketCount = 0;
			m_Buckets = nullptr;
		}

		// --- OPERATIONS --- //

		// Insert pair
		void Add(KeyType& key, T value)
		{
			unsigned int hash = GetHash(key);
			m_Buckets[hash % m_BucketCount]->Add(hash, value);
			m_ValueCount++;
		}

		// Insert pair
		void Add(std::pair<KeyType, T> pair) { Add(pair.first, pair.second); }

		// Remove by key
		void Remove(KeyType& key)
		{
			auto hash = GetHash(key);
			if (m_Buckets[hash % m_BucketCount]->Remove(hash))
				m_ValueCount--;
		}

		// Remove all key/values pairs
		void Clear()
		{
			// Clear all buckets
			for (unsigned int i = 0; i < m_BucketCount; i++)
				m_Buckets[i]->Clear();
			m_ValueCount = 0; // Reset counter
		}

		// Get value by key
		T Find(KeyType key)
		{
			auto hash = GetHash(key);
			auto bucket = m_Buckets[hash % m_BucketCount];
			if (!bucket->Values)
				return T{}; // Bucket is empty

			int index = bucket->Find(hash);
			return index >= 0 ? bucket->Values[index] : T{};
		}

		// Checks if key exists in table
		bool Contains(KeyType key)
		{
			auto hash = GetHash(key);
			auto bucket = m_Buckets[hash % m_BucketCount];
			if (!bucket->Values)
				return false;
			return bucket->Find(hash) >= 0;
		}

		// Prints table to console (for debugging purposes)
		void Print()
		{
			for (unsigned int i = 0; i < m_BucketCount; i++)
			{
				if (!m_Buckets[i]->Values)
					continue;
				std::cout << "Bucket[" << i << "] ";
				for (int j = 0; j < m_Buckets[i]->Size; j++)
					if (!m_Buckets[i]->Available[j])
						std::cout << m_Buckets[i]->Values[j] << " ";
				std::cout << std::endl;

				delete m_Buckets[i];
			}
		}

		// Total amount of pairs stored in table
		unsigned int Size() { return m_ValueCount; }

		const T operator [](const KeyType& key) { return Find(key); }
	};
}