#include "packet.h"
#include <cstring>

packet::packet(uint8_t opcode)
{
	_buffer.push_back(opcode);
	_rit = 1;
};

void packet::Write(void * data, void * container, size_t size)
{
	std::memcpy(container, data, size);
}

template<> void packet::Append(std::string data)
{
    size_t size = 1, oldSize = getSize();
    size += data.size();
    void * pData = reinterpret_cast<void*>(const_cast<char*>(data.c_str()));
    Reserve(size);
    void * pContainer = reinterpret_cast<void*>(getData()+oldSize);
    Write(pData, pContainer, size);
}

template<> std::string packet::Read()
{
    std::string data;
	while (getData()[_rit])
	{
		data += char(getData()[_rit]);
		++_rit;
	}
	++_rit;
	return data;
}

void packet::Finalize()
{
	for (size_t it = 0; it < 2; ++it)
		_buffer.emplace(_buffer.begin()+1, 0);
	uint16_t size = getSize()-3;
	Write(&size, getData()+1, sizeof(uint16_t));
}

uint8_t * packet::getData()
{
	return _buffer.data();
}

size_t packet::getSize()
{
	return _buffer.size();
}

uint8_t packet::getOpcode()
{
	return _buffer[0];
}

void packet::Reserve(size_t size)
{
	_buffer.resize(getSize()+size);
}
