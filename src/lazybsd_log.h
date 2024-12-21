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

#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

namespace lazybsd {

class lazybsd_log {
public:
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink;
    enum loggerType {
        console = 0,
        file,
    };

    lazybsd_log() {}

    ~lazybsd_log() {}

    static lazybsd_log& Instance();

    bool Init(std::string fileName, int type, int level, int maxFileSize, int maxBackupIndex);

    boost::log::sources::severity_logger<boost::log::trivial::severity_level> _logger;
};

}

#define log_trace     BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(lazybsd::lazybsd_log::Instance()._logger, boost::log::trivial::trace)
#define log_debug     BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(lazybsd::lazybsd_log::Instance()._logger, boost::log::trivial::debug)
#define log_info      BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(lazybsd::lazybsd_log::Instance()._logger, boost::log::trivial::info)
#define log_warning   BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(lazybsd::lazybsd_log::Instance()._logger, boost::log::trivial::warning)
#define log_error     BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(lazybsd::lazybsd_log::Instance()._logger, boost::log::trivial::error)
#define log_fatal     BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(lazybsd::lazybsd_log::Instance()._logger, boost::log::trivial::fatal)

#endif /* __LAZYBSD_LOG__ */