
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

#include <zlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/blowfish.h>

#include "memory_tracker.h"
#include "fileexts.h"

extern const Byte key[13]; // QZHaM;-5:)dV#
extern char arcc[4];
extern char arc0[4];

void reverse_endianness(std::vector<Byte>& buffer);

