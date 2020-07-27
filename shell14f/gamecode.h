//  *********************************************************************************
#pragma once
#include "errortype.h"
#include "windows.h"
#include "ObjectManager.h"
#include "event.h"

// For reading keyboard
#define KEYPRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)




class Game
{
private:
	enum GameState{MENU, PAUSED, RUNNING, GAMEOVER};
	GameState m_currentState;
	void ChangeState(GameState newState);
	int m_menuOption;
	Game();
	~Game();
	Game(Game& other);
public:
	static Game instance;
	ObjectManager m_objects;
	ErrorType Setup(bool bFullScreen, HWND hwnd, HINSTANCE hinstance);
	void Shutdown();
	ErrorType Main();

	ErrorType PauseMenu();
	ErrorType MainMenu();
	ErrorType StartOfGame();
	ErrorType Update();
	ErrorType EndOfGame();
	void NotifyEvent(Event evt);
};




