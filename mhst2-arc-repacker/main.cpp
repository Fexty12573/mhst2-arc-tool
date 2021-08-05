
#include <filesystem>
#include <fstream>
#include <iostream>

#include <cuchar>
#include <vector>
#include <algorithm>

#include <chrono>

#include <zlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/blowfish.h>

#include "fileexts.h"

namespace fs = std::filesystem;

#pragma pack(push, 1)
struct Entry
{
	std::string name;
	unsigned int extHash;
	uLong compSize, decompSize;

	Byte* buffer;
	size_t offset;
	bool buffer_allocated;

	void readBuffer(std::ifstream& i, size_t size)
	{
		compSize = compressBound(size);
		decompSize = size;

		Byte* temp = new Byte[decompSize];
		buffer = new Byte[compSize];
		memset(buffer, 0, compSize);

		if (temp != nullptr && buffer != nullptr)
		{
			buffer_allocated = true;

			i.read((char*)temp, decompSize);

			int result = compress(buffer, &compSize, temp, decompSize);
			if (result != 0) // Z_OK
			{
				delete[] buffer;
				buffer = nullptr;
				std::cout << "could not compress " << name << " (" << result << ")" << std::endl;
			}

			while (compSize % 8) compSize++;

			delete[] temp;
			temp = nullptr;
		}
		else
		{
			if (buffer) delete[] buffer;
			if (temp) delete[] temp;

			buffer_allocated = false;
			buffer = nullptr;
			temp = nullptr;

			return;
		}
	}
	Entry()
		: name(), extHash(0),
		compSize(0), decompSize(0),
		buffer(nullptr), offset(0),
		buffer_allocated(false)
	{
	}
	~Entry()
	{
		if (buffer != nullptr && buffer_allocated)
		{
			delete[] buffer;
		}
	}
};

struct LogEntry
{
	std::string path;
	int idx;
};
#pragma pack(pop)

void padBufferToECBBlockSize(std::vector<Byte>& buffer)
{
	using namespace CryptoPP;
	while (buffer.size() % 8) buffer.push_back(0UI8);
}
unsigned int getExtensionHash(const std::string& ext)
{
	try {
		unsigned int hash = fileExts.at(ext);
		return hash;
	}
	catch (const std::out_of_range& e)
	{
		return strtoul((std::string("0x") + &ext[1]).c_str(), nullptr, 16);
	}
}
size_t getFileSize(std::ifstream& f)
{
	f.seekg(0, f.end);
	size_t s = f.tellg();
	f.seekg(0, f.beg);

	return s;
}
void exportString(std::vector<Byte>& buffer, const std::string& str)
{
	size_t nullsize = 0x80 - str.size();
	
	buffer.insert(buffer.end(), str.begin(), str.end());
	buffer.insert(buffer.end(), nullsize, 0UI8);
}
void extendBufferToOffset(std::vector<Byte>& buffer, size_t offset)
{
	if (buffer.size() < offset)
		buffer.insert(buffer.end(), offset - buffer.size(), 0UI8);
}
void appendU32ToBuffer(std::vector<Byte>& buffer, uint32_t val)
{
	for (Byte i = 0; i < 4; i++)
		buffer.push_back((Byte)(((Byte*)&val)[i]));
}
void reverse_endianness(std::vector<Byte>& buffer)
{
	if (buffer.size() % 4) return;

	for (auto it = buffer.begin(); it != buffer.end(); it += 4)
		std::reverse(it, it + 4);
}

void repackArchive(const char* importPath, const char* outpath)
{
	auto start = std::chrono::steady_clock::now();

	const Byte key[13] = { 'Q', 'Z', 'H', 'a', 'M', ';', '-', '5', ':', ')', 'd', 'V', '#' }; // QZHaM;-5:)dV#
	auto freeMemory = []<class T>(T& t) -> void { T().swap(t); };

	if (!fs::exists(fs::path(importPath) / "orderlog.txt"))
	{
		std::cout << "orderlog does not exist!" << std::endl;
		return;
	}

	std::ofstream outfile(outpath, std::ios::out | std::ios::trunc | std::ios::binary);
	std::ifstream logfile(fs::path(importPath) / "orderlog.txt");
	std::vector<LogEntry> logentries;

	int i = 0;
	std::string line;

	while (std::getline(logfile, line))
	{
		logentries.push_back({ line, i });
		i++;
	}

	line.clear();
	logfile.close();

	uint16_t filecount = logentries.size();

	char arcc[4] = { 'A', 'R', 'C', 'C' };
	uint16_t magic = 7;
	outfile.write(arcc, 4);

	outfile.write((const char*)&magic, 2);
	outfile.write((const char*)&filecount, 2);
	outfile.seekp(0, outfile.end);

	std::vector<Byte> filedec;
	std::vector<Entry> entries;

	entries.resize(logentries.size());

	i = 0;
	for (const auto& logentry : logentries)
	{
		Entry& e = entries[i];

		size_t off = logentry.path.find_last_of('.');

		e.name = logentry.path.substr(0, off);
		e.extHash = getExtensionHash(logentry.path.substr(off));
		
		std::ifstream file(fs::path(importPath) / logentry.path, std::ios::in | std::ios::binary);
		e.decompSize = getFileSize(file);

		e.readBuffer(file, e.decompSize);
		i++;
	}

	logentries.clear();

	freeMemory(logentries);

	size_t currentOff = (8 + (filecount * 0x90)) + (32768 - ((8 + (filecount * 0x90)) % 32768));
	size_t startingOff = currentOff;

	i = 0;
	for (auto& entry : entries)
	{
		exportString(filedec, entry.name);

		appendU32ToBuffer(filedec, entry.extHash);
		appendU32ToBuffer(filedec, entry.compSize);
		appendU32ToBuffer(filedec, entry.decompSize + 0x40000000);
		appendU32ToBuffer(filedec, currentOff);

		entry.offset = currentOff;
		currentOff += entry.compSize;
	}

	extendBufferToOffset(filedec, startingOff - 8);

	for (auto it = entries.begin(); it != entries.end(); it++)
	{
		filedec.insert(filedec.end(), it->buffer, it->buffer + it->compSize);
	}

	if (!entries.empty()) entries.clear();
	freeMemory(entries);

	padBufferToECBBlockSize(filedec);

	using namespace CryptoPP;

	std::vector<Byte> fileenc;
	fileenc.resize(filedec.size());

	ECB_Mode<Blowfish>::Encryption e = ECB_Mode<Blowfish>::Encryption(key, sizeof(key));
	reverse_endianness(filedec);

	e.ProcessData(fileenc.data(), filedec.data(), filedec.size());
	freeMemory(filedec);

	reverse_endianness(fileenc);
	outfile.write((const char*)fileenc.data(), fileenc.size());
	outfile.flush();

	auto end = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Finished repacking in: " << (float)diff.count() / 1000.0f << "s" << std::endl;

	freeMemory(fileenc);

	system("pause");
}


int main(int argc, char** argv)
{
	initFileExts();

	for (int i = 1; i < argc; i++)
	{
		std::string out = std::string(argv[i]) + fs::path(argv[i]).filename().string() + "-new.arc";
		std::cout << "repacking archive " << argv[i] << " to " << out << std::endl;
		repackArchive(argv[i], out.c_str());
	}
}
