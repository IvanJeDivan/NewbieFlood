#pragma once
#include <cstdint>
#include "opcode.h"
#include <vector>

class packet
{
	// Data container.
    std::vector<uint8_t> _buffer;
	// Read iterator.
	size_t _rit;
	public:
        packet(uint8_t opcode);
		static void Write(void * data, void * container, size_t size);
		// Data appender
		template<class T>
		void Append(T data)
		{
			size_t size = sizeof(T), oldSize = getSize();
			void * pData = reinterpret_cast<void*>(&data);
			Reserve(size);
			void * pContainer = reinterpret_cast<void*>(getData()+oldSize);
			Write(pData, pContainer, size);
		}
        // Returns value of a sort of data type.
		template<class T>
		T Read()
		{
			T data;
			if (_rit < getSize())
			{
				size_t size = sizeof(T);
				void * pContainer = reinterpret_cast<void*>(getData()+_rit);
				Write(pContainer, &data, size);
				_rit += size;
			}
			return data;
		}
		void Reserve(size_t size);
		void Finalize();
		// Size of buffer container.
		size_t getSize();
		uint8_t * getData();
		// Opcode.
		uint8_t getOpcode();
};

template<> std::string packet::Read();
template<> void packet::Append(std::string data);
