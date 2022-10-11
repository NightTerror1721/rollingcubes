#include "raw_buffer.h"

RawBuffer::RawBuffer(const RawBuffer& right) :
	_buffer(right._buffer == nullptr ? nullptr : new Byte[right._capacity]),
	_size(right._size),
	_capacity(right._capacity)
{
	if(_buffer != nullptr)
		std::memcpy(_buffer, right._buffer, _capacity);
}

RawBuffer::RawBuffer(RawBuffer&& right) noexcept :
	_buffer(right._buffer),
	_size(right._size),
	_capacity(right._capacity)
{
	right._buffer = nullptr;
	right._size = 0;
	right._capacity = 0;
}

void RawBuffer::clear()
{
	if (_buffer != nullptr)
		delete[] _buffer;

	_buffer = nullptr;
	_size = 0;
	_capacity = 0;
}

void RawBuffer::ensureCapacity(SizeType requestSpace)
{
	if (_buffer == nullptr)
	{
		_buffer = new Byte[DefaultCapacity];
		_capacity = DefaultCapacity;
	}

	if (_capacity - _size < requestSpace)
	{
		SizeType newCapacity = _capacity * 2;
		while (newCapacity - _size < requestSpace)
			newCapacity *= 2;

		Byte* newBuffer = new Byte[newCapacity];
		std::memcpy(newBuffer, _buffer, _size);

		_capacity = newCapacity;
		delete[] _buffer;
		_buffer = newBuffer;
	}
}

void RawBuffer::push_back(const void* data, SizeType count, SizeType size)
{
	SizeType completeSize = count * size;
	ensureCapacity(completeSize);

	std::memcpy(_buffer, data, completeSize);
	_size += completeSize;
}
