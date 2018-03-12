#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class NachenBlaster;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void instantiate(Actor* obj);
    bool projectileCollision(Actor* proj);
    double getPlayerX();
    double getPlayerY();
    void alienDestroyed();
    bool playerCollision(Actor* other);
    void repairPlayer();
    void damagePlayer(int d);
    void addTorpedos();
private:
    std::list<Actor*> m_objects;
    NachenBlaster* m_player;
    int m_levelNumber;
    int m_nEnemiesDestroyed;
    int m_nEnemiesOnScreen;
    bool collided(Actor* one, Actor* two);

    
};

#endif // STUDENTWORLD_H_
