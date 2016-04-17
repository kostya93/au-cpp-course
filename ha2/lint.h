#pragma once
#include <stdint.h>
#include <ostream>
#include <string>

namespace apa
{
	struct lint
	{
		lint();
		lint(int32_t value);
		lint(int64_t value);
		lint(double value);
		~lint();
		lint(const lint &);
		explicit lint(const std::string & str);
		explicit operator int() const;
		explicit operator double() const;
		explicit operator bool() const;
		std::string to_string() const;

		friend bool operator==(const lint & left, const lint & right);
		friend bool operator<(const lint & left, const lint & right);

		const lint operator -() const;
		const lint& operator +() const;

		//friend const lint operator+(const lint & left, const lint & right);
		//friend const lint operator-(const lint & left, const lint & right);
		//friend const lint operator*(const lint & left, const lint & right);
		//friend const lint operator/(const lint & left, const lint & right);

		lint & operator=(const lint & right);
		lint & operator+=(const lint & right);
		lint & operator-=(const lint & right);
		lint & operator*=(const lint & right);
		lint & operator/=(const lint & right);

		const lint& operator ++();
        const lint operator ++(int);
        const lint& operator --();
        const lint operator --(int);
        
        static const lint from_integer(int64_t);

		
		static const int BASE = 1000000000;
		static const int LEN_OF_BASE = 9;

	private:
		lint(uint32_t * digits, uint32_t size, bool sign);
		bool is_zero() const;
		void del_zeros();
		uint32_t count_zeros() const;
		void reallocate(uint32_t new_size);
		
		uint32_t * _digits;
		uint32_t _size;
		uint32_t _capacity;
		bool _sign;
	};


	bool operator<=(const lint & left, const lint & right);
	bool operator>(const lint & left, const lint & right);
	bool operator>=(const lint & left, const lint & right);
	bool operator!=(const lint & left, const lint & right);

	const lint operator+(const lint & left, const lint & right);
	const lint operator-(const lint & left, const lint & right);
	const lint operator*(const lint & left, const lint & right);
	const lint operator/(const lint & left, const lint & right);

	uint32_t length_int (int64_t value);
	std::ostream& operator<<(std::ostream& os, const lint& li);
	std::istream& operator>>(std::istream& os, lint& li);
	const lint abs(const lint & right);
	const lint pow(const lint & right, int value);

}
