
#include "unpack.h"
#include <assert.h>

template<class T> void freeMemory(std::vector<T>& t)
{
	std::vector<T>().swap(t);
}

std::string readARCC(file_reader& infile, size_t filecount)
{
	using namespace CryptoPP;

	auto encfile = infile.read_until_end();
	std::vector<u8> decfile(encfile.size(), 0);

	reverse_endianness(encfile);

	ECB_Mode<Blowfish>::Decryption d(key, sizeof(key));
	d.ProcessData(decfile.data(), encfile.data(), encfile.size());

	freeMemory(encfile);

	reverse_endianness(decfile);

	file_writer outfile(infile.getfilepath() + "-decrypt.arc", std::ios::out | std::ios::binary | std::ios::trunc);

	outfile.writecstr(arc0, 4);
	outfile.write<u16>(7);
	outfile.write<u16>(filecount);
	outfile.writebuffer(decfile);

	return outfile.getfilepath();
}

void unpackArchive(const char* inpath)
{
	memory_tracker::reset();
	auto start = std::chrono::steady_clock::now();

	file_reader infile(inpath, std::ios::in | std::ios::binary);
	std::string path(inpath);

	char magic[4] = { 0 };
	infile.readstr<4>(magic);

	if (infile.read<u16>() != 7)
	{
		std::cout << "magic doesn't match. aborting.." << std::endl;
		return;
	}

	u16 filecount = infile.read<u16>();
	if (strncmp(magic, arcc, 4) == 0)
		infile.reopen(readARCC(infile, filecount), std::ios::in | std::ios::binary);
	else if (strncmp(magic, arc0, 4) == 0);
	else return;

	infile.get().seekg(8, std::ios::beg);

	fs::path basepath = path.substr(0, path.find_last_of('.'));
	fs::create_directory(basepath);

	std::ofstream logfile(basepath / "orderlog.txt", std::ios::out | std::ios::trunc);

	std::vector<HeaderEntry> entries;
	entries.resize(filecount);

	for (auto& entry : entries)
	{
		entry = infile.read<HeaderEntry>();
	}
	for (auto& entry : entries)
	{
		infile.get().seekg(entry.offset, std::ios::beg);

		u8* filebuf = new u8[entry.compSize]; // compressed data
		u8* decbuf = new u8[entry.decompSize]; // decompressed data

		if (filebuf == nullptr || decbuf == nullptr)
		{
			std::cout << "new returned nullptr. might've run out of memory. aborting.." << std::endl;
			return;
		}

		infile.readbuffer(filebuf, entry.compSize);

		uLongf size = entry.decompSize;
		uncompress(decbuf, &size, filebuf, entry.compSize);

		delete[] filebuf;

		auto dirpath = basepath / fs::path(entry.name).parent_path();
		if (dirpath != "") fs::create_directories(dirpath);

		std::string relative_path = entry.name + fileExtsReversed[entry.extHash];
		logfile << relative_path << "\n";

		auto final = basepath / relative_path;
		std::ofstream entry_file(final, std::ios::out | std::ios::binary | std::ios::trunc);

		entry_file.write((char*)decbuf, entry.decompSize);
		delete[] decbuf;
	}

	logfile.flush();
	freeMemory(entries);

	auto end = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "Finished unpacking arc in " << diff.count() / 1000.0f << "s" << std::endl;

	auto usage = memory_tracker::get_peak_heap_usage();
	std::cout << "Peak memory usage: ~" << usage / 1000000.0f << "MB (" << usage / 1000000000.0f << "GB)" << std::endl;

	std::cin.get();
}
