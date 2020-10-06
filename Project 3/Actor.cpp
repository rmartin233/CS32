#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
#include <cstdlib>
const double Pi = 3.14159265;

/* Actor class*/
Actor::Actor(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth, double size) : GraphObject(imageID, startX, startY, dir, depth, size) //Constructor: take in several parameters and pass them to GraphObject (Student world pointer will be passed for local data member)
{
    m_alive = true;
    m_world = sw;
}

bool Actor::isAlive() const{
    return m_alive;
}

void Actor::setDead(){
    m_alive = false;
}

StudentWorld* Actor::getWorld() const{
    return m_world;
}

bool Actor::isProjectile() const{
    return false;
}

bool Actor::isCreature() const{
    return false;
}
bool Actor::isdamageable() const{
    return true;
}

bool Actor::isStatic() const{
    return false;
}

void Actor::receiveDamage(int damage){
    if(damage == 0){
        setDead();
        return;
    }
    setDead();
    return;
}

int Actor::damage() const{
    return 0;
}

/*Creature class*/
Creature::Creature (StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int health) : Actor(sw, imageID, startX, startY, dir, 0, 1), m_health(health) {}

bool Creature::isCreature() const{
    return true;
}

int Creature::getHealth() const{
    return m_health;
}
bool Creature::isBacteria() const{
    return false;
}

void Creature::setHealth(int health){
    m_health += health;
}

void Socrates::addPoint(){ // increase the score
    getWorld()->increaseScore(0);
}

void Creature::receiveDamage(int damage){ // bacteria or socrate receive damage
    m_health -= damage;
        
    if (m_health <= 0){
        setDead();
        soundDie();
        addPoint();
        if(isBacteria()){
            getWorld()->BacteriaPitCount(-1); // decrease bacteria count
            int chance = rand()%2; // random food
            if (chance == 0){
                Food* newfood = new Food(getX(), getY(), getWorld());
                getWorld()->addActor(newfood);
            }
        }
    }
    else{
        soundHurt();
    }
}

/* Socrates */
Socrates::Socrates(StudentWorld* sw, double startX, double startY, Direction dir, int health, int angle, int imageID) : Creature(sw, imageID, startX, startY, dir, health){
    m_spray = 20;
    m_flame = 5; // TODO: check
    m_angle = angle;
}

int Socrates::getAngle() const{
    return m_angle;
}

void Socrates::setAngle(int angle){ // set facing angle and move 5 degrees
    m_angle += angle;
    double x = VIEW_RADIUS+cos(getAngle()*Pi/180)*VIEW_RADIUS;
    double y = VIEW_RADIUS+sin(getAngle()*Pi/180)*VIEW_RADIUS;
    moveTo(x,y);
    setDirection((getAngle() + 180)%360);
}

void Socrates::doSomething()
{
    if (! isAlive()) return;
    
    int ch;
    if (getWorld()->getKey(ch)) // user hit a key during this tick!
    {
    switch (ch) {
        case KEY_PRESS_SPACE:
            if (getSpray() >= 1){ // spray
                getWorld()->addSpray();
                m_spray--;
            }
            break;
        case KEY_PRESS_ENTER:
            if (getFlame() >= 1){ // flame
                getWorld()->addFlame();
                m_flame--;
            }
            break;
        case KEY_PRESS_LEFT:
            setAngle(5);
    //... move Socrates counterclockwise
            break;
        case KEY_PRESS_RIGHT:
            setAngle(-5);
        //... move Socrates counterclockwise
            break;
//        default:
        }
    }
    else{
        if (m_spray < 20){ //otherwise, add spray
            m_spray++;
        }
    }
}

void Socrates::soundHurt() const{ // play sound of being hurt
    getWorld()->playSound(SOUND_PLAYER_HURT);
}

int Socrates::getFlame(){
    return m_flame;
}

int Socrates::getSpray(){
    return m_spray;
}

void Socrates::restoreHealth(){
    setHealth(100-getHealth());
}

void Socrates::addFlame(){
    m_flame += 5;
}

void Socrates::soundDie() const{
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

/* Bacteria */
Bacteria::Bacteria(int startX, int startY, StudentWorld* sw, int imageID, int damage, int health) : Creature(sw, imageID, startX, startY, 0, health){
    m_plan = 0;
    m_damage = damage;
    m_food = 0;
}

bool Bacteria::generalStep(){
    StudentWorld* w = getWorld();
    bool continueStep = true;
    
    if (findSocrates72()) // for aggressive salmonella, if Socrates could be found within 72 units, skip step5 and beyond
        continueStep = false;
    
    if (w->oversocrates(this, 2 * SPRITE_RADIUS)){
        w->socratesReceiveDamage(m_damage); // if overlap with socrate
    }
    else if (m_food == 3){
        double x = 0; double y = 0;
        splitCoordinates(this, x, y); // calculate the split count
        split(this, x, y); // new bacteria
        getWorld()->BacteriaPitCount(1);
        m_food = 0;
    }
    else if(getWorld()->overlap(this, 1)){ //overlap food
            m_food++;
    }
        return continueStep;
}

void Bacteria::doSomething(){
    if (! isAlive())
        return;
    if (generalStep())
        specialization();
    return;
}

bool Bacteria::findSocrates72(){
    return false;
}

bool Bacteria::isBacteria() const{
    return true;
}

void Bacteria::soundHurt() const{
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

void Bacteria::soundDie() const{
    getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

void Bacteria::addPoint(){
    getWorld()->increaseScore(100);
}

void Bacteria::specialization(){
    if (m_plan > 0){
        m_plan--;
        
        if (getWorld()->overlap(this, 2)){ // if blocked by dirt
            int theta = rand()%360;
            setDirection(theta);
            m_plan = 10;
        }
        else{
            moveAngle(getDirection(), 3); //move current direction
        }
        return;
    }
    else{
        double x = getX(); double y = getY();
        this->getWorld()->findFood(x, y); // find nearest food if possible
        
        if (x != -1 && y != -1){
        double deltaX = x - getX();
        double deltaY = y - getY();
        setDirection(calculateAngle(deltaX, deltaY));
        m_plan = 10; // m_plan
            
            if (getWorld()->overlap(this, 2)){ // find dirt or wall, set direction
                m_plan = 10;
                setDirection(rand()%360);
                return;
            }
            else{
                m_plan = 10;
                moveAngle(getDirection(), 3);
                return;
            }
        }
        else{
            m_plan = 10;
            setDirection(rand()%360);
            return;
        }
    }
}

void Bacteria::splitCoordinates(Bacteria* b, double& x, double& y){
    x = b->getX();
    y = b->getY();
    if (x < VIEW_WIDTH/2)
        x = x + SPRITE_RADIUS;
    else
        x = x - SPRITE_RADIUS;
    if (y < VIEW_HEIGHT/2)
        y = y + SPRITE_RADIUS;
    else
        y = y - SPRITE_RADIUS;
}

void Bacteria::setAngle(Bacteria *b){
    double deltaY = getWorld()->sY() - b->getY();
    double deltaX = getWorld()->sX() - b->getX();
    double angle = calculateAngle(deltaX, deltaY);
    setDirection(angle); // set the direction of salmonella face to Socrates
}
/* Salmonella */
RegularSalmonella::RegularSalmonella(int startX, int startY, StudentWorld* sw, int imageID, int damage, int health) : Bacteria(startX, startY, sw, imageID, damage, health) {}

AggressiveSalmonella::AggressiveSalmonella(int startX, int startY, StudentWorld* sw, int imageID, int damage, int health) : Bacteria(startX, startY, sw, imageID, damage, health) {}

Ecoli::Ecoli(int startX, int startY, StudentWorld* sw, int imageID, int damage, int health) : Bacteria(startX, startY, sw, imageID, damage, health) {}


void RegularSalmonella::split(Bacteria* b, int x, int y){
    RegularSalmonella* newB = new RegularSalmonella(x, y, b->getWorld());
    getWorld()->addActor(newB);
}

void AggressiveSalmonella::split(Bacteria* b, int x, int y){
    AggressiveSalmonella* newB = new AggressiveSalmonella(x, y, b->getWorld());
    getWorld()->addActor(newB);
}

void Ecoli::split(Bacteria* b, int x, int y){
    Ecoli* newB = new Ecoli(x, y, b->getWorld());
    getWorld()->addActor(newB);
}

bool AggressiveSalmonella::findSocrates72(){
    if(getWorld()->oversocrates(this, 72)) // whether Socrates can be found with 72 units
    {
        setAngle(this); // face Socrate
        if (getWorld()->overlap(this, 2)){ // if blocked by dirt or may move outside the boundary
            return true; // return
        }
        else{
            moveAngle(getDirection(), 3);
            return true;
        }
    }
    return false;
}

void Ecoli::specialization(){
    if(getWorld()->oversocrates(this, 256)) // whether Socrates can be found with 256 units
       {
           setAngle(this); // set angle to face socrates
           
           for (int i = 0; i < 10; i++){
               if(getWorld()->overlap(this, 3)){
                   setDirection((getDirection()+10)%360); // if over dirt, change direction
               }
               else if (getWorld()->overlap(this, 2)){ // if blocked by dirt or may move outside the boundary
                   return;
               }// return
               else{
                   moveAngle(getDirection(), 2); // move towards socrates 2 units
                   return;
               }
           }
       }
    return;
}

void Ecoli::soundHurt() const{ // specialization of sound die/hurt
    getWorld()->playSound(SOUND_ECOLI_HURT);
}

void Ecoli::soundDie() const{
    getWorld()->playSound(SOUND_ECOLI_DIE);
}

/* Dirt and Food */
StaticObject::StaticObject(int startX, int startY, StudentWorld* sw, int imageID, int angle) : Actor(sw, imageID, startX, startY, angle, 1,1) {}

bool StaticObject::isStatic() const{
    return true;
}

Dirt::Dirt(int startX, int startY, StudentWorld* sw, int imageID, int angle) : StaticObject(startX, startY, sw, imageID, angle) {}

Food::Food(int startX, int startY, StudentWorld* sw, int imageID, int angle) : StaticObject(startX, startY, sw, imageID, angle) {}

bool Food::isdamageable() const{
    return false;
}

void StaticObject::doSomething() {}

/* Flame and Spray */
Projectile::Projectile(int startX, int startY, int angle, int imageID, StudentWorld* sw, int distance): Actor(sw, imageID, startX, startY, angle, 1, 1) {
    m_angle = angle;
    m_distanceLeft = distance;
}

Flame::Flame(int startX, int startY, int angle, StudentWorld* sw, int distance, int imageID):Projectile(startX, startY, angle, imageID, sw, distance) {}

int Flame::damage() const{
    return 5;
}

Spray::Spray(int startX, int startY, int angle, StudentWorld* sw, int distance, int imageID):Projectile(startX, startY, angle, imageID, sw, distance) {}

int Spray::damage() const{
    return 2;
}

bool Projectile::isProjectile() const{
    return true;
}

bool Projectile::isdamageable() const{
    return false;
}

void Projectile::doSomething(){
    if (! isAlive()) return;
    getWorld()->overlap(this, 1);
    if (! isAlive()) return;
    moveAngle(m_angle, maxDistance());
    m_distanceLeft -= maxDistance();
    if (m_distanceLeft <= 0)
        setDead();
}

int Projectile::maxDistance(){
    return SPRITE_RADIUS*2;
}

/* Goodies */
Goodie:: Goodie (int startX, int startY, int life, StudentWorld* sw, int imageID) : Actor(sw, imageID, startX, startY, 0, 1, 1){
    m_lifetime = life;
}

FlameGoodie::FlameGoodie (int startX, int startY, int life, StudentWorld* sw, int imageID): Goodie(startX, startY, life, sw, imageID) {}

LifeGoodie::LifeGoodie (int startX, int startY, int life, StudentWorld* sw, int imageID) : Goodie(startX, startY, life, sw, imageID) {}

HealthGoodie::HealthGoodie (int startX, int startY, int life, StudentWorld* sw, int imageID) : Goodie(startX, startY, life, sw, imageID) {}

void Goodie::setLifetime(){
    m_lifetime--;
}

int Goodie::getLifetime(){
    return m_lifetime;
}

void Goodie::doSomething()
{
    if (! isAlive())
        return;
    if (getWorld()->oversocrates(this, 2 * SPRITE_RADIUS)) // if player overlap with the socrate
    {
        getWorld()->increaseScore(returnPoint()); // add/decrease score
        specialization(); // special ability
        this->setDead();
        return;
    }
    setLifetime();
    if (getLifetime() <= 0)
        this->setDead();
}

void HealthGoodie::specialization(){
    getWorld()->receiveGoodies(1);
}

void FlameGoodie::specialization(){
    getWorld()->receiveGoodies(2);
}

void LifeGoodie::specialization(){
    getWorld()->receiveGoodies(3);
}

int HealthGoodie::returnPoint() const {return 250;}
int FlameGoodie::returnPoint() const {return 300;}
int LifeGoodie::returnPoint() const {return 500;}

/* Fungus */
Fungus::Fungus (int startX, int startY, int life, StudentWorld* sw, int imageID) : Goodie(startX, startY, life, sw, imageID) {}

void Fungus::specialization(){
    getWorld()->socratesReceiveDamage(20);
}

int Fungus::returnPoint() const {return -50;}

/* Pit */
bool Pit::isdamageable() const{
    return false;
}

bool Pit::isEmpty() const {
    return m_empty;
}

void Pit::doSomething(){
    if (! isAlive())
        return;
    if (isEmpty()){ // if empty, remind the studentworld
        getWorld()->BacteriaPitCount(-1);
        setDead();
        return;
    }
    int chance = rand()%50;
    if (chance == 0)
    {
        for(;;) // try to generate a bacteria can be generated (still have some number left)
        {
            int bacteria = rand()%3;
            getWorld()->playSound(SOUND_BACTERIUM_BORN); // following is creation of three typies of bacteria
            if (bacteria == 0 && m_regular != 0)
            {
                RegularSalmonella* newB = new RegularSalmonella(this->getX(), this->getY(), this->getWorld());
                getWorld()->addActor(newB);
                m_regular--;
                break;
            }
            else if (bacteria == 1 && m_aggressive != 0)
            {
                AggressiveSalmonella* newB = new AggressiveSalmonella(this->getX(), this->getY(), this->getWorld());
                getWorld()->addActor(newB);
                m_aggressive--;
                break;
            }
            else if (bacteria == 2 && m_coli != 0)
            {
                Ecoli* newB = new Ecoli(this->getX(), this->getY(), this->getWorld());
                getWorld()->addActor(newB);
                m_coli--;
                break;
            }
        }
        getWorld()->BacteriaPitCount(1);
    }
    if(m_regular + m_aggressive + m_coli == 0)
        m_empty = true;
}

double calculateAngle(double deltaX, double deltaY){ // ==0
    
    if (deltaX >= 0 && deltaY == 0) return 0;
    if (deltaX ==0 && deltaY > 0) return 90;
    if (deltaX < 0 && deltaY == 0) return 180;
    if (deltaX ==0 && deltaY < 0) return 270;

    double angle = 180*(atan(deltaY/deltaX)/(Pi));
        
    if (deltaX <0 && deltaY > 0)
        return 180 +180*(atan(deltaY/deltaX)/(Pi));
    else if (deltaX < 0 && deltaY < 0)
        return 180*(atan(deltaY/deltaX)/(Pi)) - 180;
    else if (deltaX >0 && deltaY < 0)
        return 360 + angle;
    
    return angle;
}

Pit::Pit(int startX, int startY, StudentWorld* sw) : Actor(sw, IID_PIT, startX, startY, 0, 1, 1){
    m_regular = 5; //check
    m_aggressive = 3;
    m_coli = 2;
    m_empty = false;
}
