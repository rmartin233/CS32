#ifndef StreetMap_h
#define StreetMap_h

#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include "ExpandableHashMap.h"

class StreetMap {
public:
StreetMap();
~StreetMap();
bool load(std::string mapFile);
bool getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const;
    
private:
    
};

#endif /* StreetMap_h */
