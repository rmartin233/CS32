#include "provided.h"
#include <list>
#include <map>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* sm;
    struct Node {
        Node* parent;
        GeoCoord location;
        double f;
        double g;
        double h;
        
        Node(GeoCoord coord, GeoCoord end, Node* current = nullptr, Node* par = nullptr){
            parent = par;
            location = coord;
            if (current != nullptr)
                g = current->g + distanceEarthMiles(current->location, location); // g calculation
            else
                g = 0.0;
            h = distanceEarthMiles(location, end); // h is distance from this point to end
            f = g + h;
        }
        
        bool operator<(const Node& rhs){
            return f < rhs.f;
        }
    };
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    this->sm = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    route.clear();// clear the rpute plan
    map<GeoCoord, Node*> openList;
    map<GeoCoord, Node*> closedList;
    vector<StreetSegment> initialSeg; // if either the end or the start is a invalid position, return bad_coordinate
    vector<StreetSegment> engSeg;
    if (! sm->getSegmentsThatStartWith(start, initialSeg) || ! sm->getSegmentsThatStartWith(end, initialSeg))
        return BAD_COORD;
            
    if(start == end){
        totalDistanceTravelled = 0.0;
        return DELIVERY_SUCCESS;
    }
    bool found = false;
    
    Node* startNode = new Node(start, end);
    openList[start] = startNode;
    Node* current;
    while (!openList.empty() && ! found) // while there are still segments to find
    {
        current = openList.begin()->second; //iterate beginning from the Openlist
        for (auto it = openList.begin(); it!=openList.end(); it++) {
            if(it->second->f < current->f){
                current = it->second; //highest priority (lowest f)
            }
        }
        if(current->location == end)
        {
            found = true; // if the route is found
            break;
        }
        openList.erase(openList.find(current->location));//remove from the openlist
        vector<StreetSegment> currentSegment;
        sm->getSegmentsThatStartWith(current->location, currentSegment);
        
        for (int i = 0; i < currentSegment.size(); i++) // A star algorithm
        {
            GeoCoord successor = currentSegment[i].end;
            double successor_current_cost = current->g + distanceEarthMiles(current->location, successor); // successor cost is g(current) + w(current, successor)

            if (openList.find(successor) != openList.end())//if in open list
            {
                Node* successorNode = openList[successor];
                if (successorNode->g <= successor_current_cost)
                    continue;// valid route with lower cost
            }
            else if (closedList.find(successor) != closedList.end())// if in closed list
            {
                Node* successorNode = closedList[successor];
                if (successorNode->g <= successor_current_cost)
                    continue;// valid route with lower cost
                openList[successor] = successorNode;//move from closelist to openlist
                auto it = closedList.find(successor);
                closedList.erase(it);//remove from closelist
            }
            else// neither,
            {
                Node* successorNode = new Node(successor, end, current);
                openList[successor] = successorNode;
            }
            openList[successor]->parent = current; // valid route, add to the link
            openList[successor]->g = successor_current_cost;
            openList[successor]->f = successor_current_cost+openList[successor]->h;
        }
        closedList[current->location] = current;
    }
    
   if(found == true){ // go back
        totalDistanceTravelled = 0;// total distance
        while(current->parent != nullptr){
            vector<StreetSegment> s;
            sm->getSegmentsThatStartWith(current->parent->location, s);
                for(auto i : s){
                    if(i.end == current->location){
                        route.push_front(i);
                        totalDistanceTravelled += distanceEarthMiles(i.start, i.end);
                        break;
                    }
                }
            current = current->parent;
        }
            for(auto it = openList.begin(); it!=openList.end(); it++){// clean up
                delete it->second;
            }
            for(auto it = closedList.begin(); it!=closedList.end(); it++){
                delete it->second;
            }
        return DELIVERY_SUCCESS;
    }
    totalDistanceTravelled = 0;
    for(auto it = openList.begin(); it!=openList.end(); it++){// clean up
        delete it->second;
    }
    for(auto it = closedList.begin(); it!=closedList.end(); it++){
        delete it->second;
    }
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
