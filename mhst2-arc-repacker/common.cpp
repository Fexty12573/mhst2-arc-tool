
#include "common.h"
#include <stdlib.h>

const Byte key[13] = { 'Q', 'Z', 'H', 'a', 'M', ';', '-', '5', ':', ')', 'd', 'V', '#' }; // QZHaM;-5:)dV#
char arcc[4] = { 'A', 'R', 'C', 'C' };
char arc0[4] = { 'A', 'R', 'C', '\0' };

void reverse_endianness(std::vector<Byte>& buffer)
{
	size_t sz = buffer.size();
	if (sz % 4) return;

	size_t sz_4ths = sz / 4;
	uint32_t* data = (uint32_t*)buffer.data();
	
	for (size_t i = 0; i < sz_4ths; i++)
		data[i] = _byteswap_ulong(data[i]);
}
