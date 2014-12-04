#pragma once
#include <string>

enum
{
	OPCODE_NULL,
	OPCODE_UDP_FLOOD // opcode, ip, port, size, delay, time
};

class udp_connection;

struct OpcodeHandler
{
	std::string Name;
	void (udp_connection::*handler)();
};

extern OpcodeHandler OpcodeTable[2];
