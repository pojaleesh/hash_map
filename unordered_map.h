#pragma once

#include <stdexcept>
#include <vector>
#include <list>
#include <iterator>
#include <utility>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 public:
    using KeyValuePair = std::pair<const KeyType, ValueType>;

    using iterator = typename std::list<KeyValuePair>::iterator;

    using const_iterator = typename std::list<KeyValuePair>::const_iterator;

    HashMap(const Hash &new_hash = Hash());

    template<class InputIterator>
    HashMap(InputIterator first, InputIterator second,
            const Hash &new_hash = Hash());

    HashMap(std::initializer_list<KeyValuePair> list_element,
            const Hash &new_hash = Hash());

    HashMap(const HashMap& other);

    HashMap& operator=(const HashMap& other);

    ValueType& operator[](const KeyType key);

    void insert(const KeyValuePair new_node);

    void erase(const KeyType key);

    const ValueType& at(KeyType key) const;

    iterator find(const KeyType key);

    const_iterator find(const KeyType key) const;

    iterator begin() noexcept {
        return elements_.begin();
    }

    const_iterator begin() const {
        return elements_.cbegin();
    }

    iterator end() noexcept {
        return elements_.end();
    }

    const_iterator end() const {
        return elements_.cend();
    }

    size_t size() const {
        return num_elements_;
    }

    bool empty() const {
        return (num_elements_ == 0);
    }

    Hash hash_function() const {
        return hash_;
    }

    void clear();

 private:
    using TableList = std::list<typename std::list<KeyValuePair>::iterator>;

    using TableListIterator = typename TableList::const_iterator;

    static constexpr size_t initial_size_ = 20;
    static constexpr double load_factor_ = 0.5;
    size_t num_elements_ = 0;
    size_t hash_table_size_ = initial_size_;
    Hash hash_;
    std::vector<TableList> hash_table_;
    std::list<KeyValuePair> elements_;

    void check_capacity() {
        size_t capacity_factor_ = static_cast<double>(num_elements_)
                                  / hash_table_size_;
        if (capacity_factor_ >= load_factor_) {
            double_size();
        }
    }

    TableListIterator InternalFind(const KeyType key) const;

    void double_size();
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(const Hash &new_hash)
        : hash_(new_hash) {
    hash_table_.resize(hash_table_size_);
}

template<class KeyType, class ValueType, class Hash>
template<class InputIterator>
HashMap<KeyType, ValueType, Hash>::HashMap(
        InputIterator first,
        InputIterator second,
        const Hash &new_hash) : hash_(new_hash) {
    hash_table_.resize(hash_table_size_);
    while (first != second) {
        insert(*first);
        first++;
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(
        std::initializer_list<KeyValuePair> list_element,
        const Hash &new_hash) : hash_(new_hash) {
    hash_table_.resize(hash_table_size_);
    for (auto element : list_element) {
        insert(element);
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(const HashMap& other)
        : hash_(other.hash_) {
    hash_table_.resize(hash_table_size_);
    for (auto element : other.elements_) {
        insert(element);
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(const KeyValuePair new_node) {
    check_capacity();
    size_t hash_table_cell_num = hash_(new_node.first) % hash_table_size_;
    auto element_iterator = find(new_node.first);
    if (element_iterator == end()) {
        num_elements_++;
        elements_.push_front(new_node);
        hash_table_[hash_table_cell_num].push_back(elements_.begin());
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(const KeyType key) {
    size_t hash_table_cell_num = hash_(key) % hash_table_size_;
    auto element_iterator = InternalFind(key);
    if (element_iterator != hash_table_[hash_table_cell_num].end()) {
        elements_.erase(*element_iterator);
        hash_table_[hash_table_cell_num].erase(element_iterator);
        num_elements_--;
        return;
    }
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::find(const KeyType key) -> iterator {
    size_t hash_table_cell_num = hash_(key) % hash_table_size_;
    auto element_iterator = InternalFind(key);
    if (element_iterator != hash_table_[hash_table_cell_num].end()) {
        return *element_iterator;
    }
    return end();
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::find(const KeyType key) const
-> const_iterator {
    size_t hash_table_cell_num = hash_(key) % hash_table_size_;
    auto element_iterator = InternalFind(key);
    if (element_iterator != hash_table_[hash_table_cell_num].end()) {
        return *element_iterator;
    }
    return end();
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(KeyType key) const {
    auto element_iterator = find(key);
    if (element_iterator != end()) {
        return element_iterator->second;
    }
    throw std::out_of_range("out of range");
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::InternalFind(const KeyType key) const
-> TableListIterator {
    size_t index = hash_(key) % hash_table_size_;
    for (auto elem = hash_table_[index].begin();
        elem != hash_table_[index].end(); elem++) {
        if ((*elem)->first == key) {
            return elem;
        }
    }
    return hash_table_[index].end();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    num_elements_ = 0;
    hash_table_.clear();
    elements_.clear();
    hash_table_size_ = initial_size_;
    hash_table_.resize(hash_table_size_);
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[](const KeyType key) {
    auto element_iterator = find(key);
    if (element_iterator != end()) {
        return element_iterator->second;
    }
    std::pair<KeyType, ValueType> new_node(key, ValueType());
    insert(new_node);
    return find(key)->second;
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>&
HashMap<KeyType, ValueType, Hash>::operator=(const HashMap& other) {
    if (this != &other) {
        hash_ = other.hash_;
        clear();
        for (auto element : other) {
            insert(element);
        }
    }
    return *this;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::double_size() {
    hash_table_.clear();
    hash_table_size_ *= 2;
    hash_table_.resize(hash_table_size_);
    num_elements_ = 0;
    for (auto elem = elements_.begin(); elem != elements_.end(); elem++) {
        size_t hash_table_cell_num = hash_(elem->first) % hash_table_size_;
        hash_table_[hash_table_cell_num].push_back(elem);
        num_elements_++;
    }
}
