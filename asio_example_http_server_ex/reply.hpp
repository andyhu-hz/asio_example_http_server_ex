
#pragma once

#include "utils.h"
#include <boost/asio.hpp>
#include <boost/utility/string_ref.hpp>

#include <string>
#include <vector>

namespace timax
{
	class reply
	{
	public:
		enum status_type
		{
			ok = 200,
			created = 201,
			accepted = 202,
			no_content = 204,
			multiple_choices = 300,
			moved_permanently = 301,
			moved_temporarily = 302,
			not_modified = 304,
			bad_request = 400,
			unauthorized = 401,
			forbidden = 403,
			not_found = 404,
			internal_server_error = 500,
			not_implemented = 501,
			bad_gateway = 502,
			service_unavailable = 503
		};

		struct header_t
		{
			std::string name;
			std::string value;
		};

		std::vector<boost::asio::const_buffer> to_buffers();
		static reply stock_reply(status_type status);
		void reset();

		void set_status(status_type status);
		std::vector<header_t>& headers();
		std::vector<header_t> const& headers() const;
		void add_header(std::string const& name, std::string const& value);
		
		boost::string_ref get_header(const std::string& name);
		boost::string_ref get_header(const char* name, size_t size) const;

		std::vector<boost::string_ref> get_headers(const std::string& name) const;
		std::vector<boost::string_ref> get_headers(const char* name, size_t size) const;

		bool has_header(const std::string& name) const;
		bool has_header(const char* name, size_t size) const;

		std::size_t headers_num(const std::string& name) const;
		std::size_t headers_num(const char* name, size_t size) const;

		std::size_t headers_num() const;

		boost::string_ref get_header_cs(std::string const& name) const;

		std::vector<boost::string_ref> get_headers_cs(std::string const& name) const;

		bool has_header_cs(std::string const& name) const;

		std::size_t headers_num_cs(std::string const& name) const;

		void set_body(std::string body);
	private:
		std::vector<header_t> headers_;
		std::string content_;
		status_type status_;
	};
}
