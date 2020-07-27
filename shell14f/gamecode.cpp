// GameCode.cpp		

// Version  13.03	Draw Engine no longer requires a height / width
// Version 13.0   Update to wchar_t and wstring

//Version 11			5/3/08	
// These three functions form the basis of a game loop in the window created in the
// wincode.cpp file

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
#include <fstream>

const int BGDIMENSION = 2048 ; 
const int SKYLINESIZE = 512;


void gameTitles()
//loads the title images for use in menus
{

  MyDrawEngine::GetInstance()->UseCamera(false);
  MyDrawEngine::GetInstance() -> LoadPicture(L"MissionLogo.png") ; 
  //load title image to memory

  MyDrawEngine::GetInstance() -> DrawAt(Vector2D(1000,300),
                                        MyDrawEngine::GetInstance() ->
                                          FindPicture(L"MissionLogo.png"),
                                          0.5f) ; 
  //draw title image to screen 
  MyDrawEngine::GetInstance()->UseCamera(true);

}

void menuBackgrounds()
//loads background for the menus 
{

	float nextXBackground = 0 ; //next X for tiling background
  float nextYBackground = 0 ; //next Y for tiling

  nextXBackground = MyDrawEngine::GetInstance() ->
                    GetViewport().GetBottomLeft().XValue - 500 ;
  //gets the boundry of the screen to set where first image should
  //be drawn

  nextYBackground = MyDrawEngine::GetInstance() ->
                    GetViewport().GetTopRight().YValue + 500 ; 
  //gets the boundry of the screen to set where the first image should
  //be drawn

  MyDrawEngine::GetInstance() -> LoadPicture(L"scratchedmetal.PNG") ; 
  //load image for the background 

  while(nextYBackground >  MyDrawEngine::GetInstance() ->
                                         GetViewport().GetBottomRight().YValue
                                         - 500) 
  //from top to bottom of the screen 
  {

    while(nextXBackground < MyDrawEngine::GetInstance() ->
          GetViewport().GetBottomRight().XValue + 500)
    //from left to right
    {

      MyDrawEngine::GetInstance() -> DrawAt(Vector2D(nextXBackground,
                                                     nextYBackground),
                                            MyDrawEngine::GetInstance() ->
                                              FindPicture
                                                (L"scratchedmetal.PNG")) ; 
      //draw background image

      nextXBackground = nextXBackground + BGDIMENSION ; 
      //add the width for next image
  
    }

    nextYBackground = nextYBackground - BGDIMENSION ;
    //take the height for next image
    nextXBackground = MyDrawEngine::GetInstance() ->
                    GetViewport().GetBottomLeft().XValue - 500 ;
    //reset X value 

  }

}



ErrorType Game::Main()
// Called repeatedly - the game loop
{

	//Flip and clear the back buffer
	MyDrawEngine* pTheDrawEngine= MyDrawEngine::GetInstance();
	pTheDrawEngine->Flip();
	pTheDrawEngine->ClearBackBuffer();

	ErrorType err;

	switch(m_currentState)
	{

	case MENU:

		err= MainMenu();

		break;
	case PAUSED:

		err = PauseMenu();
		break;
	case RUNNING:


		err= Update();
		break;

	case GAMEOVER:

		err = FAILURE;
    break ; 
	default:
		// Not a valid state
		err = FAILURE;
    break ;
	}

	return err;
}

void Game::ChangeState(GameState newState)
{
	// Very crude state system
	// Close old state
	switch(m_currentState)
	{

	case MENU:

		break;
	case PAUSED:

		break;
	case RUNNING:

		break;

	}

	// Change the state
	m_currentState = newState;
	m_menuOption = 0;

	// Transition to new state
	switch(m_currentState)
	{

	case MENU:

		break;
	case PAUSED:

		break;
	case RUNNING:

		break;

	}

}

ErrorType Game::Setup(bool bFullScreen,
                      HWND hwnd,
                      HINSTANCE hinstance)
// Called once before entering game loop. 
{
	// Create the engines - this should be done before creating other DDraw objects
	if(FAILED(MyDrawEngine::Start(hwnd,
                                bFullScreen)))
	{

		ErrorLogger::Writeln(L"Failed to start MyDrawEngine");

		return FAILURE;

	}
	if(FAILED(MySoundEngine::Start(hwnd)))
	{

		ErrorLogger::Writeln(L"Failed to start MySoundEngine");

		return FAILURE;

	}
	if(FAILED(MyInputs::Start(hinstance,
                            hwnd)))
	{

		ErrorLogger::Writeln(L"Failed to start MyInputs");
		return FAILURE;

	}

	return (SUCCESS);

}

void Game::Shutdown()
// Called once before entering game loop. 
{
	m_objects.DeleteAllObjects();

	// (engines must be terminated last)
	MyDrawEngine::Terminate();
	MySoundEngine::Terminate();
	MyInputs::Terminate();
}
// **********************************************************************************************
// The game !!! *********************************************************************************
// **********************************************************************************************




ErrorType Game::PauseMenu()
{
	// Code for a basic pause menu

	const int NUMOPTIONS = 2;
	wchar_t options[NUMOPTIONS][11] = {L"Resume",
                                     L"Main menu"};
  int centreX = 0 ; //stores centre value of the screen 

  centreX = MyDrawEngine::GetInstance() -> GetScreenWidth()/2 
                                           -10; 
  //finds the centre of the screen 
  
  menuBackgrounds() ; //load background

  gameTitles() ;      //load the title image

	MyDrawEngine::GetInstance()->WriteText(centreX,
                                         600,
                                         L"Paused",
                                         MyDrawEngine::WHITE);


	for(int i=0;i<NUMOPTIONS;i++)
	{

		int colour = MyDrawEngine::GREY;

		if(i == m_menuOption)
		{

			colour = MyDrawEngine::WHITE;

		}

		MyDrawEngine::GetInstance()->WriteText(centreX,
                                           680 + 50*i,
                                           options[i],
                                           colour);
	}

	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard();

	if(pInputs->NewKeyPressed(DIK_UP))
	{

		m_menuOption--;

	}
	if(pInputs->NewKeyPressed(DIK_DOWN))
	{

		m_menuOption++ ;

	}
	if(m_menuOption < 0)
	{
		m_menuOption = 0 ;
	}
	else if(m_menuOption >= NUMOPTIONS)
	{
		m_menuOption = NUMOPTIONS-1 ;
	}

	if(pInputs->NewKeyPressed(DIK_RETURN))
	{
		if(m_menuOption == 0)
		{
			ChangeState(RUNNING) ;
		}
		if(m_menuOption == 1)
		{
			EndOfGame();
			ChangeState(MENU) ;
		}

	}

	return SUCCESS ;

}



ErrorType Game::MainMenu()
{
	// Code for a basic main menu

  const int NUMOPTIONS = 3 ;
	wchar_t options[NUMOPTIONS][15] = {L"Start game",
                                     L"Go Fullscreen",
                                     L"Exit"} ;
  int centreX = 0 ; 

	menuBackgrounds() ; 

  gameTitles() ;

  centreX = MyDrawEngine::GetInstance() -> GetScreenWidth()/2 
                                           -10 ; 



	MyDrawEngine::GetInstance() -> WriteText(centreX,
                                         600,
                                         L"Main menu",
                                         MyDrawEngine::WHITE) ;

	if(MyDrawEngine::GetInstance() -> IsWindowFullScreen())
	{

		wcscpy_s(options[1],
             15,
             L"Go Windowed") ;
	}

	for(int i = 0; i<NUMOPTIONS; i++)
	{

		int colour = MyDrawEngine::GREY ;

		if(i == m_menuOption)
		{

			colour = MyDrawEngine::WHITE ;

		}

		MyDrawEngine::GetInstance()->WriteText(centreX ,
                                           680+50*i,
                                           options[i],
                                           colour) ;
	}

	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard() ;

	if(pInputs -> NewKeyPressed(DIK_UP))
	{

		m_menuOption-- ;

	}
	if(pInputs -> NewKeyPressed(DIK_DOWN))
	{

		m_menuOption++ ;

	}
	if(m_menuOption < 0)
	{

		m_menuOption=0 ;

	}
	else if(m_menuOption >= NUMOPTIONS)
	{

		m_menuOption = NUMOPTIONS-1 ;

	}

	if(pInputs -> NewKeyPressed(DIK_RETURN))
	{
		if(m_menuOption == 0)
		{

			StartOfGame();
			ChangeState(RUNNING) ;

		}

		if(m_menuOption == 1)
		{
			if(MyDrawEngine::GetInstance() -> IsWindowFullScreen())
			{

				MyDrawEngine::GetInstance() -> GoWindowed() ;

			}
			else
			{

				MyDrawEngine::GetInstance() -> GoFullScreen() ;

			}
		}
		if(m_menuOption == 2)
		{

			ChangeState(GAMEOVER) ;

		}

	}

	return SUCCESS ;
}



const int NUMBEROFBUILDINGTYPES = 3 ; //number of possible buildings 



void SpawnStartBuildings()
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
    buildingNumber = rand()%NUMBEROFBUILDINGTYPES ; 

    if(buildingNumber == 0)
    //chooses building determined by random number 
    {
      //spawns buildings into game 

        Building *pBuilding = new Building ;
        pBuilding -> Initialise(1) ;
  
        Game::instance.m_objects.AddItem(pBuilding,
                                         true) ; 
  
        Building *pBuilding1 = new Building ;
        pBuilding1 -> Initialise(0) ; 

        Game::instance.m_objects.AddItem(pBuilding1,
                                         true) ; 

        Building *pBuilding2 = new Building ;
        pBuilding2 -> Initialise(2) ;

        Game::instance.m_objects.AddItem(pBuilding2,
                                         true) ;   
    }
    else if(buildingNumber == 1)
    {
        Building *pBuilding = new Building ; 
        pBuilding -> Initialise(3) ; 

        Game::instance.m_objects.AddItem(pBuilding,
                                         true) ; 
        
    }
    else if (buildingNumber == 2)
    {

      Building *pBuilding = new Building ;
      pBuilding -> Initialise(4) ; 

      Game::instance.m_objects.AddItem(pBuilding,
                                       true) ; 

    }

  }

}



void startSpawnRoads()
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



ErrorType Game::StartOfGame()
{
	// Code to set up your game

  PlayerShip *playerOne = new PlayerShip ; 
  //create the player ship with pointer
  playerOne -> Initialise(Vector2D(0,125)) ; 
  //initialise the playership

  GameManager *pManager = new GameManager ;
  pManager -> Initialise() ; 

  Enemy::SetShip(playerOne) ;
  GameManager::SetShip(playerOne) ; 

  Missile::SetShip(playerOne) ; 
 
   m_objects.AddItem(playerOne,
                     true) ; 

   m_objects.AddItem(pManager,
                     true) ;


  //add playership to thegame 

  GameManager::InitialSetup() ; 
  
	return SUCCESS ;

}



ErrorType Game::Update()
{

	m_objects.UpdateAll() ;
	m_objects.ProcessCollisions() ;
	m_objects.DrawAll() ;

  MyDrawEngine* pDrawEngine = MyDrawEngine::GetInstance() ;


	// Garbage collect any objects. Possibly slow
	m_objects.DeleteInactiveItems() ;

	// Check for entry to pause menu
	static bool escapepressed = true ;

	if(KEYPRESSED(VK_ESCAPE))
	{
		if(!escapepressed)
    {

			ChangeState(PAUSED) ;

    }

		escapepressed=true ;

	}
	else
  {

		escapepressed=false ;

  }
	// Any code here to run the game,
	// but try to do this within a game object if possible

	return SUCCESS;

}



ErrorType Game::EndOfGame()
// called when the game ends by returning to main menu
{

	m_objects.DeleteAllObjects();

	return SUCCESS;

}



Game::Game()
{

	// No-op

}



Game::~Game()
{

	// No-op

}



void Game::NotifyEvent(Event evt)
{

	m_objects.HandleEvent(evt);

}



Game Game::instance;