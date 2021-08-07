
#include "file_reader.h"


void file_reader::readbuffer(std::vector<u8>& buffer, size_t len)
{
	buffer.resize(len, 0);
	file.read((char*)buffer.data(), len);
}
void file_reader::readbuffer(u8* buffer, size_t len)
{
	file.read((char*)buffer, len);
}

std::vector<u8> file_reader::read_until_end()
{
	auto cur = file.tellg();
	size_t rest = getsize() - cur;
	std::vector<u8> buffer(rest, 0);

	file.read((char*)buffer.data(), rest);
	return buffer;
}

std::ifstream& file_reader::get() { return file; }
