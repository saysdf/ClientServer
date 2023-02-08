#pragma once

#include "dumper.h"

class DataContainer {
public:
    DataContainer(Logger& log) : log_(log), dumper_(nullptr), dump_thread_(nullptr) {

    }
    ~DataContainer() {
        if (dumper_) {
            dumper_->stop();
            if (dump_thread_) {
                dump_thread_->join();
                delete dump_thread_;
            }
            delete dumper_;
        }
    }
    int add(int value) {
        boost::unique_lock<boost::mutex> lock(numbers_mutex);
        numbers.insert(value);
        lock.unlock();

        // Calculate the average of the squares
        int sum = 0;
        //log_ << "[CONTAINER]";
        for (int i : numbers) {
            sum += i * i;
            //log_ << i << " ";
        }
        //log_ << "\n";
        int average = sqrt(sum / numbers.size());
        log_ << "[CONTAINER]Average of squares: " << average << "\n";
        return average;
    }
    void dump(string file) {
        dumper_ = new Dumper(file, numbers, numbers_mutex);
        dump_thread_ = new std::thread(&Dumper::write, dumper_);

    }
private:
    // Container to store unique integer numbers received from clients
    set<int> numbers;
    boost::mutex numbers_mutex; // Mutex to lock and unlock access to the container
    Dumper* dumper_;
    std::thread* dump_thread_;
    Logger& log_;
};