#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imId, double xPos, double yPos,  int dir, double size, int depth, StudentWorld* world)
    :GraphObject(imId, xPos, yPos, dir, size, depth){
    m_world = world;
    m_isAlive = true;
}
Actor::~Actor(){
}

void Actor::die(){
    m_isAlive = false;
}
bool Actor::isAlive(){
    return m_isAlive;
}
StudentWorld* Actor::getWorld(){
    return m_world;
}

///////////////////PROJECTILES///////////////////
Projectile::Projectile(int imId, double x, double y, int dir,bool f,int d,StudentWorld* w):Actor(imId, x,y,dir,.5,1,w){
    //note, all projectiles have depth of 1, size of 0.5
    m_friendly = f;
    m_damage = d;
}

Projectile::~Projectile(){}
int Projectile::getTag() const{
    return PROJECTILE_TAG;
}
bool Projectile::isFriendly(){
    return m_friendly;
}
void Projectile::doSomething(){
    if(!isAlive())
        return;
    if(getX() >= VIEW_WIDTH){
        die();
        return;
    }
    if(getWorld()->projectileCollision(this))
        return;
    move(); //call specific move function for each type of projectile
    getWorld()->projectileCollision(this); //check collision after movement
}
int Projectile::getDamage(){
    return m_damage;
}

//------------CABBAGE-------------
Cabbage::Cabbage(double x, double y,StudentWorld* w):Projectile(IID_CABBAGE, x,y,0,true,2,w){
}
Cabbage::~Cabbage(){}
void Cabbage::move(){
    moveTo(getX()+8, getY()); //move 8 pixels to the right
    setDirection(getDirection()+20); //rotate 20 degrees to the right
}

//-------------TURNIP---------------
Turnip::Turnip(double x, double y, StudentWorld* w):Projectile(IID_TURNIP, x,y,0,false,2,w){
}
Turnip::~Turnip(){}
void Turnip::move(){
    moveTo(getX()-6, getY());
    setDirection(getDirection()+20);
}


//---------FLATULANCE TOPEDO--------
Torpedo::Torpedo(double x, double y, int dir, bool friendly, StudentWorld* w):Projectile(IID_TORPEDO, x,y, dir,friendly,8,w){
}
Torpedo::~Torpedo(){}
void Torpedo::move(){
    if(isFriendly())
        moveTo(getX() + 8, getY());
    else
        moveTo(getX()-8, getY());
}


/////////////////////PICK-UPS////////////////////////
PickUp::PickUp(int imId, double x, double y, StudentWorld* w):Actor(imId, x,y,0,0.5,1,w){
    
}
PickUp::~PickUp(){}
int PickUp::getTag() const{
    return PICKUP_TAG;
}
void PickUp::doSomething(){
    if(!isAlive())
        return;
    if(getX()<0 || getY()<0){
        die();
        return;
    }
    if(collide()){
        return;
    }
    
    moveTo(getX()-0.75, getY()-0.75);
    collide();
}
bool PickUp::collide(){
    if(getWorld()->playerCollision(this)){
        die();
        getWorld()->increaseScore(100);
        getWorld()->playSound(SOUND_GOODIE);
        powerUp();
        return true;
    }
    return false;
}

//----------------ONE-UP------------------
OneUp::OneUp(double x, double y, StudentWorld* w):PickUp(IID_LIFE_GOODIE, x,y,w){
}
OneUp::~OneUp(){}
void OneUp::powerUp(){
    getWorld()->incLives();
}

//-----------------REPAIR-GOODIE------------
Repair::Repair(double x, double y, StudentWorld* w):PickUp(IID_REPAIR_GOODIE, x,y,w){
}
Repair::~Repair(){}
void Repair::powerUp(){
    getWorld()->repairPlayer();
}
//----------------TORPEDO-GOODIE-----------------
TorpedoGoodie::TorpedoGoodie(double x, double y, StudentWorld* w):PickUp(IID_TORPEDO_GOODIE, x,y,w){
}
TorpedoGoodie::~TorpedoGoodie(){}

void TorpedoGoodie::powerUp(){
    getWorld()->addTorpedos();
}

//////////////////////////ALIENS/////////////////////////////
Alien::Alien(int imId, double x, double y,double speed, float initHP, int dToPlayer,int pts,StudentWorld* w):Actor(imId, x,y,0,1.5,1, w){
    m_travelSpeed = speed;
    m_hitpoints = initHP;
    m_damageToPlayer = dToPlayer;
    m_pointsToAwardPlayer = pts;
}
Alien::~Alien(){}

void Alien::setFlightLength(int fl){
    m_flightLenght = fl;
}
int Alien::getFlightLength(){
    return m_flightLenght;
}
void Alien::setDir(double x, double y){
    m_dir.m_x = x;
    m_dir.m_y = y;
}
int Alien::getTag() const{
    return ALIEN_TAG;
}
void Alien::damage(int d){
    m_hitpoints-=d;
}
float Alien::getHealth(){
    return m_hitpoints;
}
float Alien::getTravelSpeed(){
    return m_travelSpeed;
}
void Alien::setTravelSpeed(float t){
    m_travelSpeed = t;
}

bool Alien::collideWithPlayer(){
    if(getWorld()->playerCollision(this)){
        getWorld()->damagePlayer(m_damageToPlayer);
        getDestroyed();
        return true;
    }
    return false;
}
void Alien::getDestroyed(){
    getWorld()->instantiate(new Explosion(this->getX(), this->getY(), getWorld()));
    getWorld()->increaseScore(m_pointsToAwardPlayer);
    getWorld()->playSound(SOUND_DEATH);
    getWorld()->alienDestroyed(); //increase the count of aliens destroyed
    drop();
    die();
}

void Alien::doSomething(){
    if(!isAlive())
        return;
    if(getX()<0){
        die();
        return;
    }
    if(getHealth()<=0){
        getDestroyed(); //explosions, sound, all the fun stuff
        return;
    }
    
    if(collideWithPlayer()){
        return;
    }
    
    changeFlightPlan(); //change flight plan for individual enemies
    
    
    if(abs(getWorld()->getPlayerY()-getY()) <=4 && getWorld()->getPlayerX() < getX()){
        if(fire()) //shoots whatever the fire method specifies. Randomness is done in the fire method
            return;
        }
    moveTo(m_dir.m_x*getTravelSpeed() + getX(), m_dir.m_y*getTravelSpeed() + getY());
    setFlightLength(getFlightLength()-1);
    collideWithPlayer();
    
}


//-------------------SMALLGON---------------
Smallgon::Smallgon(double x, double y , StudentWorld* w)
    :Alien(IID_SMALLGON, x, y, 2.0, 5*(1+(w->getLevel() -1)*0.1), 2, 250,w){
        setFlightLength(0);//initial flight length of 0
}
Smallgon::~Smallgon(){}
void Smallgon::drop(){};

void Smallgon::changeFlightPlan(){
    //flight plan
    if(getY()>=VIEW_HEIGHT-1){
        setDir(-1, -1);
        setFlightLength(randInt(1, 32));
    }
    if(getY()<=0){
        setDir(-1, 1);
        setFlightLength(randInt(1,32));
    }
    if(getFlightLength()<=0){
        setDir(-1, randInt(-1, 1));
        setFlightLength(randInt(1, 32));
    }
}

bool Smallgon::fire(){
    if(1==randInt(1, 20/getWorld()->getLevel() + 5)){
        getWorld()->instantiate(new Turnip(getX()-14, getY(), getWorld()));
        getWorld()->playSound(SOUND_ALIEN_SHOOT);
        return true;
    }
    return false;
}

//---------------SMOREGON----------------
Smoregon::Smoregon(double x, double y , StudentWorld* w)
    :Alien(IID_SMOREGON, x, y, 2.0, 5*(1+(w->getLevel() -1)*0.1),2, 250,w){
        setFlightLength(0);
    }
Smoregon::~Smoregon(){}

//drop goodies with a 1/3 chance
void Smoregon::drop(){
    if(randInt(1, 3) == 1){
        if(randInt(1, 2) == 1){
            getWorld()->instantiate(new Repair(getX(),getY(), getWorld()));
        }else{
            getWorld()->instantiate(new TorpedoGoodie(getX(),getY(), getWorld()));
        }
    }
}
bool Smoregon::fire(){
    //shoot at the player (and don't move!)
    if(1 == randInt(1, (20/getWorld()->getLevel())+5)){
        getWorld()->instantiate(new Turnip(getX()-14, getY(), getWorld()));
        getWorld()->playSound(SOUND_ALIEN_SHOOT);
        return true;
    }
    //ram the player
    if(1== randInt(1, 20/getWorld()->getLevel()+5)){
        setDir(-1, 0);
        setTravelSpeed(5.0);
        setFlightLength(VIEW_WIDTH);
        return false;
    }
    return false;
}

void Smoregon::changeFlightPlan(){
    //new flight plans
    if(getY()>=VIEW_HEIGHT-1){
        setDir(-1, -1);
        setFlightLength(randInt(1, 32));
    }
    if(getY()<=0){
        setDir(-1, 1);
        setFlightLength(randInt(1, 32));
    }
    if(getFlightLength() <=0){
        setDir(-1, randInt(-1, 1));
        setFlightLength(randInt(1, 32));
    }
}


//---------------SNAGGLEGON--------------
Snagglegon::Snagglegon(double x, double y, StudentWorld* w):Alien(IID_SNAGGLEGON, x, y, 2.0, 10*(1+(w->getLevel() -1)*0.1),15, 1000,w){
    setTravelSpeed(1.75);
    setDir(-1, -1);
}
Snagglegon::~Snagglegon(){}

void Snagglegon::drop(){
    if(1==randInt(1, 6)){
        getWorld()->instantiate(new OneUp(getX(), getY(), getWorld()));
    }
}
void Snagglegon::changeFlightPlan(){
    if(getY()<=0){
        setDir(-1, 1);
    }
    if(getY()>=VIEW_HEIGHT-1){
        setDir(-1, -1);
    }
}
bool Snagglegon::fire(){
    if(1==randInt(1, 15/getWorld()->getLevel() + 10)){
        getWorld()->instantiate(new Torpedo(getX()-14, getY(), 180, false, getWorld()));
        getWorld()->playSound(SOUND_TORPEDO);
        return true;
    }
    return false;
}



////////////////////////NACHENBLASTER//////////////////////////
NachenBlaster::NachenBlaster(StudentWorld* w):Actor(IID_NACHENBLASTER, 0, 128, 0, 1.0, 0,w){
    m_health = 50;
    m_nCabbage = 30;
    m_nTorpedos = 0;
}
NachenBlaster::~NachenBlaster(){
    
}
int NachenBlaster::getTag() const{
    return PLAYER_TAG;
}
void NachenBlaster::damage(int damage){
    m_health -= damage;
}

void NachenBlaster::doSomething(){
    if(!isAlive())
        return;
    int ch;
    if(getWorld()->getKey(ch)){
        switch (ch) {
            case KEY_PRESS_LEFT:
                if(getX()>=6)
                    moveTo(getX()-6, getY());
                break;
            case KEY_PRESS_RIGHT:
                if(getX()<VIEW_WIDTH-6)
                    moveTo(getX()+6, getY());
                break;
            case KEY_PRESS_UP:
                if(getY()<VIEW_HEIGHT-6)
                    moveTo(getX(), getY()+6);
                break;
            case KEY_PRESS_DOWN:
                if(getY()>=6)
                moveTo(getX(), getY()-6);
                break;
            case KEY_PRESS_SPACE:
                if(m_nCabbage >5){
                    getWorld()->instantiate(new Cabbage(getX()+12,getY(),getWorld()));
                    getWorld()->playSound(SOUND_PLAYER_SHOOT);
                    m_nCabbage-=5;
                }
                break;
            case KEY_PRESS_TAB:
                if(m_nTorpedos > 0){
                    getWorld()->instantiate(new Torpedo(getX()+12,getY(), 0, true, getWorld()));
                    getWorld()->playSound(SOUND_TORPEDO);
                    m_nTorpedos--;
                }
            default:
                break;
        }
        
    }
    
    if(m_nCabbage<30)
        m_nCabbage++; //add cabbage if current number is less than 30
    
}

int NachenBlaster::getHealth() const{
    return m_health;
}
int NachenBlaster::getCabbageCount() const{
    return m_nCabbage;
}
int NachenBlaster::torpedoCount() const{
    return m_nTorpedos;
}
void NachenBlaster::repair(){
    if(m_health >=40)
        m_health = 50;
    else
        m_health+=10;
}
void NachenBlaster::giveTorpedos(){
    m_nTorpedos+=5;
}


/////////////////////MISC OBJECTS/////////////////////////
//----------------------STARS------------------------
Star::Star(double x, double y, StudentWorld* w):Actor(IID_STAR, x,y, 0, 0.01*randInt(5, 50) , 3,w){
    
}
Star::~Star(){
    
}
void Star::doSomething(){
    if(getX() <= -1)
        die();
    else
        moveTo(getX()-1, getY());
        //moveTo(getX()-(2*getSize()), getY()); //change this
}
int Star::getTag() const{
    return MISC_TAG;
}

//--------------------EXPLOSIONS------------------------
Explosion::Explosion(double x, double y, StudentWorld* w):Actor(IID_EXPLOSION, x,y, 0, 1, 0,w){
    
}
Explosion::~Explosion(){
    
}
int Explosion::getTag() const{
    return MISC_TAG;
}
void Explosion::doSomething(){
    setSize(getSize()*1.5);
    if(getSize()>3.375)
        die();
}







