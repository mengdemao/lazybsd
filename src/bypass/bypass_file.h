/**
 * @file pcap.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __FILE_H__
#define __FILE_H__

class bypass_file : public bypass {
private:
    std::string file_name;

public:
    bypass_file(string device, int domain, int type, int protocol);
    ~bypass_file();

    ssize_t read(void *buf, size_t count);
	ssize_t write(const void *buf, size_t count);
};

#endif /* __FILE_H__ */