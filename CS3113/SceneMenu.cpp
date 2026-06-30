#include "SceneMenu.h"

SceneMenu::SceneMenu()                                      : Scene{ {},     nullptr   } {}
SceneMenu::SceneMenu(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}




void SceneMenu::initialise() {
    mGameState.nextSceneID = 5;
}

void SceneMenu::update(float deltaTime)
{ 
    if (IsKeyPressed(KEY_ENTER)) { //starts the game 
        lives = 3; // resets lives to 3
        mGameState.nextSceneID = 0; // first level
    }
}

void SceneMenu::render() {
    ClearBackground(BLACK);
    DrawText("How to Bee a Queen... press Enter to start", (GetScreenWidth()  / 2) -400, (GetScreenHeight() /2) -50, 40, WHITE);
    DrawText("WASD to move, Shift to Dash", (GetScreenWidth()  / 2) -400, (GetScreenHeight() /2) , 40, WHITE);
    DrawText("N to Smash, M to Slash", (GetScreenWidth()  / 2) -400, (GetScreenHeight() /2)+50, 40, WHITE);
}



void SceneMenu::shutdown()
{

}