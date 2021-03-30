//
// Created by shi on 2021/2/13.
//

#ifndef STL_VECTOR_H
#define STL_VECTOR_H

#endif //STL_VECTOR_H
#pragma once

#include "allocator.h"
#include "utils.h"
#include "algobase.h"
#include "algorithm.h"
#include "uninitialized.h"
#include "iterator.h"
#include "memory.h"

#include <cassert>
#include <iostream>


namespace mystl {

    template<class T>
    class vector {
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
    public:
        typedef mystl::allocator<T> allocator_type;
        typedef mystl::allocator<T> data_allocator;


        // typedef typename 一般使用在重命名中有::的
        typedef typename allocator_type::value_type value_type;             //	typedef T value_type;
        typedef typename allocator_type::pointer pointer;                // typedef T* pointer;
        typedef typename allocator_type::const_pointer const_pointer;          //	typedef const T* const_pointer;
        typedef typename allocator_type::reference reference;              // typedef T& reference;
        typedef typename allocator_type::const_reference const_reference;        // typedef const T& const_reference;
        typedef typename allocator_type::size_type size_type;              //	typedef size_t(unsigned int) size_type;
        typedef typename allocator_type::difference_type difference_type;        //	typedef ptrdiff_t difference_type;

        typedef value_type* iterator;               // T*
        typedef const value_type* const_iterator;         // const T*
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

        allocator_type get_allocator() { return data_allocator(); }

    private:
        iterator i_begin;   //使用空间的头部
        iterator i_end;     //使用空间的尾部
        iterator i_cap;     //占用空间的尾部

    public:

        // 构造函数，cap为16
        // noexcept ：等价于noexcept(true) 表示该函数不抛出异常，noexcept(false)表示可以抛出异常
        vector() noexcept {
            std::cout << "vector()" << std::endl;
            try_init();
        }

        // 构造函数，cap为16，end-begin=n
        // explicit使用在单参数的构造函数中，防止隐式转换，若加了explicit，则
        // 这种定义是错误的 vector<int> a = 10;
        // 若没加explicit，则是正确的，程序会自动判断=右边的值是否可以作为构造函数的参数，若可以，则将
        // 右边的值作为构造函数的参数传入，调用构造函数，所以一般单参数的构造函数会加上explicit
        explicit vector(size_type n) noexcept {
            std::cout << "vector(size_type n)" << std::endl;
            fill_init(n, value_type());
        }

        // 构造函数，cap为16，end-begin=n，并初始化值为value
        vector(size_type n, const value_type &value) noexcept {
            std::cout << "vector(size_type n, const value_type& value)" << std::endl;
            fill_init(n, value);
        }

        // 构造函数vector<int>v2(v1.begin(),v1.end())
        template<class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type= 0>
        vector(Iter first, Iter last) {
            std::cout << "vector(Iter first, Iter last)" << std::endl;
            assert(!(last < first));
            range_init(first, last);
        }

        //  移动构造函数,所以在这个函数中，一定要让传入的参数的值在move之后丢弃掉，且需要加上noexcept
        vector(vector &&rhs) noexcept: i_begin(rhs.i_begin), i_end(rhs.i_end), i_cap(rhs.i_cap) {
            std::cout << "vector(vector&& rhs)" << std::endl;
            rhs.i_begin = nullptr;
            rhs.i_end = nullptr;
            rhs.i_cap = nullptr;
        }

        // 拷贝构造
        vector(const vector &rhs) {
            std::cout << "vector(const vector &rhs)" << std::endl;
            range_init(rhs.i_begin, rhs.i_end);
        }

    private:
        void try_init() noexcept;

        void fill_init(size_type n, const value_type &value);

        void init_space(size_type size, size_type cap);

        template<class Iter>
        void range_init(Iter first, Iter last);


    public:
        // 正向迭代器（可读可写）
        iterator begin() noexcept {
            return i_begin;
        }

        // 不加const，就会和上面的重载冲突，因为只有返回值不一样，不能构成重载
        const_iterator begin() const noexcept {
            return i_begin;
        }

        iterator end() noexcept {
            return i_end;
        }

        const_iterator end() const noexcept {
            return i_end;
        }

        // 反向迭代器（可读可写）
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        const reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        const reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        // 静态迭代器（可读）
        const_iterator cbegin() const noexcept {
            return begin();
        }

        const_iterator cend() const noexcept {
            return end();
        }

        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        const_reverse_iterator crend() const noexcept {
            return rend();
        }


        // ********************************************vector相关操作
        // empty()是否为空
        bool empty() const noexcept {
            return i_begin == i_end;
        }

        // size()end-begin
        size_type size() const noexcept {
            return static_cast<size_type>(i_end - i_begin);
        }

        // max_size()返回可以容纳的最大数量，用-1来除，就是因为-1会转变为无符号数的最大
        size_type max_size() const noexcept {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }

        // capacity()cap-begin
        size_type capacity() const noexcept {
            return static_cast<size_type>(i_cap - i_begin);
        }

        // reverse(n)将大小转为n个
        void reverse(size_type n);

        reference front() {
            assert(!empty());
            return *i_begin;
        }

        reference back() {
            assert(!empty());
            return *(i_end - 1);
        }

        /*
         * emplace操作是C++11新特性，新引入的的三个成员emlace_front、empace 和 emplace_back,这些操作构造而不是拷贝元素到容器中，
         * 这些操作分别对应push_front、insert 和push_back，允许我们将元素放在容器头部、一个指定的位置和容器尾部。
         * 当调用insert时，我们将元素类型的对象传递给insert，元素的对象被拷贝到容器中，而当我们使用emplace时，我们将参数传递元素类型的构造函，
         * emplace使用这些参数在容器管理的内存空间中直接构造元素。
         * */

        // push_back 和 emplace_back的区别
        /*
         * emplace_back() 和 push_back() 的区别，就在于底层实现的机制不同。push_back() 向容器尾部添加元素时，
         * 首先会创建这个元素，然后再将这个元素拷贝或者移动到容器中（如果是拷贝的话，事后会自行销毁先前创建的这个元素）；
         * 而 emplace_back() 在实现时，则是直接在容器尾部创建这个元素，省去了拷贝或移动元素的过程
         * */
        void push_back(const value_type &value);

        template<class... Args>
        void emplace_back(Args &&...args);

        // emplace 和 insert 区别和push_back 和 emplace_back的区别一样，在哪个地方插入，返回值就是那里，比如在begin处插入，返回值就是插入后的begin
        // insert有2个版本，一个使用移动构造，一个使用拷贝构造
        // 拷贝构造(若传入的value是左值)
        iterator insert(const_iterator cur, const value_type &value);

        // 移动构造(若传入的value是右值，比如临时变量)
        iterator insert(const_iterator cur, value_type &&value) {
            std::cout << "insert(const_iterator cur,value_type&& value)" << std::endl;
            // mystl::move -->  std::move实际就是可以得到传入参数的右值，不管传入的参数是右值还是左值
            // 右值：只能放在=右边
            // 左值：两边都可以放
            return emplace(cur, mystl::move(value));
        }
        // insert(pos,n,value)

        iterator fill_insert(iterator cur, size_type n, const value_type &value);

        iterator insert(const_iterator cur, size_type n, const value_type &value) {
            return fill_insert(const_cast<iterator>(cur), n, value);
        }

        template<class... Args>
        iterator emplace(const_iterator cur, Args &&...args);

        // erase
        iterator erase(const_iterator pos);

        iterator erase(const_iterator first, const_iterator second);

        void clear() { erase(begin(), end()); }

        // resize
        void resize(size_type new_size, const value_type &value);
        // 默认插入0
        void resize(size_type new_size){return resize(new_size,value_type());};

        // reverse(begin,end)反转vector
        void reverse(){mystl::reverse(begin(),end());};



        // ********************************************操作符重载
        // []
        reference operator[](size_type n) {
            assert(n < size());
            return *(i_begin + n);
        }

        // at
        reference at(size_type n) {
            assert(n < size());
            return (*this)[n]; //注意这里的括号
        }

        // =，必须要加&，否则赋值后，不会改变原来的值
        vector &operator=(const vector &rhs); // 拷贝赋值
        vector &operator=(vector &&rhs);      // 移动赋值
        // 使用{}赋值
        vector(std::initializer_list<value_type> ilist) {
            std::cout << "vector(std::initializer_list<value_type> ilist)" << std::endl;
            range_init(ilist.begin(), ilist.end());
        }

        vector &operator=(std::initializer_list<value_type> ilist);


        // ****************************************vector相关辅助函数
        // swap 将自己和rhs交换
        void swap(vector &rhs) noexcept;

        //收回空间
        void destrop_and_recover(iterator first, iterator last, size_type n);

        void reallocate_insert(iterator cur, const value_type &value);

        template<class... Args>
        void reallocate_emplace(iterator cur, Args &&...args);

        // 在大小不够，但又继续添加元素的时候使用
        size_type get_new_cap(size_type add_size);


        // ***************************************析构函数
        ~vector() {
            destrop_and_recover(i_begin, i_end, i_cap - i_begin);
            i_begin = nullptr;
            i_end = nullptr;
            i_cap = nullptr;
        }

    };


// ---------------------------------------开始实现类的方法----------------------------------------
// ***************
// fill_insert,在insert(pos,n,value)中使用
// ***************

    template<class T>
    typename vector<T>::iterator vector<T>::fill_insert(iterator pos, size_type n, const value_type &value) {
        if (n == 0) {
            return pos;
        }
        const size_type xpos = pos - i_begin;
        const value_type value_copy = value; // 避免被覆盖 ，暂时不知道为什
        if (static_cast<size_type>(i_cap - i_end) >= n) {
            // 当还有更多的备用空间
            const size_type after_elems = i_end - pos;
            auto old_end = i_end;
            if (after_elems > n) {
                mystl::uninitialized_copy(i_end - n, i_end, i_end);
                i_end += n;
                mystl::move_backward(pos, old_end - n, old_end);
                mystl::uninitialized_fill_n(pos, n, value_copy);
            } else {
                i_end = mystl::uninitialized_fill_n(i_end, n - after_elems, value_copy);
                i_end = mystl::uninitialized_move(pos, old_end, i_end);
                mystl::uninitialized_fill_n(pos, after_elems, value_copy);
            }
        } else {
            //备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try {
                new_end = mystl::uninitialized_move(i_begin, pos, new_begin);
                new_end = mystl::uninitialized_fill_n(new_end, n, value);
                new_end = mystl::uninitialized_move(pos, i_end, new_end);
            } catch (...) {
                destrop_and_recover(new_end,new_end,new_size);
                throw ;
            }
            data_allocator::deallocate(i_begin,i_cap-i_end);
            i_begin = new_begin;
            i_end = new_end;
            i_cap = i_begin + new_size;
        }
        return i_begin + xpos;
    }
// ***************
// resize
// ***************

    template<class T>
    void vector<T>::resize(size_type new_size, const value_type &value) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), value);
        }
    }
// ***************
// erase,删除[first second)上的数据
// ***************

    template<class T>
    typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator second) {
        const auto n = first - begin();
        iterator pos = i_begin + n;
        data_allocator::destroy(mystl::move(pos + (second - first), i_end, pos), i_end);
        i_end = i_end - (second - first);
        return i_begin + n;
    }
// ***************
// erase,在第n个位置擦出
// ***************

    template<class T>
    typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
        iterator cur = i_begin + (pos - begin());
        mystl::move(cur + 1, i_end, cur);
        data_allocator::destroy(i_end - 1);
        i_end--;
        return cur;
    }


// ***************
// insert,在第n个位置插入
// ***************

    template<class T>
    typename vector<T>::iterator vector<T>::insert(const_iterator cur, const value_type &value) {
        std::cout << "insert(const_iterator cur, const value_type &value)" << std::endl;
        iterator pos = const_cast<iterator>(cur);
        const size_type n = cur - i_begin;
        // size和capacity不一样，且在最后插入
        if (i_end != i_cap && pos == i_end) {
            data_allocator::construct(mystl::address_of(*i_end), value);
            i_end++;
            // size和capacity不一样，但不再最后插入
        } else if (i_end != i_cap) {
            auto new_end = i_end;
            // 先将原来最后一个数字向后移动一位
            data_allocator::construct(mystl::address_of(*i_end), *(i_end - 1));
            new_end++;
            // 将中间部分向后移动
            mystl::copy_backward(pos, i_end - 1, i_end);
            // 在pos处添加
            *pos = mystl::move(value);
            i_end = new_end;
            // size == capacity 但不在最后插入
        } else {
            reallocate_insert(pos, value);
        }

        return i_begin + n;

    }

// ***************
// insert
// ***************
    template<class T>
    template<class... Args>
    typename vector<T>::iterator vector<T>::emplace(const_iterator cur, Args &&...args) {
        iterator pos = const_cast<iterator>(cur);
        const size_type n = cur - i_begin;

        if (i_end != i_cap && pos == i_end) {
            data_allocator::construct(mystl::address_of(*i_end), mystl::forward<Args>(args)...);
            i_end++;
        } else if (i_end != i_cap) {
            auto new_end = i_end;
            data_allocator::construct(mystl::address_of(*i_end), *(i_end - 1));
            new_end++;
            mystl::copy_backward(pos, i_end - 1, i_end);
            *pos = value_type(mystl::forward<Args>(args)...);
            i_end = new_end;
        } else {
            reallocate_emplace(pos, mystl::forward<Args>(args)...);
        }
        return begin() + n;
    }


// ***************
// get_new_cap 计算在原来基础上，添加add_size个元素，则最后应该多少空间，windows不是2倍扩充，是 a->(a+a/2),linux是2倍扩充
// ***************
    template<class T>
    typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size) {
        const auto old_size = capacity();

        if (old_size > max_size() - old_size / 2) {
            return old_size + add_size > max_size() - 16
                   ? old_size + add_size : old_size + add_size + 16;
        }
        // 考虑push前，之前的大小是否为0
        const size_type new_size = old_size == 0
                                   ? mystl::max(add_size, static_cast<size_type>(16))
                                   : mystl::max(old_size + old_size, old_size + add_size);
        return new_size;
    }


// ***************
// reallocate_insert 在push_back中使用，若空间不足，先重新分配，再添加值
// ***************

    template<class T>
    void vector<T>::reallocate_insert(iterator cur, const value_type &value) {
        // 比如原来的size为30，capacity为30，则加一个之后，capacity为60,
        // 然后生成一个60capacity的vector
        // 此时cur为i_end，把[i_begin,cur)的值给新的vector，然后最后一个加上value
        // 最后把之前的vector的[cur,i_end)的值给新的vector
        const auto new_capacity = get_new_cap(1);
        auto new_i_begin = data_allocator::allocate(new_capacity);
        //auto new_i_end = new_i_begin;

        // 把[i_begin,cur)移动到new_i_begin开始的位置，返回移动结束的位置
        auto new_i_end = mystl::uninitialized_move(i_begin, cur, new_i_begin);
        // *new_i_end = value;(这句话等价下面的一行代码)
        data_allocator::construct(mystl::address_of(*new_i_end), value);
        ++new_i_end;

        new_i_end = mystl::uninitialized_move(cur, i_end, new_i_end);

        destrop_and_recover(i_begin, i_end, i_cap - i_begin); // 把之前的vector去掉
        i_begin = new_i_begin;
        i_end = new_i_end;
        i_cap = new_i_begin + new_capacity;
    }

// ***************
// reallocate_emplace 在emplace_back中使用，若空间不足，先重新分配，再添加值
// ***************

    template<class T>
    template<class... Args>
    void vector<T>::reallocate_emplace(vector::iterator cur, Args &&... args) {
        const auto new_capacity = get_new_cap(1);
        auto new_i_begin = data_allocator::allocate(new_capacity);
        //auto new_i_end = new_i_begin;

        // 把[i_begin,cur)移动到new_i_begin开始的位置，返回移动结束的位置
        auto new_i_end = mystl::uninitialized_move(i_begin, cur, new_i_begin);
        data_allocator::construct(mystl::address_of(*new_i_end), mystl::forward<Args>(args)...);
        ++new_i_end;

        new_i_end = mystl::uninitialized_move(cur, i_end, new_i_end);

        destrop_and_recover(i_begin, i_end, i_cap - i_begin); // 把之前的vector去掉
        i_begin = new_i_begin;
        i_end = new_i_end;
        i_cap = new_i_begin + new_capacity;

    }

// ***************
// push_back
// ***************

    template<class T>
    void vector<T>::push_back(const value_type &value) {
        // 考虑添加元素后，是否超过空间
        // 可以直接添加元素
        if (i_end != i_cap) {
            // address_of会返回i_end的地址，然后construct构造value
            data_allocator::construct(mystl::address_of(*i_end), value);
            i_end++;
        }
            // 先扩容再添加
        else {
            reallocate_insert(i_end, value);
        }
    }

// ***************
// emplace_back
// ***************
    template<class T>
    template<class... Args>
    void vector<T>::emplace_back(Args &&... args) {
        if (i_end != i_cap) {
            data_allocator::construct(mystl::address_of(*i_end), mystl::forward<Args>(args)...);
            i_end++;
        } else {
            reallocate_emplace(i_end, mystl::forward<Args>(args)...);
        }

    }

// ***************
// destrop_and_recover 收回空间，析构函数使用
// ***************
    template<class T>
    void vector<T>::destrop_and_recover(iterator first, iterator last, size_type n) {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
    }


// ***************
// swap,让自己和另一个vector交换，达到operator=操作
// ***************
    template<class T>
    void vector<T>::swap(vector &rhs) noexcept {
        // 不一样才交换
        if (this != &rhs) {
            i_begin = rhs.i_begin;
            i_end = rhs.i_end;
            i_cap = rhs.i_cap;
        }
    }


// ***************
// 操作符= std::initializer_list<value_type>赋值
// ***************

    template<class T>
    vector<T> &vector<T>::operator=(std::initializer_list<value_type> ilist) {
        vector temp(ilist.begin(), ilist.end());
        swap(temp);
        return *this;
    }



// ***************
// 操作符= 拷贝赋值
// ***************

    template<class T>
    vector<T> &vector<T>::operator=(const vector &rhs) {
        // 判断是否是一个东西，通过判断地址
        if (this != &rhs) {
            vector temp(rhs.begin(), rhs.end());
            swap(temp);
        }
        return *this;
    }
// ***************
// 操作符= 移动赋值
// ***************

    template<class T>
    vector<T> &vector<T>::operator=(vector &&rhs) {
        // 判断是否是一个东西，通过判断地址
        if (this != &rhs) {
            vector temp(rhs.begin(), rhs.end());
            swap(temp);
            rhs.i_begin = nullptr;
            rhs.i_end = nullptr;
            rhs.i_cap = nullptr;
        }
        return *this;
    }

// ***************
// reverse 将容量转为n个,要判断capacity的大小，若小于n，则扩大到n，用0填充，若大于n，则保留前n个
// ***************

    template<class T>
    void vector<T>::reverse(size_type n) {
        if (capacity() < n) {
            const auto old_size = size();
            auto temp = data_allocator::allocate(n);
            mystl::uninitialized_move(i_begin, i_end, temp);
            data_allocator::deallocate(i_begin, i_cap - i_begin);//把原来的空间给清楚掉
            i_begin = temp;
            i_end = i_begin + old_size;
            i_cap = i_begin + n;
        } else {
            if (size() < n) {
                const auto old_size = size();
                auto temp = data_allocator::allocate(n);
                mystl::uninitialized_move(i_begin, i_begin + n, temp);
                data_allocator::deallocate(i_begin, i_cap - i_begin);//把原来的空间给清楚掉
                i_begin = temp;
                i_end = i_begin + old_size;
                i_cap = i_begin + n;
            } else {
                auto temp = data_allocator::allocate(n);
                mystl::uninitialized_move(i_begin, i_begin + n, temp);
                data_allocator::deallocate(i_begin, i_cap - i_begin);//把原来的空间给清楚掉
                i_begin = temp;
                i_end = i_begin + n;
                i_cap = i_begin + n;
            }

        }
    }

// ***************
// range_init 分配cap和last-first，同时拷贝first到last的值到i_begin
// ***************
    template<class T>
    template<class Iter>
    void vector<T>::range_init(Iter first, Iter last) {
        const size_type init_size = mystl::max(static_cast<size_type>(16), static_cast<size_type>(last - first));
        init_space(static_cast<size_type>(last - first), init_size);
        mystl::uninitialized_copy(first, last, i_begin);
    }

// ***************
// init_space ,分配cap和n
// ***************
    template<class T>
    void vector<T>::init_space(size_type n, size_type cap) {
        try {
            i_begin = data_allocator::allocate(cap);
            i_end = i_begin + n;
            i_cap = i_begin + cap;
        }
        catch (...) {
            i_begin = nullptr;
            i_end = nullptr;
            i_cap = nullptr;
            throw;
        }
    }

// ***************
// fill_init  若设置的n小于16，则创建16的cap，然后创建n个T，值为value
// ***************
    template<class T>
    void vector<T>::fill_init(size_type n, const value_type &value) {
        const size_type init_size = mystl::max(static_cast<size_type>(16), n);
        init_space(n, init_size);
        mystl::uninitialized_fill_n(i_begin, n, value);
    }


// ***************
// try_init  // 分配16个sizeof（T）
// ***************
    template<class T>
    void vector<T>::try_init() noexcept {
        try {
            i_begin = data_allocator::allocate(16);
            i_end = i_begin;
            i_cap = i_begin + 16;
        }
        catch (...) {
            i_begin = nullptr;
            i_end = nullptr;
            i_cap = nullptr;
        }

    }


}