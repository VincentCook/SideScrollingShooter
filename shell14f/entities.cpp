#include "entities.h"
#include "gamecode.h"
#include "myinputs.h"
#include <time.h>
#include <fstream>

//typedefs for various uses in abstraction
//heavily used in consts to make the list easily readable 
//with regards to their uses 

    // ****REFERENCES****
    //freesounds.org
    //
    //tommccann - explosion 
    //LimitSnap_Creations - rocket thrust 
    //marcuslee - laser

typedef const int BuildingType ;        //used for the list of buildings

typedef const float PhysicsFloat ;      //floats used for defining physics
typedef const Vector2D PhysicsVector ;  //vectors for defining physics
typedef const float ShipFloat ;         //floats for initialising players ship
typedef const float Graphicsfloat ;     //floats used in graphic drawing
typedef const int ShipInt ;             // ints used in players ship 
typedef const int SceneSetups ;         //consts to set up scene of game
typedef const Vector2D ShipVector ;     //vectors used in players ship
typedef const int PhysicsInt ;          //ints used in physics constants
typedef const float EnemyStats ;        //stats for enemys initialiseation

typedef Vector2D HudPosition ;          //positions for drawing hud
typedef Vector2D Coordinates ;          

int PlayerShip:: score = 0 ; 

enum EnemyType
//list of the types of enemy for calling 
{

  SPITFIRE,
  KAMAKAZE,
  RAPTOR
  
};

//list of constants mostly used toi make the code more readable 
//and easy to change when needed without searching theorugh the code.
//some only used once but still aid in abstraction of the code 
PhysicsFloat ROTATESPEED = 2.0f ; 
PhysicsFloat ACCELERATION = 2500.0f ;      //the change in speed of
                                           // the objects in game thrusting
PhysicsFloat FRICTION = 2.0f ;             //the games friction used to 
                                           //slow movement of objects 
PhysicsFloat BULLETSPEED = 1500 ;          //speed of bullets 
PhysicsVector GRAVITY(0, -450) ;           //gravity 
PhysicsInt MAXCOLLIDESPEED = 300 ;         //maxspeed can collide 
                                           //without damage

ShipFloat SHIPSIZE = 128.0f / 2 ;              //the size of ships image
ShipFloat SHIPWIDTH = 127.0f ;             //width of the ship 
ShipFloat SHIPHEIGHT = 40.0f ;             //height of teh ship 
ShipFloat COLLIDERSIZE = 64.0f ;           //size of collider for flames 
ShipVector SHIPDISPLACEMENT(50,0) ;        //vector from ship to draw behind it
ShipInt MAXHEALTH = 1000 ;                 //players max health
ShipInt MAXFUEL = 1000 ;                   //max player health
ShipFloat BULLETDELAY = 0.5f ;             //delay in frames beween bullets
ShipInt FUELCHANGE = 20 ;

Graphicsfloat ROTATEHORIZANTAL = 1.5708f ; //rotation to draw objects the 
                                           //correct way around
Graphicsfloat FLAMEANIMSPEED = 60.0f ;     //speed of animations 
                                           //used for flames
Graphicsfloat FLAMESCALE = 0.4f ;          //scale of flames
Graphicsfloat FLAMEMAGNITUDE = 600.0f ;    //magnitude for drawing 
                                           //flames rotated
Graphicsfloat OFFSCREENSPAWN = -2000.0f ;

SceneSetups NUMBEROFBUILDINGTYPES = 3 ;    //number of possible buildings 
SceneSetups BUILDINGFLOOR = 889 ;          //height the buildings should spawn
SceneSetups ROADFLOOR = 1000 ;             //height road should spawn  
SceneSetups TRUMPSCALE = 2 ; 
SceneSetups SKYLINESIZE = 512 ; 

EnemyStats SPITSHOTDELAY = 0.5f ;           //delay between shots for spitfire
EnemyStats SPITBURSTDELAY = 0.7f ;          //delay between burts of fire 

BuildingType HALLCENTRE = 0 ;              //list of buildings 
BuildingType HALLLEFT= 1 ;                 //no enum because relies on integer
BuildingType HALLRIGHT = 2 ;               //starting at 0
BuildingType FUELSTATION = 3 ;
BuildingType TRUMPTOWER = 4 ; 

enum Shooters
{

  BAD,
  GOOD
  
};



PlayerShip::PlayerShip() : GameObject(PLAYERSHIP)
//constructer for player class
//only defines the game object type
{

  

}



void PlayerShip::Initialise(Vector2D position)
//initialise the players ship
//
//Paramiters Passed:
//  in:
//    position - vector2D giving the location of the ship
{

  m_position = position ;             //set position of the ship
  m_angle = 0.0f ;                    // set the initial angle of the ship 
  velocity.set(0,0) ;                 // set the inital velocity of the ship
  LoadImage(L"PlayershipNoTex.PNG") ; //load the ships image file
  shootDelay = 0.0f ;                 //set the delay of the bullets
  health = MAXHEALTH ;                //set initial health
  fuel = MAXFUEL ;                    //set initial fuel 
  score = 0 ;                         //initialise score 
  gravityEffect = true ;
  alive = true ; 
  thruster = MySoundEngine::GetInstance() -> LoadWav(L"RocketThrust.wav") ;
                                      //load thrusting sounds 
  ammo = 30 ; 

  PlayerShip::MoveCamera();


}



void PlayerShip::ThrustFlames()
{

  Vector2D flameDirection ;                     //create vector for 
                                                //the position of
                                                //the ships thrusters flames
  Vector2D behindShip ;                         //vector for finding
                                                //the back of the ship

  Flames *shipFlame = new Flames ;
  //create a new flames pointer
  flameDirection.setBearing(m_angle - ROTATEHORIZANTAL ,
                            FLAMEMAGNITUDE) ; 
  //set the bearing of the flames to the angle and magnitude of the ship
  //take the ROTATEHORIZANTAL constant to ensure the image is the correct 
  //direction 
  behindShip = m_position - SHIPDISPLACEMENT.rotatedBy(-m_angle) ;
  //set the behindShip variable to the position of the ship - the displacement
  //which is how far it needs to be to be behind the ship
  //rotated by the angle of the ship
  
  shipFlame -> Initialise(behindShip,
                          flameDirection,
                          FLAMEANIMSPEED,
                          FLAMESCALE) ; 
  //initialise the flames passing the position direction speed 
  //and scale of the flames
  Game::instance.m_objects.AddItem(shipFlame, 
                                   false) ; 
  //add the new flames to the game 

}



void PlayerShip::ShootGun(float frametime)
//fires the gun of the ship
//passed paramiters:
//  in:
//    frametime - framerate of machine 
{

  Vector2D bulletPosition ;                     //position of bullets
                                                // fired by ship
  Vector2D bulletVelocity ;                     //veocity of bullets
                                                // fired by the ships 
  shootDelay = BULLETDELAY ; 
  //set the variable shoot delay to the constant for delaying bullets

  bulletPosition.setBearing(m_angle + ROTATEHORIZANTAL,
                            SHIPSIZE/2) ;
  //set the position of the bullets to the angle of the ship
  //and half of theships size so that the bullet fires from half way
  //down the image of the ship looking more natural
  //add the rotation constant to the angle so that it fires from
  // the front of the ship not the top
    
  bulletPosition = bulletPosition + m_position ; 
  // set the bullets position to the position of it plus the 
  // position of the ship so that it fires from the ship
  bulletVelocity.setBearing(m_angle + ROTATEHORIZANTAL, 
                            BULLETSPEED) ;
  //set the baring of the bullets velocity to the angle of the ship
  //and the base speed of the bullet 
  //apply the rotate constant to it is in the correct direction  
  bulletVelocity = bulletVelocity + velocity ; 
  //set the velocity of the bullet to itself plus the ships
  //so that it fires with respect to the ships owm velocity

  Bullet *firedBullet = new Bullet ; 
  //create new bullet through pointer 
  firedBullet -> Initialise(bulletPosition,
                            bulletVelocity,
                            GOOD) ;
  //initialise the new bullet to the position and velocity
  Game::instance.m_objects.AddItem(firedBullet,   
                                   true) ;     
  //create instance of the bullet in the game 

  ammo-- ;

}



void PlayerShip::Inputs(float frametime) 
//Takes teh inputs from the user and processes what to do
//Passed Paramiters:
//  in:
//    frametime - framerate of computer
{

  MyInputs *pInputs = MyInputs::GetInstance() ; //create pointer to the
                                                //input engine
  pInputs -> SampleKeyboard() ;                 //sameple the keyboard

  Vector2D accelerate ;                         //create vector for
                                                // ships acceleration


  if(pInputs -> KeyPressed(DIK_A))
  //if the player presses 'A' on the keyboard
  {

    m_angle = m_angle - ROTATESPEED * frametime ; 
    //set the angle fo the ship to the angle minus the speed the ship rotates 
    //then multiply by the frametoime for compatability 

  }
  if(pInputs -> KeyPressed(DIK_D))
  //if the player pressed 'D' on he keyboard 
  {

    m_angle = m_angle + ROTATESPEED * frametime ;
    //set the angle of the ship to the ship + the rotation speed
    // multiply by the frametime for compatability 
  
  }
  if(pInputs -> KeyPressed(DIK_5))
  //if the player presses '5' pn the keyboard
  {

    m_position.set(0,0) ; 
    //reset the ship to center of the screen for Debugging purposes

  }
  if(pInputs -> KeyPressed(DIK_W) &&
     m_position.YValue <= 500)
  //if  the player presses 'W' on the keyboard
  {

    if(!thrusting)
    {

      MySoundEngine::GetInstance() -> Play(thruster, 
                                           true) ;

    }

    thrusting = true ; 

    if (fuel > 0)
    //while the ship has fuel 
    {

      accelerate.setBearing(m_angle + ROTATEHORIZANTAL, 
                            ACCELERATION) ; 
      //set the bearing of the ship to the angle of the ship 
      // and the ships acceleration
      // add the ROTATEHORIZANTAL constant to make the image face correct way 
      velocity = velocity + accelerate * frametime ; 
      //set the velocity of the ship to the new velocity plus accelerate to 
      //give the ship its new speed
      //multiply by frame time for compatability

      PlayerShip::ThrustFlames() ; 
      //run the flames for the thruster 

      Particles *newEmitter = new Particles;

      newEmitter->Initialise(m_position,
                             5) ;
      Game::instance.m_objects.AddItem(newEmitter,
                                       false) ;
    
    }  

  
    fuel = fuel - FUELCHANGE * frametime ;
    //use fuel as thrusting 
  
  }
  else 
  {
    
    if(thrusting)
    {

      MySoundEngine::GetInstance() -> Stop(thruster) ; 

    }
      
  }
  if(pInputs -> KeyPressed(DIK_SPACE) && shootDelay < 0)
  //if the player presses space 
  //and the delay on the bullets has reached 0
  {

    PlayerShip::ShootGun(frametime) ;
    
     

  }
    
  //KEYS FOR DEMO PURPOSES ONLY
  if (pInputs -> NewKeyPressed(DIK_1))
  {

    Enemy *spawnSpit = new Enemy ; 
    spawnSpit -> Initialise(SPITFIRE,
                            Vector2D(m_position.XValue + 600,
                                     m_position.YValue - 600)); 
    Game::instance.m_objects.AddItem(spawnSpit,
                                     true);

  }
  if (pInputs -> NewKeyPressed(DIK_2))
  { 

    Enemy *spawnKama = new Enemy ; 
    spawnKama -> Initialise(KAMAKAZE,
                            Vector2D(m_position.XValue + 600,
                                     m_position.YValue - 600)) ;
  
    Game::instance.m_objects.AddItem(spawnKama,
                                     true) ; 

  }
  if (pInputs -> NewKeyPressed(DIK_3))
  {

    Enemy *spawnRaptor = new Enemy;
    spawnRaptor -> Initialise(RAPTOR,
                              Vector2D(m_position.XValue + 600,
                                       m_position.YValue - 600)) ;

    Game::instance.m_objects.AddItem(spawnRaptor,
                                     true) ;
  }
  if(pInputs -> NewKeyPressed(DIK_4))
  {

    PlayerShip::setScore(PlayerShip::getScore() + 100) ;

  }
  //END OF DEMO KEYS 

}



void PlayerShip::MoveCamera() 
{


  //if statements stop the camera moving too far left
  //or down 
  if ( m_position.XValue > cameraPosition.XValue)
  {

    cameraPosition.XValue = m_position.XValue ;
    
  }
  if (m_position.YValue > 0)
  {

    cameraPosition.YValue = m_position.YValue * -1  ; 

  }




  MyDrawEngine::GetInstance() -> 
                theCamera.PlaceAt(cameraPosition) ; 
 //place the camera at the ships position

}



void PlayerShip::Update(float frametime)
//definition for the update sub program for the playership class
//paramiters passed:
//  in:
//    frametime - float for the computers framerate
{
  PlayerShip::MoveCamera() ;

  shootDelay = shootDelay - frametime ; 
  //set the shoot delay to itself - the frametime to count it down
  //each frame and be compatable for every frame 

  PlayerShip::Inputs(frametime) ;


  //          ****PROCESSMOVEMENT****
  velocity = velocity - FRICTION * frametime * velocity ; 

  if(gravityEffect == true)
  //if gravity is on
  {

    velocity = velocity + GRAVITY * frametime ; 
    //apply gravity

  }

  gravityEffect = true ; 
  //set the velocity of the ship to itself - friction
  //and multiplied by itself
  //multiply by frametime for compatability

  if(m_position.XValue <= MyDrawEngine::GetInstance() ->
                                        GetViewport().GetBottomLeft().XValue)
  {

    velocity.XValue = velocity.XValue * -1 ;

  }

  if (alive == true)
  {
  
    m_position = m_position + velocity * frametime ; 
    //set the position to itself plus the new velocity
  

  }
  

  DrawHud() ; 

  if (health <= 0)
  //if the ships health reaches 0
  {

    alive = false ;

    //spawn explosion with particles 
    Explosion *newExplosion = new Explosion ;
    
    newExplosion -> Initialise(Vector2D(0,0),
                               m_position,
                               4.5f,
                               1.05f) ; 
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ; 

    Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position,
                           100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);


    Deactivate();

    //ship is killed 

  }

}



IShape2D &PlayerShip::GetCollisionShape()
//ship class subprogram to the get the shape of its collision
{

  

  collider.PlaceAt(m_position,
                   SHIPSIZE - 2) ; 
  //place the collider of the ship at the position of the ship
  // with the center being at half the ship -2

  return collider ;
  //return the collider to the class
}



void PlayerShip::ProcessCollision(GameObject &other)
//process collisions of the ship
//Passed paramiters:
//  in:
//    other - game object that the ship has collided with
//  out:
//    other - game object the ship has collided with
{

  if(other.GetType() == BUILDING ||
     other.GetType() == ROAD)
  //if collides with a building 
  {
  
    velocity =  velocity * -1 ; 
    //reverse the velocity of the ship
    gravityEffect = false ; 

    if (velocity.YValue >= MAXCOLLIDESPEED)
    //if the ship is moving fast
    {

      health = health - velocity.YValue ; 
      //reduce the health in relation to the speed

    }
    else if (velocity.XValue >= MAXCOLLIDESPEED)
    {

      health = health - velocity.XValue ;

    }

    if (other.GetType() == BUILDING)
    //if the collision is a building
    {

      Building *pBuilding = (Building*) &other ;
      //cast the building to a pointer 

      if (pBuilding -> GetBuildingType() == FUELSTATION)
      //if the building is a fuel station
      {

        ammo = 30 ; 

        if(fuel < MAXFUEL)
        //if the fuel is less than max 
        {

          fuel = fuel + FUELCHANGE ;
          //increase fuel

        }
        else
        {

          fuel = MAXFUEL ; //stops fuel going over max 

        }

      }

    }    

  }
  if (other.GetType() == ENEMY)
  //if the player hits an enemy
  {


    Deactivate() ; 
    //deactivate the player 

  }
  if (other.GetType() == MISSILE)
  {

    Deactivate() ;

  }


}



void PlayerShip::DrawHud()
{

  //set positions for the hud items to display
  HudPosition healthText(200, 100) ;
  HudPosition healthNum(290, 100) ; 
  HudPosition fuelText(200, 150) ;
  HudPosition fuelNum(250, 150) ; 
  HudPosition scoreText(1500, 100) ; 
  HudPosition scoreNum(1500, 150) ;
  HudPosition ammoText(200, 200) ;
  HudPosition ammoNum(290, 200) ; 

  //disable camera
  MyDrawEngine::GetInstance() -> UseCamera(false) ; 

  //draw hud
	MyDrawEngine::GetInstance() -> WriteText(healthText,
                                           L"HEALTH",
                                           MyDrawEngine::WHITE);

  MyDrawEngine::GetInstance() -> WriteDouble(healthNum,
                                            health,
                                            MyDrawEngine::WHITE) ; 

  MyDrawEngine::GetInstance() -> WriteText(fuelText,
                                           L"FUEL",
                                           MyDrawEngine::WHITE) ; 

  MyDrawEngine::GetInstance() -> WriteDouble(fuelNum,
                                             fuel,
                                             MyDrawEngine::WHITE) ; 

  MyDrawEngine::GetInstance() -> WriteText(scoreText,
                                           L"SCORE",
                                           MyDrawEngine::WHITE) ; 

  MyDrawEngine::GetInstance() -> WriteDouble(scoreNum,
                                             score,
                                             MyDrawEngine::WHITE) ; 
 
  MyDrawEngine::GetInstance() -> WriteText(ammoText,
                                           L"AMMO",
                                           MyDrawEngine::WHITE) ; 

  MyDrawEngine::GetInstance() -> WriteDouble(ammoNum,
                                             ammo,
                                             MyDrawEngine::WHITE) ; 

  //reactivate camera
  MyDrawEngine::GetInstance() -> UseCamera(true) ; 

}



Vector2D PlayerShip::GetPosition()
//returns the position of the ship
//passed paramiters:
//  out:
//    m_position - the position of the ship 
{

  return m_position ; 

}



void PlayerShip::setScore(int newScore)
{

  //sets the score to inputted variable 
  score = newScore ;

}



int PlayerShip::getScore()
{

  //returns the score 
  return score ;

}



Flames::Flames() : GameObject(FLAMES)
//constructer for the flames game object
{

  

}



void Flames::Initialise(Vector2D position,
                        Vector2D velocityin,
                        float animation,
                        float scale)
//initialise the flames game object 
//passed paramiters:
//  in:
//    position - vector denoting the position of the flame 
//    velocityin - the velocity being passed to the flame 
//    animation - float giving the speed of the animation needed
//    scale - the scale of the images needed
{

  velocity = velocityin ;     //set the flames velocity to the inputed velocity
  m_position = position ;     //set the position of the ship
                              //to the given position
  animationTime = animation ; //set the animationtime of the ship to given time
  currentAnimation = 0 ;      //set the current animation to position 0
  m_imageScale = scale ;      //set the scale of the image to the given 

  LoadImage(L"Flames1.PNG") ; //load the first animation fram
  LoadImage(L"Flames2.PNG") ; //load the second animation frame

  
}



void Flames::Update(float frametime)
//update sub program for the flames class
//passed paramiters: 
//  in:
//    frametime - the computers framerate
{

  currentAnimation = currentAnimation + animationTime * frametime ; 
  //set the current animation to itself plus the animation time 
  //multiply be frametime for compatability 
  
  if (currentAnimation >= 2)
  //if the current animation is more than or equal to the 
  //highest frame
  {

    Deactivate() ;
    //deactivate the object
    currentAnimation = 0 ;
    //set teh curret animation to 0 

  }

  m_position = m_position + velocity * frametime ;
  //set the position of the flames to itself plus the velocity
  //multiply by frametime for compatability
  m_imageNumber = currentAnimation ; 
  //set the image number of the animation to the current 

}



IShape2D &Flames::GetCollisionShape()
//get the collision shape of the flames class
{

  collider.PlaceAt(m_position,
                   COLLIDERSIZE * m_imageScale) ; 
  //place the collider
  return collider ; 
  //return the collider 

}



void Flames::ProcessCollision(GameObject &other)
//process the collision of the flames - not needed due to no colliding 
//but required by the engine for all game objects 
//passed paramiters:
//  in:
//    other - game object collided with
//  out:
//    other: game object collided with 
{

  //no collide 

}



Bullet::Bullet() : GameObject(BULLET)
//bullet constructor
{



}



void Bullet::Initialise(Vector2D position,
                        Vector2D initialVelocity,
                        int giveShooter)
//initialise sub program for the bullet class
//passed paramiters:
//  in:
//    position - vector giving the position of the bullet
//    initialVelocity - starting velocity of the bullet 
//    giveShooter - tells the game if the enemy or player is shooting 
{

  m_position = position ;
  //set the position of the bullet to the given one
  velocity = initialVelocity ; 
  //set the velocity to given
  timer = 3.0 ;
  //set the timers initial value 
  whoShot = giveShooter ;
  laser = MySoundEngine::GetInstance() -> LoadWav(L"laser.wav") ;
  MySoundEngine::GetInstance() -> Play(laser, 
                                       false) ;

}



void Bullet::Update(float frametime)
//update sub program for the Bullet class
//passed paramiters:
//  in:
//    frametime - variable giving the framerate 
{

  timer = timer - frametime ; 
  //set the timer to itself minus the frametime for countdown
  velocity = velocity + GRAVITY * frametime ;
  m_position = m_position + velocity * frametime ;  
  //set the position of the bullet to itself added to the velocity 
  //multiplied by the frametime for compatability

  if (timer <= 0)
  //if thetimer reaches 0
  {

    Deactivate() ; 
    //deactivate the bullet

  }

}



IShape2D &Bullet::GetCollisionShape()
//get the collision shape of the bullet
{

  collider = m_position ; 
  //set the collider to the position of the bullet
  return collider ; 
  //return the collider

}



void Bullet::ProcessCollision(GameObject &other)
//process the collision of the bullet
//passed paramiters:
//  in:
//    other - game object collided with
//  out:
//    other - game object collided with 
{

  if (other.GetType() == PLAYERSHIP &&
      whoShot == BAD)
  //if the player is hit by enemy bullet 
  {

    //deactivate the player 
    other.Deactivate() ;

    //produce explosion
    Explosion *newExplosion = new Explosion ;

    newExplosion->Initialise(Vector2D(0, 0),
                             m_position,
                             4.5f,
                             1.05f) ;
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ;


    Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);
    //deactivate the bullet 
    Deactivate() ;

  }
  if (other.GetType() == BUILDING)
  {

    Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                       false);

    Deactivate() ;

  }
  if (other.GetType() == ENEMY &&
      whoShot == GOOD)
  {

    //deactivate the player 
    other.Deactivate() ;

    //produce explosion
    Explosion *newExplosion = new Explosion ;

    newExplosion->Initialise(Vector2D(0, 0),
                             m_position,
                             4.5f,
                             1.05f) ;
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ;

    //spawn particles with the explosion
    Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);

    PlayerShip::setScore(PlayerShip::getScore() + 100) ; 
    //deactivate the bullet 
    Deactivate() ;      

  }

}



void Bullet::Draw()
//draw sub program for the bullet class
{

  MyDrawEngine::GetInstance() ->
                DrawLine(m_position,
                m_position - velocity.unitVector()*8.0f,
                MyDrawEngine::WHITE) ; 
  //use the draw engine to draw a line at the position of the bullet
  //to thebullet - the unit vector of the velocity multiplied by 8 to give a
  //short line for the bullet
  //use the draw engine to colour it white 

}



Building::Building() : GameObject(BUILDING)
//constructor for the building class
{

  

}

float Building::NextBuildingPosition = - 2000 ; 

void Building::Initialise(int buildingNumberIn)
//initialises the buildings in the game 
//passed paramiters:
//  in:
//    buildingNumber - tells the program whichbuilding to spawn 
//    position - tells where to spawn the building
//    scale - the size of the building to spawn 
{

  int randomGap = 0 ; //holds a random number for gap between buildings 

  m_drawDepth = 0 ; 

  BuildingNumber = buildingNumberIn ; 

  switch(BuildingNumber)
  //check the building number and load correct images
  {
    case HALLCENTRE :

      LoadImage(L"building1center.PNG") ; 
      m_height = 398  ;
      m_width = 188 ;
        

      break ;
    
    case HALLLEFT :
    
      LoadImage(L"building1left.PNG") ; 
      m_height = 236  ;
      m_width = 375 ;

  
      break ; 

    case HALLRIGHT :
    
      LoadImage(L"building1Right.PNG") ; 
      m_height = 234  ;
      m_width = 365 ;
      randomGap = rand()%100 ; 

      break ;

    case FUELSTATION :

      LoadImage(L"FuelStation.PNG") ; 
      m_height = 235 ;
      m_width = 497 ; 
      randomGap = rand()%100 ; 
  
      break ; 

    case TRUMPTOWER :

      LoadImage(L"TrumpTower.PNG") ; 
      m_height = 459 * TRUMPSCALE ;
      m_width = 205 * TRUMPSCALE ;
      randomGap = rand()% 100 ; 
      m_imageScale = TRUMPSCALE ; 

    
      break ; 

  }

  m_position.set(NextBuildingPosition + m_width/2 ,
                 -BUILDINGFLOOR +m_height /2) ; 
  //set the position of the building to the center of the 
  //right of the laft plus half the next building to get the
  //centre of the image
  NextBuildingPosition = NextBuildingPosition + m_width +
                         randomGap ; 
  //set the next building to the right of the current building 
  //plus a random gap

}



void Building::Update(float frametime)
//if the building gets too far to the left of the screen 
{

  int newBuildingNumber = 0 ;
  //int for choosing a random building to spawn

  if (m_position.XValue < MyDrawEngine::GetInstance()->
                          GetViewport().GetBottomLeft().XValue - 500)
  //if the building reaches the right hand side of the screen
  {

    if (BuildingNumber != HALLCENTRE &&
        BuildingNumber != HALLRIGHT)
    {

      GameManager::SpawnNewBuilding() ; 

    }

    Deactivate() ; 
    //deactivate old building

  }
  if (m_position.XValue > MyDrawEngine::GetInstance()->
                                        GetViewport().GetBottomRight().XValue
                                        + 10000)
  //whem the buildings too far to the right, defencive 
  {

    //deactivate the building 
    Deactivate() ; 
    
  }  

}



IShape2D &Building::GetCollisionShape()
//sets teh collision shape of the buildings 
{

  Vector2D bottomLeft ; //vectors to store positions of corners of building
  Vector2D topRight ; 

  //get the values of corners 
  bottomLeft.XValue = m_position.XValue - m_width/2 ;
  bottomLeft.YValue = m_position.YValue - m_height/2 ; 

  topRight.XValue = m_position.XValue + m_width/2 ; 
  topRight.YValue = m_position.YValue + m_height/2 ; 

  //place the rectangle collision box over the building 
  collider.PlaceAt(bottomLeft,
                   topRight) ; 

  return collider ; //return new collision 

}



int Building::GetBuildingType() 
//returns the type of building
{
  
  return BuildingNumber ; 

}



void Building::ProcessCollision(GameObject &other)
{

  //no collisions 

}



void Building::SetBuildingPosition()
//sets the place to begin building spawns 
{

  //set the building start to the given area off the screen 
  NextBuildingPosition = OFFSCREENSPAWN ; 

}



float Building::GetBuildingPosition()
//returns the next position to place a building 
{

  return NextBuildingPosition ; 

}



Particles::Particles() : GameObject(PARTICLES)
{



}



void Particles::Initialise(Vector2D startPosition,
                           int totalIn)
{


  int i = 0 ; 
  timer = 0.6f ;

  totalParticles = totalIn ; 

  for (i = 0; i < totalParticles; i++)
  {

    position[i] = startPosition ;
    velocity[i].XValue = float(rand()%350 - 175) ; 
    velocity[i].YValue = float(rand()%350 - 175) ; 

  }

}



void Particles::Update(float frametime)
{
  
  int i = 0 ;

  for (i = 0; i < totalParticles; i++)
  {

    velocity[i] = velocity[i] + GRAVITY * frametime ; 
    position[i] = position[i] + velocity[i] * frametime ; 

  }

  timer = timer - frametime ; 

  if (timer <= 0)
  {

    Deactivate() ; 

  }


}



void Particles::Draw() 
{

    int i = 0;

    unsigned int colours[100];

  for (i = 0; i < totalParticles; i++)
  {

    colours[i] = MyDrawEngine::WHITE;

  }

  MyDrawEngine::GetInstance()->DrawPointList(position,
                                             colours,
                                              totalParticles);
}



IShape2D &Particles::GetCollisionShape() 
{

   
  return collider ; 

}



void Particles::ProcessCollision(GameObject &other)
{



}



Road::Road() : GameObject(ROAD)
{



}



float Road::NextRoadPosition = -2000 ; 

void Road::Initialise()
{

  m_drawDepth = 1 ; 

  LoadImage(L"Road.png") ; 
  m_height = 111 ; 
  m_width = 414 ; 

  m_position.set(NextRoadPosition + m_width/2 ,
                 -ROADFLOOR +m_height /2) ; 
  //set the position of the building to the center of the 
  //right of the laft plus half the next building to get the
  //centre of the image
  NextRoadPosition = NextRoadPosition + m_width  ; 
  //set the next building to the right of the current building 
  //plus a random gap

}



void Road::Update(float frametime)
{

  if(m_position.XValue < MyDrawEngine::GetInstance()->
                          GetViewport().GetBottomLeft().XValue 
                          - 500)
  //if the road goes off the left of the screen
  {

    Road *nextRoad = new Road ;
    //create pointer to new road
    nextRoad ->Initialise() ; 
    //initialise the road
    Game::instance.m_objects.AddItem(nextRoad,
                                     true) ; 
    //add the road to game 

    Deactivate() ; 
    //deactivate old road 

  }

}



IShape2D &Road::GetCollisionShape()
{

  Vector2D bottomLeft ; //vectors to store positions of corners of building
  Vector2D topRight ; 

  //get the values of corners 
  bottomLeft.XValue = m_position.XValue - m_width/2 ;
  bottomLeft.YValue = m_position.YValue - m_height/2 ; 

  topRight.XValue = m_position.XValue + m_width/2 ; 
  topRight.YValue = m_position.YValue + m_height/2 ; 

  //place the rectangle collision box over the building 
  collider.PlaceAt(bottomLeft,
                   topRight) ; 

  return collider ; //return new collision 

}



void Road::ProcessCollision(GameObject &other)
{

  //no collisions 

}



void Road::SetRoadPosition() 
//sets the first position for roads to spawn 
{

  NextRoadPosition = OFFSCREENSPAWN ; 

}



float Road::GetRoadPosition()
//returns the position of the next place to spawn a road 
{

  return NextRoadPosition ; 

}



Enemy::Enemy() : GameObject(ENEMY)
//constructor
{



}



void Enemy::Initialise(int typeIn, 
                       Vector2D startPosition) 
//initialise the enemy ship 
{

  type = typeIn ; 


  health = 0 ;
  m_angle = 0.0f ;               // set the initial angle of the ship 
  m_position = startPosition ;   //set position of the ship
  shootDelay = 0 ;               //sets a shoot delay for the bullet spawns

  switch (type)
  {
    case SPITFIRE :
     
      width = 128.0f ;               //width of the image
      height = 50.0f ;               //height of image
      missiles = 0 ; 

      LoadImage(L"spitfire.PNG");    //load the image 

      break ; 

    case KAMAKAZE :

      width = 128.0f ;
      height = 40.0f ;
      missiles = 0 ;

      LoadImage(L"Kamakaze.PNG") ;

      break ; 

    case RAPTOR :

      width = 128.0f ;
      height = 52.0f ;
      missiles = 3 ;
      raptorGoUp = true ; 

      LoadImage(L"Raptor.PNG");

      break ; 

  }

}



void Enemy::Update(float frametime)
{

  Vector2D vectorTo ;


  if (type == SPITFIRE)
  {

    Vector2D bulletPosition ;   //position of bullets
    // fired by ship
    Vector2D bulletVelocity ;   //veocity of bullets
    //fired by the ships 
    int shotCounter = 0 ;

    vectorTo = pThePlayer->GetPosition() ;
    vectorTo = vectorTo - m_position ;

    velocity.YValue = velocity.YValue + vectorTo.YValue * frametime ;

    if (m_position.XValue > pThePlayer->GetPosition().XValue + 500 ||
        m_position.XValue < pThePlayer->GetPosition().XValue - 500)
    {

      velocity.XValue = velocity.XValue - 5 ;

    }

      if (shootDelay <= 0)
      {

        bulletPosition.setBearing(m_angle + ROTATEHORIZANTAL,
                                  SHIPSIZE / 2) ;
        //set the position of the bullets to the angle of the ship
        //and half of theships size so that the bullet fires from half way
        //down the image of the ship looking more natural
        //add the rotation constant to the angle so that it fires from
        // the front of the ship not the top

        bulletPosition = bulletPosition + m_position ;
        // set the bullets position to the position of it plus the 
        // position of the ship so that it fires from the ship
        bulletVelocity.setBearing(m_angle - ROTATEHORIZANTAL,
                                  BULLETSPEED) ;
        //set the baring of the bullets velocity to the angle of the ship
        //and the base speed of the bullet 
        //apply the rotate constant to it is in the correct direction  
        bulletVelocity = bulletVelocity + velocity ;
        //set the velocity of the bullet to itself plus the ships
        //so that it fires with respect to the ships owm velocity

        Bullet *firedBullet = new Bullet ;
        //create new bullet through pointer 
        firedBullet->Initialise(bulletPosition,
                                bulletVelocity,
                                BAD) ;
        //initialise the new bullet to the position and velocity
        Game::instance.m_objects.AddItem(firedBullet,
                                         true) ;
        //create instance of the bullet in the game 

        shootDelay = SPITSHOTDELAY ;


      }

      shootDelay = shootDelay - frametime ;

    m_position = m_position + velocity * frametime ; 

    if (m_position.XValue < MyDrawEngine::GetInstance()->
      GetViewport().GetBottomLeft().XValue
      - 500)
    {

      Deactivate() ;

    }
  }
  else if (type == KAMAKAZE)
  {

    vectorTo = pThePlayer -> GetPosition() ;
    vectorTo = vectorTo - m_position ;

    m_position = m_position + vectorTo * frametime ; 

  }
  else if(type == RAPTOR)
  {

    if(m_position.XValue >
         MyDrawEngine::GetInstance() -> 
                       GetViewport().GetBottomRight().XValue - 400 )
    {
    
      velocity.set(50, 0) ;

    }
    else
    {
      if(missiles > 0)
      {
        if(raptorGoUp)
        {

          velocity.set(0, 50) ; 

        }
        if(!raptorGoUp)
        {

          velocity.set(0, -50) ;

        }
      }
      else if(missiles <= 0)
      {

        velocity.set(100, 0) ; 

      }
    }

    if (shootDelay <= 0)
    {

      Missile *newMissile = new Missile ; 
      newMissile -> Initialise(m_position - Vector2D(0, height/ 2 - 20)) ;

      Game::instance.m_objects.AddItem(newMissile, true) ;

      shootDelay = 5 ; 

    } 

    shootDelay = shootDelay - frametime ; 

  }
}



IShape2D &Enemy::GetCollisionShape()
//sets where the collider should be placed and returns it as needed 
{

  //vectors for the calculations for corners of rectangle 
  Vector2D bottomLeft ;
  Vector2D topRight ; 

  //sets the bottom left with half the width and height taken from the 
  //centre of the object 
  bottomLeft.set(m_position.XValue - width / 2, 
                 m_position.YValue - height / 2) ; 

  //sets the top right using height and width 
  topRight.set(m_position.XValue + width / 2, 
               m_position.YValue + height / 2) ; 

  //place rectangle collider
  collider.PlaceAt(bottomLeft,
                   topRight) ; 

  //return collider .
  return collider ; 

}



void Enemy::ProcessCollision(GameObject &other)
{

  if(other.GetType() == PLAYERSHIP)
  {

    Explosion *newExplosion = new Explosion ;

    newExplosion->Initialise(Vector2D(0, 0),
                             m_position,
                             4.5f,
                             1.05f) ;
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ;

   Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);

    Deactivate() ; 

  }
  else if (other.GetType() == BUILDING)
  {

    Explosion *newExplosion = new Explosion ;

    newExplosion->Initialise(Vector2D(0, 0),
                             m_position,
                             4.5f,
                             1.05f) ;
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ;
    Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);
    Deactivate() ;

  }

}

PlayerShip *Enemy::pThePlayer = nullptr;

void Enemy::SetShip(PlayerShip *pShip)
//sets the ship pointer to passed player ship 
{

  pThePlayer = pShip ; 

}



GameManager::GameManager() : GameObject(GAMEMANAGER)
{



}


void GameManager::Initialise() 
{

  enemyTimer = 3.4f ; 

}



void GameManager::Update(float frametime)
{

  EnemySpawn(frametime) ;

}



IShape2D &GameManager::GetCollisionShape()
{

  return collider ;

}



void GameManager::ProcessCollision(GameObject &other)
{



}

PlayerShip *GameManager::pThePlayer = nullptr;

void GameManager::SetShip(PlayerShip *pShip)
{

  pThePlayer = pShip;

}

void GameManager::SpawnStartBuildings()
//spawns the buildings at the start of the game 
{

  srand((unsigned int)time(NULL)) ; 
  //seed the random number generator

  int buildingNumber = 0 ; //holds random number to choose buildings

  while(Building::GetBuildingPosition() < MyDrawEngine::GetInstance()->
                                         GetViewport().GetBottomRight().XValue
                                         + 2000)
  //loop until buildings off screen
  {
    //spawn random building
    SpawnNewBuilding() ; 

  }

}



void GameManager::StartSpawnRoads()
//sub program spawns roads at start of game 
{

  while(Road::GetRoadPosition() < MyDrawEngine::GetInstance()->
                                 GetViewport().GetBottomRight().XValue
                                 + 1000)
  //loop until road spreads across screen 
  {

    Road *nextRoad = new Road ; 
    nextRoad -> Initialise() ; 

    Game::instance.m_objects.AddItem(nextRoad,
                                     true) ; 
    //spawns roads

  }

}

float GameManager::nextBGPosition = 0 ; 

void GameManager::CreateBackground()
{

  MyDrawEngine::GetInstance() -> UseCamera(false) ; 

  MyDrawEngine::GetInstance() -> LoadPicture(L"skyline.png") ;

  while(nextBGPosition < MyDrawEngine::GetInstance()->
                                       GetViewport().GetBottomRight().XValue
                                       + 1000)
  //loop until scene spreads across screen 
  {

    MyDrawEngine::GetInstance() -> DrawAt(Vector2D(880,nextBGPosition),
                                          MyDrawEngine::GetInstance() ->
                                          FindPicture(L"skyline.png")) ; 
    
    GameManager::nextBGPosition = GameManager::nextBGPosition + SKYLINESIZE ; 

  }

  MyDrawEngine::GetInstance() -> UseCamera(true) ; 

}



void GameManager::InitialSetup() 
{

  Building::SetBuildingPosition() ; 
  Road::SetRoadPosition() ; 

  StartSpawnRoads() ; 
  SpawnStartBuildings() ; 
  CreateBackground() ; 

}



void GameManager::Draw()
{



}



void GameManager::SpawnNewBuilding()
{


  int newBuildingNumber = rand() % NUMBEROFBUILDINGTYPES ;

  if (newBuildingNumber == 0)
  //spawn a new building randomly 
  {
    Building *pBuilding = new Building ;
    pBuilding->Initialise(HALLLEFT) ;

    Game::instance.m_objects.AddItem(pBuilding,
                                     true) ;

    Building *pBuilding1 = new Building ;
    pBuilding1->Initialise(HALLCENTRE) ;

    Game::instance.m_objects.AddItem(pBuilding1,
                                     true) ;

    Building *pBuilding2 = new Building ;
    pBuilding2->Initialise(HALLRIGHT) ;
    Game::instance.m_objects.AddItem(pBuilding2,
                                     true) ;

  }
  else if (newBuildingNumber == 1)
  {

    Building *pBuilding = new Building ;
    pBuilding->Initialise(FUELSTATION) ;

    Game::instance.m_objects.AddItem(pBuilding,
                                     true) ;

  }
  else if (newBuildingNumber == 2)
  {

    Building *pBuilding = new Building;
                          pBuilding->Initialise(TRUMPTOWER) ;

    Game::instance.m_objects.AddItem(pBuilding,
                                     true) ; 


   
  }

}
  

float GameManager::enemyTimer = 0.0f ; 


void GameManager::EnemySpawn(float frametime)
{

  Vector2D spawnLocation ; 
  int whatEnemy = 0 ; 

  if(enemyTimer <= 0)
  {
  //when the score is below 500 spawn a basic enemy to the screen
  //with a random Y value and X being off the screen 
  
    if (PlayerShip::getScore() <= 300) 
    {

      spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                           GetViewport().GetBottomRight().XValue
                                           + 1000 ;

      spawnLocation.YValue = (float)(rand() % 250 - 500) ; 

      Enemy *SpawnSpit = new Enemy ;
      SpawnSpit -> Initialise(SPITFIRE,
                              spawnLocation) ;

      Game::instance.m_objects.AddItem(SpawnSpit,
                                       true) ;

      enemyTimer = float(rand()% 7 + 3) ;
      
    }
    
    else if (PlayerShip::getScore() <= 500 &&
             PlayerShip::getScore() > 300) 
    {  
      spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                           GetViewport().GetBottomRight().XValue
                                           + 1000 ;

      spawnLocation.YValue = float(rand() % 250 - 500) ; 

      Enemy *SpawnSpit = new Enemy ;
      SpawnSpit -> Initialise(SPITFIRE,
                              spawnLocation) ;

      Game::instance.m_objects.AddItem(SpawnSpit,
                                       true) ;

      enemyTimer = float(rand()% 4 + 2) ;
      
    }
    
    else if(PlayerShip::getScore() <= 700 &&
             PlayerShip::getScore() > 500) 
    {

      spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                           GetViewport().GetBottomRight().XValue
                                           + 1000 ;

      spawnLocation.YValue = float(rand() % 500) ; 

      Enemy *SpawnKama = new Enemy ;
      SpawnKama -> Initialise(KAMAKAZE,
                              spawnLocation) ;

      Game::instance.m_objects.AddItem(SpawnKama,
                                       true) ;

      enemyTimer = float(rand()% 7 + 3) ;      

    }
    else if (PlayerShip::getScore() <= 1000 &&
             PlayerShip::getScore() > 700)
    {

      whatEnemy = rand() % 2 ; 

      if (whatEnemy == 0)
      {  
          spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                               GetViewport().GetBottomRight().XValue
                                               + 1000 ;

          spawnLocation.YValue = float(rand() % 250 - 500) ; 

          Enemy *SpawnSpit = new Enemy ;
          SpawnSpit -> Initialise(SPITFIRE,
                                  spawnLocation) ;

          Game::instance.m_objects.AddItem(SpawnSpit,
                                           true) ;

          enemyTimer = float(rand()% 4 + 2) ;
      }
      else if (whatEnemy == 1)
      {
          spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                               GetViewport().GetBottomRight().XValue
                                               + 1000 ;

          spawnLocation.YValue = float(rand() % 500) ; 

          Enemy *SpawnKama = new Enemy ;
          SpawnKama -> Initialise(KAMAKAZE,
                                  spawnLocation) ;

          Game::instance.m_objects.AddItem(SpawnKama,
                                           true) ;

          enemyTimer = float(rand()% 7 + 3) ;  
                       

      }

    } 
    else if (PlayerShip::getScore() <= 1200 &&
             PlayerShip::getScore() > 1000)
    {

        spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                             GetViewport().GetBottomRight().XValue
                                             + 1000 ;

        spawnLocation.YValue = float(rand() % 500) ; 

        Enemy *Spawnraptor = new Enemy ;
        Spawnraptor -> Initialise(RAPTOR,
                                  spawnLocation) ;

        Game::instance.m_objects.AddItem(Spawnraptor,
                                         true) ;

        enemyTimer = float(rand()% 7 + 3) ;          

    }
    else if (PlayerShip::getScore() > 1200)
    {

      whatEnemy = rand() % 3 ; 

      if (whatEnemy == 0)
      { 
          spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                               GetViewport().GetBottomRight().XValue
                                               + 1000 ;

          spawnLocation.YValue = float(rand() % 250 - 500) ; 

          Enemy *SpawnSpit = new Enemy ;
          SpawnSpit -> Initialise(SPITFIRE,
                                  spawnLocation) ;

          Game::instance.m_objects.AddItem(SpawnSpit,
                                           true) ;

          enemyTimer = float(rand()% 4 + 2) ;
           
      }
      if (whatEnemy == 1)
      { 
          spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                               GetViewport().GetBottomRight().XValue
                                               + 1000 ;

          spawnLocation.YValue = float(rand() % 500) ; 

          Enemy *SpawnKama = new Enemy ;
          SpawnKama -> Initialise(KAMAKAZE,
                                  spawnLocation) ;

          Game::instance.m_objects.AddItem(SpawnKama,
                                           true) ;

          enemyTimer = float(rand()% 5 + 3) ;  
           
       }
       if (whatEnemy == 2)
       {
          spawnLocation.XValue = MyDrawEngine::GetInstance()->
                                               GetViewport().GetBottomRight().XValue
                                               + 1000 ;

          spawnLocation.YValue = float(rand() % 500) ; 

          Enemy *Spawnraptor = new Enemy ;
          Spawnraptor -> Initialise(RAPTOR,
                                    spawnLocation) ;

          Game::instance.m_objects.AddItem(Spawnraptor,
                                           true) ;

          enemyTimer = float(rand()% 7 + 3) ;      
                              
      }

    }  
    
  }

  enemyTimer = enemyTimer - frametime ; 
  
}



Explosion::Explosion() : GameObject(EXPLOSION)
{



}



void Explosion::Initialise(Vector2D velocityIn,
                           Vector2D position,
                           float animationSpeedIn,
                           float scale)
{

  velocity = velocityIn;
  m_position = position;
  animationSpeed = animationSpeedIn;
  currentAnimation = 0.0f;
  m_imageScale = scale;       

  LoadImage(L"explosion1.png");
  LoadImage(L"explosion2.png");
  LoadImage(L"explosion3.png");
  LoadImage(L"explosion4.png");
  LoadImage(L"explosion5.png");
  LoadImage(L"explosion6.png");
  LoadImage(L"explosion7.png");
  LoadImage(L"explosion8.png");
  LoadImage(L"explosion9.png");
  LoadImage(L"explosion10.png");
  LoadImage(L"explosion11.png");
  LoadImage(L"explosion12.png");
  LoadImage(L"explosion13.png");
  LoadImage(L"explosion14.png");
  LoadImage(L"explosion15.png");
  LoadImage(L"explosion16.png");

  explosionSound = MySoundEngine::GetInstance() -> LoadWav(L"Explosionsound.wav") ;
  MySoundEngine::GetInstance() -> Play(explosionSound, 
                                       false) ;

}



void Explosion::Update(float frametime)
{

  currentAnimation = currentAnimation + animationSpeed * frametime ;

  if (currentAnimation >= 16.0f)
  {

    Deactivate() ;
    currentAnimation = 0 ;

  }

  m_position = m_position + velocity * frametime ;

  m_imageNumber = currentAnimation ;

}



IShape2D &Explosion::GetCollisionShape()
{

  collider.PlaceAt(m_position,
                   128.0f * m_imageScale) ;

  return collider ; 

}



void Explosion::ProcessCollision(GameObject &other)
{



}



Missile::Missile() : GameObject(MISSILE)
{



}

PlayerShip *Missile::pThePlayer = nullptr;

void Missile::Initialise(Vector2D startPosition)
{

  m_angle = 0.0f ;
  m_position = startPosition ;

  width = 245 ; 
  height = 45 ; 

  m_imageScale = 0.3 ;
  
  LoadImage(L"missile.png"); 

}



void Missile::Update(float frametime)
{

    vectorTo = pThePlayer -> GetPosition() ;
    vectorTo = vectorTo - m_position ;

    m_position = m_position + vectorTo * frametime ; 

}



IShape2D &Missile::GetCollisionShape()
{

  //vectors for the calculations for corners of rectangle 
  Vector2D bottomLeft ;
  Vector2D topRight ; 

  //sets the bottom left with half the width and height taken from the 
  //centre of the object 
  bottomLeft.set(m_position.XValue - width / 2, 
                 m_position.YValue - height / 2) ; 

  //sets the top right using height and width 
  topRight.set(m_position.XValue + width / 2, 
               m_position.YValue + height / 2) ; 

  //place rectangle collider
  collider.PlaceAt(bottomLeft,
                   topRight) ; 

  //return collider .
  return collider ; 

}




void Missile::ProcessCollision(GameObject &other)
{

  if(other.GetType() == PLAYERSHIP)
  {

    Explosion *newExplosion = new Explosion ;

    newExplosion->Initialise(Vector2D(0, 0),
                             m_position,
                             4.5f,
                             1.05f) ;
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ;

   Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);
    Deactivate() ; 

  }
  else if (other.GetType() == BUILDING)
  {

    Explosion *newExplosion = new Explosion ;

    newExplosion->Initialise(Vector2D(0, 0),
                             m_position,
                             4.5f,
                             1.05f) ;
    Game::instance.m_objects.AddItem(newExplosion,
                                     true) ;
    Particles *newEmitter = new Particles;

    newEmitter->Initialise(m_position, 100);
    Game::instance.m_objects.AddItem(newEmitter,
                                     false);
    Deactivate() ;

  }

}



void Missile::SetShip(PlayerShip *pShip)
{

  pThePlayer = pShip ; 

}



Brick::Brick() : GameObject(BRICK) 
{


}



void Brick::Initialise(Vector2D position)
{

  height = 2 ; 
  width = 4 ; 
  m_imageScale = 10 ;
  LoadImage(L"brick.png") ; 

}



void Brick::Update(float frametime)
{



}



IShape2D &Brick::GetCollisionShape() 
{

  //sets the bottom left with half the width and height taken from the 
  //centre of the object 
  bottomLeft.set(m_position.XValue - width / 2, 
                 m_position.YValue - height / 2) ; 

  //sets the top right using height and width 
  topRight.set(m_position.XValue + width / 2, 
               m_position.YValue + height / 2) ; 

  //place rectangle collider
  collider.PlaceAt(bottomLeft,
                   topRight) ; 

  //return collider .
  return collider ; 

}



void Brick::ProcessCollision(GameObject &other)
{

  if(other.GetType() == BULLET)
  {

    Deactivate() ; 
  
  }

}

