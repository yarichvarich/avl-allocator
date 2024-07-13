#pragma once

#include <array>
#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "Shared.hpp"
#include "debug.hpp"

namespace Yaro
{
namespace Utility
{

template <typename KeyType>
struct KeyTypeTraits
{
    static KeyType max()
    {
        if constexpr (std::is_arithmetic<KeyType>::value)
        {
            return std::numeric_limits<KeyType>::max();
        }
        else
        {
            return KeyType::max();
        }
    }

    static KeyType min()
    {
        if constexpr (std::is_arithmetic<KeyType>::value)
        {
            return std::numeric_limits<KeyType>::min();
        }
        else
        {
            return KeyType::min();
        }
    }

    static KeyType abs(const KeyType &val)
    {
        if constexpr (std::is_arithmetic<KeyType>::value)
        {
            return std::abs(val);
        }
        else
        {
            return KeyType::abs(val);
        }
    }

    static bool equal(const KeyType &a, const KeyType &b)
    {
        return a == b;
    }

    static bool notEqual(const KeyType &a, const KeyType &b)
    {
        return a != b;
    }

    static bool less(const KeyType &a, const KeyType &b)
    {
        return a < b;
    }

    static bool lessEqual(const KeyType &a, const KeyType &b)
    {
        return a <= b;
    }

    static bool greater(const KeyType &a, const KeyType &b)
    {
        return a > b;
    }

    static bool greaterEqual(const KeyType &a, const KeyType &b)
    {
        return a >= b;
    }

    static KeyType add(const KeyType &a, const KeyType &b)
    {
        return a + b;
    }

    static KeyType subtract(const KeyType &a, const KeyType &b)
    {
        return a - b;
    }
};
template <typename KeyType>
struct AVLNode : public Shared<AVLNode<KeyType>>
{
    static AVLNode *create(const KeyType &k, uint16_t h, uint32_t c, AVLNode *l, AVLNode *r);
    static AVLNode *create(const AVLNode &node);

    KeyType key;
    size_t count;
    uint8_t height;

    typename Shared<AVLNode>::Ptr left;
    typename Shared<AVLNode>::Ptr right;
};

template <typename KeyType>
class AVLTree
{
  public:
    using Node = AVLNode<KeyType>;

  private:
    const typename Node::Ptr _find(typename Node::Ptr pNode, const KeyType &key) const;

    bool _pop(typename Node::Ptr &pNode, const KeyType &key);

    const KeyType *_insert(typename Node::Ptr &pNode, const KeyType &key);

    const KeyType *_findMax(typename Node::Ptr &pNode);

    void _leftRotation(typename Node::Ptr &pNode);

    void _rightRotation(typename Node::Ptr &pNode);

    void _balance(typename Node::Ptr &pNode, const KeyType &key);

    const size_t _count(const typename Node::Ptr &pNode, const KeyType &key) const;

    const int32_t _difference(const typename Node::Ptr &pNode) const;

    const int32_t _height(const typename Node::Ptr &pNode) const;

    typename Node::Ptr &_minKeyNode(typename Node::Ptr &pNode);

    typename Node::Ptr &_maxKeyNode(typename Node::Ptr &pNode);

    bool _findClosest(typename Node::Ptr pNode, const KeyType &key, KeyType &outKey,
                      KeyType (*calculateDelta)(const KeyType &l, const KeyType &r));

  public:
    AVLTree() = default;

    AVLTree(const KeyType &key);

    AVLTree(const AVLTree &other);

    AVLTree &operator=(const AVLTree &other);

    AVLTree(AVLTree &&rr);

    AVLTree &operator=(AVLTree &&rr);

    bool operator==(const AVLTree &other) const;

    bool operator!=(const AVLTree &other) const;

    inline void clear();

    inline const KeyType *insert(const KeyType &key);

    inline bool find(const KeyType &key) const;

    inline bool pop(const KeyType &key);

    inline const size_t size() const;

    inline const size_t count(const KeyType &key) const;

    inline const uint8_t height() const;

    inline const int8_t balance() const;

    bool findMax(KeyType &outKey);

    bool findMin(KeyType &outKey);

    bool findClosest(const KeyType &key, KeyType &outKey);

    bool findClosestGreater(const KeyType &key, KeyType &outKey);

    bool findClosestGreaterEqual(const KeyType &key, KeyType &outKey);

    bool findClosestLesser(const KeyType &key, KeyType &outKey);

    void print(uint8_t topOffset = 4u) const;

  private:
    typename Node::Ptr m_root = nullptr;

    size_t m_size = 0u;
};

#include "AVLTree.inl"

} // namespace Utility
} // namespace Yaro