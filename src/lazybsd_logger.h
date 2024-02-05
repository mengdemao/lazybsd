/**
 * @file lazybsd_logger.h
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

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

namespace lazybsd {

class lazybsd_logger {
public:
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink;
    enum loggerType {
        console = 0,
        file,
    };

    lazybsd_logger() {}

    ~lazybsd_logger() {}

    static lazybsd_logger& Instance();

    bool Init(std::string fileName, int type, int level, int maxFileSize, int maxBackupIndex);

    boost::log::sources::severity_logger<boost::log::trivial::severity_level> _logger;
};

#define LOG_TRACE(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::trace) << logEvent;
#define LOG_DEBUG(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::debug) << logEvent;
#define LOG_INFO(logEvent)   BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::info) << logEvent;
#define LOG_WARN(logEvent)   BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::warning) << logEvent;
#define LOG_ERROR(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::error) << logEvent;
#define LOG_FATAL(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::fatal) << logEvent;

}

#endif /* __LAZYBSD_LOG__ */