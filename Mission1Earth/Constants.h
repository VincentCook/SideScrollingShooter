#ifndef Constants_H
#define Constants_H

#include "gamecode.h"
#include "mydrawengine.h"
#include "mysoundengine.h"
#include "myinputs.h"
#include <time.h>
#include "gametimer.h"
#include "errorlogger.h"
#include <math.h>
#include "shapes.h"
#include "ObjectManager.h"
#include "entities.h"
#include "constants.h"

typedef const int BuildingType ;
typedef Vector2D HudPosition ; 
//list of constants mostly used toi make the code more readable 
//and easy to change when needed without searching theorugh the code.
//some only used once but still aid in abstraction of the code 
const float ROTATESPEED = 2.0f ; 
const float ACCELERATION = 1600.0f ;     //the change in speed of
                                         // the objects in game thrusting
const float FRICTION = 2.0f ;            //the games friction used to 
                                         //slow movement of objects 
const float SHIPSIZE = 128.0f /128 ;     //the size of ships image
const float COLLIDERSIZE = 64.0f ;       //size of collider for flames 
const Vector2D SHIPDISPLACEMENT(50,0) ;  //vector from ship to draw behind it 
const float ROTATEHORIZANTAL = 1.5708f ; //rotation to draw objects the correct
                                         // way around
const float FLAMEANIMSPEED = 60.0f ;     //speed of animations used for flames
const float FLAMESCALE = 0.4f ;          //scale of flames
const float FLAMEMAGNITUDE = 600.0f ;    //magnitude for drawing flames rotated
const float BULLETSPEED = 1500 ;         //speed of bullets 
const float BULLETDELAY = 0.5f ;         //delay in frames beween bullets
const Vector2D GRAVITY(0, -250) ;        //gravity 
const int NUMBEROFBUILDINGTYPES = 2 ;    //number of possible buildings 
const int BUILDINGFLOOR = 889 ;          //height the buildings should spawn
const int ROADFLOOR = 1000 ;             //height road should spawn  
BuildingType HALLCENTRE = 0 ; 
BuildingType HALLLEFT= 1 ;
BuildingType HALLRIGHT = 2 ; 
BuildingType FUELSTATION = 3 ;


#endif