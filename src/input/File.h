/*
   Copyright (C) 2009 Her Majesty the Queen in Right of Canada (Communications
   Research Center Canada)

   Copyright (C) 2022 Matthias P. Braendli
    http://www.opendigitalradio.org

   */
/*
   This file is part of ODR-DabMux.

   ODR-DabMux is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMux is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMux.  If not, see <http://www.gnu.org/licenses/>.
   */

#pragma once

#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include "input/inputs.h"
#include "ManagementServer.h"

namespace Inputs {

class FileBase : public InputBase {
    public:
        virtual void open(const std::string& name);
        virtual size_t readFrame(uint8_t *buffer, size_t size) = 0;
        virtual size_t readFrame(uint8_t *buffer, size_t size, std::time_t seconds, int utco, uint32_t tsta);
        virtual int setBitrate(int bitrate);
        virtual void close();

        virtual void setNonblocking(bool nonblock);
        virtual void setLoadEntireFile(bool load_entire_file);

    protected:
        /* Rewind the file
         * Returns -1 on failure, 0 on success
         */
        virtual ssize_t rewind();
        /* Read len bytes from the file into buf, and return
         * the number of bytes read, or -1 in case of error.
         */
        virtual ssize_t readFromFile(uint8_t* buf, size_t len);

        virtual ssize_t load_entire_file();

        // We use unix open() instead of fopen() because
        // of non-blocking I/O
        int m_fd = -1;

        std::string m_filename;
        bool m_nonblock = false;
        bool m_load_entire_file = false;
        std::vector<uint8_t> m_nonblock_buffer;

        size_t m_file_contents_offset = 0;
        std::vector<uint8_t> m_file_contents;
        bool m_file_open_alert_shown = false;
};

class MPEGFile : public FileBase {
    public:
        virtual size_t readFrame(uint8_t *buffer, size_t size);
        virtual int setBitrate(int bitrate);

    private:
        bool m_parity = false;
};

class RawFile : public FileBase {
    public:
        virtual size_t readFrame(uint8_t *buffer, size_t size);
};

class PacketFile : public FileBase {
    public:
        PacketFile(bool enhancedPacketMode);
        virtual size_t readFrame(uint8_t *buffer, size_t size);

    protected:
        std::array<uint8_t, 96> m_packetData;
        size_t m_packetLength = 0;

        /* Enhanced packet mode enables FEC for MSC packet mode
         * as described in EN 300 401 Clause 5.3.5
         */
        bool m_enhancedPacketEnabled = false;
        std::array<std::array<uint8_t, 204>,12> m_enhancedPacketData;
        size_t m_enhancedPacketWaiting = 0;
        size_t m_enhancedPacketLength = 0;
};

};
