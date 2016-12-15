
#pragma once

#include "picohttpparser.h"

#include <boost/utility/string_ref.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

namespace timax
{
	bool inline iequal(const char* src, size_t src_len, const char* dest, size_t dest_len)
	{
		if (src_len != dest_len)
			return false;

		for (size_t i = 0; i < src_len; i++)
		{
			if (std::tolower(src[i]) != dest[i])
				return false;
		}

		return true;
	}

	class request
	{
	public:
		int parse(const char* data, std::size_t len, std::size_t last_len)
		{
			num_headers_ = sizeof(headers_) / sizeof(headers_[0]);
			return phr_parse_request(data, len, &method_,
				&method_len_, &path_, &path_len_,
				&minor_version_, headers_, &num_headers_, last_len);
		}


		boost::string_ref method() const
		{
			return boost::string_ref(method_, method_len_);
		}

		boost::string_ref path() const
		{
			return boost::string_ref(path_, path_len_);
		}

		bool is_http1_0() const
		{
			return minor_version_ == 0;
		}

		bool is_http1_1() const
		{
			return minor_version_ == 1;
		}

		boost::string_ref get_header(const char* name, size_t size) const
		{
			auto it = std::find_if(headers_, headers_ + num_headers_, [this, name, size](struct phr_header const& hdr)
			{
				return iequal(hdr.name, hdr.name_len, name, size);
			});

			if (it == headers_ + num_headers_)
			{
				return{};
			}

			return boost::string_ref(it->value, it->value_len);
		}

		std::vector<boost::string_ref> get_headers(const char* name, size_t size) const
		{
			std::vector<boost::string_ref> headers;
			for (std::size_t i = 0; i < num_headers_; ++i)
			{
				if (iequal(headers_[i].name, headers_[i].name_len, name, size))
				{
					headers.emplace_back(boost::string_ref(headers_[i].value, headers_[i].value_len));
				}
			}

			return headers;
		}

		struct header_t
		{
			boost::string_ref name;
			boost::string_ref value;
		};
		std::vector<header_t> get_headers() const
		{
			std::vector<header_t> headers;
			for (std::size_t i = 0; i < num_headers_; ++i)
			{
				headers.emplace_back(header_t
				{
					boost::string_ref(headers_[i].name, headers_[i].name_len),
					boost::string_ref(headers_[i].value, headers_[i].value_len)
				});
			}

			return headers;
		}

		bool has_header(const char* name, size_t size) const
		{
			auto it = std::find_if(headers_, headers_ + num_headers_, [name, size](struct phr_header const& hdr)
			{
				return iequal(hdr.name, hdr.name_len, name, size);
			});

			return it != headers_ + num_headers_;
		}

		std::size_t headers_num(const char* name, size_t size) const
		{
			std::size_t num = 0;
			for (std::size_t i = 0; i < num_headers_; ++i)
			{
				if (iequal(headers_[i].name, headers_[i].name_len, name, size))
				{
					++num;
				}
			}

			return num;
		}

		std::size_t headers_num() const
		{
			return num_headers_;
		}

		boost::string_ref get_header_cs(std::string const& name) const
		{
			auto it = std::find_if(headers_, headers_ + num_headers_, [&name](struct phr_header const& hdr)
			{
				return boost::string_ref(hdr.name, hdr.name_len) == name;
			});

			if (it == headers_ + num_headers_)
			{
				return{};
			}

			return boost::string_ref(it->value, it->value_len);
		}

		std::vector<boost::string_ref> get_headers_cs(std::string const& name) const
		{
			std::vector<boost::string_ref> headers;
			for (std::size_t i = 0; i < num_headers_; ++i)
			{
				if (boost::string_ref(headers_[i].name, headers_[i].name_len) == name)
				{
					headers.emplace_back(boost::string_ref(headers_[i].value, headers_[i].value_len));
				}
			}

			return headers;
		}

		bool has_header_cs(std::string const& name) const
		{
			auto it = std::find_if(headers_, headers_ + num_headers_, [&name](struct phr_header const& hdr)
			{
				return boost::string_ref(hdr.name, hdr.name_len) == name;
			});

			return it != headers_ + num_headers_;
		}

		std::size_t headers_num_cs(std::string const& name) const
		{
			std::size_t num = 0;
			for (std::size_t i = 0; i < num_headers_; ++i)
			{
				if (boost::string_ref(headers_[i].name, headers_[i].name_len) == name)
				{
					++num;
				}
			}

			return num;
		}

	private:
		const char* method_;
		size_t method_len_;
		const char* path_;
		size_t path_len_;
		int minor_version_;
		struct phr_header headers_[100];
		size_t num_headers_;
	};
}


