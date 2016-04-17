#include "any.h"

namespace utils
{
    bad_any_cast::bad_any_cast(const std::string & msg)
    : msg_(msg)
    {}

    const char * bad_any_cast::what() const throw()
    {
        return msg_.c_str();
    }

    bad_any_cast::~bad_any_cast() throw()
    {}

    Cloneable::~Cloneable()
    {}

    any::any()
        : value_(nullptr)
    {}

    any::any(const any& right)
        : value_(right.value_->clone())
    {}

    any& any::operator=(any right)
    {
        swap(*this, right);
        return *this;
    }

    void swap(any& left, any& right)
    {
        std::swap(left.value_, right.value_);
    }

    bool any::empty() const
    {
        return !value_;
    }

    any::~any()
    {
       delete value_;
    }
}

