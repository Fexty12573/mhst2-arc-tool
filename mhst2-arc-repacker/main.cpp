
#include "repack.h"
#include "unpack.h"


int main(int argc, char** argv)
{
	initFileExts();

	for (int i = 1; i < argc; i++)
	{
		if (fs::is_directory(argv[i]))
		{
			std::string out = std::string(argv[i]) + "-repacked.arc";
			std::cout << "repacking archive " << argv[i] << " to" << fs::path(out).filename() << std::endl;
			repackArchive(argv[i], out.c_str());
		}
		else
		{
			std::cout << "unpacking archive " << argv[i] << " to " << fs::path(argv[i]).parent_path() << std::endl;
			unpackArchive(argv[i]);
		}
	}
}
