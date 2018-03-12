#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

const int PROJECTILE_TAG = 0;
const int PICKUP_TAG = 1;
const int ALIEN_TAG = 2;
const int PLAYER_TAG = 3;
const int MISC_TAG = 4;


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

struct Vector2{ //used for setting alien's directions
    double m_x;
    double m_y;
};


class Actor: public GraphObject{
public:
    Actor(int imId, double xPos, double yPos,  int dir, double size, int depth, StudentWorld* world);
    virtual ~Actor();
    void die();
    bool isAlive();
    virtual void doSomething()=0;
    virtual int getTag() const=0;
    StudentWorld* getWorld();
private:
    bool m_isAlive;
    StudentWorld* m_world;
    
};

class Projectile: public Actor{
public:
    Projectile(int imId, double x, double y, int dir, bool f, int d, StudentWorld* w);
    virtual ~Projectile();
    virtual int getTag() const;
    virtual void doSomething();
    virtual void move()=0;
    bool isFriendly();
    int getDamage();
private:
    bool m_friendly;
    int m_damage;
};

///////////////////PROJECTILES///////////////////////////
//-------------------CABBAGE-----------------------
class Cabbage: public Projectile{
public:
    Cabbage(double x, double y, StudentWorld* w);
    virtual ~Cabbage();
    virtual void move();
};

//------------------TURNIP----------------------
class Turnip: public Projectile{
public:
    Turnip(double x, double y, StudentWorld* w);
    virtual ~Turnip();
    virtual void move();
};

//------------FLATULANCE TORPEDO--------------
class Torpedo: public Projectile{
public:
    Torpedo(double x, double y, int dir, bool friendly, StudentWorld* w);
    virtual ~Torpedo();
    virtual void move();
};


///////////////////////PICK-UPS/////////////////////////
class PickUp:public Actor{
public:
    PickUp(int imId, double x, double y, StudentWorld* w);
    virtual ~PickUp();
    virtual int getTag() const;
    virtual void doSomething();
    virtual bool collide();
    virtual void powerUp()=0;
};
//---------------------EXTRA-LIFE--------------------
class OneUp:public PickUp{
public:
    OneUp(double x, double y, StudentWorld* w);
    virtual ~OneUp();
    virtual void powerUp();
};

//-----------------REPAIR-GOODIE--------------------
class Repair:public PickUp{
public:
    Repair(double x, double y, StudentWorld* w);
    virtual ~Repair();
    virtual void powerUp();
};
//-----------------TORPEDO-GOODIE-------------------
class TorpedoGoodie:public PickUp{
public:
    TorpedoGoodie(double x, double y,StudentWorld* w);
    virtual ~TorpedoGoodie();
    virtual void powerUp();
};

/////////////////////NACHENBLASTER///////////////////
//NachenBlaster class (main player character)
class NachenBlaster: public Actor{
public:
    NachenBlaster(StudentWorld* w);
    virtual ~NachenBlaster();
    
    virtual void doSomething();
    int getHealth() const;
    int getCabbageCount() const;
    int torpedoCount() const;
    virtual int getTag() const;
    void damage(int damage);
    void repair(); //always repairs 10
    void giveTorpedos();

private:
    int m_health;
    int m_nCabbage;
    int m_nTorpedos;
};

/////////////////////ALIENS/////////////////////////
class Alien: public Actor{
public:
    Alien(int imId, double x, double y, double speed, float initHP, int dToPlayer ,int pts,StudentWorld* w);
    virtual ~Alien();
    virtual void doSomething();
    void setFlightLength(int fl);
    int getFlightLength();
    virtual int getTag() const;
    void damage(int d);
    float getHealth();
    float getTravelSpeed();
    void setTravelSpeed(float t);
    bool collideWithPlayer();
    void getDestroyed();
    //pure virtual functions
    virtual void drop()=0;
    virtual bool fire()=0;
    virtual void changeFlightPlan()=0;
protected:
    void setDir(double x, double y);
private:
    int m_flightLenght;
    float m_travelSpeed;
    float m_hitpoints;
    Vector2 m_dir;
    int m_damageToPlayer;
    int m_pointsToAwardPlayer;
    
};
//----------------SMALLGON--------------------
class Smallgon:public Alien{
public:
    Smallgon(double x, double y, StudentWorld* w);
    virtual ~Smallgon();
    virtual void drop();
    virtual bool fire();
    virtual void changeFlightPlan();
private:
    
};

//--------------SMOREGON------------------
class Smoregon:public Alien{
public:
    Smoregon(double x, double y, StudentWorld* w);
    virtual ~Smoregon();
    virtual void drop();
    virtual bool fire();
    virtual void changeFlightPlan();
};

//--------------SNAGGLEGON--------------------
class Snagglegon:public Alien{
public:
    Snagglegon(double x, double y, StudentWorld* w);
    virtual ~Snagglegon();
    virtual void drop();
    virtual bool fire();
    virtual void changeFlightPlan();
};

////////////////////////MISC_OBJECTS////////////////////
//Star class
class Star: public Actor{
public:
    Star(double x, double y, StudentWorld* w);
    virtual ~Star();
    virtual void doSomething();
    virtual int getTag() const;
};

//Explosion class
class Explosion: public Actor{
public:
    Explosion(double x, double y, StudentWorld* w);
    virtual ~Explosion();
    virtual void doSomething();
    virtual int getTag() const;

};




#endif // ACTOR_H_
