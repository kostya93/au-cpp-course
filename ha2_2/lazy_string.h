#pragma once
#include <string>
#include <memory>
#include <cctype>

namespace std_utils
{
	template <class CharT, class Traits = std::char_traits<CharT>>
	class lazy_basic_string;
}

namespace modifer
{
	template <class CharT, class Traits = std::char_traits<CharT>>
	class lazy_basic_string_modifer
	{
	public:
		operator CharT() const
		{
			return str_.get_c(pos_);
		}
		lazy_basic_string_modifer & operator=(CharT c)
		{
			str_.set_c(pos_, c);
			return *this;
		}
		
	private:
		lazy_basic_string_modifer(std_utils::lazy_basic_string<CharT, Traits> & str, size_t pos)
			: str_(str)
			, pos_(pos)
		{}
		std_utils::lazy_basic_string<CharT, Traits> & str_;
		size_t pos_;
		friend class std_utils::lazy_basic_string<CharT, Traits>;
	};
}

namespace std_utils
{
	template <class CharT, class Traits>
	class lazy_basic_string
	{
	public:

		using traits_type            = Traits;
		using value_type             = CharT;
		using reference              = CharT&;
		using const_reference        = const CharT&;
		using pointer                = CharT*;
		using const_pointer          = const CharT*;
		using iterator               = std::iterator<std::random_access_iterator_tag, CharT>;
		using const_iterator         = std::iterator<std::random_access_iterator_tag, const CharT>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using difference_type        = std::ptrdiff_t;
		using size_type              = size_t;

		lazy_basic_string()
			:ptr(nullptr)
		{}
		lazy_basic_string(const lazy_basic_string& str)
			: ptr(str.ptr)
		{}
		lazy_basic_string(const CharT * c_str)
			: ptr(new std::basic_string<CharT, Traits>(c_str))
		{}
		lazy_basic_string(size_t n, CharT c)
			: ptr(new std::basic_string<CharT, Traits>(n, c))
		{}

		lazy_basic_string(CharT c)
			: ptr(new std::basic_string<CharT, Traits>(1, c))
		{}

		lazy_basic_string & operator=(const lazy_basic_string & str)
		{
			lazy_basic_string tmp(str);
			std::swap(ptr, tmp.ptr);
			return *this;
		}

		lazy_basic_string & operator=(const lazy_basic_string && str)
		{
			ptr = std::move(str.ptr);
			return *this;
		}

		lazy_basic_string & operator+=(const lazy_basic_string & right)
		{
			lazy_basic_string<CharT, Traits> sbs((*ptr.get() + *right.ptr.get()).c_str());
			return *this = sbs;
		}

		const CharT& operator[](size_t pos) const
		{
			return get_c(pos);
		}

		modifer::lazy_basic_string_modifer<CharT, Traits> operator[](size_t pos)
		{
			return modifer::lazy_basic_string_modifer<CharT, Traits>(*this, pos);
		}

		friend bool operator==(const lazy_basic_string & left, const lazy_basic_string & right)
		{
			if (left.empty() && right.empty())
				return true;

			if (left.empty() || right.empty())
				return false;

			return *left.ptr.get() == *right.ptr.get();
		}

		friend bool operator<(const lazy_basic_string & left, const lazy_basic_string & right)
		{
			if (left.empty() && right.empty())
				return false;

			if (left.empty())
				return true;

			if (right.empty())
				return false;

			return *left.ptr.get() < *right.ptr.get();
		}

		const CharT& get_c(size_t pos) const
		{
			return (*ptr.get()).at(pos);
		}
		void set_c(size_t pos, CharT c)
		{
			if ((*ptr.get()).at(pos) == c)
				return;

			std::basic_string<CharT, Traits> tmp_bstr(*ptr.get());
			tmp_bstr[pos] = c;
			lazy_basic_string<CharT, Traits> tmp(tmp_bstr.c_str());
			*this = tmp;
		}

		void swap(lazy_basic_string & str)
		{
			std::swap(ptr, str.ptr);
		}

		void clear()
		{
			ptr = nullptr;
		}

		size_t size() const
		{
			if (!ptr)
				return 0;

			return ptr.get()->size();
		}

		bool empty() const
		{
			return size() == 0;
		}

		const CharT * c_str() const
		{
			return ptr.get()->c_str();
		}

	private:
		std::shared_ptr<std::basic_string<CharT, Traits>> ptr;
	};
	
	template <class CharT, class Traits = std::char_traits<CharT>>
	void swap(lazy_basic_string<CharT, Traits> & left, lazy_basic_string<CharT, Traits> & right)
	{
		left.swap(right);
	}


	template <class CharT, class Traits = std::char_traits<CharT>>
	const lazy_basic_string<CharT, Traits> operator+(const lazy_basic_string<CharT, Traits> & left, const lazy_basic_string<CharT, Traits> & right)
	{
		lazy_basic_string<CharT, Traits> tmp = left;
		tmp += right;
		return tmp;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	const lazy_basic_string<CharT, Traits> operator+(const lazy_basic_string<CharT, Traits> & left, const CharT * right)
	{
		lazy_basic_string<CharT, Traits> tmp = left;
		tmp += right;
		return tmp;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	const lazy_basic_string<CharT, Traits> operator+(const CharT * left, const lazy_basic_string<CharT, Traits> & right)
	{
		lazy_basic_string<CharT, Traits> tmp = left;
		tmp += right;
		return tmp;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	const lazy_basic_string<CharT, Traits> operator+(const lazy_basic_string<CharT, Traits> & left, CharT right)
	{
		lazy_basic_string<CharT, Traits> tmp = left;
		tmp += right;
		return tmp;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	const lazy_basic_string<CharT, Traits> operator+(CharT left, const lazy_basic_string<CharT, Traits> & right)
	{
		lazy_basic_string<CharT, Traits> tmp = left;
		tmp += right;
		return tmp;
	}


	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator<=(const lazy_basic_string<CharT, Traits> & left, const lazy_basic_string<CharT, Traits> & right)
	{
		return left < right || left == right;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator>(const lazy_basic_string<CharT, Traits> & left, const lazy_basic_string<CharT, Traits> & right)
	{
		return !(left <= right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator>=(const lazy_basic_string<CharT, Traits> & left, const lazy_basic_string<CharT, Traits> & right)
	{
		return !(left < right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator!=(const lazy_basic_string<CharT, Traits> & left, const lazy_basic_string<CharT, Traits> & right)
	{
		return !(left == right);
	}


	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator<=(const lazy_basic_string<CharT, Traits> & left, const CharT * right)
	{
		return left < right || left == right;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator>(const lazy_basic_string<CharT, Traits> & left, const CharT * right)
	{
		return !(left <= right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator>=(const lazy_basic_string<CharT, Traits> & left, const CharT * right)
	{
		return !(left < right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator!=(const lazy_basic_string<CharT, Traits> & left, const CharT * right)
	{
		return !(left == right);
	}


	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator<=(const CharT * left, const lazy_basic_string<CharT, Traits> &  right)
	{
		return left < right || left == right;
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator>(const CharT * left, const lazy_basic_string<CharT, Traits> &  right)
	{
		return !(left <= right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator>=(const CharT * left, const lazy_basic_string<CharT, Traits> &  right)
	{
		return !(left < right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	bool operator!=(const CharT * left, const lazy_basic_string<CharT, Traits> &  right)
	{
		return !(left == right);
	}

	typedef lazy_basic_string<char> lazy_string;
	typedef lazy_basic_string<wchar_t> lazy_wstring;

	struct ci_char_traits : public std::char_traits<char> {
		static bool eq(char c1, char c2) {
			return std::toupper(c1) == std::toupper(c2);
		}
		static bool lt(char c1, char c2) {
			return std::toupper(c1) <  std::toupper(c2);
		}
		static int compare(const char* s1, const char* s2, size_t n) {
			while (n-- != 0) {
				if (std::toupper(*s1) < std::toupper(*s2)) return -1;
				if (std::toupper(*s1) > std::toupper(*s2)) return 1;
				++s1; ++s2;
			}
			return 0;
		}
		static const char* find(const char* s, int n, char a) {
			auto const ua(std::toupper(a));
			while (n-- != 0)
			{
				if (std::toupper(*s) == ua)
					return s;
				s++;
			}
			return nullptr;
		}
	};

	typedef lazy_basic_string<char, ci_char_traits> lazy_istring;
}