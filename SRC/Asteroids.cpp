#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "Bullet.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <iostream>
#include <vector>
#include "Life.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** asteroids menu. */
void Asteroids::Load() {
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	mGameWindow->AddMouseListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);
	Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	// Heart from: https://deanlofiplays.itch.io/heart-life-pack-1
	Animation* heart_anim = AnimationManager::GetInstance().CreateAnimationFromFile("heart", 16, 16, 16, 16, "heart.png");

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Create some asteroids and add them to the world
	CreateAsteroids(10);
	CreateGUI();

	// Start the game
	GameSession::Start();
}

/** Start an asteroids game. */
void Asteroids::Start()
{
	mGameWorld->AddObject(CreateSpaceship());
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		mSpaceship->Shoot();
		break;
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}

void Asteroids::OnMouseButton(int button, int state, int x, int y) {
	if (button == 0 && state == 0) {
		std::cout << "Left click at (" << x << ", " << y << ")" << std::endl;

		if (mCurrentState == MENU) {
			if (x >= 20 && x <= 60 && y >= 185 && y <= 200) {
				std::cout << "Play" << std::endl;
				for (auto asteroid : mAsteroidList) {
					mGameWorld->FlagForRemoval(asteroid->GetThisPtr());
				}
				mAsteroidList.clear();
				UpdateState(PLAYING);
				Start();
			}
			if (x >= 20 && x <= 125 && y >= 225 && y <= 240) {
				std::cout << "Instructions Clicked" << std::endl;
				UpdateState(INSTRUCTIONS);
			}
			if (x >= 20 && x <= 90 && y >= 265 && y <= 280) {
				std::cout << "Settings Clicked" << std::endl;
				UpdateState(SETTINGS);
			}
			if (x >= 20 && x <= 125 && y >= 305 && y <= 320) {
				std::cout << "Leaderboard Clicked" << std::endl;
				UpdateState(LEADERBOARD);
			}
			if (x >= 20 && x <= 60 && y >= 345 && y <= 360) {
				std::cout << "Quitting!" << std::endl;
				Stop();
			}
		}
		if (mCurrentState == INSTRUCTIONS) {
			if (x >= 20 && x <= 65 && y >= 345 && y <= 360) {
				std::cout << "Return to Menu" << std::endl;
				UpdateState(MENU);
			}
		}
		if (mCurrentState == SETTINGS) {
			if (x >= 20 && x <= 65 && y >= 345 && y <= 360) {
				std::cout << "Return to Menu" << std::endl;
				UpdateState(MENU);
			}
		}
		if (mCurrentState == LEADERBOARD) {
			if (x >= 20 && x <= 65 && y >= 345 && y <= 360) {
				std::cout << "Return to Menu" << std::endl;
				UpdateState(MENU);
			}
		}
		if (mCurrentState == GAME_OVER) {
			if (x >= 20 && x <= 65 && y >= 345 && y <= 360) {
				std::cout << "Return to Menu" << std::endl;
				UpdateState(MENU);
				for (auto extraLife : mExtraLifeList) {
					mGameWorld->FlagForRemoval(extraLife->GetThisPtr());
				}
				mExtraLifeList.clear();
				mLevel = 0;
				mScoreKeeper.SetScore(mAsteroidCount * -10);
				mPlayer.SetLives(3);
			}
		}
	}
}

void Asteroids::OnMouseMoved(int x, int y) {
	//std::cout << "Mouse Moved To (" << x << ", " << y << ")" << std::endl;
}

void Asteroids::OnMouseDragged(int x, int y) {

}

// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		if (mAsteroidCount > 0) {
			shared_ptr<GameObject> explosion = CreateExplosion();
			explosion->SetPosition(object->GetPosition());
			explosion->SetRotation(object->GetRotation());
			mGameWorld->AddObject(explosion);

			mAsteroidCount--;
			if (mCurrentState == PLAYING) {
				mScoreKeeper.AddScore();
			}
		}

		if (mAsteroidCount <= 0)
		{
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
	if (object->GetType() == GameObjectType("NewLife")) {
		mPlayer.AddLife();
		mLivesLabel->SetText("Lives: " + std::to_string(mPlayer.GetLives()));
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 *(mLevel - 1);
		CreateAsteroids(num_asteroids);

		if (mExtraLife) {
			int randomDigit = 1 + (rand() % 4);
			if (randomDigit == 4) {
				shared_ptr<GameObject> NewLife = make_shared<Life>();
				NewLife->SetBoundingShape(make_shared<BoundingSphere>(NewLife->GetThisPtr(), 4.0f));
				Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("heart");
				shared_ptr<Sprite> spaceship_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
				NewLife->SetSprite(spaceship_sprite);
				NewLife->SetScale(0.5f);
				mExtraLifeList.push_back(NewLife);
				mGameWorld->AddObject(NewLife);
			}
		}
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> spaceship_shape = make_shared<Shape>("spaceship.shape");
	shared_ptr<Shape> thruster_shape = make_shared<Shape>("thruster.shape");
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	mSpaceship->SetSpaceshipShape(spaceship_shape);
	mSpaceship->SetThrusterShape(thruster_shape);
	mSpaceship->SetBulletShape(bullet_shape);

	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;
}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mAsteroidList.push_back(asteroid);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	//Play Label
	playLabel = make_shared<GUILabel>("Play");
	playLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	playLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	playLabel->SetColor(GLVector3f(0.0f, 1.0f, 0.0f));

	playLabel->SetVisible(true);

	mGameDisplay->GetContainer()->AddComponent(playLabel, GLVector2f(0.1f, 0.5f));

	//Instructions Label
	instructionsLabel = make_shared<GUILabel>("Instructions");
	instructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	instructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	instructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));

	instructionsLabel->SetVisible(true);

	mGameDisplay->GetContainer()->AddComponent(instructionsLabel, GLVector2f(0.18f, 0.4f));

	// Actual instructions labels

	controlsLabel = make_shared<GUILabel>("Controls:");
	controlsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	controlsLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	controlsLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(controlsLabel, GLVector2f(0.5f, 0.8f));

	controlsUpLabel = make_shared<GUILabel>("Up Arrow Key To Accelerate");
	controlsUpLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	controlsUpLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	controlsUpLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(controlsUpLabel, GLVector2f(0.5f, 0.7f));

	controlsLeftLabel = make_shared<GUILabel>("Left Arrow Key To Rotate The Ship Left");
	controlsLeftLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	controlsLeftLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	controlsLeftLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(controlsLeftLabel, GLVector2f(0.5f, 0.6f));

	controlsRightLabel = make_shared<GUILabel>("Right Arrow Key To Rotate The Ship Right");
	controlsRightLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	controlsRightLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	controlsRightLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(controlsRightLabel, GLVector2f(0.5f, 0.5f));

	controlsSpaceLabel = make_shared<GUILabel>("Space Bar To Shoot Bullets");
	controlsSpaceLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	controlsSpaceLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	controlsSpaceLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(controlsSpaceLabel, GLVector2f(0.5f, 0.4f));

	goalLabel = make_shared<GUILabel>("Shoot Asteroids To Gain Points!");
	goalLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	goalLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	goalLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(goalLabel, GLVector2f(0.5f, 0.3f));

	//Settings Label
	settingsLabel = make_shared<GUILabel>("Settings");
	settingsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	settingsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	settingsLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));

	settingsLabel->SetVisible(true);

	mGameDisplay->GetContainer()->AddComponent(settingsLabel, GLVector2f(0.14f, 0.3f));

	// Settings Menu Labels
	settingsTitleLabel = make_shared<GUILabel>("Settings:");
	settingsTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	settingsTitleLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
	settingsTitleLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(settingsTitleLabel, GLVector2f(0.5f, 0.8f));

	//Leaderboard Label
	leaderboardLabel = make_shared<GUILabel>("Leaderboard");
	leaderboardLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	leaderboardLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	leaderboardLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));

	leaderboardLabel->SetVisible(true);

	mGameDisplay->GetContainer()->AddComponent(leaderboardLabel, GLVector2f(0.18f, 0.2f));

	//Quit Label
	quitLabel = make_shared<GUILabel>("Quit");
	quitLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	quitLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	quitLabel->SetColor(GLVector3f(1.0f, 0.0f, 0.0f));

	quitLabel->SetVisible(true);

	mGameDisplay->GetContainer()->AddComponent(quitLabel, GLVector2f(0.1f, 0.1f));

	//Back Label
	backLabel = make_shared<GUILabel>("Back");
	backLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	backLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	backLabel->SetColor(GLVector3f(1.0f, 0.0f, 1.0f));

	backLabel->SetVisible(false);

	mGameDisplay->GetContainer()->AddComponent(backLabel, GLVector2f(0.1f, 0.1f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mScoreLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component = static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent
	mLivesLabel->SetVisible(false);
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer
	shared_ptr<GUIComponent> game_over_component = static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);
	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);
	if (lives_left > 0) { SetTimer(1000, CREATE_NEW_PLAYER); }
	else {
		UpdateState(GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

void Asteroids::UpdateState(GameState newState)
{
	mCurrentState = newState;

	switch (newState) {
	case MENU:
		mScoreLabel->SetVisible(false);
		mLivesLabel->SetVisible(false);
		mGameOverLabel->SetVisible(false);
		backLabel->SetVisible(false);

		controlsLabel->SetVisible(false);
		controlsLeftLabel->SetVisible(false);
		controlsRightLabel->SetVisible(false);
		controlsUpLabel->SetVisible(false);
		controlsSpaceLabel->SetVisible(false);
		goalLabel->SetVisible(false);

		settingsTitleLabel->SetVisible(false);

		playLabel->SetVisible(true);
		settingsLabel->SetVisible(true);
		instructionsLabel->SetVisible(true);
		leaderboardLabel->SetVisible(true);
		quitLabel->SetVisible(true);
		break;

	case PLAYING:
		playLabel->SetVisible(false);
		settingsLabel->SetVisible(false);
		leaderboardLabel->SetVisible(false);
		instructionsLabel->SetVisible(false);
		quitLabel->SetVisible(false);

		mScoreLabel->SetVisible(true);
		mLivesLabel->SetVisible(true);
		break;
	case INSTRUCTIONS:
		playLabel->SetVisible(false);
		settingsLabel->SetVisible(false);
		leaderboardLabel->SetVisible(false);
		instructionsLabel->SetVisible(false);
		quitLabel->SetVisible(false);

		controlsLabel->SetVisible(true);
		controlsLeftLabel->SetVisible(true);
		controlsRightLabel->SetVisible(true);
		controlsUpLabel->SetVisible(true);
		controlsSpaceLabel->SetVisible(true);
		goalLabel->SetVisible(true);
		backLabel->SetVisible(true);
		break;
	case SETTINGS:
		playLabel->SetVisible(false);
		settingsLabel->SetVisible(false);
		leaderboardLabel->SetVisible(false);
		instructionsLabel->SetVisible(false);
		quitLabel->SetVisible(false);

		settingsTitleLabel->SetVisible(true);
		backLabel->SetVisible(true);
		break;
	case LEADERBOARD:
		playLabel->SetVisible(false);
		settingsLabel->SetVisible(false);
		leaderboardLabel->SetVisible(false);
		instructionsLabel->SetVisible(false);
		quitLabel->SetVisible(false);

		backLabel->SetVisible(true);
		break;
	case GAME_OVER:
		mGameOverLabel->SetVisible(true);
		backLabel->SetVisible(true);
		break;
	}
}