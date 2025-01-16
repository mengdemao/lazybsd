/**
 * @file lazybsd_log.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-04
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#ifndef __LAZYBSD_LOG__
#define __LAZYBSD_LOG__

#include <boost/log/core.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/exceptions.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>

namespace lazybsd::log {
    bool init(void);
}

template <typename... T>
static inline void log_trace(fmt::format_string<T...> fmt, T&&... args)
{
    BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(trace) << fmt::format(fmt, args...);
}

template <typename... T>
static inline void log_debug(fmt::format_string<T...> fmt, T&&... args)
{
    BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(debug) << fmt::format(fmt, args...);
}

template <typename... T>
static inline void log_info(fmt::format_string<T...> fmt, T&&... args)
{
    BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(info) << fmt::format(fmt, args...);
}

template <typename... T>
static inline void log_warning(fmt::format_string<T...> fmt, T&&... args)
{
    BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(warning) << fmt::format(fmt, args...);
}

template <typename... T>
static inline void log_error(fmt::format_string<T...> fmt, T&&... args)
{
    BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(error) << fmt::format(fmt, args...);
}

template <typename... T>
static inline void log_fatal(fmt::format_string<T...> fmt, T&&... args)
{
    BOOST_LOG_FUNCTION(); BOOST_LOG_TRIVIAL(fatal) << fmt::format(fmt, args...);
}

#endif /* __LAZYBSD_LOG__ */