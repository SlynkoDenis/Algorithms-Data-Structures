#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

bool operator== (const std::vector<int>& lhs, const std::vector<int>& rhs) {
    if (lhs.size() != rhs.size())
        return false;
    for (size_t i = 0, end_index = lhs.size(); i < end_index; ++i) {
        if (lhs[i] != rhs[i])
            return false;
    }

    return true;
}

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& obj) {
    for (const auto& it : obj) {
        out << it << " ";
    }

    return out;
}

template <typename RAIt, typename T> RAIt BinarySearch(RAIt first, RAIt last, const T& to_find) {
    if (last < first)
        throw std::invalid_argument("invalid range");
    if (last - first == 1)
        return first;

    //RAIt tmp_last = last;

    RAIt tmp_iter = first + (last - first) / 2;
    while (last - first > 1) {
        if (*tmp_iter < to_find) {
            first = tmp_iter;
            tmp_iter += (last - first) / 2;
        } else {
            last = tmp_iter;
            tmp_iter -= (last - first) / 2;
        }
    }

    return first;
}

template <typename RAIt> void InsertionSort(RAIt first, RAIt last) {
    if (last - first <= 1)
        return;

    for (size_t i = 1, end_index = last - first; i < end_index; ++i) {
        auto tmp_elem = *(first + i);

        auto pos = BinarySearch(first, first + i, tmp_elem);

        size_t tmp_pos = (*pos <= tmp_elem) ? end_index - (last - pos) + 1 : end_index - (last - pos);
        for (size_t j = i; j > tmp_pos; --j) {
            *(first + j) = *(first + j - 1);
        }
        *(first + tmp_pos) = tmp_elem;
    }
}

void MakeTestVector(std::vector<int>& obj, size_t n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rand_distr;

    obj.resize(n);
    for (auto& it : obj) {
        it = rand_distr(gen) % (2 * n);
    }
}

template <typename RAIt> void Merge(RAIt first_begin, RAIt first_end, RAIt second_begin, RAIt second_end, typename std::vector<typename std::iterator_traits<RAIt>::value_type>::iterator buffer) {
    if (first_end < first_begin || second_end < second_begin)
        throw std::invalid_argument("invalid range");

    RAIt tmp = first_begin;
    RAIt tmp_buffer = buffer;
    while (tmp != first_end) {
        *tmp_buffer = *tmp;
        ++tmp;
        ++tmp_buffer;
    }


    while (buffer != tmp_buffer && second_begin != second_end) {
        if (*buffer <= *second_begin) {
            *first_begin = *buffer;
            ++buffer;
        } else {
            *first_begin = *second_begin;
            ++second_begin;
        }

        ++first_begin;
    }

    if (second_begin < second_end)
        while (second_begin != second_end) {
            *first_begin = *second_begin;
            ++second_begin;
            ++first_begin;
        }
    else if (buffer < tmp_buffer)
        while (buffer != tmp_buffer) {
            *first_begin = *buffer;
            ++first_begin;
            ++buffer;
        }
}

template <typename RAIt>
void MergeSort(RAIt first, RAIt last) {
    if (last - first <= 1)
        return;

    size_t sz = static_cast<size_t>(last - first);
    if (sz <= 12) {
        InsertionSort(first, last);
        return;
    }

    size_t i = 0;
    for (; i + 15 < sz; i += 8) {
        InsertionSort(first + i, first + i + 8);
    }
    InsertionSort(first + i, last);

    std::vector<typename std::iterator_traits<RAIt>::value_type> buffer(static_cast<size_t>(std::pow(2.0, static_cast<size_t>(std::log(sz) / std::log(2)))));
    i = 8;
    for (; i < sz; i *= 2) {
        for (size_t j = 0; j < sz / i; j += 2) {
            Merge(first + i * j, first + i * (j + 1), first + i * (j + 1),
                  first + std::min(i * (j + 2), sz), buffer.begin());
        }
    }
}

int main()
{
    size_t n = 10000;

    std::vector<int> unsorted_vector;
    MakeTestVector(unsorted_vector, n);

    std::vector<int> sorted_vector = unsorted_vector;
    std::sort(sorted_vector.begin(), sorted_vector.end());

    //std::cout << "Initial vector:\n" << unsorted_vector << std::endl;

    //InsertionSort(unsorted_vector.begin(), unsorted_vector.end());
    MergeSort(unsorted_vector.begin(), unsorted_vector.end());

    //std::cout << "Unsorted vector:\n" << unsorted_vector << std::endl;
    //std::cout << "Sorted vector:\n" << sorted_vector << std::endl;

    assert(sorted_vector == unsorted_vector);

    return 0;
}
