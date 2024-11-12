#include "common.h"
#include "advance_server.h"

enum {
    START,
    STOP,
    SEND_COMMAND,
    REQ_INTERACT,
    RSP_INTERACT,
    RSP_STATE,
    RSP_ASSIGN,
    RSP_REPORT,
    EXIT
};

const menu_items MENU_ITEMS = {
    {START,         "START"},
    {STOP,          "STOP"},
    {SEND_COMMAND,  "SEND_COMMAND"},
    {REQ_INTERACT,  "REQ_INTERACT"},
    {RSP_INTERACT,  "RSP_INTERACT"},
    {RSP_STATE,     "RSP_STATE"},
    {RSP_ASSIGN,    "RSP_ASSIGN"},
    {RSP_REPORT,    "RSP_REPORT"},
    {EXIT,          "EXIT"}
};

int main(int argc, const char* argv[]) {
    advance_server server;
    while (true) {
        switch (input_item("menu", MENU_ITEMS)) {
        case START:
            server.start();
            break;
        case STOP:
            server.stop();
            break;
        case SEND_COMMAND:
            server.send_command();
            break;
        case REQ_INTERACT:
            server.req_interact();
            break;
        case RSP_INTERACT:
            server.rsp_interact();
            break;
        case RSP_STATE:
            server.rsp_state();
            break;
        case RSP_ASSIGN:
            server.rsp_assign();
            break;
        case RSP_REPORT:
            server.rsp_report();
            break;
        default:
            return 0;
        }
    }

    return 0;
}
