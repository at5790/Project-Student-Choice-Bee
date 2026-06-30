#include "LevelC.h"
#include <random>

//for end game
int totalEnemiesLevelC = 6; 

constexpr int SCREEN_WIDTH = 1400;
constexpr int SCREEN_HEIGHT = 900; 
// for shield
extern Entity *gSlash;
void showShield( Entity *target); 
void shootWeb(Entity *web, Entity *shooter, bool side);

// for spidersC[i]
constexpr int max_spider_C = 8;

Entity *spidersC[max_spider_C];
Entity *spiderWebC[max_spider_C]; 

int spiderCHealth[max_spider_C] = {1,1,1,1,1,1,1,1}; 
bool spiderCHurt[max_spider_C] = {}; 

bool spiderCDead[max_spider_C] = {}; 
float spiderCDeathTimer[max_spider_C] = {}; 

bool spiderCAttacking[max_spider_C] = {}; 
float spiderCAttackTimer[max_spider_C] = {};
bool spiderCWebbed[max_spider_C] = {};
float spiderCAttackSpeed[max_spider_C] = {};
bool spiderCFacingRight[max_spider_C] = {true, false, true}; 

// for raindrop

const int max_raindropC = 12; 
Entity *rainDropsC[max_raindropC];
bool rainSplashingC[max_raindropC] = {};
float rainTimerC[max_raindropC] = {};

// for queen bee
enum QueenAbilities { QUEEN_SMASH, QUEEN_SLASH, QUEEN_IDLE};

int queenHealth = 1; 
bool queenHurt = false; 
QueenAbilities queenState = QUEEN_IDLE; // for now bee idles
float queenAttackTimer = 0.0f;
float queenAttackSpeed = 0.0f;
bool queenAttacking = false;

float queenSmashSpeed = 0.5f;
float queenSmashTimer = 0.0f;
    //from pong
bool queenBeeMovingUp = false;

Entity *queenSlash = nullptr; 
float queenSlashDistance = 0.0f;
const float queenSlashSpeed = 125.0f;

void spawnqueenSlash(Entity& slash, Entity& queen); 
void queenLerp(Entity *queen, Entity *target, float lerp, float deltaTime);

// 

void queenLerp(Entity *queen, Entity *target, float lerp, float deltaTime) {
   // position' = position + (target position - position) * lerp factor * delta time
    
    Vector2 queenPosition = queen->getPosition();


    queenPosition.x = queenPosition.x + (target->getPosition().x - queenPosition.x) * lerp * deltaTime ;
    queenPosition.y = queenPosition.y + (target->getPosition().y - queenPosition.y) * lerp * deltaTime ; 

    queen->setPosition(queenPosition);
}

void spawnqueenSlash(Entity& slash, Entity& queen) {

    slash.activate(); 

    Vector2 queenPosition = queen.getPosition(); 

    if (queen.isFacingRight()) {
        slash.setPosition( {queenPosition.x + 45.0f, queenPosition.y});
        slash.setDirection(RIGHT);
        slash.moveRight();
    } else {
        slash.setPosition( { queenPosition.x -45.0f, queenPosition.y });
        slash.setDirection(LEFT); 
        slash.moveLeft();
    }

    queenSlashDistance = 300.0f;

}

LevelC::LevelC()                                      : Scene{ {},     nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
    mGameState.nextSceneID = 5; // placeholder 

    int totalEnemiesLevelC = 5; 


    for (int i=0; i < max_spider_C; i++) {
    
     int randint = (rand()%2 == 1);

     spiderCHealth[i] = 5; 
     spiderCHurt[i] = false; 

     spiderCDead[i] = false; 
     spiderCDeathTimer[i] = 0.0; 

     spiderCAttacking[i] = 0.0; 
     spiderCAttackTimer[i] = 0.0;
     spiderCWebbed[i] = false;
     spiderCAttackSpeed[i] = 3.0 + i*.4;

     
     spiderCFacingRight[i] = randint; 
}   

    float sizeRatio = 48.0f / 64.0f;

    mGameState.jumpSound = LoadSound("assets/game/5 Minute Timer with Rain Sounds  Timers.mp3");
    mGameState.dashSound = LoadSound("assets/game/Player Dash (Nr. 1  Cuphead Sound) - Sound Effect for editing.mp3");
    mGameState.buzzing = LoadSound("assets/game/Bee Buzzing - Nature Sound Effects HD - Daily Moving Videos - DMV (128k).mp3");
    SetSoundVolume(mGameState.buzzing, 0.3f);

    /* ----------- MAP ----------- */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
        (unsigned int *) mLevelData, // grid data
        "assets/game/TilesNonSliced.png",   // texture filepath
        TILE_DIMENSION,              // tile size
        12, 7,                        // texture cols & rows
        mOrigin                      // in-game origin
    );

    /* ----------- PROTAGONIST ----------- */
    std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
        { DOWN,  {  108,109  }},
        { LEFT,  {  12,13,14,15,16,17,18,19  }},
        { UP,    {  12,13,14,15,16,17,18,19  }},
        { RIGHT, {  12,13,14,15,16,17,18,19  }},

    };

        /* ----------- Antogonist ----------- */
    std::map<Direction, std::vector<int>> spiderAnimationAtlas = {
        { DOWN,  { 40, 40, 40,41,41,  41,42,42,  42,43,43, 43,44, 44, 44  }}, // spider dies
        { LEFT,  { 8,9,10,11,12,13,14,15  }},
        { UP,    { 24,25,26,27,28,29  }}, // spider shoots web 
        { RIGHT, { 8,9,10,11,12,13,14,15  }},

    };

    for ( int i =0; i < max_spider_C; i ++) {

        float spawnSpiderX = 500 + (rand() % (SCREEN_WIDTH - 1000)) ; // 30 is padding for right side 

        spidersC[i] = new Entity(
            {spawnSpiderX, mOrigin.y + 290.0f}, // position
            { 160.0f , 100.0f },              // scale
            "assets/game/GandalfHardcore Pixel Art Spider.png",                      // texture file address
            ATLAS,                                       // single image or atlas?
            { 6,8 },                                    // atlas dimensions
            spiderAnimationAtlas,                       // actual atlas
            NPC                                       // entity type
        );


        spidersC[i]->setAIType(Patroller);
        spidersC[i]->setAIState(IDLE);
        spidersC[i]->setAcceleration({ 0.0f, 9.0f });


        spidersC[i]->setColliderDimensions({
            spidersC[i]->getScale().x / 1.1f,
            spidersC[i]->getScale().y 
        });

        spidersC[i]->setSpeed(55);

        if (spiderCFacingRight[i]) spidersC[i]->moveRight();
        else spidersC[i]->moveLeft();

        spiderWebC[i] = new Entity( 
            {spidersC[i]->getPosition().x, spidersC[i]->getPosition().y } ,
            {75,75},
            "assets/game/text-to-png-4809682f-e20b-4438-ba7f-7bef84bac21d.png",
            EMPTY
        );

        spiderWebC[i]->deactivate(); 

    }

        /* ----------- PROTAGONIST ----------- */
    std::map<Abilities, std::vector<int>> xochitlAbilityAtlas = {
        { HOVERING, {  0,1,2,3,4,5,6,7,8,9,10,11  }},
        { SLASHING, { 168, 169, 168, 169,168, 169 } },
        { SMASHING, {180, 181, 182} }, 
        { DASHING, {132, 133, 134,135,136,137,138,139,140} },
        { CRAWLING, {48 ,49, 50, 51, 52, 53 ,54, 55}},
        { TAKEOFF, { 120, 121, 122,129,130}} ,
        { STANDING, { 36, 37, 38, 39, 40, 41, 42, 43}},
        { INVINCIBLE, {132, 133, 134, 136, 138, 139,140, 132, 133, 134, 136,138, 139,140, 132, 133, 134,136,138, 139,140, 132, 133, 134,136, 138, 139,140, 132, 133, 134, 136,138, 139, 140}}
    };


    mGameState.xochitl = new Entity(
        { mOrigin.x - 300.0f, mOrigin.y - 200.0f }, // position
        { 110.0f * sizeRatio, 100.0f },              // scale
        "assets/game/bb-sheet.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 16, 12 },                                    // atlas dimensions
        xochitlAnimationAtlas,                       // actual atlas
        PLAYER                                       // entity type
    );

    mGameState.xochitl->setAbilityAtlas(xochitlAbilityAtlas); 

    mGameState.xochitl->setJumpingPower(750.0f);

    mGameState.xochitl->setColliderDimensions({
        mGameState.xochitl->getScale().x / 2.5f,
        mGameState.xochitl->getScale().y / 2.3f
    });
    mGameState.xochitl->setAcceleration({ 0.0f, 1.0f });
    mGameState.xochitl->setInvincible();

    // BOSS

    mGameState.queenBee = new Entity(
        { mOrigin.x + 300.0f, mOrigin.y -200.0f }, // position
        { 250.0f * sizeRatio, 250.0f },              // scale
        "assets/game/bb-sheet.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 16, 12 },                                    // atlas dimensions
        xochitlAnimationAtlas,                       // actual atlas
        EMPTY                                       // entity type
    );

    mGameState.queenBee->setAbilityAtlas(xochitlAbilityAtlas); 



    mGameState.queenBee->setColliderDimensions({
        mGameState.queenBee->getScale().x / 2.5f,
        mGameState.queenBee->getScale().y / 2.0f
    });
    mGameState.queenBee->setAcceleration({ 0.0f, 0.0f });
    mGameState.queenBee->setSpeed(45.0f);
    mGameState.queenBee->activate(); 

    mGameState.queenBee->setAIState(IDLE); 

     queenHealth = 100; 
     queenHurt = false; 
     queenState = QUEEN_SLASH; // for now bee idles
     queenAttackTimer = 0.0f;
     queenAttackSpeed = 2.0f;
     queenAttacking = false;
     queenSmashTimer = 0.0f;
        //from pong
     queenBeeMovingUp = false;

         /* ----------- Slash ----------- */
    std::map<Direction, std::vector<int>> queenslashAtlas = {
        { RIGHT,  {  1  }},
        { LEFT,  {  1 }}, 
        { UP,  {  1 }},
        { DOWN,  {  1 }}
    };

    queenSlash = new Entity(
        { 0.0f, 0.0f}, // position
        { 250.0f * sizeRatio, 250.0f},              // scale
        "assets/game/slash-Sheet.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 1, 3},                                    // atlas dimensions
        queenslashAtlas,                       // actual atlas
        PLAYER                                       // entity type
    );

    queenSlash->setColliderDimensions( {queenSlash->getScale().x / 2, queenSlash->getScale().y / 2});

    queenSlash->deactivate();

    /*
        ----------- sun -----------
    */
                /* ----------- MITE ----------- */
    std::map<Direction, std::vector<int>> miteAnimationAtlas = {
        { DOWN,  { 20,21,22,23,24 }}, 
        { LEFT,  { 10, 11,12,13   }},
        { UP,    {   }}, // spider shoots web 
        { RIGHT, { 10, 11,12,13 }},
    };


    // //     Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);
    // mGameState.sun = new Entity(
    //     { mOrigin.x, mOrigin.y}, // position
    //     { 110.0f * sizeRatio, 100.0f },              // scale
    //     "assets/game/Scarab Sprite Sheet.png",                      // texture file address
    //     ATLAS,                                       // single image or atlas?
    //     { 5, 5 },                                    // atlas dimensions
    //     miteAnimationAtlas,                       // actual atlas
    //     NPC                                       // entity type
    // );

    // mGameState.sun->setAIType(LERPER);
    // mGameState.sun->setAIState(IDLE);

    // mGameState.sun->setLerpFactor(0.2);

    // mGameState.sun->setColliderDimensions({
    //     mGameState.sun->getScale().x / 1.1f,
    //     mGameState.sun->getScale().y / 1.1f
    // });


    std::map<Direction, std::vector<int>> rainAnimationAtlas = {
        {DOWN, { 0, 1}},
        {RIGHT, { 0, 1}},
        {LEFT, { 0, 1}},
        {UP , { 2,3,4,5,6,7,8,9}}
    };

    for (int i=0; i<max_raindropC; i++) {
        float spawnX = 15 + (rand() % (SCREEN_WIDTH - 30)) ; ; // 30 is padding for right side
        float spawnY = 0 - (rand() % 650) ;

        rainDropsC[i] = new Entity (
            { spawnX, spawnY}, 
            { 64,64},
            "assets/game/rain_splash_atlas.png",
            ATLAS,
            {1,10},
            rainAnimationAtlas,
            EMPTY
        );

        rainDropsC[i]->setSpeed(150);
        rainDropsC[i]->moveDown();
        rainSplashingC[i] = false; 
        rainTimerC[i] = 0.0f;

        rainDropsC[i]->setColliderDimensions( {rainDropsC[i]->getScale().x / 4.0f, rainDropsC[i]->getScale().y /5 } );

    }

    mGameState.jar = new Entity( 
        {mOrigin.x, mOrigin.y } ,
        {64,64},
        "assets/game/—Pngtree—king crown transparent background_14708628.png",
        EMPTY
    );

    mGameState.jar->deactivate();
    
}

void LevelC::update(float deltaTime)
{
    

    mGameState.xochitl->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.xochitl,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    // mGameState.sun->update(deltaTime, mGameState.xochitl, mGameState.map,
    //         nullptr, 0);
    
    if (mGameState.queenBee->isActive()) { 
        
        mGameState.queenBee->resetMovement();

        if (queenState == QUEEN_SLASH) {
            mGameState.queenBee->setSpeed(75.0);

            if (mGameState.queenBee->getPosition().x < mGameState.xochitl->getPosition().x) {
                mGameState.queenBee->moveRight();

            } else {
                mGameState.queenBee->moveLeft();
            }

            if (!(queenAttacking)) {

                if (queenBeeMovingUp) {
                    mGameState.queenBee->moveUp();

                } else {
                    mGameState.queenBee->moveDown();
                }

            }

            if (mGameState.queenBee->getPosition().y < (135.0)) {
                queenBeeMovingUp = false;
            }
            
            if (mGameState.queenBee->getPosition().y > (SCREEN_HEIGHT - 400)) {
                queenBeeMovingUp = true;
            }

            if (queenAttackSpeed > 0.0f) {
                queenAttackSpeed -=deltaTime;
            }


            if (!(queenAttacking) && queenAttackSpeed <= 0.0f) {
                queenAttacking = true;
                queenAttackTimer = 10.0;
                mGameState.queenBee->setSpeed(0); 
                mGameState.queenBee->slash();

                if (!(queenSlash->isActive()) ) {
                    spawnqueenSlash(*queenSlash, *mGameState.queenBee); 
                }

         
            }

            if (queenAttacking) {
                queenAttackTimer -=deltaTime;

                if (queenAttackTimer <= 0.0f) {
                    queenAttacking = true;
                    queenState = QUEEN_SMASH; 
                    queenAttackSpeed = 0.0;
                    queenAttackTimer = 5.0f; 
                    queenSmashTimer = 0.0f;
                }
            }

        }

        if (queenState == QUEEN_IDLE) {
            mGameState.queenBee->setSpeed(0.0f);
            mGameState.queenBee->idle();

        }
        mGameState.queenBee->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0); 
        
    }

    //boss slashes and can damge player 
    if (queenSlash->isActive()) {
        queenSlash->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0); 

        queenSlashDistance -= queenSlashSpeed *deltaTime;

        if (queenSlash->checkCollision(mGameState.xochitl)) {
            queenSlash->deactivate();
         
            if (!(mGameState.xochitl->isInvincible())) {
                lives--;
                mGameState.xochitl->setInvincible();
            }
        }

        if (queenSlashDistance <= 0.0f) {
            queenSlash->deactivate();
        }
    }

    //queen can b hit by player smashing
    if (mGameState.queenBee->isActive()) {
        if(mGameState.xochitl->isSmashing() && mGameState.xochitl->checkCollision(mGameState.queenBee) && !(queenHurt)) {
            queenHealth-= 1;
            queenHurt = true;

        }

        //now for slashing
        if(gSlash->isActive() && gSlash->checkCollision(mGameState.queenBee) && (queenState == QUEEN_SMASH) ){
            gSlash->deactivate();
            queenHealth-= 1;
            

        }
    }

    if (queenState == QUEEN_SMASH) {
        
        queenLerp(mGameState.queenBee, mGameState.xochitl, 0.5f, deltaTime);

        // to face player 
        if (mGameState.queenBee->getPosition().x < mGameState.xochitl->getPosition().x) {
                mGameState.queenBee->moveRight();

        } else {
            mGameState.queenBee->moveLeft();
        }
        
        queenAttackTimer -= deltaTime; 
        queenSmashTimer -= deltaTime; 

        if (queenSmashTimer <= 0.0f) {
            mGameState.queenBee->smash();
            queenSmashTimer = queenSmashSpeed; 
        }
        
        if (mGameState.queenBee->checkCollision(mGameState.xochitl)) {
            if (!(mGameState.xochitl->isInvincible())) {
                lives--;
                mGameState.xochitl->setInvincible();
            }
        }

        if (queenAttackTimer <= 0.0f) {
            queenState = QUEEN_SLASH;
            queenAttacking = false;
            queenAttackSpeed = 3.0;
        }
    }

    //player cant damage slashing bee wiht slahses forces movement 

    if (mGameState.queenBee->isActive() && gSlash->isActive() && gSlash->checkCollision(mGameState.queenBee) && (queenState == QUEEN_SLASH)) {
        gSlash->deactivate();
        showShield(mGameState.queenBee);
    }

    if (queenHealth <= 0) {
        totalEnemiesLevelC--;
        mGameState.queenBee->deactivate(); 
        
    }
    if (!mGameState.xochitl->isSmashing()) {
        queenHurt = false; 
    }

    
    for (int i = 0; i<max_spider_C; i++) {
    // slash

    // check if slash touches spider
    if (gSlash->isActive() && gSlash->checkCollision(spidersC[i])){
        // if touches and gslash is active then deactivate teh slash and shield the target this case the spider

        gSlash->deactivate();
        showShield(spidersC[i]); 
    }

    // smash

    if (spidersC[i]->isActive() && mGameState.xochitl->isSmashing() && mGameState.xochitl->checkCollision(spidersC[i]) && !(spiderCHurt[i])) { // spider is active, bee is smashing, and collising with spider? 
        spiderCHealth[i] -=1; 
        spiderCHurt[i] = true; 
        if ((spiderCHealth[i] <= 0) && !(spiderCDead[i])) {
            spiderCDead[i] = true;
            totalEnemiesLevelC--;
            spiderCDeathTimer[i] = 3.0f;

            spiderCAttacking[i] = false;
            spiderCWebbed[i] = false;

            spiderWebC[i]->deactivate();

            spidersC[i]->setSpeed(0);
            spidersC[i]->setDirection(DOWN);
            
        }
    }

    if (!(mGameState.xochitl->isSmashing())) {
        spiderCHurt[i] = false; 
    }

    if (spiderCDead[i]) {
        spiderCDeathTimer[i] -= deltaTime;
        if (spiderCDeathTimer[i] <= 0) {
            spidersC[i]->deactivate();
            spiderCDead[i] = false;
        }
    }

    if (!spiderCDead[i]) {

    // making spider attack soon 
    if (spiderCAttackSpeed[i] > 0) {
        spiderCAttackSpeed[i] -= deltaTime;
    }

    // spider is moving and not attacking but now spider should attack 
    if (spidersC[i]->isActive() && !(spiderCAttacking[i]) && (spiderCAttackSpeed[i] <= 0.0f) ) {
        spiderCAttacking[i] = true;  
        spiderCAttackTimer[i] = 0.5f;  // how long to be animating 
        spidersC[i]->setSpeed(0); // stop moving the spider 
        spiderCFacingRight[i] = (spidersC[i]->getDirection() == RIGHT);
        spidersC[i]->setDirection(UP); // shoot web animation
    }
        //spidersC[i][i] attacking so shoot web
    if (spiderCAttacking[i]) {
        spiderCAttackTimer[i] -= deltaTime; 
        spidersC[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        if (!(spiderCWebbed[i])) { //spider didn't shoot web then shoot 
            shootWeb(spiderWebC[i], spidersC[i], spiderCFacingRight[i]); // from main
            spiderCWebbed[i] = true;
        }
        

        // spider attack time is done so reset 
        if (spiderCAttackTimer[i] <= 0.0f) {
            spiderCAttacking[i] = false;
            spiderCAttackSpeed[i] = 5.0 + (rand() % 4);
            spidersC[i]->setSpeed(55); // start moving the spider again
            spiderCWebbed[i] = false;          

            bool directionGuess = (rand()%2 == 0);

            if (directionGuess) spidersC[i]->moveRight();
            else spidersC[i]->moveLeft();
        }
    } else {
        spidersC[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);
    }

    if (spiderWebC[i]->isActive()) { //spider web is spawned
        spiderWebC[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr,0);

        if (spiderWebC[i]->checkCollision(mGameState.xochitl)) { // web collieds with player 
            spiderWebC[i]->deactivate();
            
            if (!(mGameState.xochitl->isInvincible())) { // check for player already didnt take damage
                lives --;
                mGameState.xochitl->setInvincible(); 
            }

        }

        if (spiderWebC[i]->isCollidingRight() ) {
            spiderWebC[i]->resetMovement();
            spiderWebC[i]->moveUp();
            spiderWebC[i]->moveLeft();
        }

        if (spiderWebC[i]->isCollidingLeft()) {
            spiderWebC[i]->resetMovement();
            spiderWebC[i]->moveUp();
            spiderWebC[i]->moveRight();
        }


        if (spiderWebC[i]->getPosition().y < mOrigin.y - 600.0f) {
            spiderWebC[i]->deactivate();
        }

    }
    }
    
    }


    // if (mGameState.sun->checkCollision(mGameState.xochitl)) {

    //     if (!(mGameState.xochitl->isInvincible())) {
    //         lives--;
    //         mGameState.xochitl->setInvincible();

    //     }

    // }

    for (int i=0; i< max_raindropC; i++) {
        rainDropsC[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        // touches bottom
        if (!(rainSplashingC[i]) && rainDropsC[i]->isCollidingBottom()) {
            rainSplashingC[i] = true;
            rainTimerC[i] = 0.35;

            rainDropsC[i]->setSpeed(0);
            rainDropsC[i]->setDirection(UP);

        }

        // touches player
        if (!(rainSplashingC[i]) && rainDropsC[i]->checkCollision(mGameState.xochitl)) {
            rainSplashingC[i] = true;
            rainTimerC[i] = 0.35;

            rainDropsC[i]->setSpeed(0);
            rainDropsC[i]->setDirection(UP);

            if (!(mGameState.xochitl->isInvincible())) {
                lives--;
                mGameState.xochitl->setInvincible();
            }



        }

        if (!(rainSplashingC[i]) && (rainDropsC[i]->getPosition().y > SCREEN_HEIGHT)) {
            rainSplashingC[i] = true;
            rainTimerC[i] = 0.35;

            rainDropsC[i]->setSpeed(0);
            rainDropsC[i]->setDirection(UP);

        }

        // rain is splashing 

        if (rainSplashingC[i]) {
            rainTimerC[i] -= deltaTime; 

            if (rainTimerC[i] <= 0.0f) {
                
                float newspawnX = 15 + (rand() % (SCREEN_WIDTH - 30)) ;;
                float newspawnY = 0 - (rand() % 50) ;

                rainDropsC[i]->setPosition( {newspawnX, newspawnY});
                rainDropsC[i]->resetMovement();
                rainDropsC[i]->setSpeed(150);
                rainDropsC[i]->moveDown();
                rainSplashingC[i] = false; 
                rainTimerC[i] = 0.0f;

            }
        }


    }

    if (lives == 0) {
        mGameState.nextSceneID=3;
    }

     if (totalEnemiesLevelC <= 0 ) {
        mGameState.jar->activate();
    }

    if (mGameState.jar->checkCollision(mGameState.xochitl) && (totalEnemiesLevelC <= 0)) {mGameState.nextSceneID = 4;}
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.xochitl->render();
    
    // mGameState.sun->render();

    for (int i = 0; i<max_spider_C; i++){

        if (spidersC[i]->isActive()) spidersC[i]->render();
    }

    for (int i = 0; i<max_spider_C; i++){
        if (spiderWebC[i]->isActive()) spiderWebC[i]->render();
    }

    for ( int i =0; i<max_raindropC; i++) {
        rainDropsC[i]->render();
      
    }

    mGameState.map->render();

    if (mGameState.queenBee->isActive()) mGameState.queenBee->render(); 

    if (queenSlash->isActive()) {queenSlash->render();}
    
    if(mGameState.jar->isActive()) {mGameState.jar->render();}
}

void LevelC::shutdown()
{
    delete mGameState.xochitl;
    mGameState.xochitl = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    // delete mGameState.sun;
    // mGameState.sun = nullptr;

    delete mGameState.queenBee;
    mGameState.queenBee = nullptr;

    for  ( int i =0; i<max_raindropC; i++) {
        delete rainDropsC[i];
        rainDropsC[i] = nullptr; 
    }

    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.dashSound);
    UnloadSound(mGameState.buzzing);
    mGameState.jumpSound = {};
}
