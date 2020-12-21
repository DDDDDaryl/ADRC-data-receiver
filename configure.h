//
// Created by Frank Young on 2020/12/20.
//

#ifndef RECEIVER_CONFIGURE_H
#define RECEIVER_CONFIGURE_H

#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "json.hpp"

enum datatype {
    plain_text = 0,
    type_double
};

struct settings {
    std::string PC_COM;
    std::string IC_COM;
    std::string MCU_config_filename;
    std::string reference_filename;
    double reference_sample_time = 0;

    std::unordered_map<std::string, datatype> field_list {
            {"PC_COM", plain_text},
            {"IC_COM", plain_text},
            {"MCU_config_filename", plain_text},
            {"reference_filename", plain_text},
            {"reference_sample_time", type_double}
    };
    std::unordered_map<std::string, std::string> field_data {
            {"PC_COM", {}},
            {"IC_COM", {}},
            {"MCU_config_filename", {}},
            {"reference_filename", {}},
            {"reference_sample_time", {}}
    };

    void init();
};

class configure {
public:
    configure() = default;
    ~configure() = default;
    void init();

public:
    bool parse();
    const settings& get_settings() const;

private:
    settings m_settings;
    std::string m_config_filename = "sys_config.json";
    nlohmann::json m_j;

    std::string m_pattern = "\"(.*?)\":\"?(.*?)\"?[,|}]";
    std::regex m_r;

//    std::unordered_set<std::string> m_param_set;
};


#endif //RECEIVER_CONFIGURE_H
