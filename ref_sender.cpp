//
// Created by Frank Young on 2020/12/18.
//

#include "ref_sender.h"
#include <utility>

using namespace std;

ref_sender::ref_sender(std::string COM, size_t baud_rate, size_t time_interval, string ref_file_name)
: m_ref_file_name(std::move(ref_file_name)), m_COM(std::move(COM)), m_baud_rate(baud_rate), m_time_interval(time_interval) {
    /*open COM*/
    auto ret = w.open(m_COM.c_str(), m_baud_rate, 0, 8, 1, 1);
    if (!ret) {
        std::cerr << "Open port " << m_COM << " failed." << std::endl;
        system("pause");
        exit(1);
    }
    /*initialize buffer*/
    size_t ptr = 0;
    m_send_buf[ptr++] = 0xeb;
    m_send_buf[ptr++] = 0x90;
    m_send_buf[ptr++] = msg_len;
    ptr += 4;
    m_send_buf[ptr++] = 0x0d;
    m_send_buf[ptr] = 0x0a;
}

bool ref_sender::read_ref_file() {
//    ifstream is(m_ref_file_name);
//    if (!is) {
//        std::cerr << "file open failed\r\n";
//        system("pause");
//        exit(1);
//    }

    csv2::Reader<csv2::delimiter<','>,
                csv2::quote_character<'"'>,
                csv2::first_row_is_header<true>,
                csv2::trim_policy::trim_whitespace> reader;

    if ( reader.mmap(m_ref_file_name) ) {
        const auto header = reader.header();
        int ref_idx = 0;
        string res, cell_val;
        for (const auto it : header) {
            res.clear();
            it.read_value(res);
            if (res == "ref"|| res == "ref\r")
                break;
            ++ref_idx;
        }
        if (res != "ref" && res != "ref\r") {
            cerr << "Column \"ref\" not found." << endl;
            return false;
        }
        for (const auto row : reader) {
            int curr_cell = 0;
            for (const auto cell : row) {
                if (curr_cell++ == ref_idx) {
                    cell.read_value(cell_val);
                    m_ref_buf.emplace_back(stof(cell_val));
                }
            }
        }
    }

    return true;
}

bool ref_sender::send() {
    fl_busy = true;
    m_curr_pnt = m_ref_buf.cbegin();
    trans_ref_ver tr{};
    timer.setInterval( [&]() {
        if ( m_curr_pnt == m_ref_buf.cend() ) {
            timer.stop();
            fl_busy = false;
            cerr << "Sending reference finished." << endl;
        }


        tr.data = *(m_curr_pnt++);
        auto wr_ptr = m_send_buf + 3;
        for (auto b : tr.d)
            *(wr_ptr++) = b;
        if ( !w.send(m_send_buf, msg_len) ) {
            cerr << "Sending reference failed." << endl;
            timer.stop();
            fl_busy = false;
        }
    }, m_time_interval );
    return true;
}

bool ref_sender::if_busy() const {
    return fl_busy;
}
