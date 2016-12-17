
#pragma once

#include "utils.h"

#include "picohttpparser.h"

#include <boost/utility/string_ref.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>

#include <string>
#include <vector>

namespace timax
{
	class request
	{
	public:
		int parse(const char* data, std::size_t len, std::size_t last_len)
		{
			data_ = data;
			num_headers_ = sizeof(headers_) / sizeof(headers_[0]);
			header_size_ = phr_parse_request(data_, len, &method_,
				&method_len_, &path_, &path_len_,
				&minor_version_, headers_, &num_headers_, last_len);

			auto content_length = get_header("content-length", 14);

			if (content_length.empty()
				|| !boost::conversion::try_lexical_convert<size_t>(content_length.data(), content_length.size(), body_len_))
			{
				body_len_ = 0;
			}

			return header_size_;
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

		boost::string_ref get_header(const std::string& name) const
		{
			return get_header(name.data(), name.size());
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

		std::vector<boost::string_ref> get_headers(const std::string& name) const
		{
			return get_headers(name.data(), name.size());
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

		bool has_header(const std::string& name) const
		{
			return has_header(name.data(), name.size());
		}
		bool has_header(const char* name, size_t size) const
		{
			auto it = std::find_if(headers_, headers_ + num_headers_, [name, size](struct phr_header const& hdr)
			{
				return iequal(hdr.name, hdr.name_len, name, size);
			});

			return it != headers_ + num_headers_;
		}

		std::size_t headers_num(const std::string& name) const
		{
			return headers_num(name.data(), name.size());
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

		boost::string_ref body() const
		{
			return boost::string_ref(data_ + header_size_, body_len_);
		}

		boost::string_ref request_data() const
		{
			boost::string_ref(data_, header_size_ + body_len_);
		}

		int header_size() const { return header_size_; }
		size_t body_len() const { return body_len_; }
	private:
		const char* data_;
		const char* method_;
		size_t method_len_;
		const char* path_;
		size_t path_len_;
		int minor_version_;
		struct phr_header headers_[100];
		size_t num_headers_;

		int header_size_;
		size_t body_len_;
	};
}


