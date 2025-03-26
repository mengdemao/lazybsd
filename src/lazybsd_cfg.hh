/**
 * @file lazybsd_cfg.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-20
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <array>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <cstddef>
#include <cstdint>
#include <lazybsd.h>
#include <string>

#include "lazybsd_cfg.h"

#ifndef LAZYBSD_CFG_HH
#define LAZYBSD_CFG_HH

namespace lazybsd::cfg {

/**
 * @brief init lazybsd cfg module
 *
 * @return true
 * @return false
 */
bool init(void);

struct config {
private:
    int parserFile(const std::string& fileName, boost::property_tree::ptree& cfg);
    int parserString(const std::string& stream, boost::property_tree::ptree& cfg);
    int ptree2struct(const boost::property_tree::ptree& cfg);

public:
    config() noexcept;
    ~config() = default;

    /**
     * @brief  加载配置文件
     * @param  cfg_file json/xml配置文件
     * @return int 执行结果
     */
    int loadFile(const std::string cfg_file);

    /**
     * @brief  加载配置文件
     * @param  cfg_string json/xml配置字符串
     * @return int 执行结果
     */
    int loadString(const std::string cfg_string);
};

};  // namespace lazybsd::cfg

#endif  // LAZYBSD_CFG_HH