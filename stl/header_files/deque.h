//
// Created by shilinkun on 2021/3/12.
//

#include "iterator.h"
#include "allocator.h"
#include "algobase.h"
#include "uninitialized.h"

#pragma once
#ifndef STL_DEQUE_H
#define STL_DEQUE_H

#endif //STL_DEQUE_H


namespace mystl {
//    deque map 的初始大小
#define DEQUE_MAP_INIT_SIZE 8


// deque 一个buf的大小
    template<class T>
    struct deque_buf_size {
        static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
    };

// deque 迭代器设计
// 迭代器的图示见 /stl/deque迭代器.png
    template<class T, class Ref, class Ptr>
    struct deque_iterator : public iterator<random_access_iterator_tag, T> {
        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef deque_iterator self;


        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T *value_pointer;
        typedef T **map_pointer;

        static const size_type buffer_size = deque_buf_size<T>::value;


        // 迭代器所含的数据成员
        value_pointer cur; // 指向缓冲区的当前元素
        value_pointer first;  // 指向缓冲区的头部
        value_pointer last;  // 指向缓冲区的尾部
        map_pointer node; // 缓冲区的所在的节点



        // 构造、复制、移动函数
        deque_iterator() noexcept: cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {

        }


        // 将new_node指向的缓冲区复制到本身
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + buffer_size;
        }


        // 操作符重载
        self &operator=(const iterator &rhs) {
            if (this != &rhs) {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        bool operator<=(const self &rhs) const { return !(rhs < *this); }

        reference operator*() const {
            return *cur;
        }

        pointer operator->() const {
            return cur;
        }

        difference_type operator-(const self &x) const {
            return static_cast<difference_type>(buffer_size) * (node - x.node) + (cur - first) - (x.cur - x.first);
        }

        self operator-(difference_type n) const
        {
            self tmp = *this;
            return tmp -= n;
        }
        self& operator-=(difference_type n)
        {
            return *this += -n;
        }

        self& operator+=(difference_type n)
        {
            const auto offset = n + (cur - first);
            if (offset >= 0 && offset < static_cast<difference_type>(buffer_size))
            { // 仍在当前缓冲区
                cur += n;
            }
            else
            { // 要跳到其他的缓冲区
                const auto node_offset = offset > 0
                                         ? offset / static_cast<difference_type>(buffer_size)
                                         : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
            }
            return *this;
        }
        // 前++
        self &operator++() {
            ++cur;
            // 到缓冲区结尾，需要跳到下一个缓冲区
            if (cur == last) {
                set_node(node + 1);
                cur = first;

            }
            return *this;
        }

        // 后++
        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        self &operator--() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;

            }
            --cur;
            return *this;
        }

        // 后--
        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }


    };

    // deque 实现
    template<class T>
    class deque {

    public:
        typedef mystl::allocator<T> allocator_type;
        typedef mystl::allocator<T> data_allocator;
        // 注意里面放的T*
        typedef mystl::allocator<T *> map_allocator;

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        // map_pointer 其实就是指向一个指针，这个指针指向的是数据T
        typedef pointer *map_pointer;
        typedef const_pointer *const_map_pointer;

        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;


        // deque 中一个buf的大小
        static const size_type buffer_size = deque_buf_size<T>::value;

    private:
        // deque 中的数据成员
        iterator begin_;             // 指向第一个节点
        iterator end_;               // 指向最后一个节点
        map_pointer map_;            // 指向一块map，map中都是指针，指向一块缓冲区
        size_type map_size_;         // map中的指针的数目

    public:

        // 构造等一系列函数

        deque() {
            fill_init(0, value_type());
        }

        explicit deque(size_type n) {
            fill_init(n, value_type());
        }

        deque(size_type n, const value_type &value) {
            fill_init(n, value);
        }


        bool empty() const noexcept {
            return begin() == end();
        }

        size_type size() const noexcept {
            // 这里的-号是重载之后的
            return end_ - begin_;
        }


        // 访问元素相关操作
        reference operator[](size_type n) {
            return begin_[n];
        }

        // 其实就是调用[]
        reference at(size_type n) {
            return (*(this))[n];
        }

        reference front() {
            return *begin();
        }

        reference back() {
            // 记住-1
            return *(end() - 1);
        }

    public:
        // 迭代器相关操作
        iterator begin() noexcept {
            return begin_;
        }

        iterator end() noexcept {
            return end_;
        }

        const_iterator         end()     const noexcept
        { return end_; }

        // 在头部插入
        void push_front(const value_type &value);

        // 在尾部插入
        void push_back(const value_type &value);

        // 弹出头部元素
        void pop_front();

        // 弹出尾部元素
        void pop_back();

    private:
        // deque常见辅助函数
        // 初始化map
        void map_init(size_type nelem);

        // 初始化deque里面的数据
        void fill_init(size_type n, const value_type &value);

        // 创建一块map,但没有初始化值
        map_pointer create_map(size_type size);

        // 创建map中 nstart到nfinish中每一个指针所指向的buffer
        void create_buffer(map_pointer nstart, map_pointer nfinish);

        // 需要重新创建n个空间
        void require_capacity(size_type n, bool front);

        // 删除[nstart,nfinish]之间的buffer
        void destroy_buffer(map_pointer nstart, map_pointer nfinish);

    };


//    --------------------------------------------------------------------------------------------------


    template<class T>
    void deque<T>::push_front(const value_type &value) {

        if (begin_.cur != begin_.first) {
            // cur不是第一个，则可以直接插入
            data_allocator::construct(begin_.cur - 1, value);
            --begin_.cur;
        } else {
            // 是第一个，所以要重新创建buffer
            std::cout << "push_front" << std::endl;
            require_capacity(1, true);
            try {
                --begin_;
                data_allocator::construct(begin_.cur, value);

            } catch (...) {
                ++begin_;
                throw;
            }
        }

    }

    template<class T>
    void deque<T>::pop_back() {
        if (end_.cur != end_.first) {
            // 不是最后一个buffer的first
            --end_.cur;
            data_allocator::destroy(end_.cur);
        } else {
            // 是最后一个buffer的第一个元素，所以先--end_
            --end_;
            data_allocator::destroy(end_.cur);
            destroy_buffer(end_.node + 1, end_.node + 1);
        }
    }

    template<class T>
    void deque<T>::destroy_buffer(map_pointer nstart, map_pointer nfinish) {
        for (map_pointer m = nstart; m <= nfinish; m++) {
            data_allocator::deallocate(*m, buffer_size);
            *m = nullptr;
        }

    }

    template<class T>
    void deque<T>::pop_front() {
        if (begin_.cur != begin_.last - 1) {
            // cur没有到结尾,直接弹出
            data_allocator::destroy(begin_.cur);
            ++begin_.cur;
        } else {
            data_allocator::destroy(begin_.cur);
            ++begin_.cur;
            // 由于是此时第一个buffer为空
            destroy_buffer(begin_.node - 1, begin_.node - 1);
        }

    }


    template<class T>
    void deque<T>::require_capacity(size_type n, bool front) {

        if (front && (static_cast<size_type>(begin_.cur - begin_.first) < n)) {
            // 在最前面添加,此时begin_.cur = begin_.first
            const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
            if (need_buffer > static_cast<size_type>(begin_.node - map_)) {
                // 重新分配map的大小
                return;
            }
            create_buffer(begin_.node - need_buffer, begin_.node - 1);

        } else if (!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n)) {
            // 在最后添加空间，此时end_.last = end_.cur + 1
            // 必须保证创建的一个个空间大小位buffer_size
            const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;

            if (need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1)) {
                // 相当于目前创建的map的大小已经不能提供新的指针指向缓冲区，所以要重新分配map的大小
                return;
            }
            create_buffer(end_.node + 1, end_.node + need_buffer);

        }
    }

    template<class T>
    void deque<T>::push_back(const value_type &value) {
        // 还有位置，直接添加
        if (end_.cur != end_.last - 1) {
            data_allocator::construct(end_.cur, value);
            ++end_.cur;
        } else {
            // 需要重新申请空间
            // 需要1个，且在最后添加
            require_capacity(1, false);
            data_allocator::construct(end_.cur, value);
            ++end_;
        }
    }

    template<class T>
    void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
        map_pointer cur;
        try {

            for (cur = nstart; cur <= nfinish; ++cur) {
                *cur = data_allocator::allocate(buffer_size);
            }
        } catch (...) {
            while (cur != nstart) {
                --cur;
                data_allocator::deallocate(*cur, buffer_size);
                *cur = nullptr;
            }

            throw;
        }

    }

    template<class T>
    typename deque<T>::map_pointer deque<T>::create_map(size_type size) {
        map_pointer mp = nullptr;
        mp = map_allocator::allocate(size);
        for (size_type i = 0; i < size; ++i) {
            // 让map中的每一个指针都指向空
            *(mp + i) = nullptr;
        }
        return mp;
    }

    template<class T>
    void deque<T>::map_init(size_type nelem) {
        // 需要分配的缓冲区的个数
        const size_type nnode = nelem / buffer_size + 1;
        map_size_ = mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nnode + 2);
        try {
            map_ = create_map(map_size_);
        } catch (...) {
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }

        // 让nstart和nfinish都指向map中的最中央,方便头尾部扩充
        map_pointer nstart = map_ + (map_size_ - nnode) / 2;
        map_pointer nfinish = nstart + nnode - 1;
        try {
            create_buffer(nstart, nfinish);
        } catch (...) {
            map_allocator::deallocate(map_, map_size_);
            map_size_ = 0;
            map_ = nullptr;
            throw;
        }

        //  即初始化的时候，begin_和end_都是指向mao_的第三个位置，因为一般都是创建8个map，指向中间的部分
        begin_.set_node(nstart);
        end_.set_node(nfinish);
        begin_.cur = begin_.first;
        end_.cur = end_.first + (nelem % buffer_size);


    }

    template<class T>
    void deque<T>::fill_init(size_type n, const value_type &value) {
        // 构造可以装下n个数字的map
        map_init(n);
        // 现在才开始赋值
        if (n != 0) {
            for (auto cur = begin_.node; cur < end_.node; ++cur) {
                mystl::uninitialized_fill(*cur, *cur + buffer_size, value);
            }
            mystl::uninitialized_fill(end_.first, end_.cur, value);
        }
    }
}