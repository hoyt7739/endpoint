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

#ifndef WS_MESSAGE_H
#define WS_MESSAGE_H

#include <functional>
#include <string>

enum ws_opcode {

    WS_OPCODE_CONTINUE  = 0x0,
    WS_OPCODE_TEXT      = 0x1,
    WS_OPCODE_BINARY    = 0x2,
    WS_OPCODE_CLOSE     = 0x8,
    WS_OPCODE_PING      = 0x9,
    WS_OPCODE_PONG      = 0xA
};

using pack_output = std::function<void(const char* message, int size)>;
using unpack_handshake_output = std::function<void(bool accept, const std::string& key)>;
using unpack_rhandshake_output = std::function<void(bool accept)>;
using unpack_frame_output = std::function<void(ws_opcode opcode, const char* payload, int size)>;

int pack_handshake(const std::string& host, const std::string& key, pack_output output);
int unpack_handshake(const char* message, int size, unpack_handshake_output output);

int pack_rhandshake(const std::string& key, pack_output output);
int unpack_rhandshake(const char* message, int size, const std::string& key, unpack_rhandshake_output output);

int pack_frame(ws_opcode opcode, bool mask, const char* payload, int size, pack_output output);
int unpack_frame(const char* message, int size, unpack_frame_output output);

#endif
