#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <initializer_list>

namespace io
{
	using io_byte_t = char;
	using bostream = std::basic_ostream<io_byte_t>;
	using bistream = std::basic_istream<io_byte_t>;
	using biostream = std::basic_iostream<io_byte_t>;

	inline bostream& write_bin(bostream& os, const void* buffer, std::size_t size, std::size_t count = 1)
	{
		return os.write(reinterpret_cast<const bostream::char_type*>(buffer), size * count);
	}

	inline bistream& read_bin(bistream& is, void* buffer, std::size_t size, std::size_t count = 1)
	{
		return is.read(reinterpret_cast<bostream::char_type*>(buffer), size * count);
	}

	template <typename _Ty>
	inline bostream& write_obj(bostream& os, const _Ty* objs_buffer, std::size_t count = 1)
	{
		return write_bin(os, objs_buffer, sizeof(_Ty), count);
	}

	template <typename _Ty>
	inline bostream& write_obj(bostream& os, std::initializer_list<_Ty> objs)
	{
		return write_bin(os, objs.begin(), sizeof(_Ty), objs.size());
	}

	template <typename _Ty>
	inline bistream& read_obj(bistream& is, _Ty* objs_buffer, std::size_t count = 1)
	{
		return read_bin(is, objs_buffer, sizeof(_Ty), count);
	}
}
