#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

template <typename T> std::ostream& operator <<(std::ostream& os, const std::vector<T>& v) {
    for (const auto& it : v)
        os << it << " ";

    return os;
}

template <typename T> bool operator ==(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    if (lhs.size() != rhs.size())
        return false;

    for (size_t i = 0, end_index = lhs.size(); i < end_index; ++i)
        if (lhs[i] != rhs[i])
            return false;

    return true;

}

template <typename RAIt, typename T> RAIt Partition(RAIt first, RAIt last, const T& pivot) {
    if (last - first < 2)
        return first;

    --last;
    while (last - first > 0) {
        if (*first < pivot && *last > pivot) {
            ++first;
            --last;
        } else if (*first < pivot)
            ++first;
        else if (*last > pivot)
            --last;
        else if (*first == pivot && *last != pivot) {
            std::swap(*first, *last);
            ++first;
        } else if (*last == pivot && *first != pivot) {
            std::swap(*first, *last);
            --last;
        } else if (*first == *last && *first == pivot) {
            auto ptr = first + 1;
            while (ptr != last && *ptr <= *first) {
                if (*ptr != *first)
                    std::swap(*first, *ptr);
                ++first;
                ++ptr;
            }
            if (ptr == last)
                return first;
            else
                ++first;
        } else {
            std::swap(*first, *last);
            ++first;
            --last;
        }
    }

    return first;
}

template <typename RAIt> void InsertionSort(RAIt first, RAIt last) {
    if (last - first < 2)
        return;

    for (size_t i = 1, end_index = last - first; i < end_index; ++i) {
        for (size_t j = i; j > 0; --j) {
            if (*(first + j) < *(first + j - 1))
                std::swap(*(first + j), *(first + j - 1));
            else
                break;
        }
    }
}

template <typename T> T median(const T& first, const T& second, const T& third) {
    if (first <= second) {
        return second <= third ? second : third;
    } else
        return first <= third ? first : third;
}

template <typename RAIt> auto RandomPivot(RAIt first, RAIt last) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, last - first - 1);

    return *(first + dis(gen));
}

template <typename RAIt> void QSort(RAIt first, RAIt last) {
    if (last - first <= 16)
        return InsertionSort(first, last);

    //auto pivot = median(*first, *(last - 1), *(first + (last - first) / 2));
    auto pivot = RandomPivot(first, last);
    auto mid = Partition(first, last, pivot);

    if (mid - first < last - mid) {
        QSort(first, mid);
        QSort(mid, last);
    } else {
        QSort(mid, last);
        QSort(first, mid);
    }
}

void fillData(std::vector<int>& v) {
    size_t n = v.size();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n * 2);

    for (auto& it : v)
        it = dis(gen);
}

int main() {
    size_t n = 1000;

    std::vector<int> unsorted_vector(n);
    fillData(unsorted_vector);
    std::vector<int> sorted_vector = unsorted_vector;
    std::sort(sorted_vector.begin(), sorted_vector.end());

    //std::cout << "Unsorted before: " << unsorted_vector << std::endl;

    QSort(unsorted_vector.begin(), unsorted_vector.end());

    //std::cout << "Unsorted after: " << unsorted_vector << std::endl;

    assert(unsorted_vector == sorted_vector);

    return 0;
}
