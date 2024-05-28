/**
 * @file lazybsd_args.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-25
 *
 * @brief lazybsd参数解析函数
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <config.h>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <lazybsd_args.h>
#include <lazybsd_version.h>

#include <boost/program_options.hpp>

using namespace boost;
using namespace std;

namespace lazybsd {

/**
 * @brief 解析输入的命令行
 *
 * @param argc 输入参数个数
 * @param argv 输入参数内容
 * @return int 执行结果
 */
std::optional<lazybsd_value> lazybsd_args(int argc, char* argv[])
{
	lazybsd_value value = {false};

	try {
		program_options::variables_map		variables_map;
		program_options::options_description description("Usage: lazybsd_start [options]");
		description.add_options()
					("config,c",    program_options::value<string>(), "Path of config file.")
					("proc_id,p", 	program_options::value<int32_t>(), "proc id")
					("proc_type,t",	program_options::value<string>(), "proc type")
					("help,h", 		"Display this information.")
					("version,v", 	"Display compiler version information.")
					("debug,d", 	"Run as debug mode.");

		store(parse_command_line(argc, argv, description), variables_map);
		notify(variables_map);

		if (1 == argc) {
			std::cout << description << "\n";
			return std::nullopt;
		}

		if (variables_map.count("config") != 0) {
			value.config_file = variables_map["config"].as<std::string>();
		}

		if (variables_map.count("proc_id") != 0) {
			value.proc_id = variables_map["proc_id"].as<int32_t>();
		}

		if (variables_map.count("proc_type") != 0) {
			value.proc_type = variables_map["proc_type"].as<std::string>();
		}

		if (variables_map.count("help") != 0) {
			std::cout << description << "\n";
			return std::nullopt;
		}

		if (variables_map.count("version") != 0) {
			fmt::print("{} \r\n"
						 "Copyright (C) 2024 MengDemao mengdemao19951021@gmail.com \n"
						 "This is free software; see the source for copying conditions.  There is NO\n"
						 "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
						 version::lazybsd_version_string());
			return std::nullopt;
		}

		if (variables_map.count("debug") != 0) {
			value.debug = true;
		}
	}
	catch (std::exception& e) {
		std::cerr << "error: " << e.what() << "\n";
		return std::nullopt;
	}
	catch (...) {
		std::cerr << "Exception of unknown type!\n";
		return std::nullopt;
	}

	return value;
}

}