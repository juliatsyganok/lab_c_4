#include "Tree.h"

#include <algorithm>
#include <utility>

struct Tree::Node {
    std::shared_ptr<Patient> data;

    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node(const Patient& patient)
        : data(std::make_shared<Patient>(patient)) {}
};

Tree::Tree() = default;
Tree::~Tree() = default;

Tree::Tree(Tree&& other) noexcept
    : root(std::move(other.root)),
      count(std::exchange(other.count, 0))
{}

Tree& Tree::operator=(Tree&& other) noexcept {
    if (this != &other) {
        root = std::move(other.root);
        count = std::exchange(other.count, 0);
    }

    return *this;
}

bool Tree::insertNode(
    std::unique_ptr<Node>& node,
    const Patient& patient)
{
    if (!node) {
        node = std::make_unique<Node>(patient);
        return true;
    }

    if (patient < *node->data) {
        return insertNode(node->left, patient);
    }

    if (*node->data < patient) {
        return insertNode(node->right, patient);
    }

    return false;
}

bool Tree::insert(const Patient& patient) {
    if (insertNode(root, patient)) {
        ++count;
        return true;
    }

    return false;
}

std::weak_ptr<Patient> Tree::find(
    const Patient& patient) const
{
    const Node* current = root.get();

    while (current) {
        if (patient == *current->data) {
            return current->data;
        }

        if (patient < *current->data) {
            current = current->left.get();
        } else {
            current = current->right.get();
        }
    }

    return {};
}

bool Tree::removeNode(
    std::unique_ptr<Node>& node,
    const Patient& patient)
{
    if (!node) {
        return false;
    }

    if (patient < *node->data) {
        return removeNode(node->left, patient);
    }

    if (*node->data < patient) {
        return removeNode(node->right, patient);
    }

    if (!node->left) {
        auto child = std::move(node->right);
        node = std::move(child);
    }
    else if (!node->right) {
        auto child = std::move(node->left);
        node = std::move(child);
    }
    else {
        auto* successor = &node->right;

        while ((*successor)->left) {
            successor = &(*successor)->left;
        }

        node->data = std::move((*successor)->data);

        auto child = std::move((*successor)->right);
        *successor = std::move(child);
    }

    return true;
}

bool Tree::remove(const Patient& patient) {
    if (removeNode(root, patient)) {
        --count;
        return true;
    }

    return false;
}

std::size_t Tree::size() const {
    return count;
}

std::size_t Tree::nodeHeight(const Node* node) {
    if (!node) {
        return 0;
    }

    return 1 + std::max(
        nodeHeight(node->left.get()),
        nodeHeight(node->right.get()));
}

std::size_t Tree::height() const {
    return nodeHeight(root.get());
}

void Tree::traverse(
    const Node* node,
    const Criterion& criterion,
    Selection& result,
    bool reverse)
{
    if (!node) {
        return;
    }

    const Node* first = reverse
        ? node->right.get() : node->left.get();

    const Node* second = reverse
        ? node->left.get() : node->right.get();

    traverse(first, criterion, result, reverse);

    if (criterion.matches(*node->data)) {
        result.pushBack(node->data);
    }

    traverse(second, criterion, result, reverse);
}

Selection Tree::select(
    const Criterion& criterion) const
{
    Selection result;

    traverse(root.get(), criterion, result, false);

    return result;
}

Selection Tree::inOrder() const {
    AllCriterion criterion;
    return select(criterion);
}

Selection Tree::reverseOrder() const {
    AllCriterion criterion;
    Selection result;

    traverse(root.get(), criterion, result, true);

    return result;
}
