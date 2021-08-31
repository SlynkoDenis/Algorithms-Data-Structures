#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>


namespace Binomial {
    template<typename N>
    struct Node final {
        N key;
        std::vector<Node<N> *> children;

        Node() : key(N()) {};

        explicit Node(const N &new_key) : key(new_key) {};

        template<typename ... Args>
        explicit Node(Args &&... args) : key(std::forward<Args>(args)...) {};

        Node(const Node &other) = delete;

        Node(Node &&other) noexcept : key(std::move(other.key)), children(std::move(other.children)) {
            other.key = N();
            other.children.clear();
        };

        Node &operator=(const Node &other) = delete;

        Node &operator=(Node &&other) noexcept {
            for (auto &it : children) {
                delete_from(it);
            }
            key = std::move(other.key);
            children = std::move(other.children);

            return *this;
        };

        ~Node() noexcept {
            for (auto &it : children) {
                delete_from(it);
            }
        };
    };

    template<typename N>
    void delete_from(Node<N> *start) {
        if (start == nullptr) {
            return;
        }
        else if (start->children.empty()) {
            delete start;
        }
        for (size_t i = start->children.size() - 1; i >= 0; --i) {
            delete_from(start->children[i]);
            start->children.pop_back();
        }
        delete start;
    }

    template<typename N>
    Node<N> *copy_from(const Node<N> *other) {
        if (!other) {
            return nullptr;
        }

        auto tmp = new Node<N>{other->key};
        if (other->children.empty()) {
            return tmp;
        }

        tmp->children.resize(other->children.size());
        for (size_t i = 0, end = other->children.size(); i < end; ++i) {
            tmp->children[i] = copy_from(other->children[i]);
        }
        return tmp;
    }


    template<typename N>
    class BinomialTree final {
        Node<N> *top;
        size_t sz;

    public:
        BinomialTree() : top(nullptr), sz(0) {};

        BinomialTree(Node<N> *node, size_t new_sz) : top(node), sz(new_sz) {};

        BinomialTree(const BinomialTree &other) : top(nullptr), sz(other.sz) {
            try {
                top = copy_from(other.top);
            } catch (...) {
                delete_from(top);
                top = nullptr;
                sz = 0;

                throw;
            }
        };

        BinomialTree(BinomialTree &&other) noexcept : top(other.top), sz(other.sz) {
            other.top = nullptr;
            other.sz = 0;
        };

        BinomialTree &operator=(const BinomialTree &other) {
            BinomialTree<N> tmp(other);
            std::swap(*this, tmp);

            return *this;
        };

        BinomialTree &operator=(BinomialTree &&other) noexcept {
            delete top;

            top = other.top;
            sz = other.sz;

            other.top = nullptr;
            other.sz = 0;

            return *this;
        };

        ~BinomialTree() noexcept {
            delete top;
        };

        [[nodiscard]] size_t size() const {
            return sz;
        };

        [[nodiscard]] size_t get_degree() const {
            return static_cast<size_t>(std::log2(sz));
        };

        [[nodiscard]] bool empty() const {
            return sz == 0;
        };

        void merge(BinomialTree *other) {
            if (other->size() != size()) {
                throw std::invalid_argument("size of trees");
            }

            top->children.push_back(other->top);
            sz *= 2;

            other->top = nullptr;
            other->sz = 0;
        };

        const N &get_top_key() const {
            return top->key;
        };

        BinomialTree<N> *decay() {
            if (!top || top->children.empty()) {
                return nullptr;
            }

            size_t decr_size = std::pow(2.0f, top->children.size() - 1);
            auto res = new BinomialTree<N>{top->children[top->children.size() - 1], decr_size};
            sz -= decr_size;
            top->children.pop_back();

            return res;
        };
    };


    template<typename N>
    class BinomialHeap final {
        size_t sz;
        std::vector<BinomialTree<N> *> trees;
        N min;

    public:
        BinomialHeap() : sz(0), min(N()) {};

        explicit BinomialHeap(size_t new_sz) : sz(new_sz), trees(new_sz, nullptr), min(N()) {};

        BinomialHeap(const BinomialHeap &other) : sz(other.sz), trees(other.trees.size(), nullptr), min(other.min) {
            try {
                for (size_t i = 0, end_ = trees.size(); i < end_; ++i) {
                    if (other.trees[i]) {
                        trees[i] = new BinomialTree<N>{other.trees[i]};
                    }
                }
            } catch (...) {
                for (auto &it : trees) {
                    delete it;
                }
                trees.clear();
                sz = 0;
                min = N();

                throw;
            }
        };

        BinomialHeap(BinomialHeap &&other) noexcept : sz(other.sz), trees(std::move(other.trees)),
                                                      min(std::move(other.min)) {
            other.sz = 0;
        };

        BinomialHeap &operator=(const BinomialHeap &other) {
            BinomialHeap<N> tmp(other);
            std::swap(*this, tmp);

            return *this;
        };

        BinomialHeap &operator=(BinomialHeap &&other) noexcept {
            for (auto &it : trees) {
                delete it;
            }

            sz = other.sz;
            trees = std::move(other.trees);
            min = std::move(other.min);

            other.sz = 0;

            return *this;
        };

        ~BinomialHeap() noexcept {
            for (auto &it : trees) {
                delete it;
            }
        };

        [[nodiscard]] size_t size() const {
            return sz;
        };

        [[nodiscard]] bool empty() const {
            return sz == 0;
        };

        template<typename ... Args>
        void insert(Args &&... args);

        const N &get_min() const {
            return min;
        };

        N extract_min();

        void merge(BinomialTree<N> *tree);

        void merge(BinomialHeap<N> other);
    };

    template<typename N>
    template<typename ... Args>
    void BinomialHeap<N>::insert(Args &&... args) {
        merge(new BinomialTree<N>{new Node<N>{std::forward<Args>(args) ...}, 1});
        if (sz == 1) {
            min = trees[0]->get_top_key();
        }
    }

    template<typename N>
    void BinomialHeap<N>::merge(BinomialTree<N> *tree) {
        if (!tree) {
            return;
        }

        min = std::min(min, tree->get_top_key());

        auto tree_degree = tree->get_degree();
        if (tree_degree >= trees.size()) {
            trees.reserve(tree_degree + 1);
            for (size_t i = trees.size(); i < tree_degree; ++i) {
                trees.push_back(nullptr);
            }
            trees.push_back(tree);

            sz += tree->size();
            return;
        } else if (!trees[tree_degree]) {
            trees[tree_degree] = tree;
            sz += tree->size();

            return;
        } else {
            sz += tree->size();

            size_t end_sz = tree_degree + 1;
            while (end_sz != trees.size() && trees[end_sz] != nullptr) {
                ++end_sz;
            }
            if (end_sz == trees.size()) {
                trees.push_back(nullptr);
            }

            BinomialTree<N> *tmp = trees[tree_degree];
            for (size_t i = tree_degree; i < end_sz; ++i) {
                if (tmp->get_top_key() <= tree->get_top_key()) {
                    tmp->merge(tree);
                    tree = tmp;
                    tmp = trees[i + 1];
                } else {
                    tree->merge(tmp);
                    tmp = trees[i + 1];
                }

                trees[i] = nullptr;
            }
            trees[end_sz] = tree;
        }
    }

    template<typename N>
    void BinomialHeap<N>::merge(BinomialHeap<N> other) {
        trees.resize(static_cast<size_t>(std::log2(sz + other.sz)) + 1, nullptr);

        for (auto &it : other.trees) {
            merge(it);
        }
    }

    template<typename N>
    N BinomialHeap<N>::extract_min() {
        if (trees.empty()) {
            throw std::range_error("heap underflow");
        }

        N min_ref = min;
        auto tmp = std::find_if(trees.begin(), trees.end(), [&min_ref](BinomialTree<N> *other) {
            if (other) {
                return min_ref == other->get_top_key();
            }
            else {
                return false;
            }
        });
        BinomialTree<N> *tree_to_delete = *tmp;
        trees[tmp - trees.begin()] = nullptr;

        sz -= tree_to_delete->size();

        auto tree = tree_to_delete->decay();
        while (tree) {
            merge(tree);
            tree = tree_to_delete->decay();
        }
        delete tree_to_delete;

        size_t i = 0;
        for (size_t end = trees.size(); i < end; ++i) {
            if (trees[i]) {
                min = trees[i]->get_top_key();
                break;
            }
        }
        for (size_t end = trees.size(); i < end; ++i) {
            if (trees[i] != nullptr && min > trees[i]->get_top_key()) {
                min = trees[i]->get_top_key();
            }
        }

        return min_ref;
    }
}
