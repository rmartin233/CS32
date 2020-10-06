#include "Map.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;


void test_log(const string &msg, const char &sep = ' ') {
  cerr << sep << sep << sep << " " << msg << " " << sep << sep << sep << endl;
}

void test()
{
    
    Map m;
    Map result;
    ValueType value1;
    KeyType key1 = "Fred";
    ValueType value2;
    KeyType key2 = "Ethel";
    ValueType value3;
    KeyType key3 = "Martin";
    ValueType value4;
    KeyType key4 = "NONE";
    
    assert(m.empty()); // empty
    assert(m.insert("Fred", 2.7) && m.insert("Ethel", 3.5)); // test insert function
    assert(m.insertOrUpdate("Martin", 3.8) && m.insertOrUpdate("Ethel", 3.2)); // test insertOrUpdate function (insert update)
    assert( m.update("Fred", 2.9)); // test the update function
    assert(! m.update("Moe", 3.8)); // not exist, cannot update
    assert(! m.insert("Martin", 3.7)); // already exist, cannot insert
    assert(m.size() == 3 && ! m.empty()); // not empty and size = 3
    assert(m.contains("Fred") && m.contains("Ethel") && m.contains("Martin")); // test the contain function
    
    assert(m.insert("Alen", 3.0));
    assert(m.erase("Alen") && ! m.erase("AlenAlias") && m.size() == 3); // test the erase function

    Map m2(m); // test the copy constructor
    
    assert(m2.size() == 3 && m2.contains("Fred") && m2. contains("Ethel") && m2.contains("Martin"));
    assert(m2.get(key1, value1));
    assert(m2.get(key2, value2));
    assert(m2.get(key3, value3));
    assert(value1 == 2.9 && value2 == 3.2 && value3 == 3.8);
    assert(! m2.get(key4, value4)); // reterive value of a not existing key
    assert(! m2.get(4, key4, value4)); // i out of bonds
    assert(! m2.get(-1, key4, value4)); // i out of bonds
    assert(m2.get(0, key4, value4) && (key4 == "Fred" || key4 == "Ethel" || key4 == "Martin")); // First element in m2
    
    
    value1 = 0;
    value2 = 0;
    value3 = 0; // reassign the value
    
    Map m3;
    assert(m3.insert("Fred", 2.9) && m3.insert("Bla", 3.5));
    Map m4(m3); // copy constructor
    assert(m4.size() == 2 && m4.contains("Fred") && m4. contains("Bla")); // check whether the copy is successful
    
    
    m3 = m2; // assignment operator
    assert(m3.size() == 3 && m3.contains("Fred") && m3. contains("Ethel") && m3.contains("Martin")); // check whether the copy is successful
    m3.get(key1, value1);
    m3.get(key2, value2);
    m3.get(key3, value3);
    assert(value1 == 2.9 && value2 == 3.2 && value3 == 3.8); // check whether the value-key pair is correct
    
   
    
    assert(combine(m, m4, result)); // test combine
    assert(result.contains("Fred") && result.contains("Ethel") && result.contains("Martin") && result.contains("Bla") && result.size() == 4);
    
    Map m5;
    assert(m5.insert("Fred", 2.8) && m5.insert("Bla", 3.5));
    assert(! combine(m, m5, result)); // "fred" with different value, the function returns false
    assert(! result.contains("Fred") && result.contains("Ethel") && result.contains("Martin") && result.contains("Bla") && result.size() == 3); // Fred should not be in the result, but bla should stay in the result
    
    Map resultNotempty;
    assert(resultNotempty.insert("Fake", 2.8)); // special case when result is not empty
    assert(combine(m, m4, resultNotempty));
    assert(! resultNotempty.contains("Fake") && resultNotempty.size() == 4);
    
    reassign(m2, resultNotempty); // test reassign
    assert(resultNotempty.size() == 3 && resultNotempty.contains("Fred") && resultNotempty.contains("Ethel") && resultNotempty.contains("Martin"));
    assert(resultNotempty.get(key1, value1));
    assert(resultNotempty.get(key2, value2));
    assert(resultNotempty.get(key3, value3));
    assert(value1 != 2.9 && value2 != 3.2 && value3 != 3.8); // test whether values have been changed

    
    Map m6;
    assert(m6.insert("Martin", 3.9)); // test reassign with one element
    reassign(m6, resultNotempty);
    
    Map* m7 = &m2; // test reassign with same pointer
    reassign(*m7,m2);
    assert(m2.size() == 3 && m2.contains("Fred") && m2.contains("Ethel") && m2.contains("Martin"));
    assert(m2.get(key1, value1));
    assert(m2.get(key2, value2));
    assert(m2.get(key3, value3));
    assert(value1 != 2.9 && value2 != 3.2 && value3 != 3.8); // test whether values have been changed
    
    m7 = &m; // test when two parameter all points to the same Map
    Map* resultPtr = &m;
    assert(! combine(m5, *m7, *resultPtr));
    assert(resultPtr->size() == 3 && ! resultPtr->contains("Fred") && resultPtr->contains("Ethel") && resultPtr->contains("Martin"));
    
    
    Map* m8 = &m2; // test when three parameter all points to the same Map
    m7 = &m2;
    assert(combine(*m8, *m7, *resultPtr));
    assert(resultPtr->size() == 3 && resultPtr->contains("Fred") && resultPtr->contains("Ethel") && resultPtr->contains("Martin"));
}
 int main()
 {
     test();
     cout << "Passed all tests" << endl;
 }
