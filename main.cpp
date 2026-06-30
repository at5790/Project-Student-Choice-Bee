#include "CS3113/ShaderProgram.h"
#include "CS3113/LevelC.h" // needed to include wouldnt see it for some reason 
#include "CS3113/SceneMenu.h" 
#include "CS3113/SceneGameOver.h" 
#include "CS3113/SceneWin.h" 
#include <queue>
#include <iostream>

/**
* Author: [Atiya Thounaojam]
* Assignment: [How to Bee a Queen]
* Date due: [June 30, 2005], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
// Global Constants
constexpr int SCREEN_WIDTH     = 1400,
              SCREEN_HEIGHT    = 900,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3;// changed

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Camera2D gCamera = { 0 };

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;

//inherited scenes added 
LevelC *gLevelC = nullptr; // third level 
SceneGameOver *gGameOver = nullptr;  //game over scene
SceneWin *gWin = nullptr ; // win scene 
SceneMenu *gMenu = nullptr; // menu scene 

// A =0, B =1, C=2, Game over = 3, win = 4, menu = 6

int currLevel = 0;

Effects *gEffects = nullptr;

ShaderProgram gShader;

// added for player lives 

int lives = 3; 

// for consistent music

Music bgm = {} ;
Music rainbgm = {} ; 
Music bossbgm = {} ; 

// playing buzzing?
bool buzzing = false;


// From Pong project

// slash treated as entity
constexpr float SLASH_SPEED = 450.0f;
Entity *gSlash = nullptr; 
float gSlashDistance = 0; 

// shield

Texture2D gShieldTexture = {}; 
Entity *shieldedEntity = nullptr; 
float gShieldTimer = 0.0f;

constexpr char SHIELD[] = "assets/game/—Pngtree—blue shield badge clipart illustration_20869771.png";

Vector2 gLightPosition = { 0.0f, 0.0f };

// hearts 
Texture2D gHeartsTexture = {}; 
constexpr char HEARTS[] = "assets/game/Heart.png"; 


// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

// for game 
void spawnSlash(Entity& slash, Entity& player);
void showShield(Entity *target); 
void shootWeb(Entity *web, Entity *shooter, bool side);


// from pong project but here replaced pong for slash and now includes plauer for function input
void spawnSlash(Entity& slash, Entity& player) {

    slash.activate(); 

    Vector2 playerPos = player.getPosition(); 

    if (player.isFacingRight()) {
        slash.setPosition( {playerPos.x + 45.0f, playerPos.y});
        slash.setDirection(RIGHT);
        slash.moveRight();
    } else {
        slash.setPosition( { playerPos.x -45.0f, playerPos.y });
        slash.setDirection(LEFT); 
        slash.moveLeft();
    }

    gSlashDistance = SCREEN_WIDTH;

}

void showShield(Entity *target) {
    shieldedEntity = target;
    gShieldTimer = 1.0f; //how long shield will b shown
}

void shootWeb(Entity *web, Entity *spider, bool side) {
    Vector2 pos = spider->getPosition(); 

    web->setPosition( {pos.x, pos.y -30 }); // web spawns here

    
    //web->setAcceleration({0.0f, -300.0f});

    web->setMovement({ 0.0f, 0.0f});
    web->moveUp();
    web->setSpeed(100);

    if (side) {//facing to the right
        web->moveRight();
    } else {
        web->moveLeft();
    }
    
    web->activate();
}

void switchToScene(Scene *scene) 
{   
    if (scene == nullptr) return; 


    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (gCurrentScene!=nullptr && gCurrentScene->getState().xochitl != nullptr) { 
        StopSound(gCurrentScene->getState().buzzing); // used chat to figure out why buzzing wasnt looping
        buzzing = false;
    }

    if (gCurrentScene->getState().xochitl != nullptr) gCamera.target = gCurrentScene->getState().xochitl->getPosition();
    else gCamera.target = ORIGIN;

    // 
    if (currLevel == 0) { PlayMusicStream(bgm); StopMusicStream(rainbgm); StopMusicStream(bossbgm); }
    else if (currLevel == 1) { PlayMusicStream(bgm); PlayMusicStream(rainbgm); StopMusicStream(bossbgm);}
    else if (currLevel == 2) { StopMusicStream(bgm); PlayMusicStream(rainbgm); PlayMusicStream(bossbgm);}
    else { StopMusicStream(bgm); StopMusicStream(rainbgm); StopMusicStream(bossbgm);}
}


void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Effects & Shaders");
    InitAudioDevice();

    
    bgm = LoadMusicStream("assets/game/jumping.mp3");
    
    SetMusicVolume(bgm, 0.7f);


    rainbgm = LoadMusicStream("assets/game/5 Minute Timer with Rain Sounds  Timers.mp3");
    
    SetMusicVolume(rainbgm, 0.9f);

    bossbgm = LoadMusicStream("assets/game/Dark Souls III OST 10 - Vordt of the Boreal Valley.mp3");
    
    SetMusicVolume(bossbgm, 0.9f);
  

    gHeartsTexture = LoadTexture(HEARTS);
    gShieldTexture = LoadTexture(SHIELD); 


    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#1F3B4D");

    gLevelC = new LevelC(ORIGIN, "#1F3B4D");

    gGameOver = new SceneGameOver(ORIGIN, "#FFFFFF");
    gWin = new SceneWin(ORIGIN, "#FFFFFF");
    gMenu = new SceneMenu(ORIGIN, "#FFFFFF");

// _____________________________________________________

    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);

    gLevels.push_back(gLevelC);

    gLevels.push_back(gGameOver);
    gLevels.push_back(gWin);
    gLevels.push_back(nullptr);
    gLevels.push_back(gMenu);

    currLevel = 6;
    switchToScene(gLevels[currLevel]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    gEffects = new Effects(ORIGIN, (float) SCREEN_WIDTH * 1.5f, (float) SCREEN_HEIGHT * 1.5f);

    // gEffects->start(FADEIN);
    // gEffects->start(FADEOUT);
    // gEffects->start(SHRINK);
    // gEffects->start(GROW);
    

    /* ----------- Slash ----------- */
    std::map<Direction, std::vector<int>> slashAtlas = {
        { RIGHT,  {  1  }},
        { LEFT,  {  1 }}, 
        { UP,  {  1 }},
        { DOWN,  {  1 }}
    };

    gSlash = new Entity(
        { 0.0f, 0.0f}, // position
        { 64.0f,64.0f },              // scale
        "assets/game/slash-Sheet.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 1, 3},                                    // atlas dimensions
        slashAtlas,                       // actual atlas
        PLAYER                                       // entity type
    );

    gSlash->deactivate();

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if (gCurrentScene->getState().xochitl == nullptr) return; 

    gCurrentScene->getState().xochitl->resetMovement();

    // for idle 
    bool touchingGround = gCurrentScene->getState().xochitl->isTouchingGround();

    if      (IsKeyDown(KEY_A)) 
    {
        gCurrentScene->getState().xochitl->moveLeft();
        if (!buzzing) {
            PlaySound(gCurrentScene->getState().buzzing);
            buzzing = true;
        }
    }
    else if (IsKeyDown(KEY_D)) 
    {
        gCurrentScene->getState().xochitl->moveRight();
        if (!buzzing) {
            PlaySound(gCurrentScene->getState().buzzing);
            buzzing = true;
        }
    } 

    if ( (IsKeyDown(KEY_D) || IsKeyDown(KEY_A)) && touchingGround) {
        gCurrentScene->getState().xochitl->crawl();
    } 
    else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_S)) { gCurrentScene->getState().xochitl->idle(); }
    else {
        gCurrentScene->getState().xochitl->stopCrawling();
    }

    if (IsKeyPressed(KEY_W) && touchingGround ) {
        gCurrentScene->getState().xochitl->takeOff();
    } else if (IsKeyDown(KEY_W)) {
        gCurrentScene->getState().xochitl->moveUp();
        if (!buzzing) {
            PlaySound(gCurrentScene->getState().buzzing);
            buzzing = true;
        }

    }
    if (IsKeyDown(KEY_S))
    {
        gCurrentScene->getState().xochitl->moveDown();
        if (!buzzing) {
            PlaySound(gCurrentScene->getState().buzzing);
            buzzing = true;
        }
    } 
    
    if ( (!IsKeyDown(KEY_A)) && (!IsKeyDown(KEY_D)) && (!IsKeyDown(KEY_W)) && (!IsKeyDown(KEY_S)) ) gCurrentScene->getState().xochitl->idle(); 

   
    if (IsKeyPressed(KEY_LEFT_SHIFT)) {

        if ((gCurrentScene->getState().xochitl->getDirection() == RIGHT) && !(gCurrentScene->getState().xochitl->isCollidingRight())) gCurrentScene->getState().xochitl->dash();

        if ((gCurrentScene->getState().xochitl->getDirection() == LEFT) && !(gCurrentScene->getState().xochitl->isCollidingLeft())) gCurrentScene->getState().xochitl->dash();

        PlaySound(gCurrentScene->getState().dashSound);
        }

    if (IsKeyPressed(KEY_N)) {
        gCurrentScene->getState().xochitl->smash();
    }

    if (IsKeyPressed(KEY_M)) {

        gCurrentScene->getState().xochitl->slash();

        if (!(gSlash->isActive())) {
            spawnSlash(*gSlash, *gCurrentScene->getState().xochitl);
        }
    }



    if (GetLength(gCurrentScene->getState().xochitl->getMovement()) > 1.0f)
        gCurrentScene->getState().xochitl->normaliseMovement();

  

    if (IsKeyPressed(KEY_ONE) ) {

            currLevel = 0 ;
            switchToScene(gLevels[currLevel]); // to go back to first level "debug level"
    }

            // int id = gCurrentScene->getState().nextSceneID;
            // switchToScene(gLevels[id]);
    if (IsKeyPressed(KEY_TWO)) {

            currLevel = 1;
            switchToScene(gLevels[currLevel]);
    }

    if (IsKeyPressed(KEY_THREE)) {

            currLevel = 2;
            switchToScene(gLevels[currLevel]);
    }

}

void update()
{
    UpdateMusicStream(bgm);
    UpdateMusicStream(rainbgm);
    UpdateMusicStream(bossbgm);

    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        

        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);


        // for slash  
        if (gSlash->isActive()) {
            // slashes same idea as pong but repalced . with -> too 

            // position
            Vector2 mPosition = gSlash->getPosition();

            // correct change of position
            if (gSlash->getDirection() == RIGHT) { 
                mPosition.x += SLASH_SPEED * deltaTime;
            }

            if (gSlash->getDirection() == LEFT) { 
                mPosition.x -= SLASH_SPEED* deltaTime;
            }

            // direct from pong
            // //X Checks

            // if (mPosition.x < 0.0f) {
            //     fakeBeeScore++;
            //     gPong[i].deactivate();
            // }
            // if (mPosition.x > SCREEN_WIDTH){
            //     playerScore++;
            //     gPong[i].deactivate();

            // }
            // gPong[i].setPosition(mPosition); 

            // now compare with slash distance and no more adding score, just deactivate normally
            gSlashDistance -=  SLASH_SPEED* deltaTime;

            if (gSlashDistance <= 0.0f) {
                gSlash->deactivate(); 
            }
            // updated position
            gSlash->setPosition(mPosition); 

        }   

        // for shield 

        if (gShieldTimer >0) {
            gShieldTimer -= deltaTime; // this decrease timer back to 0 eventually

            if (gShieldTimer <= 0) {
                shieldedEntity = nullptr; 
            }
        }
        // have a placeholder scene and if its being used then start from whereever current scene is 
        int id = gCurrentScene->getState().nextSceneID;
        if (id != 5) {
            currLevel = id;
            switchToScene(gLevels[currLevel]);
        }

        if (gCurrentScene->getState().xochitl != nullptr) { //debug seg fault 
            Vector2 cameraTarget = { gCurrentScene->getState().xochitl->getPosition().x, ORIGIN.y };
            panCamera(&gCamera, &cameraTarget);
            gEffects->update(FIXED_TIMESTEP, &cameraTarget);

            gLightPosition = gCurrentScene->getState().xochitl->getPosition();

        
        }
        else { gEffects->update(FIXED_TIMESTEP, nullptr); }
     
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    // here gShader begins so changed if level B or C 
    // gShader.begin();

    if ((currLevel == 2) || (currLevel == 1)) {gShader.begin(); gShader.setVector2("lightPosition", gLightPosition);} 


    gCurrentScene->render();

    if (gSlash->isActive()) {
        gSlash->render();
    }

    // from Pong project changed name

    Rectangle HeartArea = {
        2.0f, 0.0f,
        ((static_cast<float>(gHeartsTexture.width) / 3 )* lives) -1.1f, // max 3 lives so total witdh would show whole png but 0 lives mean 0 width so no lives would b shown 
        static_cast<float>(gHeartsTexture.height) // heights of hearts are the same
    } ;

    Vector2 objectOriginHearts = {
    0.0f,
    0.0f

    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationAreaHearts = {
        5.0f,
        5.0f,
        ((static_cast<float>(gHeartsTexture.width) / 3 ) * lives ) -1.1f,
        static_cast<float>(gHeartsTexture.height)
    };

    if (gCurrentScene->getState().xochitl != nullptr) {
    DrawTexturePro(
        gHeartsTexture, 
        HeartArea, 
        destinationAreaHearts, 
        objectOriginHearts, 
        0.0f, 
        WHITE
    ); }

    if (shieldedEntity == nullptr) { } // this way if its nothing being protected dont render the shield 
    else { // somethings being sheileded

        Rectangle ShieldArea = { // include the whole shield png 
            0.0f ,0.0f 
            , static_cast<float>(gShieldTexture.width), static_cast<float>(gShieldTexture.height)

        };

        Rectangle destinationAreaShield = { shieldedEntity->getPosition().x , shieldedEntity->getPosition().y + 10, 55.0f, 55.f}; // where to put shield so at target psotion and size of shield bottom and right 


        Vector2 objectOriginShield = { // center of shield right at the target position
             55/2, 55/2

        };

        DrawTexturePro(
            gShieldTexture, 
            ShieldArea, 
            destinationAreaShield, 
            objectOriginShield, 
            0.0f, 
            WHITE
        );

    }
    
   

    // Whole texture (UV coordinates)
    
    if (gCurrentScene->getState().xochitl != nullptr) {
        Entity *player = gCurrentScene->getState().xochitl;

    }

    gShader.end();
    gEffects->render();
    EndMode2D();
    EndDrawing();
}

void shutdown()
{
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    delete gEffects;
    gEffects = nullptr;

    gShader.unload();

    delete gSlash;
    gSlash= nullptr; 

    UnloadTexture(gHeartsTexture) ; 
    UnloadTexture( gShieldTexture);

    UnloadMusicStream(bgm);
    UnloadMusicStream(rainbgm);
    UnloadMusicStream(bossbgm);
    bgm = {}; 
    rainbgm = {}; 
    bossbgm = {}; 
    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();


        render();
    }

    shutdown();


    return 0;
}
