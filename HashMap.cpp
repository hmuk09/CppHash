#include <algorithm>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <vector>

using std::swap;

const size_t start_size = (1 << 3);

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using KeyValPair = std::pair<const KeyType, ValueType>;
    using iterator = typename std::list<KeyValPair>::iterator;
    using const_iterator = typename std::list<KeyValPair>::const_iterator;

private:
    size_t table_size, elements_size;
    std::vector<std::list<iterator>> hash_table;
    std::list<KeyValPair> elements_list;
    Hash hasher;

public:
    HashMap(Hash hash = Hash())
    : table_size(start_size)
    , elements_size(0)
    , hasher(hash) {
        hash_table.resize(table_size);
    }

    HashMap(HashMap& temp)
    : table_size(temp.table_size)
    , elements_size(temp.elements_size)
    , hash_table(temp.hash_table)
    , elements_list(temp.elements_list)
    , hasher(temp.hasher) {
    }

    HashMap& operator=(HashMap& temp) {
        if (this == &temp)
            return (*this);
        *this = HashMap(temp);
        return (*this);
    }

    HashMap& operator=(HashMap&& temp) {
        if (this == &temp)
            return (*this);
        swap(table_size, temp.table_size);
        swap(elements_size, temp.elements_size);
        swap(hash_table, temp.hash_table);
        swap(elements_list, temp.elements_list);
        swap(hasher, temp.hasher);
        return (*this);
    }

    template <class iter>
    HashMap(iter first, iter last, Hash hash = Hash())
    : table_size(start_size)
    , elements_size(0)
    , hasher(hash) {
        hash_table.resize(table_size);
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    HashMap(std::initializer_list<KeyValPair>& init, Hash hash = Hash())
    : table_size(start_size)
    , elements_size(0)
    , hasher(hash) {
        hash_table.resize(table_size);
        for (const auto& element : init) {
            insert(element);
        }
    }

    size_t size() const {
        return elements_size;
    }

    bool empty() const {
        return elements_size == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    void insert(std::pair<KeyType, ValueType>& new_element) {
        if (find(new_element.first) != end())
            return;

        const int multiply_limit = 4;
        const int size_multiplier = 2;

        if (elements_size * multiply_limit >= table_size) {
            std::list<KeyValPair> t_list = elements_list;

            size_t new_table_size = table_size * size_multiplier;
            clear();
            hash_table.resize(new_table_size);
            table_size = new_table_size;

            for (const auto& element : t_list) {
                insert(element);
            }
        }

        ++elements_size;
        elements_list.push_back(new_element);
        hash_table[hash_function()(new_element.first) % table_size].push_back(--elements_list.end());
    }

    void erase(KeyType& key) {
        size_t hash_index = hasher(key) % table_size;
        for (auto element = hash_table[hash_index].begin(); element != hash_table[hash_index].end(); ++element) {
            if ((*element)->first == key) {
                --elements_size;
                elements_list.erase(*element);
                hash_table[hash_index].erase(element);
                break;
            }
        }
    }

    void clear() {
        elements_list.clear();
        hash_table.clear();
        elements_size = 0;
        table_size = start_size;
        hash_table.resize(start_size);
    }

    iterator begin() {
        return elements_list.begin();
    }

    iterator end() {
        return elements_list.end();
    }

    const_iterator begin() const {
        return elements_list.begin();
    }

    const_iterator end() const {
        return elements_list.end();
    }

    iterator find(const KeyType& key) {
        size_t hash_index = hasher(key) % table_size;

        for (const auto& element : hash_table[hash_index]) {
            if (element->first == key)
                return element;
        }

        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t hash_index = hasher(key) % table_size;

        for (const auto& element : hash_table[hash_index]) {
            if (element->first == key)
                return element;
        }

        return end();
    }

    ValueType& operator[](const KeyType& key) {
        KeyValPair element = {key, ValueType()};

        insert(element);
        auto iter = find(element.first);

        return iter->second;
    }

    const ValueType& at(const KeyType& key) const {
        auto iter = find(key);
        if (iter == end())
            throw std::out_of_range("error");

        return iter->second;
    }
};
