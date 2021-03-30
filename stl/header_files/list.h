//
// Created by shi on 2021/2/17.
//

#ifndef STL_LIST_H
#define STL_LIST_H

#endif //STL_LIST_H

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
    struct list_node_base;
    template<class T>
    struct list_node;

    template<class T>
    struct node_traits {
        typedef list_node_base<T> *base_ptr;
        typedef list_node<T> *node_ptr;
    };

    // list 的节点结构,分为2种，一种是base，一种是node，base作为第一个节点，不存放数据，由于是环状，达到左闭右开
    template<class T>
    struct list_node_base {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        base_ptr prev;  // 前一节点
        base_ptr next;  // 下一节点

        list_node_base() = default;

        node_ptr as_node() {
            return static_cast<node_ptr>(self());
        }

        void unlink() {
            prev = next = self();
        }

        base_ptr self() {
            return static_cast<base_ptr>(&*this);
        }
    };

    template<class T>
    struct list_node : public list_node_base<T> {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        T value;  // 数据域

        list_node() = default;

        list_node(const T &v)
                : value(v) {
        }

        list_node(T &&v)
                : value(mystl::move(v)) {
        }

        base_ptr as_base() {
            return static_cast<base_ptr>(&*this);
        }

        node_ptr self() {
            return static_cast<node_ptr>(&*this);
        }
    };

    // list迭代器
    template<class T>
    struct list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T> {
        typedef T value_type;
        typedef T &reference;
        typedef T *pointer;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_iterator<T> self;

        base_ptr node_; // 指向当前节点

        // 构造函数
        list_iterator() = default;

        list_iterator(base_ptr x) : node_(x) {
            std::cout << "list_iterator(base_ptr x)" << std::endl;
        }

        list_iterator(const list_iterator &rhs) : node_(rhs.node_) {}

        // 由于不能使用默认指针操作，所以必须重载 ,且注意，在取×的时候会as_node，相当于一个地址，若as_base就没有value，若as_node就有value
        reference operator*() const { return node_->as_node()->value; }

        pointer operator->() const { return &(operator*()); }

        // 前++
        self &operator++() {
            node_ = node_->next;
            return *this;
        }

        // 后++的int其实没有用，只是为了区分
        self operator++(int) {
            self temp = *this;
            ++*this; // 先前++，后×
            return temp;
        }

        // 前--
        self &operator--() {
            node_ = node_->prev;
            return *this;
        }

        // 后--
        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }

        // 重载比较操作符
        bool operator==(const self &rhs) const { return node_ == rhs.node_; }

        bool operator!=(const self &rhs) const { return node_ != rhs.node_; }

    };

    template<class T>
    struct list_const_iterator : public iterator<bidirectional_iterator_tag, T> {
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_const_iterator<T> self;

        base_ptr node_;

        list_const_iterator() = default;

        list_const_iterator(base_ptr x)
                : node_(x) {}

        list_const_iterator(node_ptr x)
                : node_(x->as_base()) {}

        list_const_iterator(const list_iterator<T> &rhs)
                : node_(rhs.node_) {}

        list_const_iterator(const list_const_iterator &rhs)
                : node_(rhs.node_) {}

        reference operator*() const { return node_->as_node()->value; }

        pointer operator->() const { return &(operator*()); }

        self &operator++() {
            node_ = node_->next;
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--() {
            node_ = node_->prev;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        // 重载比较操作符
        bool operator==(const self &rhs) const { return node_ == rhs.node_; }

        bool operator!=(const self &rhs) const { return node_ != rhs.node_; }
    };

    // list类
    template<class T>
    class list {
    public:
        typedef typename mystl::allocator<T> allocator_type;
        typedef typename mystl::allocator<T> data_allocator;

        typedef typename mystl::allocator<list_node<T>> node_allocator;
        typedef typename mystl::allocator<list_node_base<T>> base_allocator;

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef list_iterator<T> iterator;
        typedef list_const_iterator<T> const_iterator;

        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

    private:
        base_ptr node_; // 由于是环状链表，必须要保留一个不存放数据的节点，保证左闭右开
        size_type size_; // 大小


    public:
        // 构造函数
        list() {
            fill_init(0, value_type());
        }

        explicit list(size_type n) {
            fill_init(n, value_type());
        }

        list(size_type n, const T &value) {
            fill_init(n, value);
        }

        list(std::initializer_list<T> ilist) {
            copy_init(ilist.begin(), ilist.end());
        }

        // 拷贝构造
        list(const list &rhs) {
            std::cout << "list(const list &rhs)" << std::endl;
            copy_init(rhs.begin(), rhs.end());
        }

        // 移动构造
        list(list &&rhs) noexcept: node_(rhs.node_), size_(rhs.size_) {
            std::cout << "list(list &&rhs)" << std::endl;
            rhs.node_ = nullptr;
            rhs.size_ = 0;
        }

        // 拷贝赋值构造
        list &operator=(const list &rhs) {
            std::cout << "list &operator=(const list &rhs)" << std::endl;
            if (this != &rhs) {
                assign(rhs.begin(), rhs.end());
            }
            return *this;
        }

        // 移动赋值构造
        list &operator=(list &&rhs) noexcept {
            splice(end(), rhs);
            return *this;
        }


        // 迭代器操作,begin的时候就next
        iterator begin() noexcept {
            return node_->next;
        }

        const_iterator begin() const noexcept {
            return node_->next;
        }

        iterator end() noexcept {
            return node_;
        }

        const_iterator end() const noexcept {
            return node_;
        }

        // 容器相关操作
        bool empty() const noexcept {
            return node_->next == node_;
        }

        size_type size() const noexcept {
            return size_;
        }

        size_type max_size() const noexcept {
            return static_cast<size_type>(-1);
        }

        // reference 就是int& 感觉可以用value_type
        reference front() {
            return *begin();
        }

        reference back() {
            return *(--end());
        }

        void clear();

        iterator erase(const_iterator pos);

        iterator erase(const_iterator first, const_iterator second);

        template<class iter>
        void assign(iter first, iter second) {
            copy_assign(first, second);
        }

        void assign(std::initializer_list<T> ilist) {
            copy_assign(ilist.begin(), ilist.end());
        }

        void splice(const_iterator pos, list &rhs);

        template<class iter>
        iterator insert(const_iterator pos, iter first, iter second) {
            size_type n = mystl::distance(first, second);
            return copy_insert(pos, n, first);
        }

        iterator insert(const_iterator pos, const value_type &value) {
            auto link_node = create_node(value);
            ++size_;
            return link_iter_node(pos, link_node->as_base());
        }

        void push_front(const value_type &value);

        void push_back(const value_type &value);

        void pop_front() {
            auto node = node_->next;
            unlink_nodes(node, node);
            destroy_node(node->as_node());
            --size_;
        }

        void pop_back() {
            auto node = node_->prev;
            unlink_nodes(node, node);
            destroy_node(node->as_node());
            --size_;
        }

        void resize(size_type new_size) {
            return resize(new_size, value_type());
        }

        void resize(size_type new_size, const value_type &value);

    private:

        // 容器辅助函数

        iterator link_iter_node(const_iterator pos, base_ptr node);

        template<class iter>
        iterator copy_insert(const_iterator pos, size_type n, iter first);

        void fill_init(size_type n, const value_type &value);

        template<class iter>
        void copy_init(iter first, iter second);


        template<class iter>
        void copy_assign(iter first, iter second);

        // 创建节点
        template<class ...Args>
        node_ptr create_node(Args &&...args);

        void destroy_node(node_ptr p);

        // 在头部连接
        void link_nodes_at_front(base_ptr first, base_ptr last);

        // 在尾部连接节点
        void link_nodes_at_back(base_ptr first, base_ptr last);

        void link_nodes(base_ptr pos, base_ptr first, base_ptr last);

        void unlink_nodes(base_ptr first, base_ptr last);

//        void link_nodes_at_back(node_ptr node);
    };

//--------------------------------------方法实现------------------------------

    template<class T>
    void list<T>::resize(size_type new_size, const value_type &value) {
        auto i = begin();

    }

    template<class T>
    void list<T>::push_front(const value_type &value) {
        auto node = create_node(value);
        link_nodes_at_front(node->as_base(), node->as_base());
        ++size_;
    }

    template<class T>
    void list<T>::push_back(const value_type &value) {
        auto node = create_node(value);
        link_nodes_at_back(node->as_node(), node->as_node());
        ++size_;
    }



//    反面教材
//    template<class T>
//    void list<T>::link_nodes_at_back(node_ptr node) {
//        // 自己画一下当添加2个时候，其实把第一个就丢掉了
//        std::cout << "link_nodes_at_back(node_ptr node)" << std::endl;
//        node_->next = node;
//        node->next = node_;
//        node_->prev = node;
//        node->prev = node_;
//    }

    /*
     *   从begin及后面，都可以随时转变为as_base和as_node
     *         end()      begin()
     *   |---------------------------------------|
     *   |     node_       first       last      |
     *   |    |-----|     |-----|     |-----|    |
     *   |    |     |     |     |     |     |    |
     *   |--->|  n  |---->|  n  |     |  n  |--->|
     *        |     |     |     |     |     |
     *        |-----|     |-----|     |-----|
     *        |     |     |     |     |     |
     *   |<---|  p  |<----|  p  |     |  p  |<---|
     *   |    |     |     |     |     |     |    |
     *   |    |-----|     |-----|     |-----|    |
     *   |                                       |
     *   |---------------------------------------|
     *
     *
     * */
    template<class T>
    void list<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
        std::cout << "link_nodes_at_back(base_ptr first, base_ptr last)" << std::endl;
        last->next = node_;
        first->prev = node_->prev;
        first->prev->next = first;
        node_->prev = last;
    }
    /*
     * 其中first和last之间还是保持之前的连接关系，所以这里就是吧first和last和pos三者进行连接
     *
     *     node_
     *   |-----|       |-----|       |-----|        |-----|
     *   |     |       |     |       |     |        |     |
     *   |  n  |------>|  n  |       |  n  |        |  n  |
     *   |     |       |     |       |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|
     *   |     |       |     |       |     |        |     |
     *   |  p  |<------|  p  |       |  p  |        |  p  |
     *   |     |       |     |       |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|
     *
     *               first              last
     *              |-----|            |-----|
     *              |     |            |     |
     *              |  n  |            |  n  |
     *              |     |            |     |
     *              |-----|            |-----|
     *              |     |            |     |
     *              |  p  |            |  p  |
     *              |     |            |     |
     *              |-----|            |-----|
     *
     *                        |
     *                        V
     *
     *
     *    node_         first         last
     *   |-----|       |-----|       |-----|        |-----|     |-----|        |-----|
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |  n  |------>|  n  |       |  n  |------->|  n  |     |  n  |        |  n  |
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|     |-----|        |-----|
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |  p  |<------|  p  |       |  p  |<-------|  p  |     |  p  |        |  p  |
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|     |-----|        |-----|
     * */
// ***************
// link_nodes_at_front 在 头部连接 [first, last] 的结点
// ***************
    template<class T>
    void list<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
        first->prev = node_;
        last->next = node_->next;
        last->next->prev = last;
        node_->next = first;
    }



// ***************
// link_nodes 在 pos 处连接 [first, last] 的结点
// ***************

    /*
     * 其中first和last之间还是保持之前的连接关系，所以这里就是吧first和last和pos三者进行连接
     *
     *      A            pos
     *   |-----|       |-----|       |-----|        |-----|
     *   |     |       |     |       |     |        |     |
     *   |  n  |------>|  n  |       |  n  |        |  n  |
     *   |     |       |     |       |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|
     *   |     |       |     |       |     |        |     |
     *   |  p  |<------|  p  |       |  p  |        |  p  |
     *   |     |       |     |       |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|
     *
     *               first              last
     *              |-----|            |-----|
     *              |     |            |     |
     *              |  n  |            |  n  |
     *              |     |            |     |
     *              |-----|            |-----|
     *              |     |            |     |
     *              |  p  |            |  p  |
     *              |     |            |     |
     *              |-----|            |-----|
     *
     *                        |
     *                        V
     *
     *
     *      A           first         last            pos
     *   |-----|       |-----|       |-----|        |-----|     |-----|        |-----|
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |  n  |------>|  n  |       |  n  |------->|  n  |     |  n  |        |  n  |
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|     |-----|        |-----|
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |  p  |<------|  p  |       |  p  |<-------|  p  |     |  p  |        |  p  |
     *   |     |       |     |       |     |        |     |     |     |        |     |
     *   |-----|       |-----|       |-----|        |-----|     |-----|        |-----|
     * */
    template<class T>
    void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last) {
        pos->prev->next = first;
        first->prev = pos->prev;
        pos->prev = last;
        last->next = pos;
    }

// ***************
// unlink_nodes 将first和last断开连接
// ***************
    /*
     * 相当于就是把first和last之间给断开链接
     *         end()      begin()
     *            |------------------------------|
     *            |   first              last    |    last
     *   |-----|  |  |-----|            |-----|  |  |-----|
     *   | pos |  |  |     |            |     |  V  |     |
     *   |  n  |--x->|  n  |            |  n  |--x->|  n  |
     *   |     |     |     |            |     |     |     |
     *   |-----|     |-----|            |-----|     |-----|
     *   |     |     |     |            |     |     |     |
     *   |  p  |<-x--|  p  |            |  p  |<----|  p  |
     *   |     |  ^  |     |            |     |  |  |     |
     *   |-----|  |  |-----|            |-----|  |  |-----|
     *            |------------------------------|
     * */
    template<class T>
    void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
        first->prev->next = last->next; // pos的next不再指向first，指向本身
        last->next->prev = first->prev; // pos的prev不再指向last，指向本身
    }

// ***************
// link_iter_node 在pos处连接一个node
// ***************
    template<class T>
    typename list<T>::iterator list<T>::link_iter_node(const_iterator pos, base_ptr node) {
        if (pos == node_->next) {
            link_nodes_at_front(node, node);
        } else if (pos == node_) {
            link_nodes_at_back(node, node);
        } else {
            link_nodes(pos.node_, node, node);
        }
        return iterator(node);
    }

// ***************
// create_node
// ***************

    template<class T>
    template<class ...Args>
    typename list<T>::node_ptr list<T>::create_node(Args &&...args) {
        node_ptr p = node_allocator::allocate(1);
        try {
            data_allocator::construct(mystl::address_of(p->value), mystl::forward<Args>(args)...);
            p->prev = nullptr;
            p->next = nullptr;

        } catch (...) {
            node_allocator::deallocate(p);
            throw;
        }
        return p;
    }

// ***************
// fill_init 初始化n个value
// ***************
    template<class T>
    void list<T>::fill_init(size_type n, const value_type &value) {
        node_ = base_allocator::allocate(1); // 这个节点是最开始的节点
        node_->unlink();
        size_ = n;
        try {
            for (; n > 0; --n) {
                auto node = create_node(value);
                //巧妙，看似是传了2个node，然后接在后面，但其实2个是一样的
                link_nodes_at_back(node->as_base(), node->as_base());
//                link_nodes_at_back(node->as_node());
            }
        } catch (...) {
            //clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            throw;
        }
    }

// ***************
// copy_init 以[first,second)初始化
// ***************
    template<class T>
    template<class iter>
    void list<T>::copy_init(iter first, iter second) {
        node_ = base_allocator::allocate(1);
        node_->unlink();
        // 计算出两个迭代器的距离
        size_type n = mystl::distance(first, second);
        size_ = n;
        try {
            for (; n > 0; --n, ++first) {
                auto node = create_node(*first);
                link_nodes_at_back(node->as_base(), node->as_base());
            }
        } catch (...) {
            base_allocator::deallocate(node_);
            node_ = nullptr;
            throw;
        }
    }

// ***************
// copy_assign 把[first,second)中的值复制过来
// ***************
    template<class T>
    template<class iter>
    void list<T>::copy_assign(iter first, iter second) {
        auto f1 = begin();
        auto l1 = end();
        for (; f1 != l1 && first != second; ++f1, ++first) {
            *f1 = *first;
        }
        if (first == second) {
            // 表明需要赋值的元素为空，赋值后，原来的容器就相当于空
            clear();

        } else {
            insert(l1, first, second);
        }


    }

// ***************
// splice 将rhs接在pos之前
// ***************
    template<class T>
    void list<T>::splice(const_iterator pos, list<T> &rhs) {
        // 用于移动赋值构造，所以需要把rhs去掉
        if (!rhs.empty()) {
            auto f = rhs.node_->next;
            auto l = rhs.node_->prev;
            rhs.unlink_nodes(f, l); // 把rhs的连接断开
            link_nodes(pos.node_, f, l);
            size_ += rhs.size_;
            rhs.size_ = 0;  // 把rhs的size变为0
        }
    }

// ***************
// clear 清空list
// ***************
    template<class T>
    void list<T>::clear() {
        if (size_ != 0) {
            auto cur = node_->next;
            for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next) {
                destroy_node(cur->as_node());
            }
            node_->unlink();
            size_ - 0;
        }
    }

// ***************
// destroy_node 销毁节点
// ***************
    template<class T>
    void list<T>::destroy_node(node_ptr p) {
        data_allocator::destroy(mystl::address_of(p->value));
        node_allocator::deallocate(p);
    }

// ***************
// copy_insert 把从first开始的n个值插入到pos中
// ***************
    /*
     * link_nodes前面的代码所实现的功能
     *
     *                       end        end         end
     *                        r
     *                       next       next        next
     *        |-----|      |-----|     |-----|     |-----|
     *        | pos |      |     |     |     |     |     |
     *        |  n  |      |  n  |---->|  n  |---->|  n  |
     *        |     |      |     |     |     |     |     |
     *        |-----|      |-----|     |-----|     |-----|
     *        |     |      |     |     |     |     |     |
     *        |  p  |      |  p  |<----|  p  |<----|  p  |
     *        |     |      |     |     |     |     |     |
     *        |-----|      |-----|     |-----|     |-----|
     *                      first       first       first
     * link_nodes就把pos，r，end三者连起来
     * 最终实现全部链接
     * 返回r
     * */

    template<class T>
    template<class iter>
    typename list<T>::iterator list<T>::copy_insert(const_iterator pos, size_type n, iter first) {
        iterator r(pos.node_);
        if (n != 0) {
            const auto add_size = n;
            auto node = create_node(*first);
            r = iterator(node);
            iterator end = r;
            try {
                for (--n, ++first; n > 0; --n, ++first, ++end) {
                    auto next = create_node(*first);
                    end.node_->next = next->as_base();
                    next->prev = end.node_;
                }
                size_ += add_size;
            } catch (...) {

            }
            link_nodes(pos.node_, r.node_, end.node_);
        }
        return r;
    }


// ***************
// erase 把pos处的删除,返回下一个迭代器
// ***************
    template<class T>
    typename list<T>::iterator list<T>::erase(const_iterator pos) {
        auto n = pos.node_;
        auto next = n->next;
        unlink_nodes(n, n);// 把n这个节点给断开，unlink_nodes已经把前后连接起来
        destroy_node(n->as_node()); // 去除掉
        --size_;
        return iterator(next);
    }

// ***************
// erase 把[first,second)之间删除
// ***************
    template<class T>
    typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator second) {
        if (first != second) {
            unlink_nodes(first, second.node_->prev); // 先断开连接
            while (first != second) {
                // 再挨着摧毁
                auto cur = first.node_;
                ++first;
                destroy_node(cur->as_node());
                --size_;
            }
        }
        return iterator(second.node_);
    }
}


