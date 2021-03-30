#include <iostream>
#include "header_files/vector.h"
#include "header_files/list.h"
#include <vector>
#include <list>
#include "header_files/deque.h"
#include "header_files/stack.h"
#include "header_files/queue.h"
#include <algorithm>
#include "header_files/heap_algo.h"

using namespace std;
using namespace mystl;


int main() {
    //vector<int> a;
    //cout << sizeof(a) << endl;
    //vector<int> b(11, 1);
    //cout << b[10] << endl;
    //cout << b.at(2) << endl;
    //vector<int> c(30, 2);
    //b = c; // 调用拷贝赋值
    //cout << b[1] << endl;
    //cout << b.size() << endl;
    //cout << c.size() << endl;
    //cout << b.capacity() << endl;
    //cout << c.capacity() << endl;
    //vector<int> d;
    //d = std::move(c); // 调用移动赋值，记住移动赋值必须先声明，再移动 vector<int> d = std::move(c)是错的
    //cout << d.size() << endl;
    //vector<int> c(10);
    //vector<int> d(b.begin(), b.end());
    //cout << b.size() << endl;
    //cout << b.empty() << endl;
    //cout << b.max_size() << endl;
    //cout << b.capacity() << endl;

//    mystl::vector<int> a(30,1);
//    //cout << a[0] << endl;
//    cout << a.size() << endl;
//    cout << a.capacity() << endl;
//    //vector<int> b(3, 1);
//    a.push_back(2);
//    a.emplace_back(1);
//    cout << a[31] << endl;
//    cout << a.size() << endl;
//    cout << a.capacity() << endl;
//    cout << a[29] << endl;
//    cout << a[30] << endl;
//    //cout << a[31] << endl; // 出错，超出引用
//    cout << a.max_size() << endl;

//    mystl::vector<int> b = {1,2,3};
//    b.reverse();
//    for (auto i:b)
//        cout << i << " ";
//    cout << b.size() << endl;
//    cout << b.capacity() << endl;
//    b.emplace_back(4);
//    cout << b[3] << endl;
//    b.push_back(5);
//    b.emplace(b.begin(),0);
//    b.erase(b.begin());
//    cout << b[0] << endl;

//    auto iterator = b.insert(b.begin()+5,6);
//    cout << b[0] << endl;
//    cout << *iterator << endl;
//    for (auto i:b)
//        cout << i << " ";


//    cout << "-----------" << endl;
//    std::vector<int> b(30, 1);
//    //cout << a[0] << endl;
//    cout << b.size() << endl;
//    cout << b.capacity() << endl;
//    //vector<int> b(3, 1);
//    b.push_back(2);
//    cout << b.size() << endl;
//    cout << b.capacity() << endl;
//    cout << b[29] << endl;
//    cout << b[30] << endl;
//    //cout << b[31] << endl; // 出错，超出引用
//    cout << b.max_size() << endl;
//    mystl::list<int> a;
//    cout << sizeof(a) << endl;
//    mystl::list<int>::iterator it = a.begin();
////    ++it;
//    cout << *(it) << endl;
//    std::list<int> b;
//    cout << *(b.begin()) << endl;
//    mystl::list<int> b = {1,2,3};
////    cout << *(b.begin()) << endl;
////    b.clear();
////    b.assign({1,2,3});
////    cout << *(b.begin()) << endl;
//    mystl::list<int> c;  // mystl::list<int> c = b 会调用拷贝构造函数
////    mystl::list<int> d = std::move(b); // 会调佣移动构造
//    c = b; // 会调用拷贝赋值构造函数
//    cout << *(c.begin()) << endl;
//    auto e = {1,2,3};
//    c.assign(e.begin(),e.begin());
//    cout << *(c.begin()) << endl;
////    mystl::list<int> e;
////    e = std::move(b); // 会调用移动赋值拷贝构造
////    auto it1 = c.begin();
////    cout << *it1 << endl;
//
////    std::list<int> c;
////    c.assign({1,2,3});
////    cout << *(c.begin()) << endl;
//    std::list<int> d;
//    d.assign(e.begin(),e.begin());
//    cout << *(d.begin()) << endl;
//    mystl::list<int> a = {1, 2, 3, 4};
//    auto b = a.begin();
//    b++;
//    a.insert(b, 5);
//    auto it = a.begin();
//    while (it != a.end()) {
//        cout << *it++ << endl;
//    }
//    a.push_front(0);
//    it = a.begin();
//    while (it != a.end()) {
//        cout << *it++ << endl;
//    }
//    mystl::deque<int> a(0, 1);
//    std::cout << *(a.begin()) << std::endl;
//    std::cout << a.size() << std::endl;
//    a.push_back(2);
//    std::cout << *(--a.end()) << endl;
//    a.pop_back();
//    std::cout << *(--a.end()) << endl;
//    a.push_front(1);
//    std::cout << *(a.begin()) << std::endl;

//    mystl::queue<int>a;
//    a.push(1);
//    a.push(2);
//    std::cout << a.front() << std::endl;

    mystl::vector<int>b{0,1,2};
    mystl::make_heap(b.begin(), b.end());
//    for(auto i:b){
//        cout << i << " ";
//    }
//    cout << endl;
//    mystl::vector<int> a{0, 1};
//    make_heap(a.begin(), a.end());
//    for(auto i:a){
//        cout << i << " ";
//    }
////    pop_heap(a.begin(),a.end());
////    cout << endl;
////    for(auto i:a){
////        cout << i << " ";
////    }
//    cout << endl;
//    a.push_back(2);
//    mystl::push_heap(a.begin(),a.end());
//    a.push_back(3);
//    mystl::push_heap(a.begin(),a.end());
//    a.push_back(4);
//    mystl::push_heap(a.begin(),a.end());
//    a.push_back(8);
//    mystl::push_heap(a.begin(),a.end());
//    a.push_back(9);
//    mystl::push_heap(a.begin(),a.end());
//    a.push_back(3);
//    mystl::push_heap(a.begin(),a.end());
//    a.push_back(5);
//    mystl::push_heap(a.begin(),a.end());
//
//
//    for(auto i:a){
//        cout << i << " ";
//    }
////    make_heap(a.begin(), a.end());
////    for(auto i:a){
////        cout << i << " ";
////    }
    return 0;
}