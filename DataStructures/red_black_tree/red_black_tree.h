#pragma once

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace RedBlackTree {
    template<typename K, typename V>
    class Node final {
    public:
        explicit Node(K tmp_key = static_cast<K>(0),
                      const V tmp_data = static_cast<V>(0)) : data{tmp_key, tmp_data}, color(false), anc(nullptr),
                                                              left(nullptr), right(nullptr) {};

        Node(const Node &other) : data{other.data}, color(other.color),
                                  anc(nullptr), left(nullptr), right(nullptr) {};

        Node &operator=(const Node &other) {
            if (this != &other) {
                Node tmp(other);
                std::swap(*this, tmp);
            }

            return *this;
        };

        Node(Node &&other) noexcept : data{other.data}, color(other.color),
                                      anc(other.anc), left(other.left), right(other.right) {
            other.anc = nullptr;
            other.left = nullptr;
            other.right = nullptr;
        };

        Node &operator=(Node &&other) noexcept {
            data = other.data;
            color = other.color;
            anc = other.anc;
            left = other.left;
            right = other.right;

            other.anc = nullptr;
            other.left = nullptr;
            other.right = nullptr;

            return *this;
        };

        void inorder_traversal(std::vector<int> &res) const {
            /**
            *  @brief Writes all keys starting from the given as the top in inorder way.
            *  @param res  target vector.
            */
            if (left) {
                left->inorder_traversal(res);
            }

            res.emplace_back(data.first);

            if (right) {
                right->inorder_traversal(res);
            }
        };

        void inorder_print() {
            if (left) {
                left->inorder_print();
            }

            std::cout << "Node with key " << data.first << " contains value " << data.second << std::endl;

            if (right) {
                right->inorder_print();
            }
        };

        Node *node_search(const K &key_to_find);

        [[nodiscard]] bool check_rb(bool flag = true) const;

        void rewrite(const Node *tmp) {
            data.first = tmp->data.first;
            data.second = tmp->data.second;
        };

        [[nodiscard]] bool has_sons() const {
            return left || right;
        }

        [[nodiscard]] bool get_color() const {
            return color;
        };

        K get_key() const {
            return data.first;
        };

        Node *get_left() {
            return left;
        };

        Node *get_right() {
            return right;
        };

        Node *get_son_in_order() {
            if (left) {
                return left;
            } else if (right) {
                return right;
            }
            return nullptr;
        }

        Node *get_anc() {
            return anc;
        };

        void set_color(bool new_color) {
            color = new_color;
        };

        void set_left(Node *new_left) {
            left = new_left;
        };

        void set_right(Node *new_right) {
            right = new_right;
        };

        void set_anc(Node *new_anc) {
            anc = new_anc;

        };

        void set_key(K new_key) {
            data.first = new_key;
        };

        [[nodiscard]] bool equals(const Node *other) const;

    private:
        std::pair<K, V> data;
        // false - black, true - red
        bool color;
        Node *anc;
        Node *left;
        Node *right;
    };

    template <typename K, typename V>
    bool Node<K, V>::equals(const Node* other) const {
        if (!other) {
            return false;
        }

        if ((data.second) && (other->data.second)) {
            return ((data.second == other->data.second) && (data.first == other->data.first));
        }

        if ((!data.second) && (!other->data.second)) {
            return (data.first == other->data.first);
        } else {
            return false;
        }
    }

    template<typename K, typename V>
    Node<K, V> *Node<K, V>::node_search(const K &key_to_find) {
        if (key_to_find < data.first && left) {
            return left->node_search(key_to_find);
        } else if (key_to_find > data.first && right) {
            return right->node_search(key_to_find);
        }
        return this;
    }

    template<typename K, typename V>
    bool Node<K, V>::check_rb(bool flag) const {
        /**
        *  @brief Checks if Node is the top of a Red-Black Tree.
        *  @param flag  true for default mode, false for zeroing static variables.
        */
        if (anc && color && anc->color) {
            return true;
        }

        static unsigned long s_tr_sum = 0;
        static unsigned long s_cur_sum = 0;

        if (!flag) {
            s_tr_sum = 0;
            s_cur_sum = 0;

            return false;
        }

        if (!color) {
            ++s_cur_sum;
        }

        if ((!left) && (!right)) {
            if ((s_tr_sum) && (s_cur_sum != s_tr_sum)) {
                return true;
            } else if ((!s_tr_sum) && (s_cur_sum)) {
                s_tr_sum = s_cur_sum;
                s_cur_sum = 0;
                return false;
            } else {
                s_cur_sum = 0;
                return false;
            }
        }

        unsigned long tmp_cur_sum = s_cur_sum;

        if (left && left->check_rb()) {
            return true;
        }

        s_cur_sum = tmp_cur_sum;
        if (right && right->check_rb()) {
            return true;
        }

        if (left) {
            s_tr_sum = 0;
            s_cur_sum = 0;

            if (left->check_rb()) {
                return true;
            }
        }

        if (right) {
            s_tr_sum = 0;
            s_cur_sum = 0;

            if (right->check_rb()) {
                return true;
            }
        }

        s_tr_sum = 0;
        s_cur_sum = 0;
        return false;
    }

    template <typename K, typename V>
    bool are_equivalent(const Node<K, V> *lhs, const Node<K, V> *rhs) {
        if ((!lhs) && (!rhs)) {
            return true;
        }
        if ((!lhs) || (!rhs)) {
            return false;
        }
        return lhs->equals(rhs);
    }


    template<typename K, typename V>
    class RedBlackTree {
    public:
        RedBlackTree() : top(nullptr) {};

        RedBlackTree(const RedBlackTree &other) {
            top = copy_construct(other.top, nullptr);
        };

        RedBlackTree &operator=(const RedBlackTree &other) {
            if (this != &other) {
                RedBlackTree<K, V> tmp(other);
                std::swap(*this, tmp);
            }

            return *this;
        };

        RedBlackTree(RedBlackTree &&other) noexcept : top(other.top) {
            other.top = nullptr;
        };

        RedBlackTree &operator=(RedBlackTree &&other) noexcept {
            RedBlackTree<K, V> tmp;
            tmp.top = top;
            top = other.top;
            other.top = nullptr;

            return *this;
        };

        ~RedBlackTree() noexcept {
            while (top) {
                if (top->get_left()) {
                    top = top->get_left();
                } else if (top->get_right()) {
                    top = top->get_right();
                } else {
                    Node<K, V> *node_to_delete = top;
                    top = top->get_anc();

                    if (!top) {
                        delete node_to_delete;
                        break;
                    }

                    if (are_equivalent(node_to_delete, top->get_left())) {
                        top->set_left(nullptr);
                    } else {
                        top->set_right(nullptr);
                    }

                    delete node_to_delete;
                }
            }
        };

        void inorder_print_tree() const {
            /**
            *  @brief Prints all keys and data in inorder way.
            */
            if (top) {
                top->inorder_print();
            }
        };

        Node<K, V> *find_node(const K &key_to_find) const {
            auto *res = tree_search(key_to_find);
            if ((!res) || (res->get_key() != key_to_find)) {
                return nullptr;
            }

            return res;
        };

        bool validate();

        Node<K, V> *insert_node(const K &tmp_keyey, const V &tmp_data);

        void delete_node(const K &k);

    private:
        Node<K, V> *top;

        void make_rb(Node<K, V> *pr_node);

        void del_make_rb(Node<K, V> *y);

        void change_anc(Node<K, V> *top1, Node<K, V> *target_node);

        void rotate_mid(Node<K, V> *bot, Node<K, V> *mid, Node<K, V> *top1);

        void rotate_c(Node<K, V> *bot, Node<K, V> *mid, Node<K, V> *top1);

        Node<K, V> *tree_search(const K &key_to_find) const {
            if (!top) {
                return nullptr;
            } else {
                return top->node_search(key_to_find);
            }
        };

        Node<K, V> *copy_construct(Node<K, V> *copy_top, Node<K, V> *parent) {
            if (!copy_top) {
                return nullptr;
            } else {
                auto *res = new Node<K, V>{*(copy_top)};

                res->set_anc(parent);
                res->set_left(copy_construct(copy_top->get_left(), res));
                res->set_right(copy_construct(copy_top->get_right(), res));

                return res;
            }
        };
    };

    template<typename K, typename V>
    Node<K, V> *RedBlackTree<K, V>::insert_node(const K &tmp_key, const V &tmp_data) {
        Node<K, V> *par = tree_search(tmp_key);
        if (!par) {
            top = new Node<K, V>{tmp_key, tmp_data};
            top->set_color(false);

            return top;
        } else if (par->get_key() == tmp_key) {
            return par;
        }

        auto *res = new Node<K, V>{tmp_key, tmp_data};
        res->set_color(true);
        res->set_anc(par);

        if (tmp_key < par->get_key()) {
            par->set_left(res);
        } else {
            par->set_right(res);
        }

        make_rb(res);

        return res;
    }

    template<typename K, typename V>
    void RedBlackTree<K, V>::make_rb(Node<K, V> *pr_node) {
        auto *x = pr_node;
        while (true) {
            if (!x || !x->get_color()) {
                return;
            }

            auto *y = x->get_anc();
            if (!y) {
                x->set_color(false);
                return;
            }
            if (x->get_color() && !x->get_anc()->get_color()) {
                return;
            }

            unsigned char mark = 0;
            auto *z = y->get_anc();
            if (!z) {
                return;
            }

            if (z->get_right() && are_equivalent(z->get_right(), y)) {
                mark += 2;
            }

            if (y->get_right() && are_equivalent(y->get_right(), x)) {
                mark += 1;
            }

            switch (mark) {
                case 3: {
                    auto *t = z->get_left();
                    if (!t) {
                        z->set_color(true);
                        y->set_color(false);

                        rotate_mid(x, y, z);
                    } else if (t->get_color()) {
                        y->set_color(false);
                        z->set_color(true);
                        t->set_color(false);

                        make_rb(z);
                    } else {
                        y->set_color(false);
                        z->set_color(true);

                        rotate_mid(x, y, z);
                    }
                    break;
                }
                case 2: {
                    auto *t = z->get_left();
                    if (!t) {
                        z->set_color(true);
                        x->set_color(false);

                        rotate_c(x, y, z);
                    } else if (t->get_color()) {
                        y->set_color(false);
                        z->set_color(true);
                        t->set_color(false);

                        make_rb(z);
                    } else {
                        x->set_anc(z);
                        y->set_anc(x);
                        if (x->get_right()) {
                            x->get_right()->set_anc(y);
                        }

                        y->set_left(x->get_right());
                        x->set_right(y);
                        z->set_right(x);

                        make_rb(y);
                    }
                    break;
                }
                case 1: {
                    auto *t = z->get_right();
                    if (!t) {
                        z->set_color(true);
                        x->set_color(false);

                        rotate_c(x, y, z);
                    } else if (t->get_color()) {
                        y->set_color(false);
                        z->set_color(true);
                        t->set_color(false);

                        make_rb(z);
                    } else {
                        x->set_anc(z);
                        y->set_anc(x);
                        if (x->get_left()) {
                            x->get_left()->set_anc(y);
                        }

                        y->set_right(x->get_left());
                        x->set_left(y);
                        z->set_left(x);

                        make_rb(y);
                    }
                    break;
                }
                case 0: {
                    auto *t = z->get_right();
                    if (!t) {
                        y->set_color(false);
                        z->set_color(true);

                        rotate_mid(x, y, z);
                    } else if (t->get_color()) {
                        y->set_color(false);
                        z->set_color(true);
                        t->set_color(false);

                        make_rb(z);
                    } else {
                        y->set_color(false);
                        z->set_color(true);

                        rotate_mid(x, y, z);
                    }
                    break;
                }
                default: {
                    throw std::logic_error("insert function");
                }
            }
            return;
        }
    }

    template<typename K, typename V>
    void RedBlackTree<K, V>::delete_node(const K &k) {
        auto *nd = tree_search(k);
        while (true) {
            if ((!nd) || (nd->get_key() != k)) {
                return;
            }

            if ((nd->get_left()) && (nd->get_right())) {
                auto *tmp = nd->get_right();
                while (tmp->get_left()) {
                    tmp = tmp->get_left();
                }

                nd->rewrite(tmp);
                tmp->set_key(k);

                nd = tmp;
                continue;
            }

            if (nd->get_color()) {
                if (nd->get_anc()) {
                    if (are_equivalent(nd->get_anc()->get_left(), nd)) {
                        nd->get_anc()->set_left(nullptr);
                    } else {
                        nd->get_anc()->set_right(nullptr);
                    }
                }

                delete nd;
                return;
            }

            if (nd->has_sons()) {
                if (nd->get_anc()) {
                    auto *son = nd->get_son_in_order();
                    if (son->get_color()) {
                        if (nd->get_anc()->get_left()) {
                            if (are_equivalent(nd->get_anc()->get_left(), nd)) {
                                nd->get_anc()->set_left(son);
                            } else {
                                nd->get_anc()->set_right(son);
                            }
                        }
                        else {
                            nd->get_anc()->set_right(son);
                        }

                        son->set_color(false);
                        son->set_anc(nd->get_anc());

                        delete nd;
                        return;
                    }
                } else {
                    auto *son = nd->get_son_in_order();
                    son->set_color(false);
                    son->set_anc(nullptr);
                    top = son;

                    delete nd;
                    return;
                }
            }

            if (!nd->get_anc()) {
                top = nullptr;
                delete nd;

                return;
            }

            del_make_rb(nd);

            Node<K, V> *son = nd->get_son_in_order();
            if (son) {
                son->set_anc(nd->get_anc());
            }

            if (are_equivalent(nd->get_anc()->get_left(), nd)) {
                nd->get_anc()->set_left(son);
            } else {
                nd->get_anc()->set_right(son);
            }

            delete nd;
            return;
        }
    }

    template<typename K, typename V>
    void RedBlackTree<K, V>::del_make_rb(Node<K, V> *y) {
        if (!y) {
            return;
        }

        Node<K, V> *x = y;

        while (true) {
            auto *parent = x->get_anc();
            if (!parent) {
                x->set_color(false);
                return;
            }

            Node<K, V> *sibling = are_equivalent(parent->get_left(), x) ? parent->get_right() : parent->get_left();

            if (sibling->get_color()) {
                sibling->set_color(false);
                parent->set_color(true);

                if (are_equivalent(parent->get_left(), x)) {
                    rotate_mid(sibling->get_right(), sibling, parent);
                } else {
                    rotate_mid(sibling->get_left(), sibling, parent);
                }

                parent = x->get_anc();
                sibling = are_equivalent(parent->get_left(), x) ? parent->get_right() : parent->get_left();
            }

            bool flag = false;
            if ((!parent->get_color()) && (!sibling->get_color())) {
                if (sibling->get_left() && sibling->get_left()->get_color()) {
                    flag = true;
                }
                if (sibling->get_right() && sibling->get_right()->get_color()) {
                    flag = true;
                }

                if (!flag) {
                    sibling->set_color(true);
                    x = parent;

                    continue;
                }
            }

            if ((parent->get_color()) && (!sibling->get_color())) {
                if (sibling->get_left()) {
                    flag = sibling->get_left()->get_color();
                }
                if (sibling->get_right() && sibling->get_right()->get_color()) {
                    flag = true;
                }

                if (!flag) {
                    parent->set_color(false);
                    sibling->set_color(true);

                    return;
                }
            }

            if (!sibling->get_color()) {
                if (are_equivalent(parent->get_left(), x)) {
                    if (sibling->get_left()) {
                        flag = !sibling->get_left()->get_color();
                    } else {
                        flag = true;
                    }

                    if (sibling->get_right() && sibling->get_right()->get_color()) {
                        flag = true;
                    }

                    if (!flag) {
                        auto *tmp = sibling->get_left();

                        tmp->set_color(false);
                        sibling->set_color(true);

                        tmp->set_anc(parent);
                        parent->set_right(tmp);
                        sibling->set_anc(tmp);

                        if (tmp->get_right()) {
                            tmp->get_right()->set_anc(sibling);
                        }
                        sibling->set_left(tmp->get_right());

                        tmp->set_right(sibling);

                        parent = x->get_anc();
                        sibling = are_equivalent(parent->get_left(), x) ? parent->get_right() : parent->get_left();
                    }
                } else {
                    if (sibling->get_right()) {
                        flag = !sibling->get_right()->get_color();
                    } else {
                        flag = true;
                    }

                    if (sibling->get_left() && sibling->get_left()->get_color()) {
                        flag = true;
                    }

                    if (!flag) {
                        auto *tmp = sibling->get_right();

                        tmp->set_color(false);
                        sibling->set_color(true);

                        tmp->set_anc(parent);
                        parent->set_left(tmp);
                        sibling->set_anc(tmp);

                        if (tmp->get_left()) {
                            tmp->get_left()->set_anc(sibling);
                        }
                        sibling->set_right(tmp->get_left());

                        tmp->set_left(sibling);

                        parent = x->get_anc();
                        sibling = are_equivalent(parent->get_left(), x) ? parent->get_right() : parent->get_left();
                    }
                }
            }

            if (!sibling->get_color()) {
                if (parent->get_left() == x) {
                    if (sibling->get_right()) {
                        flag = !sibling->get_right()->get_color();
                    } else {
                        flag = true;
                    }

                    if (!flag) {
                        sibling->set_color(parent->get_color());
                        parent->set_color(false);
                        sibling->get_right()->set_color(false);

                        rotate_mid(sibling->get_right(), sibling, parent);
                    }
                } else {
                    if (sibling->get_left()) {
                        flag = !sibling->get_left()->get_color();
                    } else {
                        flag = true;
                    }

                    if (!flag) {
                        sibling->set_color(parent->get_color());
                        parent->set_color(false);
                        sibling->get_left()->set_color(false);

                        rotate_mid(sibling->get_left(), sibling, parent);
                    }
                }
            }

            return;
        }
    }


    template <typename K, typename V>
    void RedBlackTree<K, V>::change_anc(Node<K, V> *top1, Node<K, V> *target_node) {
        if (top1->get_anc()) {
            if (are_equivalent(top1->get_anc()->get_left(), top1)) {
                top1->get_anc()->set_left(target_node);
            } else {
                top1->get_anc()->set_right(target_node);
            }
        } else {
            top = target_node;
        }
    }

    template<typename K, typename V>
    void RedBlackTree<K, V>::rotate_mid(Node<K, V> *bot, Node<K, V> *mid, Node<K, V> *top1) {
        if (!bot || !mid || !top1) {
            throw std::invalid_argument("incoming nodes in rotate_mid function");
        }

        change_anc(top1, mid);

        mid->set_anc(top1->get_anc());
        top1->set_anc(mid);

        if (top1->get_left()) {
            if (are_equivalent(top1->get_left(), mid)) {
                if (mid->get_right()) {
                    mid->get_right()->set_anc(top1);
                }

                top1->set_left(mid->get_right());
                mid->set_right(top1);
            } else {
                if (mid->get_left()) {
                    mid->get_left()->set_anc(top1);
                }

                top1->set_right(mid->get_left());
                mid->set_left(top1);
            }
        } else {
            if (mid->get_left()) {
                mid->get_left()->set_anc(top1);
            }

            top1->set_right(mid->get_left());
            mid->set_left(top1);
        }

        top1 = mid;
    }

    template<typename K, typename V>
    void RedBlackTree<K, V>::rotate_c(Node<K, V> *bot, Node<K, V> *mid, Node<K, V> *top1) {
        if (!bot || !mid || !top1) {
            throw std::invalid_argument("incoming nodes in rotate_c function");
        }

        change_anc(top1, bot);

        bot->set_anc(top1->get_anc());
        top1->set_anc(bot);
        mid->set_anc(bot);

        if (top1->get_left()) {
            if (are_equivalent(top1->get_left(), mid)) {
                if (bot->get_right()) {
                    bot->get_right()->set_anc(top1);
                } if (bot->get_left()) {
                    bot->get_left()->set_anc(mid);
                }

                top1->set_left(bot->get_right());
                mid->set_right(bot->get_left());

                bot->set_left(mid);
                bot->set_right(top1);
            } else {
                if (bot->get_right()) {
                    bot->get_right()->set_anc(mid);
                }
                if (bot->get_left()) {
                    bot->get_left()->set_anc(top1);
                }

                top1->set_right(bot->get_left());
                mid->set_left(bot->get_right());

                bot->set_left(top1);
                bot->set_right(mid);
            }
        } else {
            if (bot->get_right()) {
                bot->get_right()->set_anc(mid);
            } if (bot->get_left()) {
                bot->get_left()->set_anc(top1);
            }

            top1->set_right(bot->get_left());
            mid->set_left(bot->get_right());

            bot->set_left(top1);
            bot->set_right(mid);
        }

        top1 = bot;
    }

    template<typename K, typename V>
    bool RedBlackTree<K, V>::validate() {
        /**
        *  @brief Checks if Tree is a valid Red-Black Tree.
        *  @returns false if Tree is correct, true otherwise.
        */
        if (!top) {
            return false;
        }

        if (top->get_color()) {
            return true;
        }

        std::vector<int> in_keys;
        top->inorder_traversal(in_keys);
        std::vector<int> copy_keys{in_keys};
        std::sort(in_keys.begin(), in_keys.end());

        for (size_t i = 0, end_index = in_keys.size(); i < end_index; ++i) {
            if (in_keys[i] != copy_keys[i]) {
                return true;
            }
        }

        return top->check_rb(false) || top->check_rb();
    }
}
