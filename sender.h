//
// Created by Frank Young on 2020/08/26.
//

#ifndef RECEIVER_SENDER_H
#define RECEIVER_SENDER_H

#include "json.hpp"

enum ID_table {
    sys_running_state	        = 0x09,
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
};

class sender {
private:



public:
    sender();
    ~sender();
    void update_send();

};


#endif //RECEIVER_SENDER_H
