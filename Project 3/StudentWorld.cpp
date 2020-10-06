#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>

const double Pi = 3.14159265;
using namespace std;

GameWorld* createStudentWorld(string assetPath){
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath) {m_BacteriaPitCount = 0;}

void StudentWorld::BacteriaPitCount(int i){
    m_BacteriaPitCount+=i;
}

int StudentWorld::isEmpty() const{
    return m_BacteriaPitCount;
}

int StudentWorld::init() //initialize the student world
{
    int L = getLevel();
    m_s = new Socrates(this);
    double x, y;
    
    for (int i = 0; i < L; i++){
        randomCoord(x, y); // random put pit
        if (! overlap(x, y)){
            Pit* newpit = new Pit(x, y, this);
            m_a.push_back(newpit);
            BacteriaPitCount(1);
        }
        else{
            i--;
        }
    }
    
    int maxfood = min(5*L, 25);
    for (int i = 0; i < maxfood; i++){ // random food position
        randomCoord(x, y);
        if (! overlap(x, y)){
            Food* newfood = new Food(x, y, this);
            m_a.push_back(newfood);
        }
        else{
            i--;
        }
    }

    int dirtNum = max(180 - 20 * L, 20);
    for (int i = 0; i < dirtNum; i++) // random dirt position
    {
        randomCoord(x, y);
        if (! overlap(x, y)){
            Dirt* m_d = new Dirt(x, y, this);
            m_a.push_back(m_d);
        }
        else{
            i--;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*>::iterator ior = m_a.begin();
    while (ior != m_a.end())
    {
        (*ior)->doSomething();
        if (! m_s->isAlive()){
            decLives();
            m_BacteriaPitCount = 0;
            return GWSTATUS_PLAYER_DIED;
        }
            ior++;
        
        if (m_BacteriaPitCount == 0){
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    if (m_s->isAlive())
        m_s->doSomething();
    // move Socrates
    deleteActors(); // delete dead actor
    addGoodies(); // add goodies
    setGameStatText(combine()); // game stat
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp(){
    if (m_a.size() != 0){ // erase all in the list
        list<Actor*>::iterator ior = m_a.begin();
        while (ior != m_a.end()){
            delete *ior;
            ior = m_a.erase(ior);
        }
    }
    delete m_s; // erase player
    m_s = nullptr;
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

void StudentWorld::addFlame(){ // add 16 flames counterclockwise
    for (int i = 0; i < 16; i++){
        int angle = m_s->getDirection() + i*22;
        int x = m_s->getX() + SPRITE_WIDTH*cos(angle*Pi/180);
        int y = m_s->getY() + SPRITE_WIDTH*sin(angle*Pi/180);
    Flame* newFlame = new Flame(x, y, angle, this);
    m_a.push_back(newFlame);
    }
    playSound(SOUND_PLAYER_FIRE); // order
}

void StudentWorld::addSpray()
{
    int angle = m_s->getDirection();
    Spray* newSpray = new Spray(m_s->getX() + SPRITE_WIDTH*cos(angle*Pi/180), m_s->getY() + SPRITE_WIDTH*sin(angle*Pi/180), angle, this);
    m_a.push_back(newSpray);
    playSound(SOUND_PLAYER_SPRAY);
}

bool StudentWorld::overlap(Actor *b, int index){
    list<Actor*>::iterator ior = m_a.begin();
    int temp = 0;
    if (index == 2 || index ==3) temp = 3;
    if (index == 4) temp = 2;
    
    double x = b->getX() + temp*cos(Pi*b->getDirection()/180);
    double y = b->getY() + temp*sin(Pi*b->getDirection()/180);
    while (ior != m_a.end())
    {
        double iX = (*ior)->getX();
        double iY = (*ior)->getY();
        switch (index){
            case 1:
                if (calculateDistance(x, y, iX, iY) <= SPRITE_WIDTH){ //other overlap
                    if (b->isCreature() && (*ior)->isStatic() && (! (*ior)->isdamageable())){
                        (*ior)->setDead();
                        return true;}
                    else if (b->isProjectile()){ //projectile overlap
                        if((*ior)->isdamageable() && (*ior)->isAlive()){
                        (*ior)->receiveDamage(b->damage());
                        b->setDead();
                        return true;
                        }
                    }
                }
                break;
            case 2:
                    if ( (*ior)->isStatic() && (*ior)->isdamageable() && (calculateDistance(x, y, VIEW_WIDTH/2,VIEW_HEIGHT/2) >= VIEW_RADIUS || calculateDistance(x, y, iX, iY) <= SPRITE_WIDTH/2)){ // dirt and wall overlap
                        return true;
                    }
                break;
            case 3:
                if ((*ior)->isStatic() && (*ior)->isdamageable() && calculateDistance(x, y, iX, iY) <= SPRITE_WIDTH/2){ // only for dirt overlap
                    return true;
                }
                break;
        }
        ior++;
    }
    return false;
}

void StudentWorld::findFood(double &x, double &y)
{
    int smallX = -1;
    int smallY = -1;
    int smallDistance = 128;
    list<Actor*>::iterator ior = m_a.begin(); // ieration for nearest food
    while (ior != m_a.end())
    {
        if (((*ior)->isStatic() && ! (*ior)->isdamageable()) && calculateDistance(x, y, (*ior)->getX(), (*ior)->getY()) <= smallDistance)
        {
            smallX = (*ior)->getX();
            smallY = (*ior)->getY();
            smallDistance = calculateDistance(x, y, smallX, smallY);
        }
        ior++;
    }
    x = smallX; y = smallY;
}

bool StudentWorld::overlap(double x, double y){
    list<Actor*>::iterator ior = m_a.begin();
    
    while (ior != m_a.end()){
        if (! (*ior)->isdamageable() && calculateDistance(x, y, (*ior)->getX(), (*ior)->getY()) <= SPRITE_WIDTH){
            return true;
        }
        ior++;
    }
    return false;
}

bool StudentWorld::oversocrates(Actor* a, int distance) const{ // overlap with socrate whthin given distance
    if (calculateDistance(a->getX(), a->getY(), m_s->getX(), m_s->getY()) <= distance)
        return true;
    return false;
}

void StudentWorld::addGoodies(){
    int chanceFungus = max(510-getLevel()*10, 200);
    int generateFungus = rand()%chanceFungus;
    int chanceGoodie = max(510-getLevel()*10, 250);
    int generateGoodie = rand()%chanceGoodie;
    int goodieType = rand()% 10;
    if (generateFungus == 0){
        int angle = Pi * (rand()%360)/180;
        int life = max(rand()%(300 -10*getLevel()), 50);
        Fungus* newFungus = new Fungus(VIEW_RADIUS*cos(angle)+VIEW_WIDTH/2, VIEW_RADIUS*sin(angle)+VIEW_HEIGHT/2, life, this);
        m_a.push_back(newFungus);
    }
    if (generateGoodie == 0){ // randomly generate a type of goodie
        int angle = Pi * (rand()%360)/180;
        int life = max(rand()%(300 -10*getLevel()), 50);
        if (goodieType >= 0 && goodieType <= 5){
            HealthGoodie* newgoodie = new  HealthGoodie(VIEW_RADIUS*cos(angle)+VIEW_WIDTH/2, VIEW_RADIUS*sin(angle)+VIEW_HEIGHT/2, life, this);
            m_a.push_back(newgoodie);
        }
        else if (goodieType <= 8){
            FlameGoodie* newgoodie = new  FlameGoodie(VIEW_RADIUS*cos(angle)+VIEW_WIDTH/2, VIEW_RADIUS*sin(angle)+VIEW_HEIGHT/2, life, this);
            m_a.push_back(newgoodie);
        }
        else{
            LifeGoodie* newgoodie = new  LifeGoodie(VIEW_RADIUS*cos(angle)+VIEW_WIDTH/2, VIEW_RADIUS*sin(angle)+VIEW_HEIGHT/2, life, this);
            m_a.push_back(newgoodie);
        }
    }
}

void StudentWorld::addActor(Actor* a){
    this->m_a.push_back(a);
}

string StudentWorld::combine()  const // display string combination
{
    ostringstream oss;
    oss.fill('0');
    if(getScore() < 0){
        oss << "Score: -";
        oss << setw(5);
    }
    else{
        oss << "Score: ";
        oss << setw(6);
    }
    oss << to_string(abs(getScore())); // minus score
    oss << "  Level:";
    oss.fill(' ');
    oss << setw(3);
    oss << to_string(getLevel()) << "  Lives: " << to_string(getLives()) << "  Health:";
    oss << setw(4);
    oss << to_string(m_s->getHealth()) << "  Sprays: " << to_string(m_s->getSpray()) << "  Flames:";
    oss << setw(3);
    oss << to_string(m_s->getFlame());
    return oss.str();
}

double StudentWorld::sX() const{
    return m_s->getX();
}

double StudentWorld::sY() const{
    return m_s->getY();
}

void StudentWorld::receiveGoodies(int index){ // three types of goodies
    playSound(SOUND_GOT_GOODIE);
    switch (index){
        case 1:
            m_s->restoreHealth();
            break;
        case 2:
            m_s->addFlame();
            break;
        case 3:
            incLives();
            break;   
    }
}

void StudentWorld::socratesReceiveDamage (int damage){
    m_s->receiveDamage(damage);
}

void StudentWorld::deleteActors(){
    list<Actor*>::iterator itor = m_a.begin();
    while(itor != m_a.end()){
        if(! (*itor)->isAlive()){
            delete *itor;
            itor = m_a.erase(itor);
        }else{
            itor++;
        }
    }
}
double calculateDistance(double x1, double y1, double x2, double y2)
{
    double x0 = x1 - x2;
    double y0 = y1 - y2;
    double distance;
    distance = pow(x0, 2) + pow(y0, 2); //Euclid distance
    distance = sqrt(distance);
    return distance;
}

void randomCoord(double& x, double& y){
    int temp = randInt(0, 120) + randInt(0, 120); // random coordinates from polar coordinates
    int r = 0;
    if (temp >= 120)
        r = 2*120-temp;
    else
        r = temp;
    int theta = randInt(0, 360);
    x = VIEW_WIDTH/2 + r*cos(theta*Pi/180);
    y = VIEW_WIDTH/2 + r*sin(theta*Pi/180);
}

