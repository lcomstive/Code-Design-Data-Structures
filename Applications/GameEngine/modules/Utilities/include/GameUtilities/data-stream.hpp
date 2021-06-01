#pragma once
#include <string>
#include <vector>
#include <exception>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#define _WRITESTREAM(type, streamType) template<> DataStream* write<type>(type t) { \
																		char data[sizeof(type)]; \
																		memcpy(data, &t, sizeof(type)); \
																		_write(streamType, data, sizeof(type)); \
																		return this; }

#define _READSTREAM(type, streamType) template<> type read<type>() { \
																		type t; \
																		memcpy(&t, _read(streamType, sizeof(type)), sizeof(type)); \
																		return t; }

namespace Utilities
{
	class DataStream
	{
	private:
		enum class StreamType
		{
			CHAR = 1,
			INT,
			UINT,
			FLOAT,
			DOUBLE,
			BOOL,
			LONG,
			ULONG,
			LLONG,
			SHORT,
			USHORT,
			STRING,
			CHARARRAY
		};

		static const int _initialStreamSize;

		bool m_Writing;
		unsigned int m_Index, m_Length;
		char* m_Data;

		void _write(StreamType streamType, char* data, unsigned int length);
		char* _read(StreamType streamType, unsigned int length);
		char* _readArray(StreamType streamType, unsigned int* length);

	public:
		DataStream(std::string path);
		DataStream(DataStream* other);
		DataStream(const DataStream& other);
		DataStream(char* data, unsigned int length);
		DataStream(unsigned int initialSize = _initialStreamSize);
		DataStream(std::vector<char> data, unsigned int length = 0);
		~DataStream();

		static DataStream Empty()
		{
			DataStream stream = DataStream((unsigned int)0);
			stream.m_Writing = false;
			return stream;
		}

		bool reading() const { return !m_Writing; }
		bool writing() const { return m_Writing; }
		bool empty() const { return length() == 0; }
		unsigned int length() const { return writing() ? m_Index : m_Length; }
		bool available() const { return writing() ? false : m_Index < (m_Length - 1); }

		void setReading();
		void setWriting();

		std::vector<char> data()
		{
			std::vector<char> d = std::vector<char>();
			getData(&d);
			return d;
		}

		void getData(std::vector<char>* output)
		{
			if (!output)
				return;
			output->resize(length());
			if (length() > 0)
				memcpy(output->data(), m_Data, length());
		}
		void getData(char output[])
		{
			if (output == nullptr)
				output = new char[length()];
			memcpy(output, m_Data, (size_t)length());
		}

		template<typename T> DataStream* write(T t)
		{
#if _WIN32
			throw std::exception("Tried writing invalid type to stream");
#else
			throw "Tried writing invalid type to stream";
#endif
		}

		DataStream* write(char* c, unsigned int length) { _write(StreamType::CHARARRAY, c, length); return this; }

		template<> DataStream* write<std::byte>(std::byte b) { write((char)b); return this; }
		template<> DataStream* write<unsigned char>(unsigned char c) { _write(StreamType::CHAR, (char*)&c, sizeof(unsigned char)); return this; }
		template<> DataStream* write<bool>(bool b) { char c = b ? '1' : '0'; _write(StreamType::BOOL, &c, sizeof(bool)); return this; }
		template<> DataStream* write<char*>(char* c) { _write(StreamType::CHARARRAY, c, (unsigned int)strlen((const char*)c)); return this; }
		template<> DataStream* write<const char*>(const char* c) { _write(StreamType::CHARARRAY, (char*)c, (unsigned int)strlen(c)); return this; }
		template<> DataStream* write<unsigned char*>(unsigned char* c) { _write(StreamType::CHARARRAY, (char*)c, (unsigned int)strlen((const char*)c)); return this; }
		template<> DataStream* write<std::string>(std::string s) { _write(StreamType::STRING, (char*)s.c_str(), (unsigned int)s.size()); return this; }
		// template<> DataStream* write<size_t>(size_t t) { write<unsigned int>((unsigned int)t); return this; }

		_WRITESTREAM(int, StreamType::INT)
		_WRITESTREAM(unsigned int, StreamType::UINT)
		_WRITESTREAM(float, StreamType::FLOAT)
		_WRITESTREAM(double, StreamType::DOUBLE)
		_WRITESTREAM(long, StreamType::LONG)
		_WRITESTREAM(long long, StreamType::LLONG)
		_WRITESTREAM(short, StreamType::SHORT)
		_WRITESTREAM(unsigned short, StreamType::USHORT)
		// _WRITESTREAM(unsigned long, StreamType::ULONG)

#if _WIN32
		template<typename T> T read() { throw std::exception("Tried reading invalid type"); }
		template<typename T> T readArray(unsigned int* length) { throw std::exception("Tried reading invalid type"); }
#else
		template<typename T> T read() { throw "Tried reading invalid type"; }
		template<typename T> T readArray(unsigned int* length) { throw "Tried reading invalid type"; }
#endif

		template<> char* readArray<char*>(unsigned int* length)
		{
			if (length == nullptr) return nullptr;
			return (char*)_readArray(StreamType::CHARARRAY, length);
		}

		template<> const char* readArray<const char*>(unsigned int* length)
		{
			if (length == nullptr) return nullptr;
			return (const char*)_readArray(StreamType::CHARARRAY, length);
		}

		template<> unsigned char* readArray<unsigned char*>(unsigned int* length)
		{
			if (length == nullptr) return nullptr;
			return (unsigned char*)_readArray(StreamType::CHARARRAY, length);
		}

		template<> std::byte read<std::byte>() { return (std::byte)read<unsigned char>(); }
		template<> unsigned char read<unsigned char>() { return _read(StreamType::CHAR, sizeof(unsigned char))[0]; }
		template<> bool read<bool>() { return _read(StreamType::BOOL, sizeof(bool))[0] == (unsigned char)'1'; }
		template<> char* read<char*>() { unsigned int length = 0; return readArray<char*>(&length); }
		template<> const char* read<const char*>() { unsigned int length = 0; return readArray<const char*>(&length); }
		template<> unsigned char* read<unsigned char*>() { unsigned int length = 0; return readArray<unsigned char*>(&length); }
		template<> std::string read<std::string>()
		{
			unsigned int length = 0;
			char* data = _readArray(StreamType::STRING, &length);
			return std::string(data, length);
		}
		// template<> size_t read<size_t>() { return (size_t)read<unsigned int>(); }

		_READSTREAM(int, StreamType::INT)
		_READSTREAM(unsigned int, StreamType::UINT)
		_READSTREAM(float, StreamType::FLOAT)
		_READSTREAM(double, StreamType::DOUBLE)
		_READSTREAM(long, StreamType::LONG)
		_READSTREAM(long long, StreamType::LLONG)
		_READSTREAM(short, StreamType::SHORT)
		_READSTREAM(unsigned short, StreamType::USHORT)
		// _READSTREAM(unsigned long, StreamType::ULONG)
	};
}