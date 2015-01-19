#include "Pacman.h"
#include <time.h>
#include <sstream>
#include <iostream>
#include <fstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250), _cMunchieFrameTime(500), _cCherryFrameTime(500)
{
	srand(time(NULL));
	_pacman = new Player();
	_pop = new SoundEffect();
	_nom = new SoundEffect();
	_gameStart = new SoundEffect();
	_pacmanDies = new SoundEffect();
	_win = new SoundEffect();

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Munchie();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = 0;
		_munchies[i]->frameTime = rand() % 500 + 50;
	}

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherry[i] = new Cherry();
		_cherry[i]->currentFrameTime = 0;
		_cherry[i]->frameCount = 0;
		_cherry[i]->frameTime = rand() % 500 + 50;
	}
	
	for (int i = 0; i < BATTERYCOUNT; i++)
	{
		_battery[i] = new Battery();
	}

	_paused = false;
	_pKeyDown = false;
	_stopped = false;
	nextLevel = false;

	_started = false;
	_started2 = false;
	_sKeyDown = false;

	overheadCount = 0;
	totMunchies = 5;
	
	_level = 1;
	scoreMultiplier = 1;

	_pacman->speedMultiplier = 1.0f;
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->dead = false;
	_pacman->munchieseaten = 0;
	_pacman->lives = 3;
	_pacman->respawn = true;
	_pacman->levelN = 1;

	for (int a = 0; a < BLUEGHOSTS; a++)
	{
		_bGhost[a] = new GhostBlue();
		_bGhost[a]->direction = rand() % 3 + 0;
		_bGhost[a]->speed = 1.5 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(3-1.5)));
		_bGhost[a]->isWandering = false;
		_bGhost[a]->toX = 350;
		_bGhost[a]->toY = 350;
		_bGhost[a]->invert = 0;
		_bGhost[a]->update = 0;
	}

	for (int a = 0; a < PINKGHOSTS; a++)
	{
		_pGhost[a] = new GhostPink();
		_pGhost[a]->direction = rand() % 3 + 0;
		_pGhost[a]->speed = 0.2f;
		_pGhost[a]->isWandering = false;
		//_pGhost[a]->speed = 0.5 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2-0.1)));
	}

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Ultimate Pacman!", 60);
	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE),CONSOLE_FULLSCREEN_MODE,0);
	Audio::Play(_gameStart);
	Input::Initialise();
	
	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->texture;
	delete _pacman->sourceRect;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i]->blueTexture;
		delete _munchies[i]->invertedTexture;
		delete _munchies[i]->sourceRect;
	}

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		delete _cherry[i]->texture1;
		delete _cherry[i]->texture2;
		delete _cherry[i]->sourceRect;
	}

	for (int i = 0; i < BLUEGHOSTS; i++)
	{
		delete _bGhost[i]->texture;
		delete _bGhost[i]->sourceRect;
	}

	for (int i = 0; i < PINKGHOSTS; i++)
	{
		delete _pGhost[i]->texture;
		delete _pGhost[i]->sourceRect;
	}

	for (int i = 0; i < BATTERYCOUNT; i++)
	{
		delete _battery[i]->texture;
		delete _battery[i]->sourceRect;
	}
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->posRect = new Rect(350.0f, 350.0f, 32, 32);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	
	// Load Sounds
	_pop->Load("Sounds/pop.wav"); 	_gameStart->Load("Sounds/PacmanIntro.wav");	_nom->Load("Sounds/nom.wav");	_pacmanDies->Load("Sounds/PacManDies.wav");	_win->Load("Sounds/Win.wav");	
	// Load Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->blueTexture = new Texture2D();
		_munchies[i]->blueTexture->Load("Textures/Munchie.tga", true);
		_munchies[i]->invertedTexture = new Texture2D();
		_munchies[i]->invertedTexture->Load("Textures/MunchieInverted.tga", true);
		_munchies[i]->sourceRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 12, 12);
	}

	// Load Cherry
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherry[i]->texture1 = new Texture2D();
		_cherry[i]->texture1->Load("Textures/Cherry.png", true);
		_cherry[i]->texture2 = new Texture2D();
		_cherry[i]->texture2->Load("Textures/CherryInverted.png", true);
		_cherry[i]->sourceRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 12, 12);
	}

	// Load the battery
	for (int i = 0; i < BATTERYCOUNT; i++)
	{
		_battery[i]->texture = new Texture2D();
		_battery[i]->texture->Load("Textures/Powerup.png", true);
		_battery[i]->posRect = new Rect(rand() % Graphics::GetViewportWidth() - 50, rand() % Graphics::GetViewportHeight() - 50, 50, 50);
		_battery[i]->sourceRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 50, 50);
	}

	// Load Blue Ghost
	for (int a = 0; a < BLUEGHOSTS; a++)
	{
		_bGhost[a]->texture = new Texture2D();
		_bGhost[a]->texture->Load("Textures/GhostBlue.png", true);
		_bGhost[a]->wtexture = new Texture2D();
		_bGhost[a]->wtexture->Load("Textures/GhostOrange2.png", true);
		_bGhost[a]->posRect = new Rect(rand() % Graphics::GetViewportWidth(), Graphics::GetViewportHeight() + 20, 20, 20);
		_bGhost[a]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	// Load Pink Ghost
	for (int a = 0; a < PINKGHOSTS; a++)
	{
		_pGhost[a]->texture = new Texture2D();
		_pGhost[a]->texture->Load("Textures/GhostPink2.png", true);
		_pGhost[a]->wtexture = new Texture2D();
		_pGhost[a]->wtexture->Load("Textures/GhostOrange.png", true);
		_pGhost[a]->posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 20, 20);
		_pGhost[a]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
	_stringPosition2 = new Vector2(Graphics::GetViewportWidth() - 30, 25.0f);

	// Set PMenu Paramters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", true);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight()); 
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	_deathStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// Set SMenu Parameters
	_sMenuBackground = new Texture2D();
	_sMenuBackground->Load("Textures/StartScreen4.png", true);
	_sMenuObj = new Texture2D();
	_sMenuObj->Load("Textures/Objectives1.png", true);
	_sMenuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight()); 
	_sMenuObjBack = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_sMenuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// Set DMenu Parameters
	_dMenuBackground = new Texture2D();
	_dMenuBackground->Load("Textures/GameOver.png", true);
	_dMenuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight()); 
	_dMenuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, (Graphics::GetViewportHeight() / 2.0f) +  50);
}

void Pacman::Update(int elapsedTime)
{
	CheckGhostCollisions();
	UpdateBlueGhosts(elapsedTime);
	UpdatePinkGhosts(elapsedTime);
	UpdateMunchie(elapsedTime);
	UpdateCherry(elapsedTime);
	
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	
	Input::MouseState* mouseState = Input::Mouse::GetState();
	Input(elapsedTime, keyboardState, mouseState);
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Score: " << _pacman->munchieseaten << "\nLives left: " << _pacman->lives << "\nLevel: " << _level << "\nOrbs left: " << totMunchies << "\nMultiplier: " << scoreMultiplier;
	
	SpriteBatch::BeginDraw(); // Starts Drawing
		
	//Draws Pacman
	if ((!_pacman->dead)) 
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->posRect, _pacman->sourceRect); 
	}
	
	//Draws everything else on the screen
	if (_pacman->lives > 0)
	{
		DrawMunchie(elapsedTime);
		DrawCherry(elapsedTime);
		DrawBlueGhost(elapsedTime);
		DrawPinkGhost(elapsedTime);
		DrawBattery(elapsedTime);
	}

	//Draws the death menu when pacman is out of lives
	if (_pacman->lives < 1)
	{
		std::ifstream file("Hiscores/Top10.txt");
		std::string str;
		std::string file_contents;

		while (std::getline(file, str))
		{
			file_contents += str;
			file_contents.push_back('\n');
		} 

		std::stringstream dMenuStream; 
		dMenuStream << "GAME OVER! \nFINAL SCORE: " << _pacman->munchieseaten << "\nPress space to restart\n\nTop 5: \n" << file_contents; 
 		SpriteBatch::Draw(_dMenuBackground, _dMenuRectangle, nullptr); 
		SpriteBatch::DrawString(dMenuStream.str().c_str(), _dMenuStringPosition, Color::White); 
	}		

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	//Draws pause menu
	if ((_paused))
	{
		std::stringstream menuStream;
		if (!_stopped)
		{
			menuStream << "PAUSED!";
		}
		else
		{
			menuStream << "LEVEL COMPLETE! \nPRESS SPACE TO CONTINUE!";
		}
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	//Draws life lost screen
	if ((_stopped) && (_pacman->dead) && (!nextLevel))
	{
		std::stringstream menuStream;
		menuStream << "LIFE LOST! \nPRESS SPACE TO CONTINUE!";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	//Draws and prepares the next level
	if ((_stopped) && (totMunchies < 1) && (nextLevel))
	{
		if (_level < 5)
		{
			_level++;
			totMunchies = (_level * 5);
			for (int a = 0; a < BLUEGHOSTS; a++)
			{
				_bGhost[a]->speed -= 0.3;
			}
			for (int a = 0; a < PINKGHOSTS; a++)
			{
				_pGhost[a]->direction = rand() % 3 + 0;
				_pGhost[a]->speed += 0.05;
			}
		}
		else
		{
			totMunchies = 9999;
			for (int a = 0; a < BLUEGHOSTS; a++)
			{
				_bGhost[a]->speed += 1;

			}
		}
		scoreMultiplier++;
		std::stringstream menuStream;
		menuStream << "LEVEL COMPLETE! \nPRESS SPACE TO SPAWN!";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	
	if ((!_started) && (!_stopped))
	{ 
		//Draws the start menu
		if (!_started2)
		{
		std::stringstream sMenuStream; 
		sMenuStream << ""; 
 		SpriteBatch::Draw(_sMenuBackground, _sMenuRectangle, nullptr); 
		SpriteBatch::DrawString(sMenuStream.str().c_str(), _sMenuStringPosition, Color::Red); 
		}
		//Draws the objectives menu
		else
		{
		
		std::stringstream sMenuStream; 
		sMenuStream << ""; 
		SpriteBatch::Draw(_sMenuObj, _sMenuObjBack, nullptr); 
		SpriteBatch::DrawString(sMenuStream.str().c_str(), _sMenuStringPosition, Color::Red); 
		}
	} 	SpriteBatch::EndDraw();
}

void Pacman::WanderingGhost(int elapsedTime) //Moves the blue ghost to a random position
{
	for (int a = 0; a < BLUEGHOSTS; a++)
	{
		if (_bGhost[a]->isWandering = true)
		{
			SpriteBatch::Draw(_bGhost[a]->texture, _bGhost[a]->posRect, _bGhost[a]->sourceRect);
			_bGhost[a]->toX = rand() % 768;
			_bGhost[a]->toY = rand() % 1024;
			//_bGhost[a]->speed = 0.2f;
			if ((_bGhost[a]->posRect->X = _bGhost[a]->toX) && (_bGhost[a]->posRect->Y = _bGhost[a]->toY))
			{
				SpriteBatch::Draw(_bGhost[a]->texture, _bGhost[a]->posRect, _bGhost[a]->sourceRect);
				_bGhost[a]->isWandering = false;
			}

		}
		
	}
}

void Pacman::DrawBlueGhost(int elapsedTime) //Draws the blue ghost
{
	for (int b = 0; b < 3; b++)
	{
		for (int a = 0; a < BLUEGHOSTS; a++)
		{
			if (_bGhost[a]->invert == 0)
			{
				SpriteBatch::Draw(_bGhost[a]->texture, _bGhost[a]->posRect, _bGhost[a]->sourceRect);
			}
			else
			{
				SpriteBatch::Draw(_bGhost[a]->wtexture, _bGhost[a]->posRect, _bGhost[a]->sourceRect);
			}
		}
	}
}

void Pacman::DrawPinkGhost(int elapsedTime) //Draws the pink ghost
{
	for (int a = 0; a < PINKGHOSTS; a++)
	{
		SpriteBatch::Draw(_pGhost[a]->texture, _pGhost[a]->posRect, _pGhost[a]->sourceRect);
	}
}

void Pacman::DrawCherry(int elapsedTime) //Draws the cherry
{
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		for (int i = 0; i < overheadCount/50 ; i++)
		{
			if ((!_cherry[i]->eaten))
			{
				if (_cherry[i]->sourceRect->Intersects(*_pacman->posRect))
				{
					_cherry[i]->eaten = true;
					_pacman->munchieseaten = _pacman->munchieseaten + (scoreMultiplier * 3);
					for (int a = 0; a < BLUEGHOSTS; a++)
					{
						_bGhost[a]->speed -= 0.3f;
					}
					Audio::Play(_nom); 
				}
			
			if (_cherry[i]->frameCount == 0)
			{
				SpriteBatch::Draw(_cherry[i]->texture2, _cherry[i]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
			}
			else
			{
				SpriteBatch::Draw(_cherry[i]->texture1, _cherry[i]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
				if (_cherry[i]->frameCount >= 60)
					_cherry[i]->frameCount = 0;
			}
			}
		}
	}
}

void Pacman::DrawMunchie(int elapsedTime)
{
	for (int i = ((_level*5) - 4); i < MUNCHIECOUNT; i++)
	{
		for (int i = 0; i < overheadCount/5; i++)
		{			
			if ((!_munchies[i]->eaten))
			{
				if (_munchies[i]->sourceRect->Intersects(*_pacman->posRect))
				{
					_munchies[i]->eaten = true;
					_pacman->munchieseaten = _pacman->munchieseaten + (scoreMultiplier * 1);
					if (totMunchies > 0)
					{
						totMunchies--;
					}
					else
					{
						totMunchies = 0;
					}
					for (int a = 0; a < BLUEGHOSTS; a++)
					{	
						if (_level < 5)
						{
							_bGhost[a]->speed += 0.1f;
						}
						else
						{
							_bGhost[a]->speed += 0.2f;
						}
					}
					Audio::Play(_pop); 
				}
				
				if (_munchies[i]->frameCount == 0)
				{
					// Draws Red Munchie
					SpriteBatch::Draw(_munchies[i]->invertedTexture, _munchies[i]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
				}
				else
				{
					// Draw Blue Munchie
					SpriteBatch::Draw(_munchies[i]->blueTexture, _munchies[i]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
					if (_munchies[i]->frameCount >= 60)
						_munchies[i]->frameCount = 0;
				}
			}
			
		}
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	//Checks if user is ready to start
	if (state->IsKeyDown(Input::Keys::RETURN))
	{
		_started2 = true;
	}

	//Checks if user is has read the objectives screen
	if ((state->IsKeyDown(Input::Keys::SPACE)) && (_started2 = true))
	{
		_started = true;
		_started2 = true;
	}

	//Checks if user is ready to respawn
	if ((state->IsKeyDown(Input::Keys::SPACE)) && (_pacman->dead = true))
	{
		_stopped = false;
		_pacman->dead = false;
		_paused = false;
	}

	//Checks if user is ready to progress to the next level
	if ((state->IsKeyDown(Input::Keys::SPACE)) && (_stopped))
	{
		_stopped = false;
		nextLevel = false;
		_pacman->dead = false;
		_paused = false;
	}
	
	//Checks if is ready to restart the game and then resets the variables
	if ((state->IsKeyDown(Input::Keys::SPACE)) && (_pacman->lives < 1))
	{
		_pacman->posRect->X = 350.0f;
		_pacman->posRect->Y = 350.0f;
		for (int a = 0; a < BLUEGHOSTS; a++)
		{
			_bGhost[a]->posRect->X = rand() % Graphics::GetViewportWidth();
			_bGhost[a]->posRect->Y = Graphics::GetViewportHeight() + 20;
		}
		for (int a = 0; a < PINKGHOSTS; a++)
		{
			_pGhost[a]->posRect->X = rand() % Graphics::GetViewportWidth();
			_pGhost[a]->posRect->Y = rand() % Graphics::GetViewportHeight();
			_pGhost[a]->direction = rand() % 3 + 0;
		}

		_pacman->munchieseaten = 0;
		_pacman->lives = 3;
		totMunchies = 5;
		_started2 = false;
		_level = 1;
		overheadCount = 0;
	}

	//Checks if the user wants to pause
	if (state->IsKeyUp(Input::Keys::P))
	_pKeyDown = false;

	//Checks if the user is mid game and not paused/dead
	if ((!_paused) && (_started) && (!_stopped))
		{
			//Checks if the user wants to use boost
			if ((state->IsKeyDown(Input::Keys::LEFTSHIFT)) || (state->IsKeyDown(Input::Keys::RIGHTSHIFT)))
			{
				_pacman->speedMultiplier = 4.0f;
			}
			if ((state->IsKeyUp(Input::Keys::LEFTSHIFT)) && (state->IsKeyUp(Input::Keys::RIGHTSHIFT)))
			{
				_pacman->speedMultiplier = 2.5f;
			}

			// Checks if D key is pressed
			if ((state->IsKeyDown(Input::Keys::D)) || (state->IsKeyDown(Input::Keys::RIGHT)))
			{
				_pacman->posRect->X += (_cPacmanSpeed * _pacman->speedMultiplier) * elapsedTime; //Moves Pacman across X axis
				_pacman->direction = 4;
				_pacman->sourceRect->Y = _pacman->sourceRect->Width * _pacman->direction;
			}

			// Checks if A key is pressed
			else if ((state->IsKeyDown(Input::Keys::A)) || (state->IsKeyDown(Input::Keys::LEFT)))
			{
				_pacman->posRect->X -= (_cPacmanSpeed * _pacman->speedMultiplier) * elapsedTime; //Moves Pacman across X axis
				_pacman->direction = 2;
				_pacman->sourceRect->Y = _pacman->sourceRect->Width * _pacman->direction;
			}

			// Checks if W key is pressed
			else if ((state->IsKeyDown(Input::Keys::W)) || (state->IsKeyDown(Input::Keys::UP)))
			{
				_pacman->posRect->Y -= (_cPacmanSpeed * _pacman->speedMultiplier) * elapsedTime; //Moves Pacman across Y axis
				_pacman->direction = 3;
				_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
			}

			// Checks if S key is pressed
			else if ((state->IsKeyDown(Input::Keys::S)) || (state->IsKeyDown(Input::Keys::DOWN)))
			{
				_pacman->posRect->Y += (_cPacmanSpeed * _pacman->speedMultiplier) * elapsedTime; //Moves Pacman across Y axis
				_pacman->direction = 1;
				_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
			}
			CheckViewportCollision();
			UpdatePacman(elapsedTime);
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::MouseState* mouseState)
{

}

void Pacman::CheckViewportCollision()
{
		if (_pacman->posRect->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
		{
			// Pacman hit right wall - reset his position
			_pacman->posRect->X = 0 - _pacman->sourceRect->Width;
		}

		if (_pacman->posRect->X + _pacman->sourceRect->Width < 0) //0 is game width
		{
			// Pacman hit left wall - reset his position
			_pacman->posRect->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
		}

		if (_pacman->posRect->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight()) //768 is game height
		{
			// Pacman hit top wall - reset his position
			_pacman->posRect->Y = Graphics::GetViewportHeight()- _pacman->sourceRect->Height;
		}

		if (_pacman->posRect->Y + _pacman->sourceRect->Height < 32) //0 is game height
		{
			// Pacman hit bottom wall - reset his position
			_pacman->posRect->Y = 32- _pacman->sourceRect->Height;
		}
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->frameTime += elapsedTime;
		if (_munchies[i]->frameTime > _cMunchieFrameTime)
		{
			_munchies[i]->frameCount++;
				if (_munchies[i]->frameCount >= 2) 
					_munchies[i]->frameCount = 0;
					_munchies[i]->frameTime = 0;
		}
	}

}

void Pacman::UpdatePacman(int elapsedTime)
{
		_pacman->currentFrameTime += elapsedTime;
		_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame; 
		if (_pacman->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman->frame++;
			overheadCount = overheadCount + 1; //INCREMENTS COUNT
			_pacman->spawnTimer++;
			if (_pacman->frame >= 2) 
			{
				_pacman->frame = 0;
				_pacman->spawnTimer = 0;
			}
			_pacman->currentFrameTime = 0;
		}
}

void Pacman::UpdateBlueGhosts(int elapsedTime)
{
	if ((_started) && (!_paused) && (overheadCount > 10) && (_pacman->respawn = true) && (!_stopped))
	{
		for (int a = 0; a < (_level - 1); a++)
		{
			if ((rand() % 1000) < 3)
			{
				_bGhost[a]->toX = rand() % 768;
				_bGhost[a]->toY = rand() % 1024;
				_bGhost[a]->invert = 1;
				_bGhost[a]->isWandering = true;
			}
			else
			{
				if ((!_bGhost[a]->isWandering) || (overheadCount % 15 <1))
				{
					if (_level < 5)
					{
						_bGhost[a]->toX =  _pacman->posRect->X;
						_bGhost[a]->toY =  _pacman->posRect->Y;
						_bGhost[a]->invert = 0;
						_bGhost[a]->isWandering = false;
					}
					else
					{
						float v = _pacman->speedMultiplier - _bGhost[a]->speed;
						float d1 =  _pacman->posRect->X - _bGhost[a]->posRect->X;
						float d2 = _pacman->posRect->Y - _bGhost[a]->posRect->Y;
						float t1 = d1/v;
						float t2 = d2/v;
						_bGhost[a]->toX =  _pacman->posRect->X + (_pacman->speedMultiplier * t1);
						_bGhost[a]->toY =  _pacman->posRect->Y + (_pacman->speedMultiplier * t2);
						_bGhost[a]->invert = 0;
						_bGhost[a]->isWandering = false;
					}
				}
			}
					
			if (_bGhost[a]->posRect->X < _bGhost[a]->toX)
			{
				_bGhost[a]->posRect->X += _bGhost[a]->speed;
			}
			if (_bGhost[a]->posRect->Y < _bGhost[a]->toY)
			{
				_bGhost[a]->posRect->Y += _bGhost[a]->speed;
			}
			if (_bGhost[a]->posRect->X > _bGhost[a]->toX)
			{
				_bGhost[a]->posRect->X -= _bGhost[a]->speed;
			}
			if (_bGhost[a]->posRect->Y > _bGhost[a]->toY)
			{
				_bGhost[a]->posRect->Y -= _bGhost[a]->speed;
			}
		}
	}
}

void Pacman::UpdatePinkGhosts(int elapsedTime)
{
	if ((_started) && (!_paused) && (overheadCount > 10) && (_pacman->respawn = true) && (!_stopped))
	{
	for (int a = 0; a < (PINKGHOSTS); a++)
		{
		if (_pGhost[a]->direction == 0) //Moves Right
			{
				_pGhost[a]->posRect->X += _pGhost[a]->speed * elapsedTime;
			}
			else if (_pGhost[a]->direction == 1) //Moves Left
			{
				_pGhost[a]->posRect->X -= _pGhost[a]->speed * elapsedTime;
			}
			else if (_pGhost[a]->direction == 2) //Moves Down
			{
				_pGhost[a]->posRect->Y += _pGhost[a]->speed * elapsedTime;
			}
			else if (_pGhost[a]->direction == 3) //Moves Up
			{
				_pGhost[a]->posRect->Y -= _pGhost[a]->speed * elapsedTime;
			}
		
			if (_pGhost[a]->posRect->X + _pGhost[a]->posRect->Width >= Graphics::GetViewportWidth()) //Hits Right edge
			{
				_pGhost[a]->direction = 1; 
			}
			else if (_pGhost[a]->posRect->X <= 0) //Hits left edge
			{
				_pGhost[a]->direction = 0; 
			}
			else if (_pGhost[a]->posRect->Y + _pGhost[a]->posRect->Height >= Graphics::GetViewportHeight()) //Hits Bottom
			{
				_pGhost[a]->direction = 3;
			}
			else if(_pGhost[a]->posRect->Y <= 0) // Hits top
			{
				_pGhost[a]->direction = 2;
			}
		}
	}
}

void Pacman::CheckGhostCollisions()
{
	for(int count1=0; count1< BLUEGHOSTS; count1++)
	{
		if(!_pacman->dead)
		{
			if (_bGhost[count1]->posRect->Intersects(*_pacman->posRect))
			{
				_pacman->dead = true;
				_pacman->lives = _pacman->lives - 1;
				_bGhost[count1]->speed -= 0.5;
				scoreMultiplier = 1;
				Audio::Play(_pacmanDies); 
				//_pacman->respawn = false;
				overheadCount = 0;
					
				if (_pacman->lives > 0)
				{
					_pacman->posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 32, 32);
					SpriteBatch::Draw(_pacman->texture, _pacman->posRect, _pacman->sourceRect);
					_stopped= true;	
				}
			}
		}
	}

	for(int count2=0; count2 < PINKGHOSTS; count2++)
	{
		if(!_pacman->dead)
		{
			if (_pGhost[count2]->posRect->Intersects(*_pacman->posRect))
			{
				_pacman->dead = true;
				_pacman->lives = _pacman->lives - 1;
				_bGhost[count2]->speed -= 0.5;
				scoreMultiplier = 1;
				Audio::Play(_pacmanDies); 
				//_pacman->respawn = false;
				overheadCount = 0;
				
				if (_pacman->lives > 0)
				{
					_pacman->posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 32, 32);
					SpriteBatch::Draw(_pacman->texture, _pacman->posRect, _pacman->sourceRect);
					_stopped = true;
				}
			}
		}
	}
}

void Pacman::UpdateCherry(int elapsedTime)
{
	for (int i = 0; i < BLUEGHOSTS; i++)
	{
		_cherry[i]->frameTime += elapsedTime;
		if (_cherry[i]->frameTime > _cCherryFrameTime)
		{
			_cherry[i]->frameCount++;
				if (_cherry[i]->frameCount >= 2) 
					_cherry[i]->frameCount = 0;
					_cherry[i]->frameTime = 0;
		}
	}
}

void Pacman::DrawBattery(int elapsedTime)
{
	for (int i = 0; i < BATTERYCOUNT; i++)
	{
		if ((!_battery[_level]->eaten))
		{
			if (totMunchies < 1)
			{
				if (_battery[_level]->sourceRect->Intersects(*_pacman->posRect))
				{
					_battery[_level]->eaten = true;
					Audio::Play(_win);
					nextLevel = true;
					_stopped = true;
					_paused = true;
					overheadCount = 0;
				}
				SpriteBatch::Draw(_battery[_level]->texture, _battery[_level]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
			}
			
		}
	}
}




	