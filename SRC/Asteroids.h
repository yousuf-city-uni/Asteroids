#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IMouseListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include <vector>

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener, public IMouseListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	void Load(void);
	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	void OnMouseDragged(int x, int y);
	void OnMouseButton(int button, int state, int x, int y);
	void OnMouseMoved(int x, int y);


	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);
	shared_ptr<GameObject> CreateExplosion();

	enum GameState {
		MENU,
		SETTINGS,
		INSTRUCTIONS,
		LEADERBOARD,
		PLAYING,
		GAME_OVER,
	};
	void UpdateState(GameState newState);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;

	shared_ptr<GUILabel> quitLabel;
	shared_ptr<GUILabel> playLabel;

	shared_ptr<GUILabel> instructionsLabel;
	shared_ptr<GUILabel> controlsLabel;
	shared_ptr<GUILabel> controlsUpLabel;
	shared_ptr<GUILabel> controlsLeftLabel;
	shared_ptr<GUILabel> controlsRightLabel;
	shared_ptr<GUILabel> controlsSpaceLabel;
	shared_ptr<GUILabel> goalLabel;

	shared_ptr<GUILabel> settingsLabel;
	shared_ptr<GUILabel> settingsTitleLabel;

	shared_ptr<GUILabel> leaderboardLabel;
	shared_ptr<GUILabel> backLabel;

	uint mLevel;
	uint mAsteroidCount;
	std::vector<shared_ptr<GameObject>> mAsteroidList;

	bool mExtraLife = true;
	std::vector<shared_ptr<GameObject>> mExtraLifeList;

	bool mBrakesEnabled = true;
	bool hasBrakes = false;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	void RemoveAllAsteroids();
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;

	GameState mCurrentState = MENU;

};

#endif