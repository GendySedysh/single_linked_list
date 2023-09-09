#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }

        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) {
            this->node_ = node;
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            this->node_ = other.node_;
        }

        BasicIterator(const BasicIterator<const Type>& other) noexcept {
            this->node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_) ? true : false;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return (this->node_ != rhs.node_) ? true : false;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_) ? true : false;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return (this->node_ != rhs.node_) ? true : false;
        }

        BasicIterator& operator++() noexcept {
            assert(this->node_ != nullptr);

            this->node_ = this->node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator<ValueType> tmp = *this;

            assert(this->node_ != nullptr);

            this->node_ = this->node_->next_node;
            return tmp;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(this->node_ != nullptr);
            return this->node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(this->node_ != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() {
        size_ = 0;
        UpdateBeforeBegin();
    }

    ~SingleLinkedList() {
        Clear();
    }

    template<typename Iter>
    void CopyIt(Iter& other) {
        std::vector<Type> values;

        for (auto value : other) {
            values.push_back(value);
        }

        SingleLinkedList tmp;
        for (auto it = values.rbegin(); it != values.rend(); it++) {
            this->PushFront(*it);
        }

        size_ = values.size();
    }

    SingleLinkedList(const SingleLinkedList& other) {
        CopyIt(other);
    }

    SingleLinkedList(const std::initializer_list<Type> i_list) {
        CopyIt(i_list);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            auto rhs_copy = SingleLinkedList(rhs);
            this->swap(rhs_copy);
        }
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);

        this->UpdateBeforeBegin();
        other.UpdateBeforeBegin();
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;

        UpdateBeforeBegin();
    }

    void PopFront() noexcept {
        Node* to_delete = head_.next_node;
        head_.next_node = to_delete->next_node;
        delete(to_delete);

        assert(size_ != 0);
        --size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* to_delete = head_.next_node;
            head_.next_node = to_delete->next_node;
            delete(to_delete);
        }
        size_ = 0;
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{ head_.next_node };
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{ nullptr };
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{ nullptr };
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{ nullptr };
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return before_begin_;
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return c_before_begin_;
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return c_before_begin_;
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;

        UpdateBeforeBegin();
        Iterator ret(new_node);
        return ret;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* to_delete = pos.node_->next_node;
        pos.node_->next_node = to_delete->next_node;
        delete(to_delete);

        assert(size_ != 0);
        --size_;

        Iterator ret(pos.node_->next_node);
        return ret;
    }

    void UpdateBeforeBegin() {
        before_begin_ = Iterator(&head_);
        c_before_begin_ = ConstIterator(&head_);
    }

private:
    Node head_;
    size_t size_;
    ConstIterator c_before_begin_;
    Iterator before_begin_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}