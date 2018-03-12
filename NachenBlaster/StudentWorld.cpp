#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <cmath>


using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    //create player character
    m_player = new NachenBlaster(this);
    //create stars
    for(int i=0; i<30; i++){
        m_objects.push_front(new Star(randInt(0, VIEW_WIDTH-1), randInt(0, VIEW_HEIGHT-1), this));
    }
    m_nEnemiesOnScreen = 0;
    m_nEnemiesDestroyed = 0;
    m_levelNumber = getLevel();
    
   
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    m_player->doSomething();
    
    list<Actor*>::iterator i = m_objects.begin();
    
    while(i!=m_objects.end()){
        (*i)->doSomething();
        i++;
    }
    
    //delete dead objects
    i=m_objects.begin();
    list<Actor*>::iterator toDestroy = i;
    while(i!=m_objects.end()){
        if(!(*i)->isAlive()){
            toDestroy =i;
            i--;
            if((*toDestroy)->getTag() == ALIEN_TAG){
                m_nEnemiesOnScreen--;
            }
            delete (*toDestroy);
            m_objects.erase(toDestroy);
        }
        i++;
    }
    
    //instantiate a new star, with a 1/15 chance
    if(randInt(0, 14) == 0){
        m_objects.push_front(new Star(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), this));
    }
    
    //determine whether to instantiate a new alien
    float remain = 6+(4*m_levelNumber) - m_nEnemiesDestroyed;
    float m = 4+(0.5*m_levelNumber);
    float mn = min(m, remain);
    
    if(m_nEnemiesOnScreen <mn){
        //which type of alien?
        int s1=60;
        int s2=20+m_levelNumber*5;
        int s3 = 5+m_levelNumber*10;
        int rand = randInt(0, s1+s2+s3);
        if(rand < 60){
            m_objects.push_front(new Smallgon(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), this));
            m_nEnemiesOnScreen++;
        }
        if(rand >=60 && rand < s1+s2){
            m_objects.push_front(new Smoregon(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), this));
            m_nEnemiesOnScreen++;
        }
        if(rand >= s1+s2){
            m_objects.push_front(new Snagglegon(VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), this));
            m_nEnemiesOnScreen++;
        }
        //m_nEnemiesOnScreen++;
        
    }
    
    ostringstream str;
    str.precision(0);
    str.setf(ios::fixed);
    
    str<<"Lives: "<<getLives()<<"  Health: "<<m_player->getHealth()*2<<"%  Score: "<<getScore()<<"  Level: "<<m_levelNumber<<"  Cabbages: "<<m_player->getCabbageCount()*3.333<<"%  Torpedos: "<<m_player->torpedoCount();
    setGameStatText(str.str());
    
    
    if(m_player->getHealth() <= 0){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    //check if the player has won the game
    if(m_nEnemiesDestroyed >= (6+(4*m_levelNumber)))
        return GWSTATUS_FINISHED_LEVEL;
    else
        return GWSTATUS_CONTINUE_GAME;
}

//check collisions between projectiles and ships, and deal appropriate damage
bool StudentWorld::projectileCollision(Actor* proj){
    if(!dynamic_cast<Projectile*>(proj) -> isFriendly()){ //unfriendly
        if(collided(proj, m_player)){
            m_player->damage(dynamic_cast<Projectile*>(proj)->getDamage());
            proj->die();
            return true;
        }
    }else{
        list<Actor*>::iterator i = m_objects.begin();
        while(i!= m_objects.end()){
            if((*i)->getTag() == ALIEN_TAG){
                if(collided(proj, (*i))){
                    dynamic_cast<Alien*>(*i)->damage(dynamic_cast<Projectile*>(proj)->getDamage());
                    if(dynamic_cast<Alien*>(*i)->getHealth() > 0)
                        playSound(SOUND_BLAST);
                    proj->die();
                    return true;
                }
            }
            i++;
        }
    }
    return false;
}

bool StudentWorld::playerCollision(Actor *other){
    return collided(other, m_player);
}
void StudentWorld::repairPlayer(){
    m_player->repair();
}
void StudentWorld::addTorpedos(){
    m_player->giveTorpedos();
}
void StudentWorld::damagePlayer(int d){
    m_player->damage(d);
}



//private helper function
bool StudentWorld::collided(Actor* one, Actor* two){
    double dist = sqrt(pow(one->getX() - two->getX(), 2) + pow(one->getY()-two->getY(), 2));
    return dist < (0.75* (one->getRadius() + two->getRadius()));
}




void StudentWorld::cleanUp()
{
    delete  m_player;
    m_player=nullptr; //make sure that when cleanUp is called twice,
    //player isn't attempted to be deleted twice
    list<Actor*>::iterator i;
    i=m_objects.begin();
    while(i!= m_objects.end()){
      delete (*i); //delete each item in the list of objects
        i++;
    }
    m_objects.clear();//clear the list of deleted pointers
}

void StudentWorld::instantiate(Actor* obj){
    if(obj != nullptr) //check, should never be an issue
        m_objects.push_front(obj);
}

double StudentWorld::getPlayerX(){
    return m_player->getX();
}
double StudentWorld::getPlayerY(){
    return m_player->getY();
}
void StudentWorld::alienDestroyed(){
    m_nEnemiesDestroyed++;
}





