#include "SceneGameOver.h"

SceneGameOver::SceneGameOver()                                      : Scene{ {},     nullptr   } {}
SceneGameOver::SceneGameOver(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}




void SceneGameOver::initialise() {
    mGameState.nextSceneID = 5;

        //LoadSound different then loadmusicstream
    mGameState.death = LoadSound("assets/game/Minecraft Steve OOF Sound Effect - Random Crap (128k).mp3");
    SetSoundVolume(mGameState.death, 0.33f);
    PlaySound(mGameState.death); 
}

void SceneGameOver::update(float deltaTime)
{ 
    if (IsKeyPressed(KEY_ENTER)) { //starts the game 
        lives = 3; // resets lives to 3
        mGameState.nextSceneID = 6; // menu level
    }
}

void SceneGameOver::render() {
    ClearBackground(BLACK);
    DrawText("Game Over... press Enter for menu", (GetScreenWidth()  / 2) - 300, (GetScreenHeight() /2) , 40, WHITE);

}

void SceneGameOver::shutdown()
{
    UnloadSound(mGameState.death);
    mGameState.death = {} ; 

}
