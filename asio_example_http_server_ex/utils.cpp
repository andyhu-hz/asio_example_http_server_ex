#include "utils.h"

#include <boost/filesystem.hpp>
#include <algorithm>
#include <cctype>
#include <ctime>

#if defined(_MSC_VER) && !defined(gmtime_r)
#define gmtime_r(tp, tm) ((gmtime_s((tm), (tp)) == 0) ? (tm) : NULL)
#endif

namespace timax
{
	bool iequal(const char* src, size_t src_len, const char* dest, size_t dest_len)
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

	//from nghttp2
	const char *MONTH[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	const char *DAY_OF_WEEK[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	template <typename Iterator>
	Iterator cpydig(Iterator d, uint32_t n, size_t len)
	{
		auto p = d + len - 1;

		do
		{
			*p-- = (n % 10) + '0';
			n /= 10;
		} while (p >= d);

		return d + len;
	}

	std::string http_date(time_t t)
	{
		/* Sat, 27 Sep 2014 06:31:15 GMT */
		std::string res(29, 0);
		http_date(&res[0], t);
		return res;
	}


	char *http_date(char *res, time_t t)
	{
		struct tm tms;

		if (gmtime_r(&t, &tms) == nullptr)
		{
			return res;
		}

		auto p = res;

		auto s = DAY_OF_WEEK[tms.tm_wday];
		p = std::copy_n(s, 3, p);
		*p++ = ',';
		*p++ = ' ';
		p = cpydig(p, tms.tm_mday, 2);
		*p++ = ' ';
		s = MONTH[tms.tm_mon];
		p = std::copy_n(s, 3, p);
		*p++ = ' ';
		p = cpydig(p, tms.tm_year + 1900, 4);
		*p++ = ' ';
		p = cpydig(p, tms.tm_hour, 2);
		*p++ = ':';
		p = cpydig(p, tms.tm_min, 2);
		*p++ = ':';
		p = cpydig(p, tms.tm_sec, 2);
		s = " GMT";
		p = std::copy_n(s, 4, p);

		return p;
	}


	reply reply_static_file(std::string const &static_path, request const &req)
	{
        if (req.path().find("..") != std::string::npos)
        {
            return reply::stock_reply(reply::bad_request);
        }
		reply rep;
		if (rep.response_file((boost::filesystem::path(static_path) / req.path().to_string()).generic_string()))
		{
			return rep;
		}
		return reply::stock_reply(reply::not_found);
	}


}


