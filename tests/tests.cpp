
#include <gtest/gtest.h>

#include "Tree.h"

#include <vector>
#include <utility>
#include <type_traits>

Tree makeTree() {
    Tree tree;
    tree.insert(Patient("A", 50, 150));
    tree.insert(Patient("B", 30, 145));
    tree.insert(Patient("C", 70, 160));
    tree.insert(Patient("D", 20, 110));
    tree.insert(Patient("E", 40, 130));
    tree.insert(Patient("F", 60, 135));
    tree.insert(Patient("G", 80, 170));
    return tree;
}

std::vector<int> ages(const Selection& selection) {
    std::vector<int> result;
    for (std::size_t i = 0; i < selection.size(); ++i) {
        if (auto patient = selection[i].lock()) {
            result.push_back(patient->getAge());
        }
    }
    return result;
}

TEST(TreeTest, Insert1) {
    Tree tree;
    EXPECT_TRUE(tree.insert(Patient("A", 50, 150)));
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_FALSE(tree.insert(Patient("B", 50, 120)));
    EXPECT_EQ(tree.size(), 1u);
}
TEST(TreeTest, Find) {
    Tree tree = makeTree();
    auto patient = tree.find(Patient(50)).lock();
    ASSERT_TRUE(patient);
    EXPECT_EQ(patient->getSurname(), "A");
    EXPECT_DOUBLE_EQ(patient->getPressure(), 150);

    EXPECT_TRUE(tree.find(Patient(99)).expired());
}

TEST(TreeTest, Height) {
    Tree tree;
    EXPECT_EQ(tree.height(), 0u);
    tree.insert(Patient("A", 50, 150));
    EXPECT_EQ(tree.height(), 1u);
    tree = makeTree();
    EXPECT_EQ(tree.height(), 3u);
}


TEST(TreeTest, Obhod1) {
    Tree tree = makeTree();
    EXPECT_EQ(ages(tree.inOrder()),
              (std::vector<int>{20,30,40,50,60,70,80}));
    EXPECT_EQ(ages(tree.reverseOrder()),
              (std::vector<int>{80,70,60,50,40,30,20}));
}


TEST(TreeTest, RemoveLeaf) {
    Tree tree = makeTree();
    EXPECT_TRUE(tree.remove(Patient(20)));
    EXPECT_EQ(tree.size(), 6u);
    EXPECT_TRUE(tree.find(Patient(20)).expired());
    EXPECT_EQ(ages(tree.inOrder()),
              (std::vector<int>{30,40,50,60,70,80}));
}

TEST(TreeTest, RemoveOneChild) {
    Tree tree = makeTree();
    ASSERT_TRUE(tree.remove(Patient(20)));
    ASSERT_TRUE(tree.remove(Patient(30)));
    EXPECT_EQ(tree.size(), 5u);
    EXPECT_TRUE(tree.find(Patient(30)).expired());
    EXPECT_EQ(ages(tree.inOrder()),
              (std::vector<int>{40,50,60,70,80}));
}

TEST(TreeTest, RemoveTwoChildren) {
    Tree tree = makeTree();
    auto removed = tree.find(Patient(50));
    auto successor = tree.find(Patient(60));
    EXPECT_TRUE(tree.remove(Patient(50)));
    EXPECT_EQ(tree.size(), 6u);
    EXPECT_TRUE(removed.expired());
    EXPECT_FALSE(successor.expired());
    EXPECT_TRUE(tree.find(Patient(50)).expired());
    EXPECT_EQ(ages(tree.inOrder()),
              (std::vector<int>{20,30,40,60,70,80}));
}

TEST(TreeTest, RemoveMissing) {
    Tree tree = makeTree();
    EXPECT_FALSE(tree.remove(Patient(99)));
    EXPECT_EQ(tree.size(), 7u);
    EXPECT_EQ(ages(tree.inOrder()),
              (std::vector<int>{20,30,40,50,60,70,80}));
}
TEST(SelectionTest, PatientCriterion) {
    Tree tree = makeTree();
    PatientCriterion criterion(50, 140);
    Selection result = tree.select(criterion);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(ages(result), (std::vector<int>{50,70,80}));
    PatientCriterion boundary(50, 150);
    EXPECT_EQ(ages(tree.select(boundary)),
              (std::vector<int>{70,80}));
}

TEST(SelectionTest, EmptySelection) {
    Tree tree = makeTree();
    PatientCriterion criterion(100, 200);
    Selection result = tree.select(criterion);
    EXPECT_TRUE(result.empty());
    EXPECT_EQ(result.size(), 0u);
    Tree emptyTree;
    AllCriterion all;
    EXPECT_TRUE(emptyTree.select(all).empty());
}

TEST(SelectionTest, DifferentCriteria) {
    Tree tree = makeTree();
    auto before = ages(tree.inOrder());
    auto oldSize = tree.size();
    auto oldHeight = tree.height();
    AllCriterion all;
    PatientCriterion criterion(40, 140);
    EXPECT_EQ(tree.select(all).size(), 7u);
    EXPECT_EQ(tree.select(criterion).size(), 3u);
    EXPECT_EQ(tree.size(), oldSize);
    EXPECT_EQ(tree.height(), oldHeight);
    EXPECT_EQ(ages(tree.inOrder()), before);
}

TEST(SelectionTest, CopyAndExpiration) {
    Tree tree = makeTree();
    PatientCriterion criterion(40, 140);
    Selection original = tree.select(criterion);
    Selection copy(original);
    Selection assigned;
    assigned = original;
    copy.pushBack(tree.find(Patient(20)));
    EXPECT_EQ(original.size(), 3u);
    EXPECT_EQ(copy.size(), 4u);
    EXPECT_EQ(assigned.size(), 3u);
    ASSERT_TRUE(tree.remove(Patient(50)));
    EXPECT_EQ(original.size(), 3u);
    EXPECT_EQ(copy.size(), 4u);
    EXPECT_TRUE(original[0].expired());
    EXPECT_TRUE(copy[0].expired());
    EXPECT_TRUE(assigned[0].expired());
    EXPECT_FALSE(original[1].expired());
    EXPECT_FALSE(copy[1].expired());
    EXPECT_FALSE(assigned[1].expired());
}

TEST(MoveTest, MoveTreeAndSelection) {
    static_assert(!std::is_copy_constructible_v<Tree>);
    static_assert(!std::is_copy_assignable_v<Tree>);
    Tree source = makeTree();
    Tree target(std::move(source));
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.height(), 0u);
    EXPECT_EQ(target.size(), 7u);
    Tree finalTree;
    finalTree = std::move(target);
    EXPECT_EQ(target.size(), 0u);
    EXPECT_EQ(finalTree.size(), 7u);
    Selection first = finalTree.inOrder();
    Selection second(std::move(first));
    EXPECT_TRUE(first.empty());
    EXPECT_EQ(first.getCapacity(), 0u);
    Selection third;
    third = std::move(second);
    EXPECT_TRUE(second.empty());
    EXPECT_EQ(second.getCapacity(), 0u);
    EXPECT_EQ(ages(third),
              (std::vector<int>{20,30,40,50,60,70,80}));
}

TEST(MemoryTest, AliveCounter) {
    EXPECT_EQ(Patient::getAlive(), 0);
    Selection result;
    {
        Tree tree = makeTree();
        EXPECT_EQ(Patient::getAlive(), 7);
        result = tree.inOrder();
        EXPECT_EQ(Patient::getAlive(), 7);
        ASSERT_TRUE(tree.remove(Patient(50)));
        EXPECT_EQ(Patient::getAlive(), 6);
    }
    EXPECT_EQ(Patient::getAlive(), 0);
    for (std::size_t i = 0; i < result.size(); ++i) {
        EXPECT_TRUE(result[i].expired());
    }
}
