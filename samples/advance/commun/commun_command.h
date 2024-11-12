#ifndef COMMUN_COMMAND_H
#define COMMUN_COMMAND_H

#include <string>
#include <vector>
#include "data_frame.h"

enum commun_command : data_frame::command_type {
    COMMAND_PING            = 0x0001,
    COMMAND_PONG            = 0x0100,
    COMMAND_REQ_FRAGMENT    = 0x0002,
    COMMAND_RSP_FRAGMENT    = 0x0200,
    COMMAND_REQ_INTERACT    = 0x0003,
    COMMAND_RSP_INTERACT    = 0x0300,
    COMMAND_REQ_STATE       = 0x0004,
    COMMAND_RSP_STATE       = 0x0400,
    COMMAND_REQ_ASSIGN      = 0x0005,
    COMMAND_RSP_ASSIGN      = 0x0500,
    COMMAND_REQ_REPORT      = 0x0006,
    COMMAND_RSP_REPORT      = 0x0600,
};

enum commun_answer : data_frame::byte_type {
    COMMUN_ANSWER_REJECT,
    COMMUN_ANSWER_ACCEPT
};

enum commun_state : data_frame::byte_type {
    COMMUN_STATE_INVALID,
    COMMUN_STATE_READY,
    COMMUN_STATE_ERROR
};

struct commun_interact {
    struct interact_item {
        time_t time = 0;
        std::string key;
        std::string value;
    };

    struct interact_entry {
        int index = 0;
        std::string name;
        std::string desc;
        std::vector<interact_item> items;
    };

    std::vector<interact_entry> entries;

    commun_interact() = default;
    commun_interact(const std::string& json);
    void from_json(const std::string& json);
    std::string to_json() const;
};

#endif
