
#pragma once


#include <boost/utility/string_ref.hpp>

#include <string>
#include <vector>
#include <cassert>

#include "picohttpparser.h"

namespace timax
{
	class request
	{
	public:
		request();
		~request();

		int parse(std::size_t last_len);

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

		int header_size() const { return header_size_; }

		bool is_chunked() const
		{
			auto val = get_header("transfer-encoding", 17);
			return val == "chunked";
		}

		boost::string_ref body() const
		{
// 			assert(header_size_ + body_len_ == buffer_.size);
			return boost::string_ref(buffer_.buffer + header_size_, body_len_);
		}
		size_t body_len() const { return body_len_; }

		struct buffer_t 
		{
			char* buffer;
			std::size_t size;
			std::size_t max_size;
			std::size_t remain_size()
			{
				return max_size - size;
			}
			char* curr_ptr()
			{
				return buffer + size;
			}
		};

		buffer_t const& raw_request() const
		{
			return buffer_;
		}

		buffer_t& raw_request()
		{
			return buffer_;
		}

		void increase_buffer(std::size_t size);

	private:
		buffer_t buffer_;
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


