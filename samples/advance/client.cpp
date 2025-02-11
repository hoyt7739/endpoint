/*
  MIT License

  Copyright (c) 2025 Kong Pengsheng

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

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
