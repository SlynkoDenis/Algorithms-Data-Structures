#pragma once

#include <stdexcept>
#include <vector>


namespace DisjointSetUnion {
    template<typename N>
    struct Node final {
        size_t rank;
        N data;
        Node *parent;

        Node() : rank(0), data(N()), parent(nullptr) {};

        Node(const Node &other) : rank(other.rank), data(other.data), parent(nullptr) {};

        Node(Node &&other) noexcept: rank(other.rank), data(std::move(other.data)), parent(other.parent) {
            other.parent = nullptr;
            other.rank = 0;
        };

        Node &operator=(const Node &other) {
            Node<N> tmp(other);
            std::swap(*this, tmp);
            return *this;
        };

        Node &operator=(Node &&other) noexcept {
            rank = other.rank;
            data = std::move(other.data);
            parent = other.data;

            other.rank = 0;
            other.data = nullptr;
            return *this;
        };

        ~Node() noexcept = default;

        explicit Node(const N &new_data) : data(new_data), rank(0), parent(nullptr) {};

        explicit Node(N &&new_data) : data(std::move(new_data)), rank(0), parent(nullptr) {};

        Node *root();
    };

    template<typename N>
    Node<N> *Node<N>::root() {
        Node<N> *result = parent;
        if (!result) {
            return this;
        }

        while (result->parent) {
            result = result->parent;
        }
        return result;
    }

    template<typename N>
    class DSU final {
        std::vector<Node<N> *> universum;

    public:
        DSU() = default;

        DSU(const DSU &other) = delete;

        DSU(DSU &&other) noexcept : universum(other.universum.size()) {
            for (size_t i = 0, end_ = other.universum.size(); i < end_; ++i) {
                universum[i] = other.universum[i];
            }

            other.universum.clear();
        };

        DSU &operator=(const DSU &other) = delete;

        DSU &operator=(DSU &&other) noexcept {
            for (auto &it : universum) {
                delete it;
            }

            universum = std::move(other.universum);
            other.universum.clear();
            return *this;
        };

        ~DSU() noexcept {
            for (auto &it : universum) {
                delete it;
            }
        };

        explicit DSU(size_t sz) : universum(sz, nullptr) {};

        void reserve(size_t cap) {
            universum.reserve(cap);
        };

        Node<N> *add_element(const N &data);

        Node<N> *add_element(N &&data);

        void unite(Node<N> *lhs, Node<N> *rhs);

        bool equivalent(Node<N> *lhs, Node<N> *rhs) const;
    };

    template<typename N>
    Node<N> *DSU<N>::add_element(const N &data) {
        universum.push_back(new Node<N>(data));
        return universum.back();
    }

    template<typename N>
    Node<N> *DSU<N>::add_element(N &&data) {
        universum.push_back(new Node<N>(std::move(data)));
        return universum.back();
    }

    template<typename N>
    bool DSU<N>::equivalent(Node<N> *lhs, Node<N> *rhs) const {
        if (lhs == nullptr || rhs == nullptr) {
            throw std::invalid_argument("elements");
        }
        if (lhs == rhs) {
            return true;
        }

        auto lparent = lhs->root();
        auto rparent = rhs->root();

        for (auto it = lhs; it != lparent; it = it->parent) {
            it->parent = lparent;
        }
        for (auto it = rhs; it != rparent; it = it->parent) {
            it->parent = rparent;
        }

        return lparent == rhs->root();
    }

    template<typename N>
    void DSU<N>::unite(Node<N> *lhs, Node<N> *rhs) {
        if (lhs == nullptr || rhs == nullptr) {
            throw std::invalid_argument("elements");
        }

        auto lparent = lhs->root();
        auto rparent = rhs->root();

        for (auto it = lhs; it != lparent; it = it->parent) {
            it->parent = lparent;
        }
        for (auto it = rhs; it != rparent; it = it->parent) {
            it->parent = rparent;
        }

        if (lparent == rparent) {
            return;
        }
        else {
            if (lparent->rank < rparent->rank) {
                lparent->parent = rparent;
            } else {
                rparent->parent = lparent;
                if (lparent->rank == rparent->rank) {
                    ++lparent->rank;
                }
            }
        }
    }
}
