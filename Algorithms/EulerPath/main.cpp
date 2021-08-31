#include <iostream>
#include <list>
#include <stack>

#include "../../DataStructures/graph/graph.h"


namespace {
    template <typename T>
    std::ostream& operator <<(std::ostream& os, const std::list<T>& object) {
        for (const auto& it : object) {
            os << it << " ";
        }
        return os;
    }

    template <typename T>
    std::ostream& operator <<(std::ostream& os, const std::vector<T>& object) {
        for (const auto& it : object) {
            os << it << " ";
        }
        return os;
    }

    template <typename N>
    void dfs(const graph::DirectedGraph<N> &target_graph, std::vector<bool> &visited, size_t i) {
        visited[i] = true;
        for (const auto& it : target_graph[i]) {
            if (!visited[static_cast<size_t>(it)]) {
                dfs(target_graph, visited, static_cast<size_t>(it));
            }
        }
    }

    template <typename N>
    void dfs(const graph::UndirectedGraph<N> &target_graph, std::vector<bool> &visited, size_t i) {
        visited[i] = true;
        for (const auto& it : target_graph[i]) {
            if (!visited[static_cast<size_t>(it)]) {
                dfs(target_graph, visited, static_cast<size_t>(it));
            }
        }
    }

    template <typename N>
    bool checkEulerPath(const graph::UndirectedGraph<N> &target_graph) {
        if (target_graph.empty()) {
            return true;
        }

        size_t oddVertex = 0;
        for (const auto &it : target_graph) {
            if (it.size() % 2 == 1) {
                ++oddVertex;
            }
        }
        if (oddVertex > 2) {
            return false;
        }

        std::vector<bool> visited(target_graph.number_of_vertices(), false);
        for (size_t i = 0, end = target_graph.number_of_vertices(); i < end; ++i) {
            if (!target_graph[i].empty()) {
                dfs(target_graph, visited, i);
                break;
            }
        }
        for (size_t i = 0, end_ = target_graph.number_of_vertices(); i < end_; ++i) {
            if (!target_graph[i].empty() && !visited[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename N>
    bool checkEulerPath(const graph::DirectedGraph<N> &target_graph) {
        if (target_graph.empty()) {
            return true;
        }

        size_t incorrectness = 0;
        for (size_t i = 0, end = target_graph.number_of_vertices(); i < end; ++i) {
            int inDegree = 0;
            for (size_t j = 0; j < end; ++j) {
                inDegree += std::count_if(target_graph[j].begin(), target_graph[j].end(), [i](const N& node) {
                    return i == node;
                });
            }

            if (std::abs(inDegree - static_cast<int>(target_graph[i].size())) == 1) {
                ++incorrectness;
            }
            if (std::abs(inDegree - static_cast<int>(target_graph[i].size())) > 1 || incorrectness > 2) {
                return false;
            }
        }

        std::vector<bool> visited(target_graph.number_of_vertices(), false);
        for (size_t i = 0, end = target_graph.number_of_vertices(); i < end; ++i) {
            if (!target_graph[i].empty()) {
                dfs(target_graph, visited, i);
                break;
            }
        }
        for (size_t i = 0, end = target_graph.number_of_vertices(); i < end; ++i) {
            if (!target_graph[i].empty() && !visited[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename N>
    std::vector<size_t> UndirectedEulerPath(graph::UndirectedGraph<N> target_graph) {
        size_t start = 0;
        for (size_t i = 0, end = target_graph.number_of_vertices(); i < end; ++i) {
            if (target_graph[i].size() % 2 == 1) {
                start = i;
                break;
            }
        }

        std::vector<size_t> result;

        std::stack<size_t> stack;
        stack.push(start);
        while (!stack.empty()) {
            auto vert = stack.top();
            if (!target_graph[vert].empty()) {
                stack.push(static_cast<size_t>(target_graph[vert][0]));
                auto second_arg = target_graph[vert][0];            // necessary because of changed state of g[vert][0] after first
                                                                    // erase in remove_edge function, which receives the second argument
                                                                    // as const reference
                target_graph.remove_edge(vert, second_arg);
            }

            if (vert == stack.top()) {
                stack.pop();
                result.push_back(vert);
            }
        }

        return result;
    }

    template <typename N>
    std::list<size_t> DirectedEulerPath(graph::DirectedGraph<N> target_graph) {
        size_t start = 0;
        for (size_t i = 0, end = target_graph.number_of_vertices(); i < end; ++i) {
            size_t inDegree = 0;
            for (size_t j = 0; j < end; ++j) {
                inDegree += std::count_if(target_graph[j].begin(), target_graph[j].end(), [i](const N& node) {
                    return i == node;
                });
            }

            if (target_graph[i].size() - inDegree == 1) {
                start = i;
                break;
            }
        }

        std::list<size_t> result;

        std::stack<size_t> stack;
        stack.push(start);
        while (!stack.empty()) {
            auto vert = stack.top();
            if (!target_graph[vert].empty()) {
                stack.push(static_cast<size_t>(target_graph[vert][0]));
                auto second_arg = target_graph[vert][0];            // necessary because of changed state of g[vert][0] after first
                                                                    // erase in remove_edge function, which receives the second argument
                                                                    // as const reference
                target_graph.remove_edge(vert, second_arg);
            }

            if (vert == stack.top()) {
                stack.pop();
                result.push_front(vert);
            }
        }

        return result;
    }
}

int main() {
    std::cout << "Undirected graph" << std::endl;
    graph::UndirectedGraph<size_t> undirected_g;
    undirected_g.generate_random_graph(4, 5);
    while (!checkEulerPath(undirected_g)) {
        undirected_g.generate_random_graph(4, 5);
    }
    std::cout << undirected_g << std::endl;
    std::cout << "The Eulerian path is: " << UndirectedEulerPath(undirected_g) << std::endl;

    std::cout << "Directed graph" << std::endl;
    graph::DirectedGraph<size_t> directed_g;
    directed_g.generate_random_graph(4, 6);
    while (!checkEulerPath(directed_g)) {
        directed_g.generate_random_graph(4, 6);
    }
    std::cout << directed_g << std::endl;
    std::cout << "The Eulerian path is: " << DirectedEulerPath(directed_g) << std::endl;

    return 0;
}
