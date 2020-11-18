//
// Created by Frank Young on 2020/08/24.
//

#include <iomanip>
#include "receiver.h"

using json = nlohmann::json;

enum info_id {
    begin_border = 0x80,
    info_id_ref = 0x81,
    info_id_transient_profile,
    info_id_output_pos,
    info_id_err,
    info_id_control_signal,
    info_id_ESO_first_order_state,
    info_id_ESO_second_order_state,
    info_id_ESO_third_order_state,
    info_id_ESO_fourth_order_state,
    end_border
};

std::map<info_id, std::pair<std::string, float>> data_table {
        {info_id_ref, {"ref", 0.0f}},
        {info_id_transient_profile, {"transient profile", 0.0f}},
        {info_id_output_pos, {"sensor_data", 0.0f}},
        {info_id_err, {"error", 0.0f}},
        {info_id_control_signal, {"control_signal", 0.0f}},
        {info_id_ESO_first_order_state, {"ESO 1st order", 0.0f}},
        {info_id_ESO_second_order_state, {"ESO 2nd order", 0.0f}},
        {info_id_ESO_third_order_state, {"ESO 3rd order", 0.0f}},
        {info_id_ESO_fourth_order_state, {"ESO 4th order", 0.0f}},
};

std::unordered_map<std::string, uint8_t> snd_tbl {
        {"sys_running_state",               0x09},
        {"Is_close_loop",                   0x0A},
        {"controller_type",                 0x0B},
        {"open_loop_input_type",            0x0C},
        {"Sample_Rate_Hz",                  0x8C},
        {"Sample_Rate_of_Sensor_Hz",        0x8D},
        {"run_time",                        0x0E},
        {"reference_signal",                0xCF},
        {"LADRC_wc",                        0x91},
        {"LADRC_wo",                        0x92},
        {"LADRC_b0",                        0x93},
        {"LADRC_wc_bar",                    0x94},
        {"PID_Kp",                          0x99},
        {"PID_Ki",                          0x9A},
        {"PID_Kd",                          0x9B},
        {"open_loop_input_sine_amp",        0xB1},
        {"open_loop_input_sine_freq",       0xB2},
        {"open_loop_input_step_amp",        0xB9},
        {"open_loop_input_step_time",       0xBA},
        {"deadzone_compensation_dac1",      0xC9},
        {"deadzone_compensation_dac2",      0xD1},
        {"ratio",                           0x95},
        {"sigma",                           0x96}
};

void receiver::receive() {
    while (true) {
        time_t curr_time;
        struct tm *ptminfo;
        time(&curr_time);
        ptminfo = localtime(&curr_time);

        std::string filename;
        std::stringstream ss;
        ss << ptminfo->tm_year + 1900 << "-" << ptminfo->tm_mon + 1 << "-" << ptminfo->tm_mday << "-" << ptminfo->tm_hour << "-" << ptminfo->tm_min;
        filename = ss.str() + ".csv";

        std::ofstream stream(filename);

        if (!stream) {
            std::cerr << "file open failed\r\n";
            exit(1);
        }

        std::cout << "New file created.\r\n";
        csv2::Writer<csv2::delimiter<','>> writer(stream);
        std::vector<std::string> table_idx;
        std::vector<std::vector<std::string>> table;

        for (const auto& key : data_table)
            table_idx.emplace_back(key.second.first);
        writer.write_row(table_idx);

        while (true) {
            memset(buf, 0, buf_size);
            if (w.receive(buf, buf_size) == 0)
                break;

//            for (int i = 0; i < buf_size; ++i) {
//                printf("%02x ", *(buf + i));
//            }

            parse(table);

//            std::cout << sta << std::endl;

//            for (const auto &i : table) {
//                for (const auto &j : i)
//                    std::cout << j << ' ';
//                std::cout << std::endl;
//            }


            if (sta == header && !table.empty()) {
//                std::cout << "row written" << std::endl;
                writer.write_rows(table);
                std::vector<std::vector<std::string>> tmp;
                table.swap(tmp);
                //table.erase(table.begin(), table.end());
            }

        }
    }
}


void receiver::parse(std::vector<std::vector<std::string>> &table) {
    size_t curr = 0;
    static size_t body_len = 0;
    static info_id id;
    static data_substate substa = LSB;
    static union trans tr;
    static std::vector<std::string> tmp(data_table.size());
    static size_t pos = 0;
    while (curr < buf_size) {
        switch (sta) {
            case header: {
                if (buf[curr] == 0xeb) {
                    if (++curr < buf_size && buf[curr] == 0x90 && ++curr < buf_size) {
                        body_len = buf[curr++];
                        pos = 3;
                        sta = body_id;
                    }

                    else {
                        while (curr < buf_size && buf[++curr] != 0xeb);
                        pos = 0;
                        break;
                    }
                }
                else if (buf[curr] == 0x90 && ++curr < buf_size) {
                    body_len = buf[curr++];
                    pos = 3;
                    sta = body_id;
                }
                else {
                    while (curr < buf_size && buf[++curr] != 0xeb);
                    pos = 0;
                    break;
                }
                break;
            }
            case body_id: {
                id = static_cast<info_id>(buf[curr++]);
                ++pos;
                sta = body_data;
                substa = LSB;
                break;
            }
            case body_data: {

                if (substa == LSB) {
                    tr.data = 0;
                    tr.d[0] = buf[curr++];
                    ++pos;
                    substa = second;
                }

                else if (substa == second) {
                    tr.d[1] = buf[curr++];
                    ++pos;
                    substa = third;
                }

                else if (substa == third) {
                    tr.d[2] = buf[curr++];
                    ++pos;
                    substa = MSB;
                }

                else {
                    tr.d[3] = buf[curr++];
                    ++pos;
                    substa = LSB;
                    if (pos >= body_len - 2)
                        sta = tail;
                    else sta = body_id;
                    if (id >= end_border || id <= begin_border)
                        std::cout << id << std::endl;
                    data_table[id].second = tr.data;
                    tr.data = 0;
                }

                break;
            }
            case tail: {
                if (buf[curr] == 0x0d) {
                    if (++curr < buf_size && buf[curr] == 0x0a) {
                        ++curr;
                        sta = header;
                        pos = 0;
                    }
                    else {
                        while (curr < buf_size && buf[++curr] != 0xeb);
                        sta = header;
                        pos = 0;
                        break;
                    }
                }
                else if (buf[curr] == 0x0a) {
                    ++curr;
                    sta = header;
                    pos = 0;
                }
                else {
                    while (curr < buf_size && buf[++curr] != 0xeb);
                    sta = header;
                    pos = 0;
                    break;
                }
                auto it = data_table.begin();
                for (int i = 0 ; i < data_table.size(); ++i, ++it) {
                    tmp[i] = std::to_string(it->second.second);
                }
//                for (auto &str : tmp)
//                    std::cout << str << " ";
//                std::cout << std::endl;

                table.emplace_back(tmp);

                break;
            }
            default:
                break;
        }
    }

}

void receiver::thread_cycle() {
    receive();
}

void receiver::wrapper(receiver *ptr) {
    ptr->thread_cycle();
}

receiver::receiver(std::string port_, size_t baudrate_)
        : port(std::move(port_)), baudrate(baudrate_)
{
    auto ret = w.open(port.c_str(), baudrate, 0, 8, 1, 1);
    if (!ret) {
        std::cerr << "Open port " << port << " failed" << std::endl;
        exit(1);
    }

    pattern = "\"(.*?)\":(.*?)[,|}]";
    r.assign(pattern);

    memset(buf, 0, buf_size);
    std::thread th(wrapper, this);
    th.detach();
}

receiver::~receiver() = default;

int receiver::send() {
    char cwd_buf[200]{};
    char* cwd = getcwd(cwd_buf, 200);
    std::string tmp(cwd);
    std::string str_cwd = tmp + "\\" + cfg_name;
    std::fstream fs(str_cwd);
    json j;
    if (!fs) {
        j = {
                {"global settings",     {
                                                {"sys_running_state",        0},
                                                {"Is_close_loop",             1},
                                                {"controller_type",          128},
                                                {"open_loop_input_type",      128},
                                                {"Sample_Rate_Hz", 50},
                                                {"Sample_Rate_of_Sensor_Hz", 50},
                                                {"run_time", 0},
                                                {"reference_signal", 0},
                                                {"deadzone_compensation_dac1", 0},
                                                {"deadzone_compensation_dac2", 0},
                                                {"ratio", 0},
                                                {"sigma", 0}
                                        }},
                {"LADRC parameters",    {
                                                {"LADRC_wc",                 15},
                                                {"LADRC_wo",                  45},
                                                {"LADRC_b0",                 60},
                                                {"LADRC_wc_bar",              4}
                                        }},
                {"PID parameters",      {
                                                {"PID_Kp",                   0},
                                                {"PID_Ki",                    0},
                                                {"PID_Kd",                   0}
                                        }},
                {"openloop parameters", {
                                                {"open_loop_input_sine_amp", 0},
                                                {"open_loop_input_sine_freq", 0},
                                                {"open_loop_input_step_amp", 0},
                                                {"open_loop_input_step_time", 0}
                                        }}
        };
        int open_cnt = 0;
        while (!fs.is_open() && open_cnt++ < 20) {
            fs.open(str_cwd, std::ios::out);
        }

        if (!fs.is_open()) {
            std::cerr << "Failed opening file.\r\n";
            exit(1);
        }
        fs << std::setw(4) << j;
        Sleep(2000);
        std::cerr << "Missing configure file, automatically created.\r\n";
        std::cerr << "Exited. Configure parameters and rerun.\r\n";
        fs.close();
        exit(0);

    } else {
        fs >> j;
    }

    auto *send_buf_ptr = static_cast<uint8_t *>(send_buf);
    std::string field, data, body;
    std::stringstream ss(data);
    uint8_t id = 0;
    union trans tr{};

    memset(send_buf, '\0', buf_size);
    *send_buf_ptr++ = 0xeb;
    *send_buf_ptr++ = 0x90;
    uint8_t len = 1;
    // temporarily ignore the length byte
    ++send_buf_ptr;


    for (const auto &p : j) {
        auto tmp = p.dump();
        for (std::sregex_iterator it(tmp.begin(), tmp.end(), r), end_it; it != end_it; ++it) {
            field = it->str(1);
            ss.clear();
            tr.data = 0;
            ss << it->str(2);
            //data = it->str(2);
            id = snd_tbl[field];
            if (id & 0x80u)
                ss >> tr.data;
            else {
                ss >> tr.udata;
//                tr.zlag = static_cast<uint8_t>(std::stoi(data));
            }
//                tr.flag = static_cast<uint8_t>(std::stoi(ss.str()));

            *send_buf_ptr++ = id;
            ++len;
            if (id & 0x80u) {
                for (const uint8_t &b : tr.d) {
                    *send_buf_ptr++ = b;
                    ++len;
                }
            } else {
                *send_buf_ptr++ = tr.flag;
                ++len;
            }
//            std::cout << "field: " << it->str(1) << "   data: " << it->str(2) << std::endl;
        }
    }

    *(send_buf + 2) = len;
    *send_buf_ptr++ = 0x0d;
    *send_buf_ptr = 0x0a;
//    *send_buf_ptr++ = 0x0d;
//    *send_buf_ptr = 0x0a;
    for (int i = 0; i < len + 4; ++i) {
        printf("%02x ", *(send_buf + i));
    }
    std::cout << std::endl;


    int byte_send = w.send(send_buf, len + 4);
    if (byte_send == len + 4) {
        printf("%d bytes message send.\r\n", byte_send);
    } else {
        printf("send mission failed.\r\n");
    }
    return byte_send;
}

