#pragma once

#include <stdexcept>
#include <tuple>
#include <utility>


namespace CartesianTree {
    template<typename K, typename P>
    struct Node {
        K key;
        P priority;
        Node<K, P> *predecessor;
        Node<K, P> *left;
        Node<K, P> *right;

        Node() : key(K()), priority(P()), left(nullptr), right(nullptr), predecessor(nullptr) {}

        Node(const Node &other) : key(other.key), priority(other.priority), left(nullptr), right(nullptr),
                                  predecessor(nullptr) {}

        Node &operator=(const Node &other) {
            Node<K, P> tmp(other);
            std::swap(tmp, *this);
            return *this;
        }

        Node &operator=(Node &&other) noexcept {
            key = std::move(other.key);
            priority = std::move(other.priority);
            left = other.left;
            right = other.right;
            predecessor = other.predecessor;

            other.left = nullptr;
            other.right = nullptr;
            other.predecessor = nullptr;
            return *this;
        }

        Node(Node &&other) noexcept: key(std::move(other.key)), priority(std::move(other.priority)), left(other.left),
                                     right(other.right), predecessor(other.predecessor) {
            other.left = nullptr;
            other.right = nullptr;
            other.predecessor = nullptr;
        }

        Node(const K &new_key, const P &new_priority) : key(new_key), priority(new_priority), left(nullptr),
                                                        right(nullptr), predecessor(nullptr) {}

        bool equals(const Node &other) {
            return key == other.key && priority == other.priority;
        }

        size_t get_weight();
    };

    template<typename K, typename P>
    bool operator==(const Node<K, P> &lhs, const Node<K, P> &rhs) {
        return lhs.priority == rhs.priority && lhs.key == rhs.key && lhs.left == rhs.left && lhs.right == rhs.right &&
               lhs.predecessor == rhs.predecessor;
    }

    template<typename K, typename P>
    size_t Node<K, P>::get_weight() {
        if (left == nullptr && right == nullptr) {
            return 1;
        }
        if (left == nullptr) {
            return right->get_weight() + 1;
        }
        if (right == nullptr) {
            return left->get_weight() + 1;
        }

        return left->get_weight() + right->get_weight() + 1;
    }

    template<typename K, typename P>
    void pre_order_copy(Node<K, P> *target, Node<K, P> *copy) {
        if (copy->left) {
            target->left = new Node<K, P>(*copy->left);
            target->left->predecessor = target;
            pre_order_copy(target->left, copy->left);
        }
        if (copy->right) {
            target->right = new Node<K, P>(*copy->right);
            target->right->predecessor = target->right;
            pre_order_copy(target->right, copy->right);
        }
    }

    template<typename K, typename P>
    Node<K, P> *merge(Node<K, P> *lhs, Node<K, P> *rhs) {
        if (lhs == nullptr && rhs == nullptr) {
            return lhs;
        } else if (lhs == nullptr) {
            return rhs;
        } else if (rhs == nullptr) {
            return lhs;
        }

        if (lhs->priority <= rhs->priority) {
            lhs->right = merge(lhs->right, rhs);
            if (lhs->right) {
                lhs->right->predecessor = lhs;
            }
            return lhs;
        } else {
            rhs->right = merge(rhs->right, lhs);
            if (rhs->right) {
                rhs->right->predecessor = rhs;
            }
            return rhs;
        }
    }

    template<typename K, typename P>
    std::tuple<Node<K, P> *, Node<K, P> *, Node<K, P> *> split(Node<K, P> *top, const K &pivot) {
        if (!top) {
            return std::make_tuple(nullptr, nullptr, nullptr);
        }

        if (top->key == pivot) {
            return std::make_tuple(top->left, top, top->right);
        } else if (top->key < pivot) {
            auto tmp_split = split(top->right, pivot);

            top->right = std::get<0>(tmp_split);
            if (top->right) {
                top->right->predecessor = top;
            }
            return std::make_tuple(top, std::get<1>(tmp_split), std::get<2>(tmp_split));
        } else {
            auto tmp_split = split(top->left, pivot);

            top->left = std::get<2>(tmp_split);
            if (top->left) {
                top->left->predecessor = top;
            }
            return std::make_tuple(std::get<0>(tmp_split), std::get<1>(tmp_split), top);
        }
    }

    template<typename K, typename P>
    struct CartesianTree {
        Node<K, P> *top;
        Node<K, P> *maximum;
        size_t sz;

        CartesianTree() : top(nullptr), maximum(nullptr), sz(0) {};

        CartesianTree(const K &top_key, const P &top_priority) {
            top = new Node<K, P>(top_key, top_priority);
            maximum = top;
            sz = 1;
        }

        explicit CartesianTree(Node<K, P> *new_top) {
            if (new_top) {
                sz = new_top->get_weight();

                top = new_top;

                maximum = top;
                while (maximum->right) {
                    maximum = maximum->right;
                }
            } else {
                top = nullptr;
                maximum = nullptr;
                sz = 0;
            }
        }

        CartesianTree(const CartesianTree &other) {
            if (other.sz) {
                sz = other.sz;

                top = new Node<K, P>(*other.top);
                pre_order_copy(top, other.top);

                maximum = top;
                while (maximum->right) {
                    maximum = maximum->right;
                }
            } else {
                sz = 0;
                top = nullptr;
                maximum = nullptr;
            }
        };

        CartesianTree(CartesianTree &&other) noexcept {
            top = other.top;
            maximum = other.maximum;
            sz = other.sz;

            other.top = nullptr;
            other.maximum = nullptr;
            other.sz = 0;
        }

        CartesianTree &operator=(CartesianTree &&other) noexcept {
            while (top) {
                if (top->left) {
                    top = top->left;
                }
                else if (top->right) {
                    top = top->right;
                }
                else {
                    auto node_to_delete = top;
                    top = top->predecessor;

                    if (!top) {
                        delete node_to_delete;
                        break;
                    }

                    if (node_to_delete == top->left) {
                        top->left = nullptr;
                    }
                    else {
                        top->right = nullptr;
                    }

                    delete node_to_delete;
                }
            }

            top = other.top;
            maximum = other.maximum;
            sz = other.sz;

            other.top = nullptr;
            other.maximum = nullptr;
            other.sz = 0;
            return *this;
        }

        CartesianTree &operator=(const CartesianTree &other) {
            CartesianTree tmp(other);
            std::swap(tmp, *this);
            return *this;
        }

        [[nodiscard]] bool empty() const {
            return sz == 0;
        }

        [[nodiscard]] size_t size() const {
            return sz;
        }

        ~CartesianTree() {
            while (top) {
                if (top->left) {
                    top = top->left;
                }
                else if (top->right) {
                    top = top->right;
                }
                else {
                    auto node_to_delete = top;
                    top = top->predecessor;

                    if (!top) {
                        delete node_to_delete;
                        break;
                    }

                    if (node_to_delete == top->left) {
                        top->left = nullptr;
                    }
                    else {
                        top->right = nullptr;
                    }

                    delete node_to_delete;
                }
            }
        }
    };

/*template <typename K, typename P>
std::tuple<CartesianTree<K, P>, Node<K, P> *, CartesianTree<K, P>> split(CartesianTree<K, P> tree, const K& pivot) {
    auto tmp_split = split(tree->top, pivot);

    return std::make_tuple(CartesianTree<K, P>(std::get<0>(tmp_split)), std::get<1>(tmp_split), CartesianTree<K, P>(std::get<2>(tmp_split)));
}*/

    template<typename K, typename P>
    std::tuple<CartesianTree<K, P>, Node<K, P> *, CartesianTree<K, P>>
    split(CartesianTree<K, P> &&tree, const K &pivot) {
        auto tmp_split = split(tree.top, pivot);

        tree.top = nullptr;
        tree.maximum = nullptr;
        tree.sz = 0;

        return std::make_tuple(CartesianTree<K, P>(std::get<0>(tmp_split)), std::get<1>(tmp_split),
                               CartesianTree<K, P>(std::get<2>(tmp_split)));
    }

    template<typename K, typename P>
    void merge(CartesianTree<K, P> &lhs, CartesianTree<K, P> &rhs) {
        if (rhs.empty()) {
            return;
        }
        else if (lhs.empty()) {
            lhs = std::move(rhs);
            return;
        }

        lhs.top = merge(lhs.top, rhs.top);
        lhs.sz += rhs.sz;
        lhs.maximum = lhs.top;
        while (lhs.maximum->right) {
            lhs.maximum = lhs.maximum->right;
        }

        rhs.sz = 0;
        rhs.top = nullptr;
        rhs.maximum = nullptr;
    }

    template<typename K, typename P>
    void merge(CartesianTree<K, P> &lhs, CartesianTree<K, P> &&rhs) {
        if (rhs.empty()) {
            return;
        }
        if (lhs.empty()) {
            lhs = std::move(rhs);
            return;
        }

        lhs.top = merge(lhs.top, rhs.top);
        lhs.sz += rhs.sz;
        lhs.maximum = lhs.top;
        while (lhs.maximum->right) {
            lhs.maximum = lhs.maximum->right;
        }
    }

    template<typename K, typename P>
    CartesianTree<K, P> build_CartesianTree(std::pair <K, P> *buffer, size_t sz) {
        if (!buffer) {
            throw std::invalid_argument("nodes list");
        }

        CartesianTree<K, P> result;
        result.sz = sz;
        result.top = new Node<K, P>(buffer->first, buffer->second);
        result.maximum = result.top;
        for (size_t i = 1; i < sz; ++i) {
            auto tmp = *(buffer + i);

            if (result.top->priority >= tmp.second) {
                auto new_node = new Node<K, P>(tmp.first, tmp.second);
                new_node->left = result.top;
                result.top->predecessor = new_node;
                result.top = new_node;
                result.maximum = result.top;
            } else if (result.maximum->priority <= tmp.second) {
                auto new_node = new Node<K, P>(tmp.first, tmp.second);
                new_node->predecessor = result.maximum;
                result.maximum->right = new_node;
                result.maximum = result.maximum->right;
            } else {
                auto ptr = result.maximum;

                while (ptr->priority > tmp.second) {
                    ptr = ptr->predecessor;
                }

                auto new_node = new Node<K, P>(tmp.first, tmp.second);
                new_node->predecessor = ptr;
                new_node->left = ptr->right;
                ptr->right = new_node;

                result.maximum = new_node;
            }
        }
        return result;
    }

    template<typename K, typename P>
    CartesianTree<K, P> insert(CartesianTree<K, P> &&tree, const std::pair <K, P> &element) {
        auto split_result = split(std::move(tree), element.first);

        if (std::get<1>(split_result)) {
            merge(std::get<0>(split_result), std::get<2>(split_result));
            return std::get<0>(split_result);
        }

        merge(std::get<0>(split_result), CartesianTree<K, P>(element.first, element.second));
        merge(std::get<0>(split_result), std::get<2>(split_result));

        return std::get<0>(split_result);
    }
}
