#ifndef STRING_VIEW_H
#define STRING_VIEW_H
#include <string>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <iosfwd>

namespace std 
{

template<class CharT, class Traits = std::char_traits<CharT>>
class basic_string_view
{

private:
	typedef Traits traits_type;
	typedef CharT value_type;
	typedef CharT* pointer;
	typedef const CharT* const_pointer;
	typedef CharT& reference;
	typedef const CharT& const_reference;
	typedef const char* const_iterator;
	typedef const_iterator iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef const_reverse_iterator reverse_iterator;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	const_pointer _data;
	size_type _size;
public:
	const static size_type npos = size_type(-1);
public:
	basic_string_view() { _data = nullptr; _size = 0; }
	basic_string_view(const basic_string_view& rhs) { _data = rhs._data; _size = rhs._size; }
	basic_string_view(const_pointer c_str, size_type count) { _data = c_str; _size = count; }
	basic_string_view(const_pointer c_str) { _data = c_str; _size = strlen(c_str); }
	template<class It, class End>
	basic_string_view(It first, End last) 
	{
		_data = &(*first);
		_size = last - first;
	}

	explicit basic_string_view(const std::basic_string<CharT>& str) 
	{
		_data = str.c_str();
		_size = str.size();
	}

	basic_string_view& operator=(const basic_string_view& rhs) 
	{
		_data = rhs._data;
		_size = rhs._size;
		return *this;
	}

	explicit operator std::basic_string<CharT>() const
	{
		return std::basic_string<CharT>(_data, _size);
	}


	iterator begin() { return _data; }
	const_iterator cbegin() { return begin(); }

	iterator end() { return _data + _size; }
	const_iterator cend() { return end(); }

	reverse_iterator rbegin() { return reverse_iterator(begin()); }
	const_reverse_iterator crbegin() { return const_reverse_iterator(begin()); }

	reverse_iterator rend() { return reverse_iterator(end()); }
	const_reverse_iterator crend() { return const_reverse_iterator(end()); }

	value_type operator[](size_type index) const { return _data[index]; }
	value_type at(size_type index) const 
	{
		if (index >= _size)
			throw std::out_of_range("Index out of bounds");
		return _data[index];
	}

	value_type front() const { return _data[0]; }
	value_type back() const { return _data[_size - 1]; }

	const_pointer data() const { return _data; }

	size_type size() const { return _size; }
	size_type length() const { return size(); }

	size_type max_size() const { return std::basic_string<CharT>().max_size(); }
	bool empty() const { return !size(); }


	void remove_prefix(size_t n) { _data += n; }
	void remove_suffix(size_t n) { _size -= n; }
	void swap(basic_string_view& rhs) 
	{
		std::swap(_data, rhs._data);
		std::swap(_size, rhs._size);
	}

	size_t copy(pointer dest, size_type count, size_type pos = 0) const
	{

		count += pos;
		size_type i = pos;

		if (pos >= size() || count >= size())
			throw std::out_of_range("pos out of bounds");

		for (; i < count; i++)
			dest[i] = _data[i];
		return i - pos;
	}
	basic_string_view substr(size_type pos = 0, size_type count = npos) const 
	{
		count = min(count, _size);

		return basic_string_view(_data + pos, count);
	}

	int compare(basic_string_view other) const 
	{
		return traits_type::compare(_data, other._data, other._size);
	}
	int compare(size_type pos1, size_type count1, basic_string_view other) const 
	{
		return substr(pos1, count1).compare(other);
	}
	int compare(size_type pos1, size_type count1, basic_string_view other, size_type pos2, size_type count2) const 
	{
		const basic_string_view a = substr(pos1, count1);
		const basic_string_view b = substr(pos2, count2);

		return a.compare(b);
	}
	int compare(const_pointer cstr) const 
	{
		return compare(basic_string_view(cstr));
	}
	int compare(size_type pos1, size_type count1, const_pointer cstr) const 
	{
		return substr(pos1, count1).compare(basic_string_view(cstr));
	}
	int compare(size_type pos1, size_type count1, const_pointer cstr, size_type count2) const 
	{
		return substr(pos1, count1).compare(basic_string_view(cstr, count2));
	}
	
	bool starts_with(basic_string_view other) const 
	{
		return substr(0, other.size()) == other;
	}
	bool starts_with(CharT ch) const 
	{
		return !empty() && traits_type::eq(front(), ch);
	}

	bool starts_with(const CharT* cstr) const 
	{
		return starts_with(basic_string_view(cstr));
	}

	bool ends_with(basic_string_view other)const 
	{
		return size() >= other.size() && compare(size() - other.size(), npos, other) == 0;
	}
	bool ends_with(CharT ch)const 
	{
		return !empty() && traits_type::eq(back(), ch);
	}
	bool ends_with(const CharT* cstr)const 
	{
		return ends_with(basic_string_view(cstr));
	}

	bool contains(basic_string_view other) const 
	{
		return find(other) != npos;
	}
	bool contains(CharT ch) const 
	{
		return find(ch) != npos;
	}
	bool contains(const CharT* cstr) const 
	{
		return find(cstr) != npos;
	}

	size_type find(basic_string_view other, size_type pos = 0) const
	{
		// TODO: optimization, we could stop looking when the remaining
		// string is smaller than the target string
		for(size_type i = pos; i < size(); i++)
		{
			if (traits_type::eq(_data[i], other.front())) 
			{
				size_type j = 0;
				while (j < other.size() && 
					traits_type::eq(_data[i + j], other[j]))
					j++;
				
				if (j == other.size()) 
					return i;
			}
		}

		return npos;
	}
	size_type find(CharT ch, size_type pos = 0) const
	{
		for (int i = pos; i < _size; i++)
			if (traits_type::eq(_data[i], ch))
				return i;
		return npos;
	}
	size_type find(const CharT* cstr, size_type pos = 0) const
	{
		return find(basic_string_view(cstr), pos);
	}
	size_type find(const CharT* cstr, size_type pos, size_type count) const 
	{
		return find(basic_string_view(cstr, count), pos);
	}

	size_type rfind(basic_string_view other, size_type pos = npos) const 
	{
		pos = std::min(_size - 1, pos);
		pos += other.size();

		for (size_type i = pos; i >= 0; i--) 
		{
			if (substr(i) == other)
				return i;
		}
		return npos;
	}
	size_type rfind(CharT ch, size_type pos = npos) const 
	{
		pos = std::min(_size - 1, pos);
		for (; pos >= 0; pos--)
			if (Traits::eq(_data[pos], ch))
				return pos;
		return npos;
	}
	size_type rfind(const CharT* cstr, size_type pos = npos) const 
	{
		return rfind(basic_string_view(cstr), pos);
	}
	size_type rfind(const CharT* cstr, size_type pos, size_type count) const 
	{
		return rfind(basic_string_view(cstr, count), pos);
	}

	size_type find_first_of(basic_string_view other, size_type pos = 0) const;
	size_type find_first_of(CharT ch, size_type pos = 0) const;
	size_type find_first_of(const CharT* cstr, size_type pos = 0) const;
	size_type find_first_of(const CharT* cstr, size_type pos, size_type count) const;

	size_type find_last_of(basic_string_view other, size_type pos = 0) const;
	size_type find_last_of(CharT ch, size_type pos = 0) const;
	size_type find_last_of(const CharT* cstr, size_type pos = 0) const;
	size_type find_last_of(const CharT* cstr, size_type pos, size_type count) const;
	
	size_type find_first_not_of(basic_string_view other, size_type pos = 0) const;
	size_type find_first_not_of(CharT ch, size_type pos = 0) const;
	size_type find_first_not_of(const CharT* cstr, size_type pos = 0) const;
	size_type find_first_not_of(const CharT* cstr, size_type pos, size_type count) const;
	size_type find_last_not_of(basic_string_view other, size_type pos = 0) const;
	size_type find_last_not_of(CharT ch, size_type pos = 0) const;
	size_type find_last_not_of(const CharT* cstr, size_type pos = 0) const;
	size_type find_last_not_of(const CharT* cstr, size_type pos, size_type count) const;
	
	
	friend std::ostream& operator<<(std::ostream& os, basic_string_view sv)
	{
		for (size_type i = 0; i < sv.size(); i++)
			os << sv[i];
		return os;
	}
};


template<class CharT, class Traits = std::char_traits<CharT>> 
bool operator==(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) 
{
	return lhs.compare(rhs) == 0; 
}

template<class CharT, class Traits = std::char_traits<CharT>> 
bool operator!=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) 
{
	return lhs.compare(rhs) != 0; 
}

template<class CharT, class Traits = std::char_traits<CharT>> 
bool operator<(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs)  
{
	return lhs.compare(rhs) <  0; 
}

template<class CharT, class Traits = std::char_traits<CharT>> 
bool operator>(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs)  
{
	return lhs.compare(rhs) >  0;
}

template<class CharT, class Traits = std::char_traits<CharT>> 
bool operator<=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) 
{
	return lhs.compare(rhs) <= 0;
}

template<class CharT, class Traits = std::char_traits<CharT>> 
bool operator>=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) 
{
	return lhs.compare(rhs) >= 0;
}


typedef basic_string_view<char> string_view;
typedef basic_string_view<wchar_t> wstring_view;
#if __cpp_char8_t
typedef basic_string_view<std::char8_t> u8string_view;
#endif
#if __cpp_char32_t
typedef basic_string_view<std::char32_t> u32string_view;
#endif
}

#endif // !STRING_VIEW_H

