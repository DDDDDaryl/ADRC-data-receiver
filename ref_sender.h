//
// Created by Frank Young on 2020/12/18.
//
/*
 * ref_sender类主要实现功能如下：
 * - 读csv文件，获得参考信号曲线，并按照固定时间间隔发送到串口
 * - 注意到该类用到的串口应不同于receiver类使用的串口，使用单独线程运行
 * */
#ifndef RECEIVER_REF_SENDER_H
#define RECEIVER_REF_SENDER_H

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "timercpp.h"
#include "csv2.hpp"
#include "WzSerialPort.h"

union trans_ref_ver {
    float data;
    unsigned udata;
    uint8_t flag;
    uint8_t d[4];
};

class ref_sender {
public:
    explicit ref_sender(std::string COM, size_t baud_rate = 115200, size_t time_interval = 20, std::string ref_file_name = "ref.csv");
    ~ref_sender() = default;

public:
    bool read_ref_file();
    bool send();
    bool if_busy() const;

private:
    WzSerialPort w;
    Timer timer;

    std::string m_ref_file_name;
    std::string m_COM;
    size_t m_baud_rate;
    double m_time_interval;
    size_t msg_len = 9; // header=2, len=1, data=4, tail=2

    std::vector<float> m_ref_buf;
    std::vector<float>::const_iterator m_curr_pnt;
    uint8_t m_send_buf[128]{};

    bool fl_busy = false;

};


#endif //RECEIVER_REF_SENDER_H
