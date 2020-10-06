#ifndef Map_h
#define Map_h
#include <string>

using KeyType = std::string;
using ValueType = double;

class Map
{
public:
    Map();
    ~Map();
    Map& operator=(const Map& rhs);
    Map(const Map& other);
    bool empty() const;
    int size() const;
    bool insert(const KeyType& key, const ValueType& value);
    bool update(const KeyType& key, const ValueType& value);
    bool insertOrUpdate(const KeyType& key, const ValueType& value);
    bool erase(const KeyType& key);
    bool contains(const KeyType& key) const;
    bool get(const KeyType& key, ValueType& value) const;
    bool get(int i, KeyType& key, ValueType& value) const;
    void swap(Map& other);
    
private:
    
    struct Pair // values in one node
    {
        KeyType   m_key;
        ValueType m_value;
    };

    int  m_size; // number of elements in one linked list
    
    struct Node
    {
        Node* m_next; // position of the next pointer
        Node* m_prev; // position of the previous pointer
        KeyType   m_key;
        ValueType m_value;
    };
    
    Node* head;
};
bool combine(const Map& m1, const Map& m2, Map& result);
void reassign(const Map& m, Map& result);
#endif /* Map_h */
