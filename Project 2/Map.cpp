#include "Map.h"
#include <iostream>

Map::Map()
{
    m_size = 0; // set size to zero
    
    Node* dummy = new Node; // create a new dummy node
    dummy->m_next = dummy; // the dummy node's next and previous pointer is it self
    dummy->m_prev = dummy;
    
    head = dummy; // set up a head points to dummy node
}

Map::~Map()
{
    Node* p = head->m_next; // create a pointer points to the first element
    while(p != head)
    {
        Node* n = p->m_next; // set up a temporary pointer points to the next element
        delete p; // delete the current values
        p = n; // the pointer points to the next element
    }
    delete head; // delete the dummy node
}

Map& Map::operator=(const Map& rhs)
{
    if (this != &rhs)
    {
        Map temp(rhs); // create a copy of right hand side Map
        swap(temp); // swap the value of the left hand side with the copy's value
    } // when go out of the scope, the temp with the Map the left hand side Map points to is deleted
    return *this;
}


Map::Map(const Map& other)
{
    m_size = 0;
    Node* dummy = new Node; // create a new Map with new dummy node
    dummy->m_next = dummy;
    dummy->m_prev = dummy;
    head = dummy;
    
    KeyType tempkey;
    ValueType tempvalue;
    for (int i = 0; i < other.size(); i++) // assign the values in the other Map into the new map
    {
        other.get(i, tempkey, tempvalue);
        this->insert(tempkey, tempvalue);
    }
}

bool Map::empty() const
{
    return size() == 0; // return whether the size ios zero
}

int Map::size() const
{
    return m_size; // return the size
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
    if(contains(key)) // if the key is already in the Map, return false
        return false;
    
    Node* newnode = new Node; // create a new node with values added in
    newnode->m_key = key;
    newnode->m_value = value;
    
    newnode->m_next = head; // assign the new node's next pointer to the head
    newnode->m_prev = head->m_prev; // assign the new node's next pointer to the original head's previous pointer (new pointes to the second last node)
    head->m_prev->m_next = newnode; // assign the original last node's next pointer to the new node
    head->m_prev = newnode; // assign head's previous pointer to new node
    m_size++;
    
    return true;
}

bool Map::update(const KeyType& key, const ValueType& value)
{
    for (Node* p = head->m_next; p != head; p = p->m_next) // repeat until the end of the Map is met
    {
        if (p->m_key == key){ // if there is a key equal to the requested key
            p->m_value = value; // change its value to requested value
            return true; // return true
        }
    }
    return false;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
    if (update(key, value))
        return true;
    if (insert(key, value))
        return true;
    return false;
}

bool Map::erase(const KeyType& key)
{
    for (Node* p = head->m_next; p != head; p = p->m_next)
    {
        if (p->m_key == key){
            p->m_next->m_prev = p->m_prev; // this key's next node's previous node points to its previous node
            p->m_prev->m_next = p->m_next; // this key's previous node's next node points to its next node
            delete p;
            m_size--;
            return true;
        }
    }
    return false;
}


bool Map::contains(const KeyType& key) const
{
    for (Node* p = head->m_next; p != head; p = p->m_next) // repeat until the end of the Map is met
    {
        if (p->m_key == key) // if one key is equal to the requested key
            return true;
    }
    return false;
}

bool Map::get(const KeyType& key, ValueType& value) const
{
    for (Node* p = head->m_next; p != head; p = p->m_next)
    {
        if (p->m_key == key){ // if one key is equal to the requested key
            value = p->m_value; // set the value parameter to key's value
            return true;
        }
    }
    return false;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
    if (i < 0 || i >= m_size) // if i is out of bounds
        return false;
    
    int count = 0;
    Node* p = head->m_next;
    for (; count < i ; p = p->m_next, count++) // use a loop to the ith node
    ;
    
    key = p->m_key; // set the key parameter to this node's key
    value = p->m_value; // set the value parameter to this node's value
    return true;
}

void Map::swap(Map& other)
{
    Node* temp = head; // create a pointer points to this node's head
    head = other.head; // assign this node's head points to other's head
    other.head = temp; // assign other's node's head to the original this node's head via the temp pointer
    
    int tempsize = m_size;
    m_size = other.m_size;
    other.m_size = tempsize;
}

bool combine(const Map& m1, const Map& m2, Map& result){
    Map other;
    if(&m2 == &result){
        result = m2; // check whether m2 is equal to result to avoid the double deletion issue
        other = m1;
    }else{
        result = m1;//assign m1 to result
        other = m2;
    }
    KeyType k;
    ValueType v;
    bool status = true;// it store the return value, since even though the
    //function may return false, result must obey the rule.
    
    for(int i = 0; i < other.size(); i++){
        other.get(i, k, v);
        //insert the node in m2 to the result according to the rules if the key is not included in result.
        if(!result.contains(k)){
            result.insert(k, v);
        }else{
            ValueType v2;
            result.get(k, v2);
            if(v2!=v){
                result.erase(k);// if same key and different value, erase this node.
                status = false;
            }
        }
    }
    return status;
}

void reassign(const Map& m, Map& result)
{
    KeyType tempkey;
    ValueType tempvalue;
    KeyType tempkey2;
    ValueType tempvalue2;
    ValueType tempvalueFirst;
    
    result = m;
    result.get(0, tempkey2, tempvalueFirst);
    //get the value from first element
    
    for (int i = 0, valueindex = 1; valueindex < result.size(); i++, valueindex++)
    {
        result.get(i, tempkey, tempvalue); // assign the current key the value the previous node's value
        result.get(valueindex, tempkey2, tempvalue2);
        result.update(tempkey, tempvalue2);
    }
    
    result.update(tempkey2, tempvalueFirst); // assign the last key the first value
}

