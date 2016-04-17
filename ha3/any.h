#include <iostream>
#include <string>
#include <typeinfo>
#include <type_traits>

namespace utils
{
    struct bad_any_cast : std::exception
    {
        bad_any_cast(const std::string & msg);
        const char * what() const throw();
        ~bad_any_cast() throw();
        std::string msg_;
    };

    struct Cloneable
    {
        virtual Cloneable * clone() const = 0;
        virtual ~Cloneable();
    };

    template <class T>
    struct Data : Cloneable
    {
        Data(const T& value)
            : value_(value)
        {}

        Data<T> * clone() const {
            return new Data<T>(*this);
        }
        T value_;
    };

    struct any
    {
        any();

        any(const any& right);

        template <class T>
        any(const T& val)
            : value_(new Data<T>(val))
        {}

        any& operator=(any right);

        template <class T>
        any& operator=(const T& right)
        {
            delete value_;
            value_ = new Data<T>(right);
            return *this;
        }

        friend void swap(any& left, any& right);

        template <class T>
        friend T * any_cast(any * right)
        {
            if (right->empty())
                return nullptr;

            using U = typename std::decay<T>::type;
            Data<U> *tmp = dynamic_cast<Data<U>*>(right->value_);

            if (!tmp)
                return nullptr;

            return &(tmp->value_);
        }

        template <class T>
        friend T const * any_cast(any const * right)
        {
            if (right->empty())
                return nullptr;

            using U = typename std::decay<T>::type;
            Data<U> *tmp = dynamic_cast<Data<U>*>(right->value_);

            if (!tmp)
                return nullptr;

            return &(tmp->value_);
        }

        template <class T>
        friend T any_cast(any & right)
        {
            if (right.empty())
                throw bad_any_cast(std::string("bad_any_cast to \"") + typeid(T).name() + "\";" + " Object is empty");

            using U = typename std::decay<T>::type;
            Data<U> *tmp = dynamic_cast<Data<U>*>(right.value_);
            
            if (!tmp)
                throw bad_any_cast(std::string("bad_any_cast to \"") + typeid(T).name() + "\"");

            return tmp->value_;
        }

        template <class T>
        friend T any_cast(const any & right)
        {
            using T_without_ref = typename std::remove_reference<T>::type;
            const bool flag = std::is_reference<T>::value && (!std::is_const<T_without_ref>::value);
            static_assert(!flag, "wrong const");

            if (right.empty())
                throw bad_any_cast(std::string("bad_any_cast to \"") + typeid(T).name() + " const \";" + " Object is empty");


            using U = typename std::decay<T>::type;
            Data<U> *tmp = dynamic_cast<Data<U>*>(right.value_);

            if (!tmp)
                throw bad_any_cast(std::string("bad_any_cast to \"") + typeid(T).name() + " const\"");

            return tmp->value_;
        }


        bool empty() const;

        ~any();

        Cloneable * value_;
    };
}






