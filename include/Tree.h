#pragma once
#include "Patient.h"
#include "Criterion.h"
#include "Selection.h"
#include <memory>
#include <cstddef>

class Tree {
private:
    struct Node;
    std::unique_ptr<Node> root;
    std::size_t count = 0;
    static bool insertNode(std::unique_ptr<Node>& node, const Patient& patient);
    static bool removeNode(std::unique_ptr<Node>& node,const Patient& patient);
    static std::size_t nodeHeight(const Node* node);
    static void traverse(const Node* node, const Criterion& criterion, Selection& result, bool reverse);

public:
    Tree();
    ~Tree();
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    Tree(Tree&& other) noexcept;
    Tree& operator=(Tree&& other) noexcept;
    bool insert(const Patient& patient);
    bool remove(const Patient& patient);
    std::weak_ptr<Patient> find(const Patient& patient) const;
    std::size_t size() const;
    std::size_t height() const;
    Selection select(const Criterion& criterion) const;
    Selection inOrder() const;
    Selection reverseOrder() const;
};
