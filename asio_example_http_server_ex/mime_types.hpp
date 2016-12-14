
#pragma once

#include <string>

namespace timax
{
	namespace mime_types
	{
		/// Convert a file extension into a MIME type.
		std::string extension_to_type(const std::string& extension);

	}
}