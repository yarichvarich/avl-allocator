#pragma once

template <typename KeyType>
AVLNode<KeyType> *AVLNode<KeyType>::create(const KeyType &k, uint16_t h, uint32_t c, AVLNode *l, AVLNode *r)
{
    AVLNode *p = new AVLNode;

    p->key = k;
    p->height = h;
    p->count = c;
    p->left.reset(l);
    p->right.reset(r);

    return p;
}

template <typename KeyType>
AVLNode<KeyType> *AVLNode<KeyType>::create(const AVLNode &node)
{
    AVLNode *p = new AVLNode;

    p->key = node.key;
    p->height = node.height;
    p->count = node.count;

    if (node.left != nullptr)
    {
        p->left.reset(create(*node.left));
    }
    else
    {
        p->left = nullptr;
    }

    if (node.right != nullptr)
    {
        p->right.reset(create(*node.right));
    }
    else
    {
        p->right = nullptr;
    }

    return p;
}

template <typename KeyType>
const KeyType *AVLTree<KeyType>::_insert(typename Node::Ptr &pNode, const KeyType &key)
{
    const KeyType *res = nullptr;

    if (pNode == nullptr)
    {
        pNode.reset(Node::create(key, 1u, 1u, nullptr, nullptr));
        return &(pNode->key);
    }
    else if (key < pNode->key)
    {
        res = _insert(pNode->left, key);
    }
    else if (key > pNode->key)
    {
        res = _insert(pNode->right, key);
    }
    else
    {
        pNode->count++;
        res = &(pNode->key);
    }

    _balance(pNode, key);
    return res;
}

template <typename KeyType>
typename AVLTree<KeyType>::Node::Ptr &AVLTree<KeyType>::_minKeyNode(typename Node::Ptr &pNode)
{
    if (pNode->left != nullptr)
    {
        return _minKeyNode(pNode->left);
    }

    return pNode;
}

template <typename KeyType>
typename AVLTree<KeyType>::Node::Ptr &AVLTree<KeyType>::_maxKeyNode(typename Node::Ptr &pNode)
{
    if (pNode->right != nullptr)
    {
        return _maxKeyNode(pNode->right);
    }

    return pNode;
}

template <typename KeyType>
bool AVLTree<KeyType>::_pop(typename Node::Ptr &pNode, const KeyType &key)
{
    bool countEq1 = false;

    if (pNode == nullptr)
    {
        return false;
    }
    else if (pNode->key > key)
    {
        return _pop(pNode->left, key);
    }
    else if (pNode->key < key)
    {
        return _pop(pNode->right, key);
    }
    else
    {
        countEq1 = pNode->count == 1u;

        if (!countEq1)
        {
            --pNode->count;
            return true;
        }

        if (pNode->left != nullptr && pNode->right == nullptr)
        {
            pNode = pNode->left;
        }
        else if (pNode->left == nullptr && pNode->right != nullptr)
        {
            pNode = pNode->right;
        }
        else if (pNode->left != nullptr && pNode->right != nullptr)
        {
            typename Node::Ptr &minKeyNode = _minKeyNode(pNode->right);
            pNode->key = minKeyNode->key;
            _pop(pNode->right, minKeyNode->key);
        }
        else
        {
            pNode = nullptr;
        }
    }

    if (countEq1 && pNode != nullptr)
    {
        _balance(pNode, key);
    }

    return true;
}

template <typename KeyType>
const typename AVLTree<KeyType>::Node::Ptr AVLTree<KeyType>::_find(typename Node::Ptr pNode, const KeyType &key) const
{
    while (pNode != nullptr)
    {
        if (pNode->key == key)
        {
            return pNode;
        }
        else if (pNode->key < key)
        {
            pNode = pNode->right;
        }
        else
        {
            pNode = pNode->left;
        }
    }

    return nullptr;
}

template <typename KeyType>
bool AVLTree<KeyType>::_findClosest(typename Node::Ptr pNode, const KeyType &key, KeyType &outKey,
                                    KeyType (*calculateDelta)(const KeyType &l, const KeyType &r))
{
    if (pNode == nullptr)
    {
        return false;
    }

    KeyType minDelta = KeyTypeTraits<KeyType>::max();

    while (pNode != nullptr)
    {
        KeyType delta = calculateDelta(pNode->key, key);

        if (KeyTypeTraits<KeyType>::greater(minDelta, delta))
        {
            minDelta = delta;
            outKey = pNode->key;
        }

        if (KeyTypeTraits<KeyType>::less(pNode->key, key))
        {
            pNode = pNode->right;
        }
        else if (KeyTypeTraits<KeyType>::greater(pNode->key, key))
        {
            pNode = pNode->left;
        }
        else
        {
            break;
        }
    }

    if (pNode != nullptr)
    {
        bool iterate = true;
        bool left = false;

        while (iterate)
        {
            iterate = false;
            left = false;

            KeyType delta;

            if (pNode->left != nullptr)
            {
                delta = calculateDelta(pNode->left->key, key);

                if (KeyTypeTraits<KeyType>::greater(minDelta, delta))
                {
                    minDelta = delta;
                    outKey = pNode->left->key;
                    left = true;
                    iterate = true;
                }
            }

            if (pNode->right != nullptr)
            {
                delta = calculateDelta(pNode->right->key, key);

                if (KeyTypeTraits<KeyType>::greater(minDelta, delta))
                {
                    minDelta = delta;
                    outKey = pNode->left->key;
                    iterate = true;
                }
            }

            if (left)
            {
                pNode = pNode->left;
            }
            else
            {
                pNode = pNode->right;
            }
        }
    }

    return KeyTypeTraits<KeyType>::notEqual(minDelta, KeyTypeTraits<KeyType>::max());
}

template <typename KeyType>
void AVLTree<KeyType>::_leftRotation(typename Node::Ptr &pNode)
{
    typename Node::Ptr &x = pNode;
    typename Node::Ptr y = x->right;
    typename Node::Ptr t = y->left;

    y->left = x;
    x->right = t;

    x->height = std::max(_height(x->left), _height(x->right)) + 1u;
    y->height = std::max(_height(y->left), _height(y->right)) + 1u;

    x = y;
}
template <typename KeyType>
void AVLTree<KeyType>::_rightRotation(typename Node::Ptr &pNode)
{
    typename Node::Ptr &x = pNode;
    typename Node::Ptr y = x->left;
    typename Node::Ptr t = y->right;

    y->right = x;
    x->left = t;

    x->height = std::max(_height(x->left), _height(x->right)) + 1u;
    y->height = std::max(_height(y->left), _height(y->right)) + 1u;

    x = y;
}

template <typename KeyType>
void AVLTree<KeyType>::_balance(typename Node::Ptr &pNode, const KeyType &key)
{
    pNode->height = std::max(_height(pNode->left), _height(pNode->right)) + 1u;

    const int32_t diff = _difference(pNode);

    if (diff > 1)
    {
        if (KeyTypeTraits<KeyType>::less(key, pNode->left->key))
        {
            _rightRotation(pNode);
        }
        else if (KeyTypeTraits<KeyType>::greater(key, pNode->left->key))
        {
            _leftRotation(pNode->left);
            _rightRotation(pNode);
        }
    }
    if (diff < -1)
    {
        if (KeyTypeTraits<KeyType>::greater(key, pNode->right->key))
        {
            _leftRotation(pNode);
        }
        else if (KeyTypeTraits<KeyType>::less(key, pNode->right->key))
        {
            _rightRotation(pNode->right);
            _leftRotation(pNode);
        }
    }
}

template <typename KeyType>
const size_t AVLTree<KeyType>::_count(const typename Node::Ptr &pNode, const KeyType &key) const
{
    const auto node = _find(pNode, key);
    return (node == nullptr) ? 0u : node->count;
}

template <typename KeyType>
const int32_t AVLTree<KeyType>::_height(const typename Node::Ptr &pNode) const
{
    if (pNode != nullptr)
    {
        return pNode->height;
    }

    return 0;
}

template <typename KeyType>
const int32_t AVLTree<KeyType>::_difference(const typename Node::Ptr &pNode) const
{
    const int32_t leftHeight = _height(pNode->left);
    const int32_t rightHeight = _height(pNode->right);

    return leftHeight - rightHeight;
}

template <typename KeyType>
void AVLTree<KeyType>::print(uint8_t topOffset) const
{
    const uint32_t height = _height(m_root);

    if (height == 0u)
    {
        return;
    }
    else if (height == 1u)
    {
        std::cout << m_root->key << std::endl;
    }

    std::function<uint32_t(const typename Node::Ptr &)> findLongestNumber =
        [&findLongestNumber](const typename Node::Ptr &pNode) -> uint32_t {
        if (pNode == nullptr)
        {
            return 0u;
        }

        const uint32_t maxChild = std::max(findLongestNumber(pNode->left), findLongestNumber(pNode->right));

        return std::max(maxChild, static_cast<uint32_t>(std::to_string(pNode->key).length()));
    };

    const uint32_t maxLowerNodes = (1u << (height - 1u));
    const uint32_t segments = maxLowerNodes - 1u;
    const uint32_t offset = findLongestNumber(m_root);
    const uint32_t width = maxLowerNodes * (offset + 4u) + 1;
    const uint32_t buffWidth = width + offset * 2u;
    const uint32_t buffHeight = m_root->height * topOffset + 1u;

    std::vector<std::string> buff(buffHeight, std::string(buffWidth, ' '));

    const std::function<void(const typename Node::Ptr &, uint32_t, uint32_t, uint32_t)> printRecursive =
        [&buff, &width, &offset, &printRecursive, &topOffset](const typename Node::Ptr &pNode, uint32_t leftCoord,
                                                              uint32_t topCoord, uint32_t dist) -> void {
        if (pNode == nullptr)
        {
            return;
        }

        const uint32_t x = leftCoord;
        const uint32_t y = topCoord;

        const std::string numStr = std::to_string(pNode->key);

        const uint32_t halfLength = numStr.length() / 2u;

        for (uint32_t i = 0; i < numStr.length(); ++i)
        {
            buff[y][offset + x + i - halfLength] = numStr[i];
        }

        printRecursive(pNode->left, leftCoord - (dist / 2), topCoord + topOffset, dist / 2);
        printRecursive(pNode->right, leftCoord + (dist / 2), topCoord + topOffset, dist / 2);
    };

    printRecursive(m_root, width / 2u, 0u, width / 2u);

    for (uint32_t i = 0; i < buff.size(); ++i)
    {
        std::cout << buff[i] << std::endl;
    }
}

template <typename KeyType>
AVLTree<KeyType>::AVLTree(const KeyType &key)
{
    ++m_size;
    m_root.reset(Node::create(key, 1u, 1u, nullptr, nullptr));
}

template <typename KeyType>
AVLTree<KeyType>::AVLTree(const AVLTree &other)
    : m_root{((other.m_root != nullptr) ? Node::create(*other.m_root) : nullptr)}, m_size{other.m_size}
{
}

template <typename KeyType>
AVLTree<KeyType> &AVLTree<KeyType>::operator=(const AVLTree &other)
{
    m_root.reset(((other.m_root != nullptr) ? Node::create(*other.m_root) : nullptr));
    m_size = other.m_size;
    return *this;
}

template <typename KeyType>
AVLTree<KeyType>::AVLTree(AVLTree &&rr)
    : m_root{std::move(rr.m_root)}, m_size{rr.m_size}
{
}

template <typename KeyType>
AVLTree<KeyType> &AVLTree<KeyType>::operator=(AVLTree &&rr)
{
    m_root = std::move(rr.m_root);
    m_size = rr.m_size;
    return *this;
}

template <typename KeyType>
bool AVLTree<KeyType>::operator==(const AVLTree &other) const
{
    if (m_size != other.m_size)
    {
        return false;
    }

    bool inequal = false;

    const std::function<bool(const typename Node::Ptr &, const typename Node::Ptr &)> compareRecursive =
        [this, &compareRecursive, &inequal](const typename Node::Ptr &node1, const typename Node::Ptr &node2) -> bool {
        if (inequal)
        {
            return false;
        }

        if (node1 == nullptr && node2 == nullptr)
        {
            return true;
        }
        else if (node1 != nullptr && node2 != nullptr)
        {
            if (KeyTypeTraits<KeyType>::equal(node1->key, node2->key))
            {
                return compareRecursive(node1->left, node2->left) && compareRecursive(node1->right, node2->right);
            }
        }

        inequal = true;
        return false;
    };

    return compareRecursive(m_root, other.m_root);
}

template <typename KeyType>
bool AVLTree<KeyType>::operator!=(const AVLTree &other) const
{
    return !((*this) == other);
}

template <typename KeyType>
inline void AVLTree<KeyType>::clear()
{
    m_root.reset();
    m_size = 0u;
}

template <typename KeyType>
inline const KeyType *AVLTree<KeyType>::insert(const KeyType &key)
{
    ++m_size;
    return _insert(m_root, key);
}

template <typename KeyType>
inline bool AVLTree<KeyType>::find(const KeyType &key) const
{
    return (_find(m_root, key) == nullptr) ? false : true;
}

template <typename KeyType>
inline bool AVLTree<KeyType>::pop(const KeyType &key)
{
    bool popped = _pop(m_root, key);
    if (popped)
    {
        --m_size;
    }
    return popped;
}

template <typename KeyType>
inline const size_t AVLTree<KeyType>::size() const
{
    return m_size;
}

template <typename KeyType>
inline const size_t AVLTree<KeyType>::count(const KeyType &key) const
{
    return _count(m_root, key);
}

template <typename KeyType>
inline const uint8_t AVLTree<KeyType>::height() const
{
    return _height(m_root);
}

template <typename KeyType>
inline const int8_t AVLTree<KeyType>::balance() const
{
    return _height(m_root->left) - _height(m_root->right);
}

template <typename KeyType>
bool AVLTree<KeyType>::findMin(KeyType &outKey)
{
    if (m_root != nullptr)
    {
        auto node = _minKeyNode(m_root);
        outKey = node->key;
        return true;
    }

    return false;
}

template <typename KeyType>
bool AVLTree<KeyType>::findMax(KeyType &outKey)
{
    if (m_root != nullptr)
    {
        auto node = _minKeyNode(m_root);
        outKey = node->key;
        return true;
    }

    return false;
}

template <typename KeyType>
bool AVLTree<KeyType>::findClosest(const KeyType &key, KeyType &outKey)
{
    return _findClosest(m_root, key, outKey, [](const KeyType &l, const KeyType &r) -> KeyType { return KeyTypeTraits<KeyType>::abs(KeyTypeTraits<KeyType>::subtract(l, r)); });
}

template <typename KeyType>
bool AVLTree<KeyType>::findClosestGreater(const KeyType &key, KeyType &outKey)
{
    return _findClosest(m_root, key, outKey,
                        [](const KeyType &l, const KeyType &r) -> KeyType { return (KeyTypeTraits<KeyType>::greater(l, r) ? KeyTypeTraits<KeyType>::subtract(l, r) : KeyTypeTraits<KeyType>::max()); });
}

template <typename KeyType>
bool AVLTree<KeyType>::findClosestGreaterEqual(const KeyType &key, KeyType &outKey)
{
    return _findClosest(m_root, key, outKey,
                        [](const KeyType &l, const KeyType &r) -> KeyType { return (KeyTypeTraits<KeyType>::greaterEqual(l, r) ? KeyTypeTraits<KeyType>::subtract(l, r) : KeyTypeTraits<KeyType>::max()); });
}

template <typename KeyType>
bool AVLTree<KeyType>::findClosestLesser(const KeyType &key, KeyType &outKey)
{
    return _findClosest(m_root, key, outKey,
                        [](const KeyType &l, const KeyType &r) -> KeyType { return (KeyTypeTraits<KeyType>::less(l, r) ? KeyTypeTraits<KeyType>::subtract(r, l) : KeyTypeTraits<KeyType>::max()); });
}