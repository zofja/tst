#ifndef TST_H
#define TST_H

#include <memory>
#include <stdexcept>

namespace Detail {

    template<typename Iter, typename Acc, typename Functor>
    Acc fold(Iter first, Iter last, Acc acc, Functor functor) {
        return first != last ? fold(first.next(), last, functor(acc, *first), functor) : acc;
    }

}

template<typename C = char>
class TST {

private:

    // struct representing a single node in the TST
    struct Node {

        // constructors
        Node() = default;

        Node(const std::shared_ptr<const Node> &left, const std::shared_ptr<const Node> &center,
             const std::shared_ptr<const Node> &right, const C &value, const bool word)
                : _left(left), _center(center), _right(right), _value(value), _word(word) {
        }

        const std::shared_ptr<const Node> _left;
        const std::shared_ptr<const Node> _center;
        const std::shared_ptr<const Node> _right;
        const C _value;
        const bool _word;

    };

    const std::shared_ptr<const Node> _root;

    explicit TST(const std::shared_ptr<const Node> &node) : _root(node) {}

    TST(const TST &left, const TST &center, const TST &right, const C &value, const bool word) :
            _root(std::make_shared<const Node>(Node(left._root, center._root, right._root, value, word))) {
    }

    // returns the length of the longest common prefix of the word str in the TST tree
    size_t prefix_count(const C *str) const {
        return (empty() and str[0] != '\0') or (not empty() and str[0] == '\0') ? 0 :
               str[0] == '\0' ? 0 :
               str[0] < value() ? left().prefix_count(str) :
               str[0] == value() ? center().prefix_count(str + 1) + 1 :
               right().prefix_count(str);
    }

    void logic_error() const {
        if (empty()) {
            throw std::logic_error("Logic error.\n");
        }
    }

public:

    // constructors
    TST() = default;

    explicit TST(const std::basic_string<C> &str) : TST(str.c_str()) {}

    explicit TST(const C *str) : TST(str == nullptr ? TST() : str[0] == '\0' ? TST() :
                                 TST(TST(), TST(str + 1), TST(), str[0], str[1] == '\0')) {
    }

    // overloaded operator+ for adding basic_string to the TST calls operator+ for C* strings
    TST operator+(const std::basic_string<C> &str) const {
        return (*this + str.c_str());
    }

    // overloaded operator+ for adding C* strings to the TST
    TST operator+(const C *str) const {
        return empty() ? TST(str) :
               str[0] != '\0' ?
               str[0] < value() ? TST(left() + str, center(), right(), value(), word()) :
               str[0] == value() ? TST(left(), center() + (str + 1), right(), value(), str[1] == '\0') :
               TST(left(), center(), right() + str, value(), word()) : TST(left(), center(), right(), value(), word());
    }

    // returns value stored in the node
    C value() const {
        logic_error();
        return _root->_value;
    }

    // returns true if there exists a word ending with a value stored in the node
    bool word() const {
        logic_error();
        return _root->_word;
    }

    // returns the left subtree
    TST left() const {
        logic_error();
        return TST(_root->_left);
    }

    // returns the center subtree
    TST center() const {
        logic_error();
        return TST(_root->_center);
    }

    // returns the right subtree
    TST right() const {
        logic_error();
        return TST(_root->_right);
    }

    // returns true if the node is null
    bool empty() const {
        return not _root;
    }

    // returns true if the word str exists in the TST
    bool exist(const std::basic_string<C> &str) const {
        return exist(str.c_str());
    }

    // returns true if the word str exists in the TST
    bool exist(const C *str) const {
        return empty() ? false :
               str[0] == value() and str[1] == '\0' ? word() :
               str[0] < value() ? left().exist(str) :
               str[0] == value() ? center().exist(str + 1) :
               str[0] > value() ? right().exist(str) : false;
    }

    // returns the longest common prefix existing in the TST
    // example: if TST = {"category", "functor", "theory"}
    // then tst.prefix("catamorphism") = {"cat"}
    std::basic_string<C> prefix(const std::basic_string<C> &str) const {
        return str.substr(0, prefix_count(str.c_str()));
    }

    // returns size of the TST
    template<typename Acc, typename Functor>
    Acc fold(Acc acc, Functor functor) const {

        return empty() ? acc : functor(left().fold(acc, functor))
                               + functor(center().fold(acc, functor))
                               + functor(right().fold(acc, functor))
                               + 1;
    }

    // returns size of the TST i.e. number of nodes
    size_t size() const {
        return this->fold(0UL, [&](size_t acc) -> size_t { return acc; });
    }
};

#endif