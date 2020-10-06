#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <iostream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    
    ExpandableHashMap<GeoCoord, vector<StreetSegment>>* m_map;
    void addItem(const GeoCoord& key, const StreetSegment* seg);
};

StreetMapImpl::StreetMapImpl()
{
    m_map = new ExpandableHashMap<GeoCoord, vector<StreetSegment>>(); // add a new map container
}

StreetMapImpl::~StreetMapImpl()
{
     delete m_map;
}

void StreetMapImpl::addItem(const GeoCoord& key, const StreetSegment* seg){
    auto segVector = m_map->find(key); // get key position
    if (segVector == nullptr)
    {
        vector<StreetSegment> segVectorNew;// if there is no segment stored, create a new one
        segVectorNew.push_back(*seg);
        m_map->associate(key, segVectorNew);
    }
    else{
        (segVector)->push_back(*seg); // is there are several segment exists, add to back
        m_map->associate(key, *segVector);
    }
}

bool StreetMapImpl::load(string mapFile)
{
     StreetMap m; // create a street map object
     ifstream infile(mapFile);
     if (!infile) // Did opening the file fail
     {
         cerr << "Error: Cannot open map data!" << endl;
         return false;
     }
     string line; // store the data of each line
     while (getline(infile, line)) // next line when next line is available
     {
         string name = line; //get street name
         getline(infile, line); // next line
         int num = 0;
         for (int i = 0; i < line.size(); i++)
         {
             num += (line[i] - '0')*(pow(10, line.size()-i-1)); // convert from string to ints
         }
         getline(infile, line); //next line
         for (int i = 0; i < num; i++)
         {
             string coord[4]; // four coordinates
             int index = 0;
             int cut = 0;
             for (int j = 0; j < line.size(); j++){
                 if (line[j] == ' ' || j+1 == line.size()) // coordinates are seperation by spaces
                 {
                     coord[index] = line.substr(j-cut, cut+1);
                     if (index != 3){
                         coord[index] = coord[index].substr(0, coord[index].length() - 1); // remove the tail space of first three coordinates
                     }
                     index++;
                     cut = 0;
                     j++;
                 }
                 cut++;
             }
             GeoCoord start = GeoCoord(coord[0], coord[1]);
             GeoCoord end = GeoCoord(coord[2], coord[3]);
             StreetSegment forward(start, end, name); // add segments
             StreetSegment reverse(end, start, name); // add reverse segments
             addItem(start, &forward);
             addItem(end, &reverse);
             
             if(i+1 != num)
                 getline(infile, line);
         }
     }
     return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    auto segVector = m_map->find(gc);

        if (segVector == nullptr)
        {
            return false;
        }
        else
        {
            for (auto it = segVector->begin(); it != segVector->end(); it++)
            {
                segs.push_back(*it);// push every coordinates into segs vector
            }
        }
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
