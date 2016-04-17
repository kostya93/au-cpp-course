#pragma once
#include <algorithm>
namespace smart_ptr
{
    struct Node
    {
        Node()
        {
            next = this;
            prev = this;
        }

        bool unique ()
        {
            return prev == this;
        }

        void insert(Node const * nc)
        {
            next = nc->next;
            prev =  const_cast<Node*> (nc);
            next->prev = this;
            prev->next = this;
        }

        void release()
        {
            prev->next = next;
            next->prev = prev;
        }

        void swap(Node * nc)
        {
            if (unique() && nc->unique())
            {
                return;
            }
            else if (unique())
            {
                next = nc->next;
                prev = nc->prev;
                prev->next = this;
                next->prev = this;
                nc->prev = nc->next = nc;
            }
            else if (nc->unique())
            {
                nc->next = next;
                nc->prev = prev;
                next->prev = nc;
                prev->next = nc;
                next = prev = this;
            }
            else
            {
                std::swap(next, nc->next);
                std::swap(prev, nc->prev);
                next->prev = this;
                prev->next = this;
                (nc->next)->prev = nc;
                (nc->prev)->next = nc;
            }
        }

        void reset()
        {
            next = this;
            prev = this;
        }

        Node * next;
        Node * prev;
    };

    template <class T>
    struct linked_ptr
    {
        explicit linked_ptr(T* ptr = 0)
            : ptr_(ptr)
        {}

        linked_ptr(const linked_ptr & clp)
        {
            insert(clp);
        }

        template <class U>
        linked_ptr(const linked_ptr<U> & clp)
        {
            insert(clp);
        }

        linked_ptr& operator=(const linked_ptr& clp)
        {
            if (this != &clp)
            {
                release();
                insert(clp);
            }
            return *this;
        }


        template <class U>
        linked_ptr& operator=(const linked_ptr<U>& clp)
        {
            release();
            insert(clp);
            return *this;
        }

        ~linked_ptr()
        {
            release();
        }
        T* get() const
        {
            return ptr_;
        }
        T* operator->() const
        {
            return ptr_;
        }
        T& operator*() const
        {
            return *ptr_;
        }

        void reset(T* ptr = 0)
        {
            release();
            node.reset();
            ptr_ = ptr;
        }
        template <class U>
        void reset(U* ptr = 0)
        {
            release();
            node.reset();
            ptr_ = ptr;
        }

        bool unique()
        {
            if (!ptr_)
                return false;

            return node.unique();
        }

        void swap(linked_ptr& lp)
        {
            std::swap(ptr_, lp.ptr_);
            node.swap(lp.node);
        }

        bool operator==(T* ptr) const {return *ptr_ == *ptr;}
        bool operator !=(T* ptr) const {return *ptr_ != *ptr;}
        bool operator<(T* ptr) const {return *ptr_ < *ptr;}
        bool operator<=(T* ptr) const {return *ptr_ <= *ptr;}
        bool operator>(T* ptr) const {return *ptr_ > *ptr;}
        bool operator>=(T* ptr) const {return *ptr_ >= *ptr;}

        bool operator<(const linked_ptr& clp) const
        {
            return *ptr_ < *clp.get();
        }
        bool operator<=(const linked_ptr& clp) const
        {
            return *ptr_ <= *clp.get();
        }
        bool operator>(const linked_ptr& clp) const
        {
            return *ptr_ > *clp.get();
        }
        bool operator>=(const linked_ptr& clp) const
        {
            return *ptr_ >= *clp.get();
        }
        bool operator==(const linked_ptr& clp) const
        {
            return *ptr_ == *clp.get();
        }
        bool operator!=(const linked_ptr& clp) const
        {
            return *ptr_ != *clp.get();
        }
        explicit operator bool() const
        {
            return (bool)ptr_;
        }

    private:
        template <class U>
        friend class linked_ptr;

        template <class U>
        void insert(const linked_ptr<U> & clp)
        {
            node.insert(&clp.node);
            ptr_ = clp.ptr_;
        }

        void release()
        {
            if (unique())
            {
                checked_delete(ptr_);
            }
            else
            {
                node.release();
            }
            ptr_ = 0;
        }


        void checked_delete(T * ptr)
        {
            typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
            (void) sizeof(type_must_be_complete);
            delete ptr;
        }

        T * ptr_;
        Node node;
    };
}
