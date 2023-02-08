#pragma once
#include <iostream>
#include <set>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <fstream>
#include <thread>
#include <cstdlib>
#include "logger.h"

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;
using boost::asio::ip::tcp;

class Dumper
{
public:
    Dumper(string file_name, set<int>& numbers, boost::mutex& numbers_mutex) :
        file_name_(file_name), numbers_(numbers), numbers_mutex_(numbers_mutex), stop_(false) {}
    void write() {
        cout << "\t[DUMPER] start dumper" << endl;
        while (stop_ == false) {
            {
                boost::unique_lock<boost::mutex> lock(numbers_mutex_);
                if (numbers_.empty()) {
                    continue;
                }
                ofstream out(file_name_, ios::out);

                cout << "\t[DUMPER] making..." << endl;
                for (int number : numbers_) {
                    out << number << "\n";
                }
                cout << "\t[DUMPER] done" << endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    void stop() { stop_ = true; }
private:
    string file_name_;
    set<int>& numbers_;
    atomic_bool stop_;
    boost::mutex& numbers_mutex_;
};
