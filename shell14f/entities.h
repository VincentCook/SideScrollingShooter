#pragma once

#include "GameObject.h"
#include "mysoundengine.h"
#include "GameObject.h"
#include <fstream>

// Create your game entity classes here

typedef int Stats ; 


class PlayerShip : public GameObject
//playership class, is inherited from game object 
{

  private:
  //private member variables

    Vector2D velocity ;   //ships velocity 
    Circle2D collider ;   //colider for ship 
    bool thrusting ;      //bool to check ifthe ship is thrusting
    bool gravityEffect ;  //bool on if being effected by gravity
    float shootDelay ;    //delay for the ships guns
    Stats health ;        //health of the ship
    Stats fuel ;          //the amount of the  fuel the ship has left 
    bool alive ;     
    Vector2D cameraPosition ;   //position to place the camera
    static int score ;         // stores the players current score 
    SoundIndex thruster ;
    Stats ammo ; 


  public:
  //public members



    PlayerShip() ;                              //constructer
    void Initialise(Vector2D position) ;        //initialise
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape
    void ProcessCollision(GameObject &other) ;  //process any colisions 
    void DrawHud() ;                            //draws hud to screen
    void Inputs(float frametime) ;              //processes player controls
    void ThrustFlames() ;                       //process the flames for thrust
    void ShootGun(float frametime) ;            //fires main gun
    void MoveCamera() ;                         //has camera follow ship
    Vector2D GetPosition() ;                    //return position of ship

    static void setScore(int newScore) ; 
    static int getScore() ;
    

};



class Flames : public GameObject
//flames class inherited from game objects 
{

  private:
  //private member variables

    Vector2D velocity ;       //velocity of the flames
    Circle2D collider ;       //collider for the flames *circle*
    int currentAnimation ;  //current animation number 
    float animationTime ;     //time the animation takes

  public:
  //public members

    Flames() ;                                  //constructor
    void Initialise(Vector2D position,          //initialise 
                    Vector2D velocityin,
                    float animation,
                    float scale) ;
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape
    void ProcessCollision(GameObject &other) ;  //process any collisions 

};

class Bullet : public GameObject
//bullet class, inherited from game object
{

  private:
  //private member variables

    Vector2D velocity ;   //velocity ofthe bullet 
    Point2D collider ;    //bullets collider - point (line)
    float timer ;         //timer for bullets existance
    SoundIndex laser ; 


  public:
  //public members

    Bullet() ;                                  //constructor     
    void Initialise(Vector2D position,          //initialise
                    Vector2D initialVelocity,
                    int GiveShooter) ;
    void Draw() ;                               //draw the bullet
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape
    void ProcessCollision(GameObject &other) ;  //process any collisions 


    int whoShot ;

};



class Building : public GameObject
//building class, inherited from game objects
{

  private:
  //private member variable

    Rectangle2D collider ;                  //collider for building *rectangle*
    float m_height ;
    float m_width ; 
	  int BuildingNumber; 
    static float NextBuildingPosition;
  
  public:

    Building() ;                                //constuctor
    void Initialise(int buildingNumberIn) ;     //initialise
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape 
    void ProcessCollision(GameObject &other) ;  //process any collisions 
    int GetBuildingType() ;
    static void SetBuildingPosition() ; 
    static float GetBuildingPosition() ; 

};



class Road : public GameObject
//road class inherited from game objects 
{

  private:
    Rectangle2D collider ;                  //collider for building *rectangle*
    float m_height ;
    float m_width ; 
    static float NextRoadPosition;
  
  public:
	  static int numRoads;
    Road() ;                                //constuctor
    void Initialise() ;        //initialise
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape 
    void ProcessCollision(GameObject &other) ;  //process any collisions 
    static void SetRoadPosition() ; 
    static float GetRoadPosition() ; 


};



class Particles : public GameObject
//class for handleing particle emmiters 
{

  private:
  //private member variables
  
  Vector2D position[100] ;
  Vector2D velocity[100] ; 
  int totalParticles ; 
  Point2D collider ; 
  float timer ; 

  public:
  //public member functions 
  
    Particles() ; 
    void Initialise(Vector2D startPosition,
                    int totalIn) ;
    void Update(float frametime) ; 
    void Draw() ;                
    IShape2D &GetCollisionShape() ;                  //not used but required
    void ProcessCollision(GameObject &other) ;       //not used but required

};



class Enemy : public GameObject 
//class for enemuy ships 
{

  protected:
  //private member variables

    Vector2D velocity ; //ships velocity 
    Rectangle2D collider ; //colider for ship 
    float shootDelay ;  //delay for the ships guns
    Stats health ;      //health of the ship
    int type ;          //defines the type of enemy 
    float height ; 
    float width ; 
    int missiles ;
    bool raptorGoUp ;


    static PlayerShip *pThePlayer ; 
    
    
  public:
  //public members

    Enemy() ;                                   //constructer
    void Initialise(int TypeIn,
                    Vector2D startPosition) ;
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape
    void ProcessCollision(GameObject &other) ;  //process any colisions 
    static void SetShip(PlayerShip *pShip) ; 

};



class GameManager : public GameObject
{

  private:

    static PlayerShip *pThePlayer ; 
    Point2D collider ; 
    static float nextBGPosition ;
    static float enemyTimer ; 
    
  

  public:

    float finalScore ; 

    GameManager() ; 
    void Initialise() ; 
    void Update(float frametime) ; 
    IShape2D &GetCollisionShape() ; 
    void ProcessCollision(GameObject &other) ;
    void Draw() ; 
   
    static void CreateBackground() ; 
    static void SetShip(PlayerShip *pShip) ; 
    static void InitialSetup() ;
    static void SpawnStartBuildings() ; 
    static void StartSpawnRoads() ; 
    static void SpawnNewBuilding() ;

    static void EnemySpawn(float frametime) ; 


};

class Explosion : public GameObject
{

  private:

    Vector2D velocity ;
    Circle2D collider ; 
    float animationSpeed ; 
    float currentAnimation ; 
    SoundIndex explosionSound ; 

  public:

    Explosion() ;
    void Initialise(Vector2D velocityIn,
                    Vector2D position,
                    float animationSpeedIn,
                    float scale);
    void Update(float frametime) ;
    IShape2D &GetCollisionShape() ;
    void ProcessCollision(GameObject &other) ; 
    

};



class Missile : public GameObject
{

  protected:
  //private member variables

    Vector2D velocity ; //ships velocity 
    Rectangle2D collider ; //colider for ship 
    float height ; 
    float width ; 
    Vector2D vectorTo ; 

    static PlayerShip *pThePlayer ; 
    
    
  public:
  //public members

    Missile() ;                                   //constructer
    void Initialise(Vector2D startPosition) ;
    void Update(float frametime) ;              //update
    IShape2D &GetCollisionShape() ;             //get the collision shape
    void ProcessCollision(GameObject &other) ;  //process any colisions 
    static void SetShip(PlayerShip *pShip) ; 

};



class Brick : public GameObject
{

  private:
    
    float height ;
    float width ;
    Vector2D topRight ; 
    Vector2D bottomLeft ; 
    Rectangle2D collider ; 

  public :

    Brick() ; 
    void Initialise(Vector2D position) ; 
    void Update(float frametime) ;
    IShape2D &GetCollisionShape() ; 
    void ProcessCollision(GameObject &other) ; 
    
};



