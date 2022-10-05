#pragma once

#include <unordered_set>
#include <string>
#include <algorithm>
#include <fstream>
#include <concepts>
#include <queue>
#include <vector>

#include "logger.h"


namespace utils
{
	inline std::string lower(std::string_view str)
	{
		std::string result = std::string(str);
		std::transform(str.begin(), str.end(), result.begin(),
			[](char c) { return std::tolower(c); });
		return result;
	}

	inline std::string upper(std::string_view str)
	{
		std::string result = std::string(str);
		std::transform(str.begin(), str.end(), result.begin(),
			[](char c) { return std::toupper(c); });
		return result;
	}

	inline std::size_t file_size(std::ifstream& file)
	{
		file.seekg(0, std::ios::end);
		std::size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		return size;
	}
}



template <std::integral _Ty, _Ty _InvalidValue = _Ty(0)>
class IdManager
{
public:
	using value_type = _Ty;

	static constexpr value_type invalid_id = _InvalidValue;

private:
	value_type _nextId = (invalid_id + value_type(1));
	std::queue<value_type> _unused;
	#ifdef _DEBUG
		std::unordered_set<value_type> _debugUnusedSet;
	#endif

	IdManager() = default;
	IdManager(const IdManager&) = default;
	IdManager(IdManager&&) noexcept = default;
	~IdManager() = default;

	IdManager& operator= (const IdManager&) = default;
	IdManager& operator= (IdManager&&) noexcept = default;


	value_type next()
	{
		if (!_unused.empty())
		{
			value_type id = _unused.front();
			_unused.pop();

			#ifdef _DEBUG
				_debugUnusedSet.erase(id);
			#endif

			return id;
		}

		return _nextId++;
	}

	inline void release(value_type id)
	{
		#ifdef _DEBUG
			if (_debugUnusedSet.contains(id))
			{
				logger::error("Cannot already released '{}' id.", id);
				return;
			}
		#endif
		_unused.push(id);
	}

	inline value_type operator() () { return next(); }
};


template <typename _Ty>
class IndexedVector
{
public:
	using value_type = _Ty;
	using reference = _Ty&;
	using pointer = _Ty*;
	using const_reference = const _Ty&;
	using const_pointer = const _Ty*;
	using rvalue_reference = _Ty&&;

	using container = std::vector<value_type>;
	using size_type = container::size_type;
	using iterator = container::iterator;
	using const_iterator = container::const_iterator;

	using id_type = size_type;
	using index_type = size_type;

	static constexpr id_type invalid_id = 0;

private:
	container _container;
	std::queue<id_type> _unusedIds;
	#ifdef _DEBUG
	std::unordered_set<id_type> _debugUnusedSet;
	#endif

public:
	IndexedVector() = default;
	IndexedVector(const IndexedVector&) = default;
	IndexedVector(IndexedVector&&) noexcept = default;
	~IndexedVector() = default;

	IndexedVector& operator= (const IndexedVector&) = default;
	IndexedVector& operator= (IndexedVector&&) noexcept = default;

	bool operator== (const IndexedVector&) const = default;

private:
	static constexpr id_type indexToId(index_type index) { return index + 1; }
	static constexpr index_type idToIndex(id_type id) { return id - 1; }

public:
	inline bool empty() const { return _container.empty(); }
	inline size_type size() const { return _container.size(); }

	template <typename... _ArgsTy> requires std::constructible_from<value_type, _ArgsTy...>
	id_type createNew(_ArgsTy&&... args)
	{
		if (!_unusedIds.empty())
		{
			id_type id = _unusedIds.front();
			_unusedIds.pop();

			#ifdef _DEBUG
			_debugUnusedSet.erase(id);
			#endif
			
			std::construct_at(std::addressof(_container[idToIndex(id)]), std::forward<_ArgsTy>(args)...);
			return id;
		}
		else
		{
			id_type id = indexToId(_container.size());
			_container.emplace_back<_ArgsTy...>(std::forward<_ArgsTy>(args)...);
			return id;
		}
	}

	id_type createNewFromCopy(const_reference value) requires std::copyable<value_type>
	{
		if (!_unusedIds.empty())
		{
			id_type id = _unusedIds.front();
			_unusedIds.pop();

			#ifdef _DEBUG
			_debugUnusedSet.erase(id);
			#endif

			std::construct_at<value_type, const_reference>(std::addressof(_container[idToIndex(id)]), value);
			return id;
		}
		else
		{
			id_type id = indexToId(_container.size());
			_container.emplace_back<const_reference>(value);
			return id;
		}
	}

	void destroy(id_type id)
	{
		if (idToIndex(id) < _container.size())
		{
			#ifdef _DEBUG
			if (_debugUnusedSet.contains(id))
			{
				logger::error("Cannot already released '{}' id.", id);
				return;
			}
			#endif

			std::destroy_at(std::addressof(_container[idToIndex(id)]));

			_unusedIds.push(id);

			#ifdef _DEBUG
			_debugUnusedSet.insert(id);
			#endif
		}
	}

	inline reference at(id_type id) { return _container[idToIndex(id)]; }
	inline const_reference at(id_type id) const { return _container[idToIndex(id)]; }

	inline reference operator[] (id_type id) { return _container[idToIndex(id)]; }
	inline const_reference operator[] (id_type id) const { return _container[idToIndex(id)]; }


	inline iterator begin() { return _container.begin(); }
	inline const_iterator begin() const { return _container.begin(); }
	inline const_iterator cbegin() const { return _container.cbegin(); }

	inline iterator end() { return _container.end(); }
	inline const_iterator end() const { return _container.end(); }
	inline const_iterator cend() const { return _container.cend(); }
};
