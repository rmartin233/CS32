#include "provided.h"
#include <vector>
#include <functional>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* sm;
    DeliveryOptimizer* opt;
    PointToPointRouter* router;
    string Getangle(double angle) const{
        if(angle < 22.5) return "east";
        else if (angle < 67.5) return "northeast";
        else if (angle < 112.5) return "north";
        else if (angle < 157.5) return "northwest";
        else if (angle < 202.5) return "west";
        else if (angle < 247.5) return "southwest";
        else if (angle < 292.5) return "south";
        else if (angle < 337.5) return "southeast";
        else return "east";    }
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    this->sm = sm;
    opt = new DeliveryOptimizer(sm);
    router = new PointToPointRouter(sm);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
    delete opt;
    delete router;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    
    double oldDis = 0;
    double newDis = 0;
    double total = 0;
    double tempDis = 0;
    vector<DeliveryRequest> deli = deliveries;
    opt->optimizeDeliveryOrder(depot, deli, oldDis, newDis); //get the optimized delivery plan
    list<StreetSegment> route;
    GeoCoord current = depot;
    commands.clear();
    for (int i = 0; i <= deli.size(); i++){
        DeliveryResult temp;
        if(i==deli.size()){
            temp = router->generatePointToPointRoute(current, depot, route, tempDis);// last one is for back to original position
        }
        else{
            GeoCoord currentDest = deli[i].location;
            temp = router->generatePointToPointRoute(current, currentDest, route, tempDis);
            }
        
        if (temp != DELIVERY_SUCCESS)// check bad coordinates
                    return temp;
                
                total += tempDis;
                auto it = route.begin();
                while(it!=route.end())// begining planning the road
                {
                    string name = it->name;
                    StreetSegment start = *it;
                    DeliveryCommand current;
                    DeliveryCommand target;
                    current.initAsProceedCommand(Getangle(angleOfLine(*it)), it->name, 0);// initialize a route
                    bool delivery = false;
                    for (;it!=route.end() && it->name == name && ! delivery;it++) { //while still on this
                        current.increaseDistance(distanceEarthMiles(it->start, it->end));
                        if(i != deli.size() && deli[i].location == it->end){
                            target.initAsDeliverCommand(deli[i].item);
                            delivery = true;
                        }
                    }
                    commands.push_back(current);//push the route
                    if (delivery){ // if delivered, no turns
                        commands.push_back(target);//push the delivery
                        continue;
                    }
                
                    if(it != route.end()){
                    auto itPrev = it;
                    double turn = angleBetween2Lines(*it, *(--itPrev));//update the turn command
                    if(turn <=359 && turn >= 1){
                        string turnLorR = "left";
                        if(turn <=180)
                            turnLorR = "right";
                        current.initAsTurnCommand(turnLorR, it->name);
                        commands.push_back(current);
                    }
                }
            }
            if(i != deli.size())//goto the next delivery point
                current = deli[i].location;
        }
        totalDistanceTravelled = total;
        return DELIVERY_SUCCESS;
    }

        //******************** DeliveryPlanner functions ******************************

        // These functions simply delegate to DeliveryPlannerImpl's functions.
        // You probably don't want to change any of this code.

        DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
        {
            m_impl = new DeliveryPlannerImpl(sm);
        }

        DeliveryPlanner::~DeliveryPlanner()
        {
            delete m_impl;
        }

        DeliveryResult DeliveryPlanner::generateDeliveryPlan(
            const GeoCoord& depot,
            const vector<DeliveryRequest>& deliveries,
            vector<DeliveryCommand>& commands,
            double& totalDistanceTravelled) const
        {
            return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
        }
