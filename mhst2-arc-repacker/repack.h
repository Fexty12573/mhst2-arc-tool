
#include "common.h"

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

void padBufferToECBBlockSize(std::vector<Byte>& buffer);
unsigned int getExtensionHash(const std::string& ext);
size_t getFileSize(std::ifstream& f);
void exportString(std::vector<Byte>& buffer, const std::string& str);
void extendBufferToOffset(std::vector<Byte>& buffer, size_t offset);
void appendU32ToBuffer(std::vector<Byte>& buffer, uint32_t val);

void repackArchive(const char* importPath, const char* outpath);
