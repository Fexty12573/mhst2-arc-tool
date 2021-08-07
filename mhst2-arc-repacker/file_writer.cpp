
#include "file_writer.h"


void file_writer::writestr(const std::string& str)
{
	outfile.write(str.c_str(), str.size());
}
void file_writer::writecstr(const char* str, size_t len)
{
	outfile.write(str, len);
}
void file_writer::writebuffer(u8* buffer, size_t len)
{
	outfile.write((const char*)buffer, len);
}
void file_writer::writebuffer(const std::vector<u8>& buffer)
{
	outfile.write((const char*)buffer.data(), buffer.size());
}

std::string file_writer::getfilepath() const
{
	return filepath;
}

void file_writer::reopen(const std::string& path)
{
	filepath = path;
	outfile.close();
	outfile.open(path);
	outfile.seekp(0, outfile.beg);
}

std::ofstream& file_writer::get() { return outfile; }
