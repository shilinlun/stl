//
// Created by shi on 2021/2/15.
//

#ifndef STL_ALGORITHM_H
#define STL_ALGORITHM_H
#pragma once
#endif //STL_ALGORITHM_H

// 这个头文件包含了 mystl 的一系列算法

namespace mystl {

    // bidirectional_iterator_tag类型的reverse
    template<class BidirectionalIter>
    void reverse_dispatch(BidirectionalIter first, BidirectionalIter second, bidirectional_iterator_tag) {
        std::cout << "reverse_dispatch" << std::endl;
    }

    // random_access_iterator_tag类型的reverse
    template<class RandomIter>
    void reverse_dispatch(RandomIter first, RandomIter second, random_access_iterator_tag) {
        while (first<second){
            mystl::iter_swap(first++,--second);
        }
    }

    template<class BidirectionalIter>
    void reverse(BidirectionalIter first, BidirectionalIter second) {
        mystl::reverse_dispatch(first, second, iterator_category(first));
    }
}