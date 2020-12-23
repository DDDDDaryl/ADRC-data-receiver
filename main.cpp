#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include "WzSerialPort.h"
#include "csv2.hpp"
#include <thread>
#include <atomic>
#include <iomanip>
#include "receiver.h"
#include "sender.h"
#include <regex>
#include "ref_sender.h"
#include "configure.h"

using namespace std;
//using namespace csv2;
//using namespace nlohmann;

void receiveDemo()
{
    WzSerialPort w;
    if (w.open("COM9", 115200, 0, 8, 1))
    {
        uint8_t buf[45];
        while (true)
        {
            memset(buf, 0, 45);
            cout << w.receive(buf, 45) << endl;
            for (int i = 0; i < 45; ++i)
                printf("%02x ", buf[i]);
            cout << endl;
        }
    }
}

void sendDemo()
{
    WzSerialPort w;
    int sz = 200;
    string test(sz, 'a');

    test += "\r\n";
    if (w.open("COM6", 115200, 0, 8, 1))
    {
        for (int i = 0;i < 1;i++)
        {
            cout <<  w.send(test.c_str(), sz + 2) << endl;
        }

        cout << "send demo finished...";
    }
    else
    {
        cout << "open serial port failed...";
    }
    //w.close();
}

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
    deadzone_compensation_dac2	= 0xD1,
    ratio						1 0 01, 0 101	0x95
	sigma						1 0 01, 0 110   0x96
    st_disturbance_est_gain     1 0 01, 0 111   0x97
    新增：100为ADRC参数编号拓展
    st_need_acc_threashold      1 0 10, 0 001   0xA1
    */

atomic<bool> terminate_fl(false);

int main(int argc, char **argv) {

    configure config;
    config.parse();
    auto params = config.get_settings();

//    if (argc != 3) {
//        cerr << "Wrong input param." << endl;
//        system("pause");
//        exit(1);
//    }
//
//    auto serial = argv[1];
//    auto sserial = string(serial);
//    auto cfg = argv[2];

    auto serial = params.PC_COM;
    auto cfg = params.MCU_config_filename;

    //check
    if ( serial.find("COM") ) {
        cerr << "illegal input." << endl;
        system("pause");
        exit(1);
    }

    receiver recv(serial, 115200, cfg);
    string input;

    /*temporary entry*/
    string IC_COM = params.IC_COM;
    ref_sender reference_sender(IC_COM, 115200, params.reference_sample_time, params.reference_filename);

    while (cin >> input) {
        if (input == "send" || input == "s")
            recv.send();
        else if (input == "quit" || input == "q") {
            terminate_fl = true;
            while ( !recv.is_writing_finished() );
            break;
        }
        else if (input == "track" || input == "t") {
            if ( reference_sender.if_busy() ) {
                cerr << "Thread busy" << endl;
                continue;
            }

            if ( reference_sender.read_ref_file() )
                reference_sender.send();
        }
        else
            cout << "Usage: \"send\" to send current configure to board." << endl;
    }

    return 0;
}
