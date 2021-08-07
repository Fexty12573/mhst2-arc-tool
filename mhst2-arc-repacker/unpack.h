
#include "common.h"
#include "file_reader.h"
#include "file_writer.h"

namespace fs = std::filesystem;

#pragma pack(push, 1)
struct HeaderEntry
{
	char name[128];
	unsigned int extHash;
	unsigned int compSize;
	unsigned int decompSize : 24;
	unsigned int unkn : 8;
	unsigned int offset;
};
#pragma pack(pop)

std::string readARCC(file_reader& infile, size_t filecount);
void unpackArchive(const char* inpath);
