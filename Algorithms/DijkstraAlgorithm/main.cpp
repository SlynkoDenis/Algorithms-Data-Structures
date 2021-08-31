#include <iostream>
#include <queue>
#include <vector>
#include "../../DataStructures/binary_heap/binary_heap.h"
#include "../../DataStructures/graph/graph.h"


struct Size {
    long int value;

    Size() : value(-1) {};

    Size(const Size& other) = default;

    explicit Size(size_t new_value) : value(new_value) {};

    Size& operator =(size_t new_value) {
        value = new_value;
        return *this;
    };

    explicit operator int() const {
        return static_cast<int>(value);
    };
};

Size operator +(const Size& lhs, const Size& rhs) {
    Size result;

    if (lhs.value == -1 || rhs.value == -1) {
        result.value = -1;
    } else {
        result.value = lhs.value + rhs.value;
    }
    return result;
}

Size operator +(const Size& lhs, const size_t& rhs) {
    Size result;
    if (lhs.value != -1) {
        result.value = static_cast<long int>(rhs) + lhs.value;
    }
    return result;
}

std::ostream& operator <<(std::ostream& os, const Size& obj) {
    os << obj.value;
    return os;
}

bool operator <(const Size& lhs, const Size& rhs) {
    if (lhs.value != -1 && rhs.value != -1) {
        return lhs.value < rhs.value;
    } else if (rhs.value != -1) {
        return false;
    } else {
        return lhs.value != -1;
    }
}

bool operator ==(const Size& lhs, const Size& rhs) {
    return lhs.value == rhs.value;
}

bool operator <=(const Size& lhs, const Size& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename T>
bool operator ==(const T& lhs, const Size& rhs) {
    return lhs == rhs.value;
}

template <typename T>
bool operator ==(const Size& lhs, const T& rhs) {
    return lhs.value == rhs;
}

bool operator !=(const Size& lhs, const Size& rhs) {
    return !(lhs == rhs);
}

bool operator >(const Size& lhs, const Size& rhs) {
    return !(lhs < rhs) && (lhs != rhs);
}

namespace {
    template <typename T>
    std::ostream& operator <<(std::ostream& os, const std::vector<T>& v) {
        for (const auto& it : v) {
            os << it << " ";
        }
        return os;
    }

    std::vector<Size> Dijkstra(const graph::DirectedGraph<graph::Node> &target_graph, const size_t &start) {
        if (start >= target_graph.number_of_vertices()) {
            throw std::invalid_argument("start position");
        }

        std::vector<Size> result(target_graph.number_of_vertices());
        result[start] = 0;

        std::vector<bool> used(target_graph.number_of_vertices(), true);
        BinaryHeap::BinaryHeap<Size, size_t> bheap;

        for (size_t i = 0, end_index = target_graph.number_of_vertices(); i < end_index; ++i) {
            if (i != start) {
                bheap.insert(Size(), i);
            } else {
                bheap.insert(Size(0), i);
            }
        }

        while (!bheap.empty()) {
            decltype(auto) node = bheap.extract_min();

            used[node.get_value()] = false;
            for (const auto &it : target_graph[node.get_value()]) {
                if (used[it.number]) {
                    if (result[node.get_value()] + it.weight < result[it.number]) {
                        result[it.number] = result[node.get_value()] + it.weight;
                    }
                }
            }

            bheap.clear();
            for (size_t i = 0, end_index = target_graph.number_of_vertices(); i < end_index; ++i) {
                if (used[i]) {
                    bheap.insert(Size(result[i]), i);
                }
            }
        }

        return result;
    }

    bool BFS(const graph::DirectedGraph<graph::Node>& target_graph, const size_t& start, const size_t& goal) {
         if (start >= target_graph.number_of_vertices() || goal >= target_graph.number_of_vertices()) {
             throw std::invalid_argument("nodes");
         }

         std::vector<bool> visited(target_graph.number_of_vertices(), false);
         visited[start] = true;

         std::queue<size_t> unvisited_vertices;
         unvisited_vertices.push(start);

         while (!unvisited_vertices.empty()) {
             auto tmp = unvisited_vertices.front();
             unvisited_vertices.pop();

             if (tmp == goal) {
                 return true;
             }

             for (auto it : target_graph[tmp]) {
                 if (!visited[it.number]) {
                     unvisited_vertices.push(it.number);
                     visited[it.number] = true;
                 }
             }
         }

         return false;
    }
}

int main() {
    decltype(auto) example_graph = graph::generate_random_directed_graph<graph::Node>(10, 15, 20);
    std::cout << example_graph << std::endl << std::endl;

    std::cout << Dijkstra(example_graph, 0) << std::endl;

    return 0;
}
