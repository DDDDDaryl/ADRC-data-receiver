//
// Created by Frank Young on 2020/12/20.
//

#include "configure.h"

using json = nlohmann::json;
using namespace std;

bool configure::parse() {
    ifstream is(m_config_filename);
    if (!is) {
        cerr << "File \"" << m_config_filename << "\" is failed to open." << endl;
        system("pause");
        exit(1);
    }
    init();
    is >> m_j;
    string field, data;
    for (const auto &p : m_j) {
        auto tmp = p.dump();
        for (std::sregex_iterator it(tmp.begin(), tmp.end(), m_r), end_it; it != end_it; ++it) {
            field = it->str(1);
            data = it->str(2);
            if ( m_settings.field_list.find(field) != m_settings.field_list.end() ) {
                m_settings.field_data[field] = data;
            }
        }
    }
    m_settings.init();
    return true;
}

void configure::init() {
    m_r.assign(m_pattern);
}

const settings &configure::get_settings() const {
    return m_settings;
}

void settings::init() {
    PC_COM = field_data["PC_COM"];
    IC_COM = field_data["IC_COM"];
    MCU_config_filename = field_data["MCU_config_filename"];
    reference_filename = field_data["reference_filename"];
    reference_sample_time = stod(field_data["reference_sample_time"]);
}
