#include <string>
#include <iostream>
#include <GameUtilities/file_io.hpp>
#include <GameUtilities/data-stream.hpp>

using namespace std;
using namespace Utilities;

const int DataStream::_initialStreamSize = 1024;

DataStream::DataStream(unsigned int initialSize) : m_Writing(true), m_Index(0), m_Length(initialSize), m_Data(new char[initialSize]) { }
DataStream::DataStream(const DataStream& other) : m_Index(0), m_Writing(false)
{
	m_Length = other.length();
	m_Data = new char[m_Length];
	memcpy(m_Data, other.m_Data, (size_t)m_Length);
}

DataStream::DataStream(DataStream* other) : m_Index(0), m_Writing(false)
{
	m_Length = (unsigned int)other->length();
	m_Data = new char[m_Length];
	memcpy(m_Data, other->m_Data, (size_t)m_Length);
}

DataStream::DataStream(vector<char> data, unsigned int length) : m_Index(0), m_Writing(false)
{
	m_Length = length > 0 ? length : (unsigned int)data.size();
	m_Data = new char[m_Length];
	memcpy(m_Data, data.data(), (size_t)m_Length);
}

DataStream::DataStream(char* data, unsigned int length) : m_Index(0), m_Writing(false)
{
	if (length <= 0)
#if _WIN32
		throw exception("Length is not assigned");
#else
		throw "Length is not assigned";
#endif

	m_Length = length > 0 ? length : (unsigned int)length;
	m_Data = new char[m_Length];
	memcpy(m_Data, data, (size_t)m_Length);
}

DataStream::DataStream(string path) : DataStream(ReadFileBinary(path)) { }

DataStream::~DataStream() { delete[] m_Data; }

void DataStream::setReading()
{
	m_Writing = false;
	m_Length = m_Index;
	m_Index = 0;
}

void DataStream::setWriting()
{
	m_Writing = true;
	m_Index = 0;
}

void DataStream::_write(StreamType type, char* data, unsigned int length)
{
	if (!m_Writing)
	{
#ifndef NDEBUG
		cerr << "Tried to write to DataStream in reading mode" << endl;
#endif
#if _WIN32
		throw exception("Tried to write to DataStream that was in reading mode");
#else
		throw "Tried to write to DataStream that was in reading mode";
#endif
	}

	if ((length + m_Index + 1) > m_Length)
	{
		char* newData = new char[(size_t)m_Length + _initialStreamSize + length];
		memcpy(newData, m_Data, (size_t)m_Length);
		delete[] m_Data;
		m_Data = newData;
		m_Length += _initialStreamSize + length;
	}

	m_Data[m_Index++] = (unsigned char)type;
	if (type == StreamType::STRING || type == StreamType::CHARARRAY)
	{
		memcpy(m_Data + m_Index, &length, sizeof(unsigned int));
		m_Index += sizeof(unsigned int);
	}

	memcpy(m_Data + m_Index, data, length);
	m_Index += length;
}

char* DataStream::_read(StreamType expectedType, unsigned int length) { return _readArray(expectedType, &length); }
char* DataStream::_readArray(StreamType expectedType, unsigned int* length)
{
	if (m_Writing)
	{
#ifndef NDEBUG
		cerr << "Tried reading DataStream while in write mode" << endl;
#endif

#if _WIN32
		throw exception("Tried reading DataStream but is in writing mode");
#else
		throw "Tried reading DataStream but is in writing mode";
#endif
	}

	StreamType type = (StreamType)m_Data[m_Index++];
	if (type != expectedType)
	{
#ifndef NDEBUG
		cerr << "Read wrong type, aborting..." << endl;
#endif

#if _WIN32
		throw exception("Read wrong type, aborting...");
#else
		throw "Read wrong type, aborting...";
#endif
	}

	if (type == StreamType::STRING || type == StreamType::CHARARRAY)
	{
		memcpy(length, m_Data + m_Index, sizeof(unsigned int));
		m_Index += sizeof(unsigned int);
	}

	m_Index += *length;
	return m_Data + m_Index - *length;
}
