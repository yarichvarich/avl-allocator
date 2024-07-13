#include "../include/AVLTree.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>

static void insertRange(Yaro::Utility::AVLTree<int> &tree, int begin, int end);

TEST(SmallAVLTreeTest, insertion1)
{
    Yaro::Utility::AVLTree<int> tree;

    tree.insert(-100);
    tree.insert(-50);
    tree.insert(-25);
    tree.insert(90);
    tree.insert(100);
    tree.insert(200);
    tree.insert(99);

    EXPECT_EQ(tree.size(), 7);
}

TEST(SmallAVLTreeTest, insertion2)
{
    Yaro::Utility::AVLTree<int> tree;

    insertRange(tree, 20, 50);
    insertRange(tree, 0, 1000000);
    insertRange(tree, -100, 100);
    insertRange(tree, 0, 1000000);

    EXPECT_EQ(tree.size(), 30 + 1000000 + 200 + 1000000 + 4);
}

TEST(SmallAVLTreeTest, insertionPrint)
{
    Yaro::Utility::AVLTree<int> tree;

    insertRange(tree, 1, 16);

    EXPECT_EQ(tree.size(), 16);

    tree.print();

    tree.clear();

    insertRange(tree, -8, 8);

    EXPECT_EQ(tree.size(), 16 + 1);

    tree.print();
}

TEST(SmallAVLTree, compare1)
{
    Yaro::Utility::AVLTree<int> tree1, tree2;

    insertRange(tree1, -1000000, 1000000);
    insertRange(tree2, -1000000, 1000000);

    EXPECT_TRUE(tree1 == tree2);
}

TEST(SmallAVLTree, compare2)
{
    Yaro::Utility::AVLTree<int> tree1, tree2;

    EXPECT_TRUE(tree1 == tree2);

    tree1.insert(0);

    EXPECT_TRUE(tree1 != tree2);

    tree2.insert(0);
    tree1.insert(100);
    tree2.insert(100);

    EXPECT_TRUE(tree1 == tree2);
}

TEST(SmallAVLTree, compare3)
{
    Yaro::Utility::AVLTree<int> tree1, tree2;

    insertRange(tree1, -1000000, 1000000);
    tree1.pop(0);

    insertRange(tree2, -1000000, 1000000);

    EXPECT_TRUE(tree1 != tree2);
}

TEST(SmallAVLTree, compare4)
{
    Yaro::Utility::AVLTree<int> tree1, tree2;

    insertRange(tree1, -1000000, 1000);
    insertRange(tree2, -1000, 1000000);

    bool eq = tree1 != tree2;

    EXPECT_TRUE(eq);
}

TEST(SmallAVLTree, copy1)
{
    Yaro::Utility::AVLTree<int> tree1;

    insertRange(tree1, -10000, 10000);

    auto tree2{tree1};

    EXPECT_TRUE(tree2 == tree1);
}

TEST(SmallAVLTree, copy2)
{
    Yaro::Utility::AVLTree<int> tree1;

    auto tree2{tree1};

    EXPECT_TRUE(tree1 == tree2);

    tree1.insert(0);

    tree2 = tree1;

    EXPECT_TRUE(tree1 == tree2);

    tree2.insert(-1);

    tree1 = tree2;

    EXPECT_TRUE(tree1 == tree2);

    tree1.insert(1);

    tree2 = tree1;

    EXPECT_TRUE(tree1 == tree2);
}

TEST(SmallAVLTree, move1)
{
    Yaro::Utility::AVLTree<int> tree1;

    insertRange(tree1, -10000, 10000);

    auto tree2{tree1};
    auto tree3{std::move(tree1)};

    EXPECT_TRUE(tree2 == tree3);
    EXPECT_TRUE(tree2 != tree1);
}

TEST(SmallAVLTree, move2)
{
    Yaro::Utility::AVLTree<int> tree1;

    insertRange(tree1, -10000, 10000);

    auto tree2{tree1};
    auto tree3 = std::move(tree1);

    EXPECT_TRUE(tree2 == tree3);
    EXPECT_TRUE(tree2 != tree1);
}

class LargeAVLTreeTest : public ::testing::Test
{
  protected:
    virtual void SetUp() override
    {
        insertRange(m_tree, 2000, 5000);
        insertRange(m_tree, -1234, 230);

        m_tree.insert(-669);

        insertRange(m_tree, 10000, 15000);
        insertRange(m_tree, -234, 500);

        m_tree.insert(9991);

        insertRange(m_tree, -999, 1000);
        insertRange(m_tree, -99999, -80000);

        m_tree.insert(777);

        insertRange(m_tree, 123123, 123188);
        insertRange(m_tree, 7878, 8787);

        m_tree.insert(0);

        insertRange(m_tree, -5000, -490);
        insertRange(m_tree, 5, 15);

        m_tree.insert(9999);

        insertRange(m_tree, 1, 2);
        insertRange(m_tree, -900000, -800000);

        m_tree.insert(8888);

        insertRange(m_tree, -9000, -8888);
        insertRange(m_tree, 77777, 787878);

        m_tree.insert(1000000);

        insertRange(m_tree, 400, 499);
        insertRange(m_tree, 9998, 10020);

        m_tree.insert(3131);

        insertRange(m_tree, 4545, 5656);
        insertRange(m_tree, -4545, -3333);

        m_tree.insert(1313);

        insertRange(m_tree, -999, -888);
        insertRange(m_tree, -11111, -9999);

        m_tree.insert(-1000000);
    }

    virtual void TearDown() override
    {
    }

    Yaro::Utility::AVLTree<int> m_tree;
};

TEST_F(LargeAVLTreeTest, balance)
{
    EXPECT_LE(m_tree.balance(), 1);
}

TEST_F(LargeAVLTreeTest, height)
{
    uint8_t expectedHeight = static_cast<uint8_t>(std::log2(m_tree.size()) + 2u);
    EXPECT_LE(m_tree.height(), expectedHeight);
}

TEST_F(LargeAVLTreeTest, findElement1)
{
    EXPECT_TRUE(m_tree.find(-669));
}

TEST_F(LargeAVLTreeTest, findElement2)
{
    EXPECT_TRUE(m_tree.find(9991));
}

TEST_F(LargeAVLTreeTest, findElement3)
{
    EXPECT_TRUE(m_tree.find(777));
}

TEST_F(LargeAVLTreeTest, findElement4)
{
    EXPECT_TRUE(m_tree.find(0));
}

TEST_F(LargeAVLTreeTest, findElement5)
{
    EXPECT_TRUE(m_tree.find(9999));
}

TEST_F(LargeAVLTreeTest, findElement6)
{
    EXPECT_TRUE(m_tree.find(8888));
}

TEST_F(LargeAVLTreeTest, findElement7)
{
    EXPECT_TRUE(m_tree.find(1000000));
}

TEST_F(LargeAVLTreeTest, findElement8)
{
    EXPECT_TRUE(m_tree.find(3131));
}

TEST_F(LargeAVLTreeTest, findElement9)
{
    EXPECT_TRUE(m_tree.find(1313));
}

TEST_F(LargeAVLTreeTest, findElement10)
{
    EXPECT_TRUE(m_tree.find(-1000000));
}

TEST_F(LargeAVLTreeTest, findElement11)
{
    EXPECT_FALSE(m_tree.find(2000000));
}

TEST_F(LargeAVLTreeTest, findElement12)
{
    EXPECT_FALSE(m_tree.find(-2000000));
}

TEST_F(LargeAVLTreeTest, popElement1)
{
    EXPECT_TRUE(m_tree.pop(-1000000));
    EXPECT_FALSE(m_tree.find(-1000000));
}

TEST_F(LargeAVLTreeTest, popElement2)
{
    EXPECT_TRUE(m_tree.pop(1313));
    EXPECT_FALSE(m_tree.find(1313));
}

TEST_F(LargeAVLTreeTest, popElement3)
{
    EXPECT_TRUE(m_tree.pop(3131));
    EXPECT_TRUE(m_tree.find(3131));
}

TEST_F(LargeAVLTreeTest, popElement4)
{
    EXPECT_TRUE(m_tree.pop(8888));
    EXPECT_FALSE(m_tree.find(8888));
}

TEST_F(LargeAVLTreeTest, popElement5)
{
    EXPECT_TRUE(m_tree.pop(9999));
    EXPECT_TRUE(m_tree.find(9999));
}

TEST_F(LargeAVLTreeTest, popElement6)
{
    EXPECT_TRUE(m_tree.pop(777));
    EXPECT_TRUE(m_tree.find(777));
}

TEST_F(LargeAVLTreeTest, findClosest1)
{
    int val;
    EXPECT_TRUE(m_tree.findClosest(-1000001, val));
    EXPECT_EQ(val, -1000000);
}

TEST_F(LargeAVLTreeTest, findClosest2)
{
    int val;
    EXPECT_TRUE(m_tree.findClosest(1000001, val));
    EXPECT_EQ(val, 1000000);
}

TEST_F(LargeAVLTreeTest, findClosest3)
{
    int val;
    EXPECT_TRUE(m_tree.findClosest(-900400, val));
    EXPECT_EQ(val, -900000);
}

/*TEST_F(LargeAVLTreeTest, findClosest4)
{
    int val;
    EXPECT_TRUE(m_tree.findClosest(-1000001, val));
    EXPECT_EQ(m_tree.findClosest(15), 15);
}

TEST_F(LargeAVLTreeTest, findClosest5)
{
    int val;
    EXPECT_TRUE(m_tree.findClosest(-1000001, val));
    EXPECT_EQ(m_tree.findClosest(1313), 1313);
}
*/

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

void insertRange(Yaro::Utility::AVLTree<int> &tree, int begin, int end)
{
    for (int i = begin; i <= end; i++)
    {
        tree.insert(i);
    }
}