#pragma once

#include <memory>
#include <utility>
#include <vector>


class RawBuffer
{
public:
	using Byte = std::byte;
	using SizeType = std::size_t;

private:
	static constexpr SizeType DefaultCapacity = 1024;

private:
	Byte* _buffer = nullptr;
	SizeType _size = 0;
	SizeType _capacity = 0;

public:
	RawBuffer() = default;
	RawBuffer(const RawBuffer& right);
	RawBuffer(RawBuffer&& right) noexcept;

	inline RawBuffer& operator= (const RawBuffer& right)
	{
		std::destroy_at(this);
		return *std::construct_at(this, right);
	}

	inline RawBuffer& operator= (RawBuffer&& right) noexcept
	{
		std::destroy_at(this);
		return *std::construct_at(this, std::move(right));
	}

	inline ~RawBuffer() { clear(); }

public:
	void clear();

	void push_back(const void* data, SizeType count, SizeType size = 1);

public:
	constexpr bool empty() const { return _size == 0; }
	constexpr SizeType size() const { return _size; }
	constexpr SizeType capacity() const { return _capacity; }

	constexpr SizeType getFreeSpaceCount() const { return _capacity - _size; }

	template <typename _Ty = void>
	constexpr _Ty* data() { return reinterpret_cast<_Ty*>(_buffer); }

	template <typename _Ty = void>
	constexpr const _Ty* data() const { return reinterpret_cast<const _Ty*>(_buffer); }

public:
	template <typename _Ty>
	inline void push_back(const _Ty* data, SizeType count) { push_back(reinterpret_cast<const void*>(data), count, sizeof(_Ty)); }

	template <typename _Ty>
	inline void push_back(const _Ty& data) { push_back<_Ty>(std::addressof(data), 1); }

	template <typename _Ty>
	inline void push_back(const std::vector<_Ty>& vector) { push_back<_Ty>(vector.data(), vector.size()); }

private:
	void ensureCapacity(SizeType requestSpace);
};
