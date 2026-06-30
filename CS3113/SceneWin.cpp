#include "SceneWin.h"

SceneWin::SceneWin()                                      : Scene{ {},     nullptr   } {}
SceneWin::SceneWin(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}


void SceneWin::initialise() {
    mGameState.nextSceneID = 5;
}

void SceneWin::update(float deltaTime)
{ 
    if (IsKeyPressed(KEY_ENTER)) { //starts the game 
        lives = 3; // resets lives to 3
        mGameState.nextSceneID = 6; // menu
    }
}

void SceneWin::render() {
    ClearBackground(BLACK);
    DrawText("You Win ... press Enter for Menu", (GetScreenWidth()  / 2) -300, (GetScreenHeight() /2) , 40, WHITE);

}

void SceneWin::shutdown()
{

}