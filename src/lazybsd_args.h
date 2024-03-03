/**
 * @file lazybsd_args.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-25
 *
 * @brief lazybsd参数解析
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#ifndef __LAZYBSD_ARGS__
#define __LAZYBSD_ARGS__

#include <string>
#include <optional>

namespace lazybsd {

struct lazybsd_value {
	bool debug;					// 调试开关
	std::string config_file;	// 配置文件
	int32_t proc_id;			// proc id
	std::string proc_type;		// proc类型
};

/**
 * @brief 解析输入的命令行
 *
 * @param argc 输入参数个数
 * @param argv 输入参数内容
 * @return int 执行结果
 */
std::optional<lazybsd_value> lazybsd_args(int argc, char* argv[]);

}
#endif // __LAZYBSD_ARGS__