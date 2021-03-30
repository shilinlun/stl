//
// Created by shi on 2021/2/13.
//

#ifndef STL_MEMORY_H
#define STL_MEMORY_H
#pragma once
#endif //STL_MEMORY_H

namespace mystl
{
    // 获取对象地址
    template <class T>
    constexpr T* address_of(T& value) noexcept
    {
        return &value;
    }
}
