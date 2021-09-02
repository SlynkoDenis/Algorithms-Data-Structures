#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "red_black_tree.h"


namespace {
    std::vector<int> generate_keys(unsigned amount) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rand_distributor;

        std::vector<int> result(amount, 0);

        int tmp_num = 1;
        result[0] = 1 + (rand_distributor(gen) % (amount + 50u));
        for (unsigned i = 1; i < amount; ++i) {
            tmp_num = 1 + (rand_distributor(gen) % (amount + 50u));
            while (true) {
                bool flag = true;
                for (unsigned j = 0; j < i; ++j)
                    if (result[j] == tmp_num) {
                        flag = false;
                        tmp_num = 1 + (rand_distributor(gen) % (amount + 50u));
                        break;
                    }

                if (flag) {
                    break;
                }
            }

            result[i] = tmp_num;
        }
        return result;
    }

    [[maybe_unused]] std::string generate_random_string() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rand_distributor;

        unsigned len = (rand_distributor(gen) % 100u) + 2;

        std::string result(len, '0');
        for (size_t i = 0; i < len; ++i) {
            result[i] = static_cast<char>(rand_distributor(gen) % 26 + 0x61);
        }

        return result;
    }

    void generate_tests(const char *name) {
        if (!name) {
            throw std::invalid_argument("the name of incoming file in generate_tests function");
        }

        std::ofstream file(name);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create a file");
        }

        std::cout << "Enter amount of elements" << std::endl;
        unsigned amount = 0;
        std::cin >> amount;

        std::vector<int> keys = generate_keys(amount);

        for (size_t i = 0; i < amount; ++i) {
            file << keys[i] << std::endl;
        }

        std::next_permutation(keys.begin(), keys.end());

        file << "0\n";
        for (size_t i = 0; i < amount; ++i) {
            file << keys[i] << std::endl;
        }
    }
}

int main(int argc, char **argv) {
    try {
        if (argc == 1) {
            RedBlackTree::RedBlackTree<int, char> tree;

            int key = 0;
            while (std::cin >> key) {
                if (!key) {
                    break;
                }

                tree.insert_node(key, 0);

                if (false && tree.validate()) {
                    std::cerr << "The tree has been broken at insertion of " << key << " key" << std::endl;
                    exit(2);
                }
            }

            std::cout << "All given elements are inserted" << std::endl;

            while (std::cin >> key) {
                if (!key) {
                    break;
                }

                tree.delete_node(key);

                if (false && tree.validate()) {
                    std::cerr << "The tree has been broken at insertion of " << key << " key" << std::endl;
                    exit(2);
                }
            }

            std::cout << "All given elements are deleted" << std::endl << std::endl;
            std::cout << "The result of checking functions is " << tree.validate() << std::endl << std::endl;
            std::cout << "Remained nodes in tree are:" << std::endl;
            tree.inorder_print_tree();
        } else {
            generate_tests(argv[1]);
        }
    } catch (std::bad_alloc &xa) {
        std::cerr << "Allocation failed: " << xa.what() << std::endl;
    } catch (std::invalid_argument &xb) {
        std::cerr << "Invalid argument: " << xb.what() << std::endl;
    } catch (std::length_error &xc) {
        std::cerr << "Buffer overflow: " << xc.what() << std::endl;
    } catch (std::logic_error &xd) {
        std::cerr << "Logic error: " << xd.what() << std::endl;
    } catch (std::runtime_error &xe) {
        std::cerr << "Runtime error: " << xe.what() << std::endl;
    }
    return 0;
}
