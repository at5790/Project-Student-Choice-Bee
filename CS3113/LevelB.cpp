#include "LevelB.h"
#include <random>

// for end game
int totalEnemiesLevelB = 5; 

constexpr int SCREEN_WIDTH = 1400;
constexpr int SCREEN_HEIGHT = 900; 
// for shield
extern Entity *gSlash;
void showShield( Entity *target); 
void shootWeb(Entity *web, Entity *shooter, bool side);

// for spiders[i]

constexpr int max_spider = 5;

Entity *spiders[max_spider];
Entity *spiderWeb[max_spider]; 

int spiderBHealth[max_spider] = {5,5,5,5,5}; 
bool spiderBHurt[max_spider] = {}; 

bool spiderBDead[max_spider] = {}; 
float spiderBDeathTimer[max_spider] = {}; 

bool spiderBAttacking[max_spider] = {}; 
float spiderBAttackTimer[max_spider] = {};
bool spiderBWebbed[max_spider] = {};
float spiderBAttackSpeed[max_spider] = {};
bool spiderBFacingRight[max_spider] = {}; 

// for raindrop

const int max_raindrop = 12; 
Entity *rainDrops[max_raindrop];
bool rainSplashing[max_raindrop] = {};
float rainTimer[max_raindrop] = {};

LevelB::LevelB()                                      : Scene{ {},     nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{

    int totalEnemiesLevelB = 5; 

    mGameState.nextSceneID = 5; // placeholder 


    for (int i=0; i < max_spider; i++) {
    
     int randint = (rand()%2 == 1);

     spiderBHealth[i] = 5; 
     spiderBHurt[i] = false; 

     spiderBDead[i] = false; 
     spiderBDeathTimer[i] = 0.0; 

     spiderBAttacking[i] = 0.0; 
     spiderBAttackTimer[i] = 0.0;
     spiderBWebbed[i] = false;
     spiderBAttackSpeed[i] = 3.0 + i*.4;

     
     spiderBFacingRight[i] = randint; 
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

    for ( int i =0; i < max_spider; i ++) {

        float spawnSpiderX = 500 + (rand() % (SCREEN_WIDTH - 1000)) ; // 30 is padding for right side 

        spiders[i] = new Entity(
            {spawnSpiderX, mOrigin.y + 290.0f }, // position
            { 160.0f , 100.0f },              // scale
            "assets/game/GandalfHardcore Pixel Art Spider.png",                      // texture file address
            ATLAS,                                       // single image or atlas?
            { 6,8 },                                    // atlas dimensions
            spiderAnimationAtlas,                       // actual atlas
            NPC                                       // entity type
        );


        spiders[i]->setAIType(Patroller);
        spiders[i]->setAIState(IDLE);
        spiders[i]->setAcceleration({ 0.0f, 9.0f });


        spiders[i]->setColliderDimensions({
            spiders[i]->getScale().x / 1.1f,
            spiders[i]->getScale().y 
        });

        spiders[i]->setSpeed(55);

        if (spiderBFacingRight[i]) spiders[i]->moveRight();
        else spiders[i]->moveLeft();

        spiderWeb[i] = new Entity( 
            {spiders[i]->getPosition().x, spiders[i]->getPosition().y } ,
            {75,75},
            "assets/game/text-to-png-4809682f-e20b-4438-ba7f-7bef84bac21d.png",
            EMPTY
        );

        spiderWeb[i]->deactivate(); 

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

    for (int i=0; i<max_raindrop; i++) {
        float spawnX = 15 + (rand() % (SCREEN_WIDTH - 30)) ; ; // 30 is padding for right side
        float spawnY = 0 - (rand() % 650) ;

        rainDrops[i] = new Entity (
            { spawnX, spawnY}, 
            { 64,64},
            "assets/game/rain_splash_atlas.png",
            ATLAS,
            {1,10},
            rainAnimationAtlas,
            EMPTY
        );

        rainDrops[i]->setSpeed(200);
        rainDrops[i]->moveDown();
        rainSplashing[i] = false; 
        rainTimer[i] = 0.0f;

        rainDrops[i]->setColliderDimensions( {rainDrops[i]->getScale().x / 4.0f, rainDrops[i]->getScale().y /5 } );

        mGameState.jar = new Entity( 
            {mOrigin.x, mOrigin.y } ,
            {64,64},
            "assets/game/pngwing.com (1).png",
            EMPTY
        );

        mGameState.jar->deactivate();

    }
    
}

void LevelB::update(float deltaTime)
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

    
    for (int i = 0; i<max_spider; i++) {

  
    // slash

    // check if slash touches spider
    if (gSlash->isActive() && gSlash->checkCollision(spiders[i])){
        // if touches and gslash is active then deactivate teh slash and shield the target this case the spider

        gSlash->deactivate();
        showShield(spiders[i]); 
    }

    // smash

    if (spiders[i]->isActive() && mGameState.xochitl->isSmashing() && mGameState.xochitl->checkCollision(spiders[i]) && !(spiderBHurt[i])) { // spider is active, bee is smashing, and collising with spider? 
        spiderBHealth[i] -=1; 
        spiderBHurt[i] = true; 
        if ((spiderBHealth[i] <= 0) && !(spiderBDead[i])){
            spiderBDead[i] = true;
            totalEnemiesLevelB--;

            spiderBDeathTimer[i] = 3.0f;

            spiderBAttacking[i] = false;
            spiderBWebbed[i] = false;

            spiderWeb[i]->deactivate();
      

            spiders[i]->setSpeed(0);
            spiders[i]->setDirection(DOWN);
            
        }
    }

    if (!(mGameState.xochitl->isSmashing())) {
        spiderBHurt[i] = false; 
    }

    // spiders dead and play animation until its over 
    if (spiderBDead[i]) {
        
        spiderBDeathTimer[i] -= deltaTime;
        if (spiderBDeathTimer[i] <= 0) {
            spiders[i]->deactivate();
            spiderBDead[i] = false;
        }
    }

    if (!spiderBDead[i]) {
    // making spider attack soon 
    if (spiderBAttackSpeed[i] > 0) {
        spiderBAttackSpeed[i] -= deltaTime;
    }

    // spider is moving and not attacking but now spider should attack 
    if (spiders[i]->isActive() && !(spiderBAttacking[i]) && (spiderBAttackSpeed[i] <= 0.0f) ) {
        spiderBAttacking[i] = true;  
        spiderBAttackTimer[i] = 0.5f;  // how long to be animating 
        spiders[i]->setSpeed(0); // stop moving the spider 
        spiderBFacingRight[i] = (spiders[i]->getDirection() == RIGHT);
        spiders[i]->setDirection(UP); // shoot web animation
    }

    
        //spiders[i][i] attacking so shoot web
    if (spiderBAttacking[i]) {
        spiderBAttackTimer[i] -= deltaTime; 
        spiders[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        if (!(spiderBWebbed[i])) { //spider didn't shoot web then shoot 
            shootWeb(spiderWeb[i], spiders[i], spiderBFacingRight[i]); // from main
            spiderBWebbed[i] = true;
        }
        

        // spider attack time is done so reset 
        if (spiderBAttackTimer[i] <= 0.0f) {
            spiderBAttacking[i] = false;
            spiderBAttackSpeed[i] = 5.0 + (rand() % 4);
            spiders[i]->setSpeed(55); // start moving the spider again
            spiderBWebbed[i] = false;          

            bool directionGuess = (rand()%2 == 0);

            if (directionGuess) spiders[i]->moveRight();
            else spiders[i]->moveLeft();
        }
    } else {
        spiders[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);
    }

    if (spiderWeb[i]->isActive()) { //spider web is spawned
        spiderWeb[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr,0);

        if (spiderWeb[i]->checkCollision(mGameState.xochitl)) { // web collieds with player 
            spiderWeb[i]->deactivate();
            
            if (!(mGameState.xochitl->isInvincible())) { // check for player already didnt take damage
                lives --;
                mGameState.xochitl->setInvincible(); 
            }

        }

        if (spiderWeb[i]->isCollidingRight() ) {
            spiderWeb[i]->resetMovement();
            spiderWeb[i]->moveUp();
            spiderWeb[i]->moveLeft();
        }

        if (spiderWeb[i]->isCollidingLeft()) {
            spiderWeb[i]->resetMovement();
            spiderWeb[i]->moveUp();
            spiderWeb[i]->moveRight();
        }


        if (spiderWeb[i]->getPosition().y < mOrigin.y - 600.0f) {
            spiderWeb[i]->deactivate();
        }
    }

    }
    
    }

    if (mGameState.xochitl->getPosition().y > END_GAME_THRESHOLD) mGameState.nextSceneID = 1;

    // if (mGameState.sun->checkCollision(mGameState.xochitl)) {

    //     if (!(mGameState.xochitl->isInvincible())) {
    //         lives--;
    //         mGameState.xochitl->setInvincible();

    //     }

    // }

    for (int i=0; i< max_raindrop; i++) {
        rainDrops[i]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        // touches bottom
        if (!(rainSplashing[i]) && rainDrops[i]->isCollidingBottom()) {
            rainSplashing[i] = true;
            rainTimer[i] = 0.35;

            rainDrops[i]->setSpeed(0);
            rainDrops[i]->setDirection(UP);

        }

        // touches player
        if (!(rainSplashing[i]) && rainDrops[i]->checkCollision(mGameState.xochitl)) {
            rainSplashing[i] = true;
            rainTimer[i] = 0.35;

            rainDrops[i]->setSpeed(0);
            rainDrops[i]->setDirection(UP);

            if (!(mGameState.xochitl->isInvincible())) {
                lives--;
                mGameState.xochitl->setInvincible();
            }



        }

        if (!(rainSplashing[i]) && (rainDrops[i]->getPosition().y > SCREEN_HEIGHT)) {
            rainSplashing[i] = true;
            rainTimer[i] = 0.35;

            rainDrops[i]->setSpeed(0);
            rainDrops[i]->setDirection(UP);

        }

        // rain is splashing 

        if (rainSplashing[i]) {
            rainTimer[i] -= deltaTime; 

            if (rainTimer[i] <= 0.0f) {
                
                float newspawnX = 15 + (rand() % (SCREEN_WIDTH - 30)) ;;
                float newspawnY = 0 - (rand() % 50) ;

                rainDrops[i]->setPosition( {newspawnX, newspawnY});
                rainDrops[i]->resetMovement();
                rainDrops[i]->setSpeed(200);
                rainDrops[i]->moveDown();
                rainSplashing[i] = false; 
                rainTimer[i] = 0.0f;

            }
        }


    }

    if (totalEnemiesLevelB <= 0 ) {
        mGameState.jar->activate();
    }

    if (mGameState.jar->checkCollision(mGameState.xochitl) && (totalEnemiesLevelB <= 0)) {mGameState.nextSceneID = 2;}
}


void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.xochitl->render();
    
    // mGameState.sun->render();

    for (int i = 0; i<max_spider; i++){

        if (spiders[i]->isActive()) spiders[i]->render();
    }

    for (int i = 0; i<max_spider; i++){
        if (spiderWeb[i]->isActive()) spiderWeb[i]->render();
    }

    for ( int i =0; i<max_raindrop; i++) {
        rainDrops[i]->render();
        
    }

    mGameState.map->render();

    if(mGameState.jar->isActive()) {mGameState.jar->render();}
    
}

void LevelB::shutdown()
{
    delete mGameState.xochitl;
    mGameState.xochitl = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    // delete mGameState.sun;
    // mGameState.sun = nullptr;

    for  ( int i =0; i<max_raindrop; i++) {
        delete rainDrops[i];
        rainDrops[i] = nullptr; 
    }

    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.dashSound);
    UnloadSound(mGameState.buzzing);
    mGameState.jumpSound = {};
}
