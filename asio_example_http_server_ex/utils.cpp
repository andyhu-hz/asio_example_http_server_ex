#include "utils.h"
#include <cctype>


bool timax::iequal(const char* src, size_t src_len, const char* dest, size_t dest_len)
{
	if (src_len != dest_len)
		return false;

	for (size_t i = 0; i < src_len; i++)
	{
		if (std::tolower(src[i]) != std::tolower(dest[i]))
			return false;
	}

	return true;
}
