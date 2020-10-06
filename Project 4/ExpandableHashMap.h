#ifndef EXPANDABLE_H
#define EXPANDABLE_H
#include <list>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    struct Pair{
        KeyType key;
        ValueType value;
    };
    std::list<Pair>* m_list;
    double m_maxLoad;
    int m_numBuckets;
    int m_size;
    
    unsigned int getBucketNumber(const KeyType& key) const{
        unsigned int hasher(const KeyType& key); // return the bucket number from the hash function
        return hasher(key) % m_numBuckets;
    }
    double getLoad(int i = 0) const
    {
        return (size() + i)/(1.00 * m_numBuckets);
    }
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_numBuckets = 8;
    m_list = new std::list<Pair>[m_numBuckets];
    m_maxLoad = maximumLoadFactor;
    m_size = 0;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] m_list;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    delete [] m_list;
    m_numBuckets = 8;
    m_size = 0;
    m_list = new std::list<Pair>[m_numBuckets];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
   if (getLoad() > m_maxLoad){ // check whether exceed the cap
       m_numBuckets*=2;
    std::list<Pair>* replace = new std::list<Pair>[m_numBuckets];// replace with a larger list
    for (int i = 0; i < m_numBuckets/2;i++)
    {
        if (m_list[i].size() != 0)
        {
            typename std::list<Pair>::iterator it = m_list[i].begin();
            typename std::list<Pair>::iterator temp;
            while(it!=m_list[i].end()){ // TODO: check
                unsigned int index = getBucketNumber((*it).key); // rehash
                temp = it;
                it++;
                replace[index].splice(replace[index].end()/*target position*/, m_list[i]/*from*/, temp/*iterator*/); //use splice to move the entire list
            }
        }
    }
        delete [] m_list; // delete the original content
        m_list = replace;
    }
    Pair p = {key, value}; // create the Pair to be created
    ValueType* val =find(key);
    if (val != nullptr){
        *val = value;
    }
    else{
        unsigned int index2 = getBucketNumber(key); // hash -> bhucket number
        m_list[index2].push_back(p); // add the Pair into proper bucket
        m_size++;
    }
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned int bucket = getBucketNumber(key); // get bucket number
    typename std::list<Pair>::iterator it = m_list[bucket].begin(); // iterate throught the list
    for (; it != m_list[bucket].end(); it++){
        if ((*it).key == key)
            return &((*it).value);
    }
    return nullptr;
}
#endif /* EXPANDABLE_H */
