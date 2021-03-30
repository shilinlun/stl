//
// Created by shi on 2021/2/13.
//

#ifndef STL_CONSTRUCT_H
#define STL_CONSTRUCT_H

#endif //STL_CONSTRUCT_H
#pragma once
#include <new>
#include "utils.h"

namespace mystl {

    // construct 构造对象
    template <class T>
    void construct(T* ptr)
    {
        ::new ((void*)ptr) T(); // (void *)ptr 将变量ptr的值强制转换为 void（无类型）的指针
    }

    template <class T1,class T2>
    void construct(T1* ptr, const T2& value)
    {
        ::new ((void*)ptr) T1(value); // placement new
    }

    template <class T1,class... T2>
    void construct(T1* ptr, T2&&... args)
    {
        ::new ((void*)ptr) T1(mystl::forward<T2>(args)...);
    }


    // destroy 将对象析构

    template <class Ty>
    void destroy_one(Ty*, std::true_type) {
        std::cout << "void destroy_one(Ty*, std::true_type)" << std::endl;
    }

    template <class Ty>
    void destroy_one(Ty* pointer, std::false_type)
    {
        std::cout << "void destroy_one(Ty* pointer, std::false_type)" << std::endl;
        if (pointer != nullptr)
        {
            pointer->~Ty();
        }
    }

    template <class ForwardIter>
    void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first)
            destroy(&*first);
    }


    template <class Ty>
    void destroy(Ty* pointer)
    {
        std::cout << "void destroy(Ty* pointer)" << std::endl;
        // std::is_trivially_destructible 测试类型是否为完全无法易损坏。
        destroy_one(pointer, std::is_trivially_destructible<Ty>{});
    }

    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_cat(first, last, std::is_trivially_destructible<
                typename std::iterator_traits<ForwardIter>::value_type>{});
    }
}
