//
// Created by shi on 2021/2/13.
//

#ifndef STL_ALLOCATOR_H
#define STL_ALLOCATOR_H

#endif //STL_ALLOCATOR_H
#pragma once
#include <cstddef>
#include "construct.h"


// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构


namespace mystl
{
    template <class T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        static T* allocate(); // 分配一个T类型的内存   只调用operator new分配内存，没有调用构造函数
        static T* allocate(size_type n); // 分配n个T类型的

        static void deallocate(T* ptr); // 销毁  只调用operator delete销毁内存，没有调用析构函数
        static void deallocate(T* ptr, size_type n); // 销毁n个，但实现方式和上面一样

        static void construct(T* ptr); // 构造函数 ,在ptr所指的地方调用placement new来构造
        static void construct(T* ptr, const T& value);
        static void construct(T* ptr, T&& value);

        static void destroy(T* ptr);  // 会调用析构函数
        static void destroy(T* first, T* last);



    };

    template <class T>
    T* allocator<T>::allocate()
    {
        return static_cast<T*>(::operator new(sizeof(T)));
    }

    template <class T>
    T* allocator<T>::allocate(size_type n)
    {
        if (n==0)
        {
            return nullptr;
        }
        return static_cast<T*>(::operator new(n * sizeof(T)));

    }

    template <class T>
    void allocator<T>::deallocate(T* ptr)
    {
        if (ptr==nullptr)
        {
            return;
        }
        ::operator delete(ptr);
    }

    template <class T>
    void allocator<T>::deallocate(T* ptr, size_type n)
    {
        if (ptr==nullptr)
        {
            return;
        }
        ::operator delete(ptr);
    }

    template <class T>
    void allocator<T>::construct(T* ptr)
    {
        mystl::construct(ptr);
    }

    template <class T>
    void allocator<T>::construct(T* ptr, const T& value)
    {
        mystl::construct(ptr, value);
    }

    template <class T>
    void allocator<T>::construct(T* ptr, T&& value)
    {
        mystl::construct(ptr, mystl::move(value));
    }

    template <class T>
    void allocator<T>::destroy(T* ptr)
    {
        mystl::destroy(ptr);
    }

    template <class T>
    void allocator<T>::destroy(T* first, T* last)
    {
        mystl::destroy(first, last);
    }

}

