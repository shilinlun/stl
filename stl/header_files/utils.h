//
// Created by shi on 2021/2/13.
//

#ifndef STL_UTILS_H
#define STL_UTILS_H

#endif //STL_UTILS_H
#pragma once
#include <cstddef>

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等


namespace mystl {

    // move

    template <class T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    // forward

    template <class T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }

    template <class T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
    {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

}
