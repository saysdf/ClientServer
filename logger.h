#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    Logger(std::string file_name) : m_file_stream(file_name, std::ios::out | std::ios::app) {
        // Open the file
        std::cout << "Log file : " << file_name << " is " << (m_file_stream.is_open() ? "OK" : "FAIL") << std::endl;
    }
    Logger() {
    }
    void set_file(std::string file_name) {
        m_file_stream = std::ofstream(file_name, std::ios::out | std::ios::app);
        std::cout << "Log file : " << file_name << " is " << (m_file_stream.is_open() ? "OK" : "FAIL") << std::endl;
    }

    ~Logger() {
        mutex_.lock();
        m_file_stream.close();
        mutex_.unlock();
    }

    template <typename T>
    Logger& operator<<(const T& data) {
        mutex_.lock();
        // Write to the console
        std::cout << data;
        // Write to the file
        if (m_file_stream.is_open())
            m_file_stream << data;
        mutex_.unlock();
        return *this;
    }

private:
    std::ofstream m_file_stream;
    std::mutex mutex_;
};