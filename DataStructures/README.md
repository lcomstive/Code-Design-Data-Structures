# Lewis Comstive's Data Structures Library
*AIE Code Design & Data Structures Unit 2021*

## About
A header-only library that replicates select C++ standard-template-library structures.

| Structure | Description |
|----------:|:------------|
| Binary Tree 		 | Tree structure where each leaf-node has a maximum of two child nodes 		  |
| Linked List 		 | List where each node has a reference to the next node in the list			  |
| Double Linked List | List where each node has a reference to the previous and next node in the list |
| Hash Table  		 | Table of key-value pairs, where each key is hashed 							  |

## Usage

> Note: All library structures use templates
### Hash Table
When creating a hashtable, providing a hashing function is recommended.

The `bucketCount` parameter in constructors should be large enough to store the expected amount of key-value pairs.

Constructors:
```cpp
// Default constructor
LCDS::HashTable<KeyType, ValueType>(
	unsigned int bucketCount,
	std::function<unsigned int(KeyType&)> hashFunction
);

// Insert initial vector of key-pair values
LCDS::HashTable<KeyType, ValueType>(
	std::vector<std::pair<KeyType, ValueType>> initialValues,
	unsigned int bucketCount,
	std::function<unsigned int(KeyType&)> hashFunction
);
```

#### Example Usage
```cpp
// Function used when hashing HashTable key
unsigned int HashFunc(std::string& key)
{
	// Generate unsigned int based on key
	unsigned int hash = 0;
	for(size_t i = 0; i < key.size(); i++)
		hash += key[i];
	return hash;
}

unsigned int bucketCount = 10; // Should be large enough to store expected amount of key-value pairs
LCDS::HashTable<std::string, int> hashTable = LCDS::HashTable<std::string, int>(bucketCount, HashFunc);
```


## LICENSE

See the LICENSE file in the root directory of project for copyright information.
