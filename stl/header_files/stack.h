//
// Created by shilinkun on 2021/3/18.
//

#ifndef STL_STACK_H
#define STL_STACK_H


#include "deque.h"


namespace mystl {


    // 缺省使用 mystl::deque 作为底层容器
    template<class T, class Container = mystl::deque<T>>
//            以list作为底层容器
//    template<class T, class Container = mystl::list<T>>
    class stack {

    public:
        typedef Container container_type;

        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::reference reference;
        typedef typename Container::const_reference const_reference;

    private:
        container_type c_;

    public:
        // 构造等一系列函数
        stack() = default;

        explicit stack(size_type n) : c_(n) {

        }

        stack(size_type n, const value_type &value) : c_(n, value) {

        }


    public:

        void push(const value_type& value){
            c_.push_back(value);
        }

        reference top() {
            return c_.back();
        }

        void pop(){
            c_.pop_back();
        }

    };


}


#endif //STL_STACK_H
