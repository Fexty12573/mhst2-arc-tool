#pragma once

#include <fstream>
#include <vector>

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;
using f32 = float;
using f64 = double;

class file_reader
{
public:
	file_reader(const std::string& path, auto openmode)
		: file(path, openmode), filepath(path)
	{
		file.seekg(0, file.beg);
	}

	template<class T> T read()
	{
		char data[sizeof(T)] = { 0 };
		file.read(data, sizeof(T));

		return *(T*)data;
	}
	template<size_t len> std::string readstr()
	{
		char data[len] = { 0 };
		file.read(data, len);
		return std::string(data);
	}
	template<size_t len> void readstr(char buffer[len])
	{
		file.read(buffer, len);
	}

	template<size_t len> void readbuffer(std::vector<u8>& buffer)
	{
		buffer.resize(len, 0);
		file.read((char*)buffer.data(), len);
	}
	template<size_t len> void readbuffer(u8* buffer)
	{
		file.read((char*)buffer, len);
	}
	void readbuffer(std::vector<u8>& buffer, size_t len);
	void readbuffer(u8* buffer, size_t len);

	std::vector<u8> read_until_end();

	inline size_t getsize()
	{
		size_t cur = file.tellg();
		file.seekg(0, file.end);
		size_t s = file.tellg();
		file.seekg(cur);

		return s;
	}
	inline const std::string& getfilepath() const
	{
		return filepath;
	}

	void reopen(const std::string& path, auto openmode)
	{
		filepath = path;
		file.close();
		file.open(path, openmode);
		file.seekg(0, file.beg);
	}

	std::ifstream& get();

private:
	std::ifstream file;
	std::string filepath;
};
