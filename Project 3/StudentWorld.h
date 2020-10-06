#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
#include "Actor.h"

class Actor;
class Socrates;
class Dirt;
class Bacteria;
class Projectile;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual ~StudentWorld();
    
    
    bool overlap(double x, double y);
    bool oversocrates(Actor* a, int distance) const;
    bool overlap(Actor* b, int index);
    void findFood(double &x, double &y);
    void receiveGoodies(int index);
    void socratesReceiveDamage (int damage);
    
    double sX() const;
    double sY() const;
    void addActor(Actor* a);
    void BacteriaPitCount(int i);
    void addFlame();
    void addSpray();
    
private:
    Socrates* m_s;
    std::list<Actor*> m_a;
    int m_BacteriaPitCount;
    std::string combine() const;
    void addGoodies();
    int isEmpty() const;
    void deleteActors();
};

double calculateDistance(double x1, double y1, double x2, double y2);
void randomCoord(double& x, double& y);

#endif // STUDENTWORLD_H_
