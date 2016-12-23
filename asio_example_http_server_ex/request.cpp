
#include "request.hpp"
#include "utils.h"

#include <boost/lexical_cast/try_lexical_convert.hpp>

namespace timax
{
    int request::parse(const char* data, std::size_t len, std::size_t last_len)
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

    boost::string_ref request::get_header(const char* name, size_t size) const
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

    std::vector<boost::string_ref> request::get_headers(const char* name, size_t size) const
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

    std::vector<request::header_t> request::get_headers() const
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

    bool request::has_header(const char* name, size_t size) const
    {
        auto it = std::find_if(headers_, headers_ + num_headers_, [name, size](struct phr_header const& hdr)
        {
            return iequal(hdr.name, hdr.name_len, name, size);
        });

        return it != headers_ + num_headers_;
    }

    std::size_t request::headers_num(const char* name, size_t size) const
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

    boost::string_ref request::get_header_cs(std::string const& name) const
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

    std::vector<boost::string_ref> request::get_headers_cs(std::string const& name) const
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

    bool request::has_header_cs(std::string const& name) const
    {
        auto it = std::find_if(headers_, headers_ + num_headers_, [&name](struct phr_header const& hdr)
        {
            return boost::string_ref(hdr.name, hdr.name_len) == name;
        });

        return it != headers_ + num_headers_;
    }

    std::size_t request::headers_num_cs(std::string const& name) const
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
}