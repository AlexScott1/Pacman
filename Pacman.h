#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif
#define MUNCHIECOUNT 100
#define BLUEGHOSTS 5
#define PINKGHOSTS 5
#define CHERRYCOUNT 100
#define BATTERYCOUNT 5

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
struct Player
{
	//Vector2* position;
	Rect* posRect;
	Rect* sourceRect;
	Texture2D* texture;
	int direction;
	int frame;
	int currentFrameTime;
	float speedMultiplier;
	bool dead;
	int munchieseaten;
	int lives;
	int spawnTimer;
	bool respawn;
	int levelN;
};

struct Munchie
{
	int frameCount;
	int currentFrameTime;
	Rect* sourceRect;
	Rect* posRect;
	Texture2D* blueTexture;
	Texture2D* invertedTexture;
	int frameTime;
	bool eaten;
	
	
};

struct Cherry
{
	int frameCount;
	int currentFrameTime;
	Rect* sourceRect;
	Rect* posRect;
	Texture2D* texture1;
	Texture2D* texture2;
	int frameTime;
	bool eaten;

};

struct Battery
{
	Rect* sourceRect;
	Rect* posRect;
	Texture2D* texture;
	bool eaten;
	
};

struct GhostBlue
{
	Rect* posRect;
	Rect* sourceRect;
	Texture2D* texture;
	Texture2D* wtexture;
	int direction;
	float speed;
	int toX;
	int toY;
	bool isWandering;
	int invert;
	int update;
};

struct GhostPink
{
	Rect* posRect;
	Rect* sourceRect;
	Texture2D* texture;
	Texture2D* wtexture;
	int direction;
	float speed;
	int targetX;
	int targetY;
	bool isWandering;
};


class Pacman : public Game
{
private:
	// Data to represent Pacman
	Player* _pacman;

	// Data to represent Munchie
	Munchie* _munchies[MUNCHIECOUNT];

	// Data to represent Cherry
	Cherry* _cherry[CHERRYCOUNT];

	// Data to represent Chasing Enemy
	GhostBlue* _bGhost[BLUEGHOSTS];

	// Data to represent Patroling Enemy
	GhostPink* _pGhost[PINKGHOSTS];

	// Data to represent moving collectable
	Battery* _battery[BATTERYCOUNT];

	// Position for String
	Vector2* _stringPosition;
	Vector2* _stringPosition2;
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;
	const int _cMunchieFrameTime;
	const int _cCherryFrameTime;
	int _level;


	// Data for PMenu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	Vector2* _deathStringPosition;
	bool _paused;
	bool _stopped;
	bool _pKeyDown;

	// Data for SMenu
	Texture2D* _sMenuBackground;
	Rect* _sMenuRectangle;
	Texture2D* _sMenuObj;
	Rect* _sMenuObjBack;
	Vector2* _sMenuStringPosition;
	bool _started;
	bool _started2;
	bool _sKeyDown;
	
	// Data for DMenu
	Texture2D* _dMenuBackground;
	Rect* _dMenuRectangle;
	Vector2* _dMenuStringPosition;
	bool _gameover;
	bool _ssKeyDown;
	int _respawn;

	// Data for sound
	SoundEffect* _pop;
	SoundEffect* _gameStart;
	SoundEffect* _nom;
	SoundEffect* _pacmanDies;
	SoundEffect* _win;

	int overheadCount;
	int totMunchies;
	
	int scoreMultiplier;
	bool nextLevel;
	public:

	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

	/// <summary> Input definition </summary>
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);
	
	/// <summary> Draw </summary>
	void DrawMunchie(int elapsedTime);
	void DrawCherry(int elapsedTime);
	void DrawBlueGhost(int elapsedTime);
	void DrawPinkGhost(int elapsedTime);
	void DrawBattery(int elapsedTime);
	
	/// <summary> Check Methods </summary>
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::MouseState* mouseState);
	void CheckViewportCollision();

	/// <summary> Update Methods </summary>
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime);
	void UpdateCherry(int elapsedTime);
	void UpdateBlueGhosts(int elapsedTime);
	void UpdatePinkGhosts(int elapsedTime);
	

	/// <summary> Moving Enemy </summary>
	void CheckGhostCollisions();
	void WanderingGhost(int elapsedTime);

	/// <summary> Respawn </summary>
	//void RespawnPlayer(int elapsedTime);
	
};