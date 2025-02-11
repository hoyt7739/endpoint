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
