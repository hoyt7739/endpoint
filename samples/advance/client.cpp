#include "common.h"
#include "advance_client.h"

enum {
    START,
    STOP,
    SEND_COMMAND,
    REQ_INTERACT,
    RSP_INTERACT,
    REQ_STATE,
    REQ_ASSIGN,
    REQ_REPORT,
    EXIT
};

const menu_items MENU_ITEMS = {
    {START,         "START"},
    {STOP,          "STOP"},
    {SEND_COMMAND,  "SEND_COMMAND"},
    {REQ_INTERACT,  "REQ_INTERACT"},
    {RSP_INTERACT,  "RSP_INTERACT"},
    {REQ_STATE,     "REQ_STATE"},
    {REQ_ASSIGN,    "REQ_ASSIGN"},
    {REQ_REPORT,    "REQ_REPORT"},
    {EXIT,          "EXIT"}
};

int main(int argc, const char* argv[]) {
    advance_client client;
    while (true) {
        switch (input_item("menu", MENU_ITEMS)) {
        case START:
            client.start();
            break;
        case STOP:
            client.stop();
            break;
        case SEND_COMMAND:
            client.send_command();
            break;
        case REQ_INTERACT:
            client.req_interact();
            break;
        case RSP_INTERACT:
            client.rsp_interact();
            break;
        case REQ_STATE:
            client.req_state();
            break;
        case REQ_ASSIGN:
            client.req_assign();
            break;
        case REQ_REPORT:
            client.req_report();
            break;
        default:
            return 0;
        }
    }

    return 0;
}
