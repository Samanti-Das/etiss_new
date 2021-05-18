/**

        @copyright

        <pre>

        Copyright 2018 Infineon Technologies AG

        This file is part of ETISS tool, see <https://github.com/tum-ei-eda/etiss>.

        The initial version of this software has been created with the funding support by the German Federal
        Ministry of Education and Research (BMBF) in the project EffektiV under grant 01IS13022.

        Redistribution and use in source and binary forms, with or without modification, are permitted
        provided that the following conditions are met:

        1. Redistributions of source code must retain the above copyright notice, this list of conditions and
        the following disclaimer.

        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
        and the following disclaimer in the documentation and/or other materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
        or promote products derived from this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
        WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
        PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
        DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
        PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
        HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
        NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
        POSSIBILITY OF SUCH DAMAGE.

        </pre>

        @author Marc Greim <marc.greim@mytum.de>, Chair of Electronic Design Automation, TUM

        @date September 2, 2014

        @version 0.1

*/
/**
        @file

        @brief

        @detail




*/

#ifndef ETISS_INCLUDE_GDB_GDBCONNECTION_H_
#define ETISS_INCLUDE_GDB_GDBCONNECTION_H_

#include <string>

namespace etiss
{

namespace plugin
{

namespace gdb
{

class Connection;

/**
        @brief implements gdb's packet protocol
*/
class PacketProtocol
{
    friend class Connection;

  private:
    PacketProtocol(Connection &connection);

  public:
    virtual bool available(bool block = false);
    virtual std::string rcv(bool &isnotification);
    virtual bool snd(std::string answer, bool isnotification);

  private:
    virtual bool _available(bool block);
    virtual void tryReadPacket();
    std::string buffer;
    std::string command;
    bool command_isnotification;
    Connection &con;
    bool cfg_noack_;
};

/**
        @brief interface for gdb connections. implemented by UnixTCPGDBConnection.h . use PacketProtocol
   (Connection::getPacketProtocol) for communication with gdb
*/
class Connection
{
    friend class PacketProtocol;

  public:
    Connection();
    virtual ~Connection();
    virtual bool available() = 0;
    virtual std::string rcv() = 0;
    virtual bool snd(std::string answer) = 0;
    virtual PacketProtocol &getPacketProtocol();
    virtual bool isRelyable();
    virtual bool pendingBREAK();
    virtual void clearBREAK();

  protected:
    bool pending_break_;

  private:
    PacketProtocol packproc_;
};

} // namespace gdb

} // namespace plugin

} // namespace etiss

#endif