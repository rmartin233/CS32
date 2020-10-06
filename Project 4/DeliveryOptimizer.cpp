#include "provided.h"
#include <vector>
#include <cmath>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_streetMap;
    struct Node{
        Node(DeliveryRequest d, bool v):m_d(d),m_visited(v){}
        DeliveryRequest m_d;
        bool m_visited;
    };
    
    double path_len(vector<DeliveryRequest> vec, const GeoCoord& depot) const
    {
        double path = 0; //path length counter
        path+= distanceEarthMiles(depot, vec[0].location);
        for(int i=0;i<vec.size()-1;i++) //1 size
        {
            path += distanceEarthMiles(vec[i].location, vec[i+1].location);
        }
        path+= distanceEarthMiles(vec[vec.size()-1].location, depot);
        return path; // return the current path length
    }
    
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_streetMap = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl() {}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    GeoCoord cur = depot;
    oldCrowDistance = 0;
    vector<Node*> nodes;
    for (int i = 0; i < deliveries.size(); i++) {
        oldCrowDistance+=distanceEarthMiles(cur, deliveries[i].location);
        nodes.push_back(new Node(deliveries[i], false));
        cur = deliveries[i].location;
    }
    oldCrowDistance += distanceEarthMiles(depot, cur);// original route plan length
    oldCrowDistance = path_len(deliveries,depot);
    int N = deliveries.size();
    double T = N*2000; // initial temp
    double T_end = 1e-8; // end temp
    double q = 0.98; // temperature decrease rate
    
    
    int L = pow(N, 2); // length per iteration
    vector<DeliveryRequest> v = deliveries;
    vector<DeliveryRequest> vTemp;
    double f1,f2,df;
    
    double r;// random number for probability
    while(T > T_end){ // while the minimum temperature is not met simulated annealing
        for(int i=0; i<L; i++)// chain size
        {
            vTemp = v;
            int pos1 = rand() % deliveries.size();
            int pos2 = rand() % deliveries.size();
            DeliveryRequest temp = v[pos1];
            v[pos1] = v[pos2];
            v[pos2] = temp; // exchange
            f1 = path_len(vTemp, depot);
            f2 = path_len(v, depot);
            df = f2 - f1; // calculate the cost
            
            if(df >= 0)
            {
                r = ((double)rand())/(RAND_MAX);
                if(exp(-df/T) <= r) // use statistics for probability
                {
                    v = vTemp;
                }
            }
        }
        T *= q; // decrease the temperature
    }
    for (auto it : nodes){
        delete it;
    }
    deliveries = v; // push the optimal route into deliveries
    newCrowDistance = path_len(v, depot); // route overhead length
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
