
#include "common.h"


const Byte key[13] = { 'Q', 'Z', 'H', 'a', 'M', ';', '-', '5', ':', ')', 'd', 'V', '#' }; // QZHaM;-5:)dV#
char arcc[4] = { 'A', 'R', 'C', 'C' };
char arc0[4] = { 'A', 'R', 'C', '\0' };

void reverse_endianness(std::vector<Byte>& buffer)
{
	if (buffer.size() % 4) return;

	for (auto it = buffer.begin(); it != buffer.end(); it += 4)
		std::reverse(it, it + 4);
}
