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

class file_writer
{
public:
	file_writer(const std::string& path, auto openmode)
		: outfile(path, openmode), filepath(path)
	{
		outfile.seekp(0, outfile.beg);
	}

	template<class T> void write(const T& t)
	{
		outfile.write((const char*)&t, sizeof(T));
	}
	void writestr(const std::string& str);
	void writecstr(const char* str, size_t len);
	void writebuffer(u8* buffer, size_t len);
	void writebuffer(const std::vector<u8>& buffer);

	std::string getfilepath() const;
	void reopen(const std::string& path);
	
	std::ofstream& get();

private:
	std::ofstream outfile;
	std::string filepath;
};
