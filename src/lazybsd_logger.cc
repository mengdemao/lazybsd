/**
 * @file lazybsd_logger.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-04
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include "lazybsd_logger.h"
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

namespace lazybsd {

lazybsd_logger& lazybsd_logger::Instance() {
    static lazybsd_logger log;
    return log;
}

bool lazybsd_logger::Init(std::string fileName, int type, int level, int maxFileSize, int maxBackupIndex) {
    boost::log::formatter formatter =
        boost::log::expressions::stream
        << "["
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp",
            "%Y-%m-%d %H:%M:%S.%f") /*.%f*/
        << "|"
        << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>(
            "ThreadID")
        << "]["
        << boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
        << "]"
        << boost::log::expressions::smessage
        << "    "
        << boost::log::expressions::format_named_scope("Scope",
            boost::log::keywords::format = "(%f:%l)",
            boost::log::keywords::iteration = boost::log::expressions::reverse,
            boost::log::keywords::depth = 1);

    switch (type) {
    case console: {
        auto consoleSink = boost::log::add_console_log();
        consoleSink->set_formatter(formatter);
        boost::log::core::get()->add_sink(consoleSink);
    }
                break;
    case file: {
        boost::shared_ptr<file_sink> fileSink(new file_sink(
            boost::log::keywords::file_name = fileName,                       // file name pattern
            boost::log::keywords::target_file_name = "%Y%m%d_%H%M%S_%N.log",   // file name pattern
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(16, 0, 0),  //期货交易当日结束，夜盘算第二天
            boost::log::keywords::rotation_size = maxFileSize * 1024 * 1024,                         // rotation size, in characters
            boost::log::keywords::open_mode = std::ios::out | std::ios::app
        ));

        fileSink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
            boost::log::keywords::target = "logs",        //folder name.
            boost::log::keywords::max_size = maxFileSize * maxBackupIndex * 1024 * 1024,    //The maximum amount of space of the folder.
            boost::log::keywords::min_free_space = 10 * 1024 * 1024,  //Reserved disk space minimum.
            boost::log::keywords::max_files = 512
        ));

        fileSink->set_formatter(formatter);
        fileSink->locked_backend()->scan_for_files();
        fileSink->locked_backend()->auto_flush(true);
        boost::log::core::get()->add_sink(fileSink);
    }
             break;
    default: {
        auto consoleSink = boost::log::add_console_log();
        consoleSink->set_formatter(formatter);
        boost::log::core::get()->add_sink(consoleSink);
    }
           break;
    }
    boost::log::add_common_attributes();
    boost::log::core::get()->add_global_attribute("Scope", boost::log::attributes::named_scope());
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= level
    );
    return true;
}



}