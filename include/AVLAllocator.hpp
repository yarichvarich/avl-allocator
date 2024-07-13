#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <atomic>

#include "AVLTree.hpp"

namespace Yaro
{
namespace Utility
{

using Byte = unsigned char;

class SegmentManager
{
  public:
    struct SegmentBase
    {
        size_t head;
        size_t size;

        SegmentBase()
            : SegmentBase{0u, 0u}
        {
        }

        SegmentBase(size_t head, size_t size)
        {
            this->head = head;
            this->size = size;
        }

        bool operator==(const SegmentBase &other)
        {
            return head == other.head && size == other.size;
        }
    };

    struct HeadHeavy : public SegmentBase
    {
        size_t compareBy() const
        {
            return head;
        }
        void setCompared(size_t val)
        {
            head = val;
        }

        virtual ~HeadHeavy() = default;
    };

    struct SizeHeavy : public SegmentBase
    {
        size_t compareBy() const
        {
            return size;
        }
        void setCompared(size_t val)
        {
            size = val;
        }

        virtual ~SizeHeavy() = default;
    };

    template <typename ComparisonStrategy>
    struct Segment : public ComparisonStrategy
    {
        bool operator==(const Segment &other) const noexcept
        {
            return ComparisonStrategy::compareBy() == other.ComparisonStrategy::compareBy();
        }
        bool operator!=(const Segment &other) const noexcept
        {
            return ComparisonStrategy::compareBy() != other.ComparisonStrategy::compareBy();
        }

        bool operator<(const Segment &other) const noexcept
        {
            return ComparisonStrategy::compareBy() < other.ComparisonStrategy::compareBy();
        }
        bool operator>(const Segment &other) const noexcept
        {
            return ComparisonStrategy::compareBy() > other.ComparisonStrategy::compareBy();
        }

        bool operator<=(const Segment &other) const noexcept
        {
            return ComparisonStrategy::compareBy() <= other.ComparisonStrategy::compareBy();
        }
        bool operator>=(const Segment &other) const noexcept
        {
            return ComparisonStrategy::compareBy() >= other.ComparisonStrategy::compareBy();
        }

        Segment operator-(const Segment &other) const noexcept
        {
            Segment segment{other};
            auto cmp = std::max(ComparisonStrategy::compareBy(), other.ComparisonStrategy::compareBy()) -
                       std::min(ComparisonStrategy::compareBy(), other.ComparisonStrategy::compareBy());
            segment.ComparisonStrategy::setCompared(cmp);

            return segment;
        }

        static Segment abs(Segment val)
        {
            return val;
        }

        static Segment max()
        {
            Segment dummy;
            dummy.ComparisonStrategy::setCompared(std::numeric_limits<size_t>::max());
            return dummy;
        }

        Segment()
            : Segment(0u, 0u){};

        Segment(const SegmentBase &segment)
            : Segment(segment.head, segment.size)
        {
        }

        Segment(size_t head, size_t size)
        {
            this->head = head;
            this->size = size;
        }

        Segment(const Segment &other)
        {
            this->head = other.head;
            this->size = other.size;
        }

        Segment &operator=(const Segment &other)
        {
            this->head = other.head;
            this->size = other.size;
            return *this;
        }

        Segment(Segment &&rr)
        {
            this->head = rr.head;
            this->size = rr.size;
        }

        Segment &operator=(Segment &&rr)
        {
            this->head = rr.head;
            this->size = rr.size;
            return *this;
        }
    };
    void addSegment(const SegmentBase &segment)
    {
        m_sizeHeavySegments.insert(segment);
        m_headHeavySegments.insert(segment);
    }

    bool deleteSegment(const SegmentBase &segment)
    {
        return m_headHeavySegments.pop(segment) && m_sizeHeavySegments.pop(segment);
    }

    bool bestFitSegment(const SegmentBase &segment, SegmentBase &outSegment)
    {
        return m_sizeHeavySegments.findClosestGreaterEqual(segment, static_cast<Segment<SizeHeavy> &>(outSegment));
    }

    bool getLeftAdjacentSegment(const SegmentBase &segment, SegmentBase &outSegment)
    {
        return m_headHeavySegments.findClosestLesser(segment, static_cast<Segment<HeadHeavy> &>(outSegment));
    }

    bool getRightAdjacentSegment(const SegmentBase &segment, SegmentBase &outSegment)
    {
        return m_headHeavySegments.findClosestGreater(segment, static_cast<Segment<HeadHeavy> &>(outSegment));
    }

    size_t maxSizeSegment()
    {
        SegmentBase segment;

        if (m_sizeHeavySegments.findMax(static_cast<Segment<SizeHeavy> &>(segment)))
        {
            return segment.size;
        }

        return 0u;
    }

  private:
    AVLTree<Segment<HeadHeavy>> m_headHeavySegments;
    AVLTree<Segment<SizeHeavy>> m_sizeHeavySegments;
};

template <size_t BlockSize>
struct MemoryBlock
{
    using Bytes = std::vector<Byte>;

    SegmentManager manager;
    Bytes pool;

    MemoryBlock()
        : pool(BlockSize, ' ')
    {
        manager.addSegment({0u, BlockSize});
    }

    MemoryBlock(const MemoryBlock &other) = delete;
    MemoryBlock &operator=(const MemoryBlock &other) = delete;
    MemoryBlock(MemoryBlock &&rr) = delete;
    MemoryBlock &operator=(MemoryBlock &&rr) = delete;
};

template <typename T, size_t NumBlocks, size_t BlockSize>
class AVLAllocator
{
  public:
    using value_type = T;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using SegmentAndBlockId = std::pair<SegmentManager::SegmentBase, size_t>;

    static inline std::atomic_uint8_t start = 0u;

    pointer allocate(size_t n)
    {
        std::lock_guard<std::mutex> lk(s_mutex);
        size_t byteSize = sizeof(T) * n;
        for (size_t i = 0u; i < NumBlocks; ++i)
        {
            size_t blockId = i % NumBlocks;
            auto &block = s_blocks[blockId];

            SegmentManager::SegmentBase dummy{0, byteSize};
            SegmentManager::SegmentBase bestSegment;

            if (!block.manager.bestFitSegment(dummy, bestSegment))
            {
                continue;
            }

            dummy.head = bestSegment.head + byteSize;
            dummy.size = bestSegment.size - byteSize;

            block.manager.deleteSegment(bestSegment);

            if (byteSize != bestSegment.size)
            {
                block.manager.addSegment(dummy);
            }

            T *ptr = reinterpret_cast<T *>(&block.pool[bestSegment.head]);

            if(s_pointerSegmentMapping.find(ptr) != s_pointerSegmentMapping.end())
            {
                std::cout << "WTF!\n";
            }

            s_pointerSegmentMapping.insert({ptr, {{bestSegment.head, byteSize}, blockId}});

            return ptr;
        }

        throw std::bad_alloc();
    }

    pointer deallocate(T *ptr, size_t count = 0u)
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        count *= sizeof(T);
        size_t size = 0u;
    
        auto it = s_pointerSegmentMapping.find(ptr);
        if (it == s_pointerSegmentMapping.end())
        {
            std::cout << "Leak on: " << (void*)ptr << std::endl;
            throw std::bad_alloc();
            return nullptr;
        }

        size_t blockId = it->second.second;
        auto &block = s_blocks[blockId];
        
        SegmentManager::SegmentBase segment = it->second.first;

        s_pointerSegmentMapping.erase(it);

        if (count > segment.size)
        {
            throw std::bad_alloc();
        }
        else if (count != 0u && count != segment.size)
        {
            SegmentManager::SegmentBase remainder = {segment.head + count, segment.size - count};
            s_pointerSegmentMapping.insert({ptr + count, {remainder, blockId}});

            SegmentManager::SegmentBase freedSegment = {segment.head, count};
            block.manager.addSegment(freedSegment);
        }
        else
        {
            block.manager.addSegment(segment);
        }

        static auto mergeSegments = [&block](SegmentManager::SegmentBase &mergeInto,
                                            SegmentManager::SegmentBase &mergeWhat) -> void {
            auto &leftSegment = (mergeInto.head < mergeWhat.head) ? mergeInto : mergeWhat;
            auto &rightSegment = (mergeInto.head > mergeWhat.head) ? mergeInto : mergeWhat;

            block.manager.deleteSegment(leftSegment);
            block.manager.deleteSegment(rightSegment);

            leftSegment.size += rightSegment.size;

            block.manager.addSegment(leftSegment);
        };

        SegmentManager::SegmentBase leftSegment, rightSegment;

        if (block.manager.getLeftAdjacentSegment(segment, leftSegment))
        {
            mergeSegments(segment, leftSegment);
        }

        if (block.manager.getRightAdjacentSegment(segment, rightSegment) && (!count || count == segment.size))
        {
            mergeSegments(segment, rightSegment);
        }
        size = segment.size;
    
        return (count == size) ? nullptr : ptr + count;
    }

    AVLAllocator() = default;

    template <typename U>
    AVLAllocator(const AVLAllocator<U, NumBlocks, BlockSize> &other)
    {
    }

    template <typename U>
    AVLAllocator &operator=(const AVLAllocator<U, NumBlocks, BlockSize> &other)
    {
        return *this;
    }

    template <typename U>
    AVLAllocator(AVLAllocator<U, NumBlocks, BlockSize> &&rr)
    {
    }

    template <typename U>
    AVLAllocator &operator=(AVLAllocator<U, NumBlocks, BlockSize> &&rr)
    {
        return *this;
    }

    template <typename U>
    struct rebind
    {
        using other = AVLAllocator<U, NumBlocks, BlockSize>;
    };

    template<typename... Args>
    pointer create(Args&&... args)
    {
        return new (allocate(1u)) value_type(std::forward<Args>(args)...);
    }

    size_type max_size()
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        size_type maxSize = 0u;

        for (const auto &block : s_blocks)
        {
            maxSize = std::max(maxSize, block.manager.maxSizeSegment());
        }
        return maxSize / sizeof(value_type);
    }

  private:
    static inline std::array<MemoryBlock<BlockSize>, NumBlocks> s_blocks = std::array<MemoryBlock<BlockSize>, NumBlocks>{};
    static inline std::unordered_map<T *, SegmentAndBlockId> s_pointerSegmentMapping = std::unordered_map<T *, SegmentAndBlockId>{};
    static inline std::mutex s_mutex;
};

template<typename T, typename Alloc, typename... Args>
std::shared_ptr<T> allocMakeShared(Alloc alloc, Args&&... args)
{
    return std::shared_ptr<T>(alloc.create(std::forward<Args>(args)...), [&alloc](typename Alloc::pointer p)->void {alloc.deallocate(p);});
}

} // namespace Utility
} // namespace Yaro