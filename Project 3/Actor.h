#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "GameConstants.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;



class Actor: public GraphObject{
public:
    Actor(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth, double size);
    virtual void doSomething() = 0;
    bool isAlive() const;
    void setDead();
    StudentWorld* getWorld() const;
    virtual void receiveDamage(int damage);
    virtual bool isProjectile() const; //Projectile identifier
    virtual bool isdamageable() const; //Damagable objects (bacteria, goodie, dirt, etc.) identifier
    virtual bool isStatic() const; // food and dirt identifier
    virtual bool isCreature() const; // bacteria sna socrates identifier
    virtual int damage()const; // return damage can be made
    virtual ~Actor(){}
private:
    bool m_alive;
    StudentWorld* m_world;
};

class Creature : public Actor{
public:
    Creature(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int health);
    virtual void doSomething() = 0;
    int getHealth() const;
    void setHealth(int health);
    virtual void receiveDamage(int damage);
    virtual bool isCreature() const;
    virtual bool isBacteria() const;
    virtual void soundHurt()  const = 0;
    virtual void soundDie() const = 0;
    virtual void addPoint() = 0;
    virtual ~Creature(){}
private:
    int m_health;
};

class Socrates : public Creature{
public:
    Socrates(StudentWorld* sw, double startX = 0, double startY = 128, Direction dir = 0, int health = 100, int angle = 180, int imageID = IID_PLAYER);
    virtual void doSomething();
    int getSpray();
    int getFlame();
    void restoreHealth();
    void addFlame();
    int getAngle() const;
    void setAngle(int angle);
    virtual void soundHurt() const; // play sound of being hurt
    virtual void soundDie() const;
    virtual void addPoint();
    double sX() const;
    double sY() const;
    virtual ~Socrates(){}
private:
    int m_spray;
    int m_flame;
    int m_angle;
};

class Bacteria : public Creature{
public:
    Bacteria(int startX, int startY, StudentWorld* sw, int imageID, int damage, int health);
    virtual void doSomething();
    virtual bool isBacteria() const;
    virtual void split(Bacteria* b, int x, int y) = 0;
    virtual void soundHurt() const;
    virtual void soundDie() const;
    virtual void addPoint();
    void splitCoordinates(Bacteria* b, double& x, double& y);
    void setAngle(Bacteria* b);
    virtual ~Bacteria(){}
private:
    bool generalStep();
    virtual void specialization();
    virtual bool findSocrates72();
    double m_plan;
    int m_damage;
    int m_food;
};

class RegularSalmonella : public Bacteria{
public:
    RegularSalmonella(int startX, int startY, StudentWorld* sw, int imageID = IID_SALMONELLA, int damage = 1, int health = 4);
    virtual void split(Bacteria* b, int x, int y);
    virtual ~RegularSalmonella(){}
};

class AggressiveSalmonella : public Bacteria{
public:
    AggressiveSalmonella(int startX, int startY, StudentWorld* sw, int imageID = IID_SALMONELLA, int damage = 2, int health = 10);
    virtual void split(Bacteria* b, int x, int y);
    virtual ~AggressiveSalmonella(){}
private:
    virtual bool findSocrates72();
};

class Ecoli : public Bacteria{
public:
    Ecoli(int startX, int startY, StudentWorld* sw, int imageID = IID_ECOLI, int damage = 4, int health = 5);
    virtual void split(Bacteria* b, int x, int y);
    virtual void soundHurt() const;
    virtual void soundDie() const;
    virtual ~Ecoli(){}
private:
    virtual void specialization();
};

class StaticObject : public Actor{
public:
    StaticObject(int startX, int startY, StudentWorld* sw, int imageID, int angle);
    virtual void doSomething();
    virtual bool isStatic() const;
    virtual ~StaticObject(){}
};

class Dirt : public StaticObject{
public:
    Dirt(int startX, int startY, StudentWorld* sw, int imageID = IID_DIRT, int angle = 0);
    virtual ~Dirt(){}
};

class Food : public StaticObject{
public:
    Food(int startX, int startY, StudentWorld* sw, int imageID = IID_FOOD, int angle = 90);
    virtual bool isdamageable() const;
    virtual ~Food(){}
};

class Projectile : public Actor{
public:
    Projectile (int startX, int startY, int angle, int imageID, StudentWorld* sw, int distance);
    virtual void doSomething();
    virtual bool isProjectile() const;
    virtual bool isdamageable() const;
    virtual int damage() const = 0;
    virtual ~Projectile(){}
private:
    int m_distanceLeft;
    int m_angle;
    virtual int maxDistance();
};

class Flame : public Projectile{
public:
    Flame (int startX, int startY, int angle, StudentWorld* sw, int distance = 32, int imageID = IID_FLAME);
    virtual int damage() const;
    virtual ~Flame(){}
private:
};

class Spray : public Projectile{
public:
    Spray (int startX, int startY, int angle, StudentWorld* sw, int distance = 112, int imageID = IID_SPRAY);
    virtual int damage() const;
    virtual ~Spray(){}
private:
};

class Goodie :  public Actor{
public:
    virtual void doSomething();
    Goodie (int startX, int startY, int life, StudentWorld* sw, int imageID);
    virtual int returnPoint() const = 0;
    virtual void specialization() = 0;
    virtual ~Goodie(){}
private:
    int m_lifetime;
    int getLifetime();
    void setLifetime();
};

class HealthGoodie : public Goodie{
public:
    HealthGoodie (int startX, int startY, int life, StudentWorld* sw, int imageID = IID_RESTORE_HEALTH_GOODIE);
    virtual int returnPoint() const;
    virtual ~HealthGoodie(){}
private:
    virtual void specialization();
};

class FlameGoodie : public Goodie{
public:
    FlameGoodie (int startX, int startY, int life, StudentWorld* sw, int imageID = IID_FLAME_THROWER_GOODIE);
    virtual int returnPoint() const;
    virtual ~FlameGoodie(){}
private:
    virtual void specialization();
};

class LifeGoodie : public Goodie{
public:
    LifeGoodie (int startX, int startY, int life, StudentWorld* sw, int imageID = IID_EXTRA_LIFE_GOODIE);
    virtual int returnPoint() const;
    virtual ~LifeGoodie(){}
private:
    virtual void specialization();
};

class Fungus : public Goodie{
public:
    Fungus (int startX, int startY, int life, StudentWorld* sw, int imageID = IID_FUNGUS);
    virtual int returnPoint() const;
    virtual ~Fungus(){}
private:
    virtual void specialization();
};

class Pit : public Actor{
public:
    Pit (int startX, int startY, StudentWorld* sw);
    virtual void doSomething();
    virtual bool isdamageable() const;
    virtual ~Pit(){}
private:
    int m_regular;
    int m_aggressive;
    int m_coli;
    bool m_empty;
    bool isEmpty() const;
};

double calculateAngle(double deltaX, double deltaY);
#endif // ACTOR_H_
