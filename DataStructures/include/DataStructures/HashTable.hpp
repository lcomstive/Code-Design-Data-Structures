#pragma once
#include <vector>
#include <iostream>
#include <functional>

namespace LCDS
{
	template<typename KeyType, typename T>
	class HashTable
	{
		struct HashBucket
		{
			T* Values;
			bool* Available;
			unsigned int* Keys; // Hashes
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

			void Expand()
			{
				unsigned int newSize = Size == 0 ? 10 : (Size * 2);

				auto newValues = new T[newSize];
				auto newAvailable = new bool[newSize];
				auto newKeys = new unsigned int[newSize];

				if (Values)
				{
					memcpy(newValues, Values, sizeof(T) * Size);
					memcpy(newKeys, Keys, sizeof(unsigned int) * Size);
					memcpy(newAvailable, Available, sizeof(bool) * Size);

					// Release memory
					delete[] Values;
					delete[] Keys;
					delete[] Available;
				}

				for (unsigned int i = Size; i < newSize; i++)
				{
					newKeys[i] = UINT_MAX;
					newValues[i] = T{};
					newAvailable[i] = true;
				}

				expanded = true;

				Size = newSize;

				Keys = newKeys;
				Values = newValues;
				Available = newAvailable;
			}

			void Clear()
			{
				for (unsigned int i = 0; i < Size; i++)
					Available[i] = true;
			}

			void Add(unsigned int hash, T value)
			{
				if (!Values)
					Expand();

				for (unsigned int i = 0; i < Size; i++)
				{
					if (Keys[i] == hash) return; // Duplicate
					if (!Available[i])   continue; // Already occupied

					Keys[i] = hash;
					Values[i] = value;
					Available[i] = false;
					break;
				}
			}

			bool Remove(unsigned int hash)
			{
				int index = Find(hash);
				if (index < 0)
					return false;
				Available[index] = true;
				return true;
			}

			int Find(unsigned int hash)
			{
				if (!Values)
					return -1;
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

		unsigned int m_ValueCount = 0;
		unsigned int m_BucketCount = 0;
		HashBucket** m_Buckets;

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

		unsigned int GetHash(KeyType& input) { return m_HashFunc ? m_HashFunc(input) : DefaultHashFunc(input); }

	public:
		static const unsigned int DefaultBucketCount = 25;

		// --- CONSTRUCTORS --- //
		HashTable(
			unsigned int bucketCount = DefaultBucketCount,
			std::function<unsigned int(KeyType&)> hashFunc = nullptr)
			: m_BucketCount(bucketCount), m_ValueCount(0)
		{
			m_HashFunc = hashFunc;

			m_Buckets = new HashBucket*[m_BucketCount];
			for (unsigned int i = 0; i < m_BucketCount; i++)
				m_Buckets[i] = new HashBucket();
		}

		HashTable(
			std::vector<std::pair<KeyType, T>> values,
			unsigned int bucketCount = DefaultBucketCount,
			std::function<unsigned int(KeyType&)> hashFunc = nullptr)
			: HashTable(bucketCount, hashFunc)
		{

			for (auto& pair : values)
				Add(pair.first, pair.second);
		}

		~HashTable()
		{
			if (!m_Buckets)
				return; // Already destroyed

			for (int i = 0; i < m_BucketCount; i++)
				delete m_Buckets[i];
			delete[] m_Buckets;

			m_ValueCount = 0;
			m_BucketCount = 0;
			m_Buckets = nullptr;
		}

		// --- OPERATIONS --- //
		void Add(KeyType& key, T value)
		{
			unsigned int hash = GetHash(key);
			m_Buckets[hash % m_BucketCount]->Add(hash, value);
			m_ValueCount++;
		}
		
		void Add(std::pair<KeyType, T> pair) { Add(pair.first, pair.second); }

		void Remove(KeyType& value)
		{
			auto hash = GetHash(value);
			if (m_Buckets[hash % m_BucketCount]->Remove(hash))
				m_ValueCount--;
		}

		void Clear()
		{
			for (unsigned int i = 0; i < m_BucketCount; i++)
				m_Buckets[i]->Clear();
			m_ValueCount = 0;
		}

		T Find(KeyType key)
		{
			auto hash = GetHash(key);
			auto bucket = m_Buckets[hash % m_BucketCount];
			if (!bucket->Values)
				return T{};

			int index = bucket->Find(hash);
			return index >= 0 ? bucket->Values[index] : T{};
		}

		bool Contains(KeyType key)
		{
			auto hash = GetHash(key);
			auto bucket = m_Buckets[hash % m_BucketCount];
			if (!bucket->Values)
				return false;
			return bucket->Find(hash) >= 0;
		}

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

		unsigned int Size() { return m_ValueCount; }

		const T operator [](const KeyType& key) { return Find(key); }
	};
}