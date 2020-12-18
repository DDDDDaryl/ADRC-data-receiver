//
// Created by Frank Young on 2020/08/24.
//

#ifndef RECEIVER_RECEIVER_H
#define RECEIVER_RECEIVER_H
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <ctime>
#include <sstream>
#include <map>
#include <cstdint>
#include <regex>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "WzSerialPort.h"
#include "csv2.hpp"
#include "json.hpp"

enum state {
    header = 0,
    body_id,
    body_data,
    tail
};

enum data_substate {
    LSB = 0,
    second,
    third,
    MSB
};

union trans {
    float data;
    unsigned udata;
    uint8_t flag;
    uint8_t d[4];
};


/*
 * sys_running_state	        = 0x09,
    Is_close_loop	            = 0x0A,
    controller_type	            = 0x0B,
    open_loop_input_type	    = 0x0C,
    Sample_Rate_Hz	            = 0x8C,
    Sample_Rate_of_Sensor_Hz	= 0x8D,
    run_time	                = 0x0E,
    reference_signal			= 0xCF,
    LADRC_wc	                = 0x91,
    LADRC_wo	                = 0x92,
    LADRC_b0	                = 0x93,
    LADRC_wc_bar	            = 0x94,
    PID_Kp	                    = 0x99,
    PID_Ki	                    = 0x9A,
    PID_Kd	                    = 0x9B,
    open_loop_input_sine_amp	= 0xB1,
    open_loop_input_sine_freq	= 0xB2,
    open_loop_input_step_amp	= 0xB9,
    open_loop_input_step_time	= 0xBA,
    deadzone_compensation_dac1	= 0xC9,
    deadzone_compensation_dac2	= 0xD1
    */

class receiver {
private:
    static const size_t buf_size = 1024;
    constexpr static const char *cfg_name = "config.json";
    std::string custom_cfg_name;

    state sta = header;

    uint8_t buf[buf_size]{};
    uint8_t send_buf[buf_size]{};

    std::mutex mtx;
    WzSerialPort w;
    //std::vector<std::thread> workers;

    std::string port;
    size_t baudrate;

    std::string pattern;
    std::regex r;

    std::atomic<bool> is_writing_done;

private:
    void parse(std::vector<std::vector<std::string>> &table);

public:
    void receive();
    //void writeFile();
    void thread_cycle();
    static void wrapper(receiver *);

public:
    int send();

public:
    std::atomic<bool>& is_writing_finished();

public:
    receiver() = default;
    explicit receiver(std::string port_ = "COM9", size_t baudrate_ = 115200, std::string cfg = cfg_name);
    ~receiver();
};


#endif //RECEIVER_RECEIVER_H
