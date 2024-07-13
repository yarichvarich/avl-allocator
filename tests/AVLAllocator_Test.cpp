#include "../include/AVLAllocator.hpp"
#include <gtest/gtest.h>
#include <thread>

template <typename T>
using Allocator1 = Yaro::Utility::AVLAllocator<T, 5, 1000000>;

TEST(Allocator, alloc1)
{
    Allocator1<int> alloc;
    int *arr = alloc.allocate(80);
    alloc.deallocate(arr);
}

TEST(Allocator, alloc2)
{
    Allocator1<int> alloc;
    int *a1 = alloc.allocate(100);
    int *a2 = alloc.allocate(200);
    int *a3 = alloc.allocate(200);

    alloc.deallocate(a2);

    int *a4 = alloc.allocate(500);

    alloc.deallocate(a3);

    int *a5 = alloc.allocate(400);

    alloc.deallocate(a1);
    alloc.deallocate(a4);
    alloc.deallocate(a5);
}

TEST(Allocator, alloc3)
{
    Allocator1<char> alloc;
    char *arr = alloc.allocate(1000000);
    alloc.deallocate(arr);
}

TEST(Allocator, alloc4)
{
    Allocator1<char> alloc;

    {
        std::vector<char, Allocator1<char>> vec;
        vec.reserve(500000);
    }
    std::deque<char, Allocator1<char>> deq;
    deq.resize(500000);
    deq.resize(900000);
}

TEST(Allocator, alloc5_)
{
    Allocator1<char> alloc;
    
    constexpr uint32_t allocationSize = 5;

    char* ptrs[allocationSize];

    for(uint32_t i = 0; i < allocationSize; ++i)
    {
        ptrs[i] = alloc.allocate(1);
    }

    for(uint32_t i =0; i < allocationSize; ++i)
    {
        alloc.deallocate(ptrs[i]);
    }
}

TEST(Allocator, alloc5)
{
    Allocator1<char> alloc;
    std::allocator<char> a;

    std::mutex mut;

    std::unordered_map<void*, bool> catcher;

    const auto allocate = [&a, &alloc, &mut, &catcher]() -> void
    {
        constexpr uint32_t allocationSize = 50;
        char* ptrs[allocationSize];
        for(uint32_t i = 0; i < allocationSize; ++i)
        {
            std::lock_guard<std::mutex> lk(mut);
            ptrs[i] = alloc.allocate(20);

            if(catcher.find(ptrs[i]) != catcher.end())
            {
         //       std::cout << "Double allocation\n";
            }
            catcher[(void*)ptrs[i]] = true;

            std::cout << std::this_thread::get_id() << " allocated: " << (void*)ptrs[i] << std::endl;
        }
        for(uint32_t i = 0; i < allocationSize; ++i)
        {
            std::lock_guard<std::mutex> lk(mut);
            std::cout << std::this_thread::get_id() << " trying to deallocate: " << (void*)ptrs[i] << std::endl;
            alloc.deallocate(ptrs[i]);
        }
    };

    std::thread t1(allocate);
    std::thread t2(allocate);
    std::thread t3(allocate);

    t1.join();
    t2.join();
    t3.join();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
