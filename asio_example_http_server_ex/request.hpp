
#pragma once

#include "picohttpparser.h"

#include <boost/utility/string_ref.hpp>

#include <string>
#include <vector>

namespace timax
{
	class request
	{
	public:
		int parse(const char* data, std::size_t len, std::size_t last_len);

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
		boost::string_ref get_header(const char* name, size_t size) const;

		std::vector<boost::string_ref> get_headers(const std::string& name) const
		{
			return get_headers(name.data(), name.size());
		}
		std::vector<boost::string_ref> get_headers(const char* name, size_t size) const;

		struct header_t
		{
			boost::string_ref name;
			boost::string_ref value;
		};
		std::vector<header_t> get_headers() const;

		bool has_header(const std::string& name) const
		{
			return has_header(name.data(), name.size());
		}
		bool has_header(const char* name, size_t size) const;

		std::size_t headers_num(const std::string& name) const
		{
			return headers_num(name.data(), name.size());
		}
		std::size_t headers_num(const char* name, size_t size) const;

		std::size_t headers_num() const
		{
			return num_headers_;
		}

		boost::string_ref get_header_cs(std::string const& name) const;

		std::vector<boost::string_ref> get_headers_cs(std::string const& name) const;

		bool has_header_cs(std::string const& name) const;

		std::size_t headers_num_cs(std::string const& name) const;

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


