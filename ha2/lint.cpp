#include "lint.h"
#include <iostream>
#include <stdint.h>
#include <iomanip> 
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <cstring>

namespace apa
{
	lint::lint(int64_t value)
	{
		_sign = (value >= 0);

		if (!_sign)
			value = -value;

		uint32_t len = length_int(value);

		if (len % LEN_OF_BASE == 0)
		{
			_size = len/LEN_OF_BASE;
		}
		else
		{
			_size = len/LEN_OF_BASE + 1;
		}

		_digits = new uint32_t[_size];
		_capacity = _size;

		for (size_t i = 0; i < _size; ++i)
		{
			_digits[i] = value % BASE;
			value /= BASE;
		}
	}

	lint::lint() : lint((int64_t)0)
	{}
	
	lint::lint(int32_t value) : lint((int64_t)value)
	{}
	

	lint::lint(double value)
	{
		std::string str = std::to_string(value);
		bool sign_exist = false;
		if (str[0] == '-' || str[0] == '+')
		{
			sign_exist = true;
			if (str[0] == '-')
				_sign = false;
		}
		std::string dot = ".";
		size_t ind = str.find(dot);
		str = str.substr(0, ind);
		for (size_t i = 15 + sign_exist; i < str.size(); ++i)
		{
			str[i] = '0';
		}
		lint tmp(str);
		_sign = tmp._sign;
		_size = tmp._size;
		_capacity = tmp._capacity;
		_digits = new uint32_t[_size];
		memcpy(_digits, tmp._digits, _size*sizeof(uint32_t));
	}

	lint::lint(const std::string & str)
	{
		uint32_t len = str.size();

		_sign = true;

		if (str[0] == '-')
		{
			--len;
			_sign = false;
		}
		else if (str[0] == '+')
		{
			--len;
		}

		if (len % LEN_OF_BASE == 0)
		{
			_size = len/LEN_OF_BASE;
		}
		else
		{
			_size = len/LEN_OF_BASE + 1;
		}
		_digits = new uint32_t[_size];
		_capacity = _size;

		for (size_t i = 1; i < _size; ++i)
		{
			_digits[i - 1] = atoi( (str.substr(str.size() - LEN_OF_BASE*i, LEN_OF_BASE)).c_str() );
		}


		if (len % LEN_OF_BASE == 0)
			_digits[_size - 1] = atoi( (str.substr(1 - isdigit(str[0]), LEN_OF_BASE)).c_str() );
		else
			_digits[_size - 1] = atoi( (str.substr(1 - isdigit(str[0]), len % LEN_OF_BASE)).c_str() );

		del_zeros();

	}
	
	lint::~lint()
	{
		delete[] _digits;
	}

	lint::lint(const lint & li)
	{
		_digits = new uint32_t[li._size];
		_size = li._size;
		_capacity = _size;
		_sign = li._sign;
		memcpy(_digits, li._digits, _size*sizeof(uint32_t));
	}

	lint::operator int() const
	{
		int rez = 0;
		std::stringstream stream;
    	stream << *this;
    	stream >> rez;
    	return rez;
	}

	lint::operator double() const
	{
		double rez = 0;
		std::stringstream stream;
    	stream << *this;
    	stream >> rez;
		return rez;
	}

	lint::operator bool() const
	{
		return  *this != (lint)0;
	}


	uint32_t lint::count_zeros() const
	{
		uint32_t num_of_zeros = 0;
		for (size_t i = 0; i < _size; ++i)
		{
			if (_digits[_size - i - 1] == 0)
			{
				num_of_zeros++;
			}
			else
			{
				break;
			}
		}
		return num_of_zeros;
	}


	void lint::reallocate(uint32_t new_size)
	{
		if (new_size == 0)
			new_size = 1;

		uint32_t * tmp = new uint32_t[new_size];
		memcpy(tmp, _digits, new_size*sizeof(uint32_t));
		delete[] _digits;
		_digits = tmp;
		_size = new_size;
	}

	void lint::del_zeros()
	{
		reallocate(_size - count_zeros());
	}

	uint32_t length_int(int64_t value)
	{
		uint32_t rez = 1;
		while (value / 10 != 0)
		{
			rez++;
			value /= 10;
		}
		return rez;
	}
	

	std::string lint::to_string() const
	{
		
		std::string rez = "";
		std::stringstream os;
		if(!_sign)
			os << "-";

		os << _digits[_size - 1];

		for (uint32_t i = 1; i <= _size - 1; ++i)
		{
			uint32_t dig = _digits[_size - i - 1];

			uint32_t len_dig = length_int(dig);

			for (int i = 9; i > (int64_t)len_dig; --i)
			{
				os << '0';
			}

			os << dig;

		}
		os >> rez;
		return rez;
	}

	bool operator==(const lint & left, const lint & right)
	{
		if (left._digits == right._digits)
			return true;

		if (left._sign != right._sign || left._size != right._size)
		{
			if (!left.is_zero() || !right.is_zero())
				return false;
		}
		else
		{
			for (size_t i = 0; i < left._size; ++i)
			{
				if (left._digits[i] != right._digits[i])
					return false;
			}
		}

		return true;
	}


	bool operator<(const lint & left, const lint & right)
	{
		if(left._digits == right._digits)
			return false;

		if (left.is_zero() && right.is_zero())
			return false;

		if (!left._sign && right._sign)
			return true;

		if (left._sign && !right._sign)
			return false;

		if (left._size < right._size)
			return left._sign;

		if (left._size > right._size)
			return !left._sign;

		for (size_t i = 0; i < left._size; ++i)
		{
			if (left._digits[left._size - i - 1] == right._digits[left._size - i - 1])
				continue;

			if (left._digits[left._size - i - 1] < right._digits[left._size - i - 1])
				return left._sign;
			else
				return !left._sign;
		}
		return false;
	}

	bool operator<=(const lint & left, const lint & right)
	{
		return (left == right || left < right);
	}

	bool operator>(const lint & left, const lint & right)
	{
		return !(left <= right);
	}

	bool operator>=(const lint & left, const lint & right)
	{
		return (left == right || left > right);
	}

	bool operator!=(const lint & left, const lint & right)
	{
		return !(left == right);
	}



	const lint lint::operator -() const
	{
		lint rez (*this);
		rez._sign = !(rez._sign);
		return rez;
	}

	const lint& lint::operator +() const
	{
		return *this;
	}
	/*
	const lint operator+(const lint & left, const lint & right)
	{
		if (!left._sign && !right._sign)
		{
			return -( (-left) + (-right));
		}

		if (!left._sign && right._sign)
		{
			return right - (-left) ;
		}

		if (left._sign && !right._sign)
		{
			return left - (-right);
		}

		uint32_t max_size = (left._size > right._size ? left._size : right._size);
		uint32_t new_capacity = max_size + 1;

		uint32_t * new_digigts = new uint32_t[new_capacity];

		bool flag = 0;

		for (size_t i = 0; i < max_size; ++i)
		{
			uint32_t l = left._size > i ? left._digits[i] : 0;
			uint32_t r = right._size > i ? right._digits[i] : 0;
			new_digigts[i] = l + r + flag;
			if (new_digigts[i] >= lint::BASE)
			{
				flag = 1;
				new_digigts[i] -= lint::BASE;
			}
			else
			{
				flag = 0;
			}
		}
		
		new_digigts[new_capacity - 1] = flag;
		
		lint rez = lint(new_digigts, new_capacity, true);
		rez.del_zeros();
		return rez;
	}
	*/
	const lint operator+(const lint & left, const lint & right)
	{
		lint rez(left);
		return rez+=right;
	}
	/*
	const lint operator-(const lint & left, const lint & right)
	{
		if (left == right)
		{
			lint rez(0);
			return rez;
		}

		if (!left._sign && !right._sign)
		{
			return (-right) - (-left);
		}

		if ((!left._sign && right._sign))
		{
			return -( (-left) + right );
		}

		if ((left._sign && !right._sign))
		{
			return  left + (-right);
		}

		uint32_t max_size = (left._size > right._size ? left._size : right._size);
		uint32_t * new_digigts = new uint32_t[max_size];

		bool flag = 0;
		bool comp = left > right;

		for (size_t i = 0; i < max_size; ++i)
		{
			uint32_t l;
			uint32_t r;

			if (comp)
			{
				l = left._size > i ? left._digits[i] : 0;
				r = right._size > i ? right._digits[i] : 0;
			}
			else
			{
				l = right._size > i ? right._digits[i] : 0;
				r = left._size > i ? left._digits[i] : 0;
			}

			int32_t tmp = l - r - flag;
			
			
			if (tmp < 0)
			{
				flag = 1;
				tmp += lint::BASE;
			}
			else
			{
				flag = 0;
			}
			new_digigts[i] = uint32_t(tmp);
		}
		lint rez(new_digigts, max_size, comp);
		rez.del_zeros();
		return rez;

	}
	*/
	const lint operator-(const lint & left, const lint & right)
	{
		lint rez(left);
		return rez -= right;
	}
	/*
	const lint operator*(const lint & left, const lint & right)
	{
		uint32_t rez_size = left._size + right._size;
		uint32_t * new_digigts = new uint32_t[rez_size];

		for (size_t i = 0; i < rez_size; ++i)
			new_digigts[i] = 0;

		for (size_t i = 0; i < left._size; ++i)
		{
			uint32_t carry = 0;
			for (size_t j = 0; j < right._size; ++j)
			{
				uint64_t tmp = new_digigts[i + j] + ((uint64_t)left._digits[i])*right._digits[j] + carry;
				new_digigts[i + j] = (uint32_t)(tmp % lint::BASE);
				carry = (uint32_t)(tmp / lint::BASE);
			}
			new_digigts[i + right._size] += carry;
		}
		bool sign = (!left._sign && !right._sign) || (left._sign && right._sign);
		lint rez(new_digigts, rez_size, sign);
		rez.del_zeros();
		return rez;
	}
	*/
	const lint operator*(const lint & left, const lint & right)
	{
		lint rez(left);
		return rez *= right;
	}
	/*
	const lint operator/(const lint & left, const lint & right)
	{
		if (right == (lint)0)
		{
			int x = 0;
			return (lint)(3/x);
		}

		uint32_t * new_digigts = new uint32_t[left._size];
		lint lint_base(lint::BASE);
		lint current(0);
		lint abs_right = abs(right);

		for (int64_t i = left._size - 1; i >= 0; --i)
		{

			current = current * lint_base;
			current._digits[0] = left._digits[i];
			current.del_zeros();

			
			int dig = 0;
			int l = 0;
			int r = lint::BASE;
			while(l <= r)
			{
				int m = l + (r - l)/2;
				lint tmp = abs_right * (lint)m;
				if (tmp <= current)
				{
					dig = m;
					l = m + 1;
				}
				else
				{
					r = m - 1;
				}
			}
			new_digigts[i] = dig;
			current = current - abs_right * (lint)dig;
			
		}

		bool sign = (!left._sign && !right._sign) || (left._sign && right._sign);
		lint rez(new_digigts, left._size, sign);
		rez.del_zeros();
		return rez;

	}
	*/
	const lint operator/(const lint & left, const lint & right)
	{
		lint rez(left);
		return rez /= right;
	}
	/*
	lint & lint::operator+=(const lint & right)
	{
		*this = *this + right;
		return *this;
	}
	*/

	lint & lint::operator+=(const lint & right)
	{
		if (!_sign && !right._sign)
		{
			return *this = -( (-*this) + (-right));
		}

		if (!_sign && right._sign)
		{
			return *this = right - (-*this) ;
		}

		if (_sign && !right._sign)
		{
			return *this = *this - (-right);
		}

		uint32_t max_size = (_size > right._size ? _size : right._size);
		uint32_t new_capacity = max_size + 1;

		uint32_t * new_digigts = new uint32_t[new_capacity];

		bool flag = 0;

		for (size_t i = 0; i < max_size; ++i)
		{
			uint32_t l = _size > i ? _digits[i] : 0;
			uint32_t r = right._size > i ? right._digits[i] : 0;
			new_digigts[i] = l + r + flag;
			if (new_digigts[i] >= lint::BASE)
			{
				flag = 1;
				new_digigts[i] -= lint::BASE;
			}
			else
			{
				flag = 0;
			}
		}
		
		new_digigts[new_capacity - 1] = flag;
		
		_size = new_capacity;
		_capacity = new_capacity;
		delete[] _digits;
		_digits = new_digigts;
		del_zeros();
		return *this;
	}
	/*
	lint & lint::operator-=(const lint & right)
	{
		*this = *this - right;
		return *this;
	}
	*/

	lint & lint::operator-=(const lint & right)
	{
		if (!_sign && !right._sign)
		{
			return *this = (-right) - (-*this);
		}

		if ((!_sign && right._sign))
		{
			return *this = -( (-*this) + right );
		}

		if ((_sign && !right._sign))
		{
			return  *this = *this + (-right);
		}

		uint32_t max_size = (_size > right._size ? _size : right._size);
		uint32_t * new_digigts = new uint32_t[max_size];

		bool flag = 0;
		bool comp = *this > right;

		for (size_t i = 0; i < max_size; ++i)
		{
			uint32_t l;
			uint32_t r;

			if (comp)
			{
				l = _size > i ? _digits[i] : 0;
				r = right._size > i ? right._digits[i] : 0;
			}
			else
			{
				l = right._size > i ? right._digits[i] : 0;
				r = _size > i ? _digits[i] : 0;
			}

			int32_t tmp = l - r - flag;
			
			
			if (tmp < 0)
			{
				flag = 1;
				tmp += lint::BASE;
			}
			else
			{
				flag = 0;
			}
			new_digigts[i] = uint32_t(tmp);
		}
		delete[] _digits;
		_digits = new_digigts;
		_size = max_size;
		_capacity = max_size;
		del_zeros();
		return *this;
	}
	
	/*
	lint & lint::operator-=(const lint & right)
	{
		if (left == right)
		{
			lint rez(0);
			return rez;
		}

		if (!left._sign && !right._sign)
		{
			return (-right) - (-left);
		}

		if ((!left._sign && right._sign))
		{
			return -( (-left) + right );
		}

		if ((left._sign && !right._sign))
		{
			return  left + (-right);
		}

		uint32_t max_size = (left._size > right._size ? left._size : right._size);
		uint32_t * new_digigts = new uint32_t[max_size];

		bool flag = 0;
		bool comp = left > right;

		for (size_t i = 0; i < max_size; ++i)
		{
			uint32_t l;
			uint32_t r;

			if (comp)
			{
				l = left._size > i ? left._digits[i] : 0;
				r = right._size > i ? right._digits[i] : 0;
			}
			else
			{
				l = right._size > i ? right._digits[i] : 0;
				r = left._size > i ? left._digits[i] : 0;
			}

			int32_t tmp = l - r - flag;
			
			
			if (tmp < 0)
			{
				flag = 1;
				tmp += lint::BASE;
			}
			else
			{
				flag = 0;
			}
			new_digigts[i] = uint32_t(tmp);
		}
		lint rez(new_digigts, max_size, comp);
		rez.del_zeros();
		return rez;	
	}
	*/

	/*
	lint & lint::operator/=(const lint & right)
	{
		*this = *this / right;
		return *this;
	}
	*/
	lint & lint::operator/=(const lint & right)
	{
		if (right == (lint)0)
		{
			int x = 0;
			int b = 3/x;
			*this = *this + b;
			return *this;
		}

		uint32_t * new_digigts = new uint32_t[_size];
		lint lint_base(lint::BASE);
		lint current(0);
		lint abs_right = abs(right);

		for (int64_t i = _size - 1; i >= 0; --i)
		{

			current = current * lint_base;
			current._digits[0] = _digits[i];
			current.del_zeros();

			
			int dig = 0;
			int l = 0;
			int r = lint::BASE;
			while(l <= r)
			{
				int m = l + (r - l)/2;
				lint tmp = abs_right * (lint)m;
				if (tmp <= current)
				{
					dig = m;
					l = m + 1;
				}
				else
				{
					r = m - 1;
				}
			}
			new_digigts[i] = dig;
			current = current - abs_right * (lint)dig;
			
		}

		_sign = (!_sign && !right._sign) || (_sign && right._sign);
		delete[] _digits;
		_digits = new_digigts;
		del_zeros();
		return *this;
	}
	/*
	lint & lint::operator*=(const lint & right)
	{
		*this = *this * right;
		return *this;
	}
	*/
	lint & lint::operator*=(const lint & right)
	{
		uint32_t rez_size = _size + right._size;
		uint32_t * new_digigts = new uint32_t[rez_size];

		for (size_t i = 0; i < rez_size; ++i)
			new_digigts[i] = 0;

		for (size_t i = 0; i < _size; ++i)
		{
			uint32_t carry = 0;
			for (size_t j = 0; j < right._size; ++j)
			{
				uint64_t tmp = new_digigts[i + j] + ((uint64_t)_digits[i])*right._digits[j] + carry;
				new_digigts[i + j] = (uint32_t)(tmp % lint::BASE);
				carry = (uint32_t)(tmp / lint::BASE);
			}
			new_digigts[i + right._size] += carry;
		}
		_sign = (!_sign && !right._sign) || (_sign && right._sign);

		delete[] _digits;
		_digits = new_digigts;
		_size  = rez_size;
		del_zeros();
		return *this;
	}

	lint & lint::operator=(const lint & right)
	{
		if (_digits == right._digits)
			return *this;

		delete[] _digits;

		_size = right._size;
		_digits = new uint32_t[_size];
		_sign = right._sign;

		memcpy(_digits, right._digits, _size*sizeof(uint32_t));
		return *this;
	}


	const lint& lint::operator ++()
	{
		return (*this += (lint)1 );
	}

	const lint& lint::operator --()
	{
		return (*this -= (lint)1);
	}

	const lint lint::operator ++(int)
	{
		lint tmp(*this);
		*this += (lint)1;
		return tmp;
	}

	const lint lint::operator --(int)
	{
		lint tmp(*this);
		*this -= (lint)1;
		return tmp;
	}

	const lint lint::from_integer(int64_t value)
	{
		lint tmp(value);
		return tmp;
	}

	const lint pow(const lint & right, int value)
	{
		if (value == 0)
			return (lint)1;

		if (value == 1)
			return right;

		if (value % 2 == 0)
			return pow(right * right, value / 2);
		else
			return right * pow(right, value - 1);
	}

	const lint abs(const lint & right)
	{
		return right >= lint(0) ? right : -right;
	}

	lint::lint(uint32_t * digits, uint32_t size, bool sign)
	: _digits(digits)
	, _size(size)
	, _capacity(size)
	, _sign(sign)
	{}

	bool lint::is_zero() const
	{
		for (size_t i = 0; i < _size; ++i)
		{
			if (_digits[i] != 0)
				return false;
		}
		return true;
	}

	std::ostream& operator<<(std::ostream& os,const lint& li)
	{
		os << li.to_string();
		return os;
	}

	std::istream& operator>>(std::istream& os, lint& li)
	{
		std::string str;
		os >> str;
		lint tmp(str);
		li = tmp;
		return os;
	}
	
}