#include "LevelA.h"
#include <random>

// for finsihed round
int totalEnemies = 2; 

// for shield
extern Entity *gSlash;
void showShield( Entity *target); 
void shootWeb(Entity *web, Entity *shooter, bool side);

// for spider
int spiderHealth = 3; 
bool spiderHurt = false; 

bool spiderDead = false; 
float spiderDeathTimer = 0.0f; 

bool spiderAttacking = false; 
float spiderAttackTimer = 0.0f;
bool spiderWebbed = false;
float spiderAttackSpeed = 0.0f;
bool spiderFacingRight = true; 

// for mite / sun

int miteHealth = 1;
bool miteHurt = false;

bool miteDead = false;
float miteDeathTimer = 0.0f; 

LevelA::LevelA()                                      : Scene{ {},     nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene{ origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{   
    int totalEnemies = 2; 
    mGameState.nextSceneID = 5; // placeholder 


    float sizeRatio = 48.0f / 64.0f;

    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
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

        /* ----------- SPIDER ----------- */
    std::map<Direction, std::vector<int>> spiderAnimationAtlas = {
        { DOWN,  { 40, 40, 40,41,41,  41,42,42,  42,43,43, 43,44, 44, 44  }}, // spider dies
        { LEFT,  { 8,9,10,11,12,13,14,15  }},
        { UP,    { 24,25,26,27,28,29  }}, // spider shoots web 
        { RIGHT, { 8,9,10,11,12,13,14,15  }},

    };


    mGameState.spider = new Entity(
        {mOrigin.x + 100.0f, mOrigin.y + 290.0f}, // position
        { 160.0f , 100.0f },              // scale
        "assets/game/GandalfHardcore Pixel Art Spider.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 6,8 },                                    // atlas dimensions
        spiderAnimationAtlas,                       // actual atlas
        NPC                                       // entity type
    );


    mGameState.spider->setAIType(Patroller);
    mGameState.spider->setAIState(IDLE);
    mGameState.spider->setAcceleration({ 0.0f, 9.0f });


    mGameState.spider->setColliderDimensions({
        mGameState.spider->getScale().x / 1.1f,
        mGameState.spider->getScale().y 
    });

    mGameState.spider->setSpeed(55);
    mGameState.spider->moveRight();

    spiderHealth =3;
    spiderHurt = false;
    spiderDead = false;
    spiderDeathTimer = 0.0f;



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
    mGameState.sun = new Entity(
        { mOrigin.x, mOrigin.y}, // position
        { 110.0f * sizeRatio, 100.0f },              // scale
        "assets/game/Scarab Sprite Sheet.png",                      // texture file address
        ATLAS,                                       // single image or atlas?
        { 5, 5 },                                    // atlas dimensions
        miteAnimationAtlas,                       // actual atlas
        NPC                                       // entity type
    );

    mGameState.sun->setAIType(LERPER);
    mGameState.sun->setAIState(IDLE);

    mGameState.sun->setLerpFactor(0.2);

    mGameState.sun->setColliderDimensions({
        mGameState.sun->getScale().x / 1.1f,
        mGameState.sun->getScale().y / 1.1f
    });

     miteHealth = 1;
     miteHurt = false;

     miteDead = false;
     miteDeathTimer = 1.5f; 

    mGameState.spiderWeb = new Entity( 
        {mGameState.spider->getPosition().x, mGameState.spider->getPosition().y } ,
        {75,75},
        "assets/game/text-to-png-4809682f-e20b-4438-ba7f-7bef84bac21d.png",
        EMPTY
    );

    mGameState.spiderWeb->deactivate(); 

     spiderAttacking = false; 
     spiderAttackTimer = 0.0f;
     spiderWebbed = false;
     spiderAttackSpeed = 2.0f;

    mGameState.jar = new Entity( 
        {mOrigin.x, mOrigin.y } ,
        {64,64},
        "assets/game/pngwing.com (1).png",
        EMPTY
    );

    mGameState.jar->deactivate();
    
    
}

void LevelA::update(float deltaTime)
{
    

    mGameState.xochitl->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.xochitl,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    if (mGameState.xochitl->isCollidingRight() || mGameState.xochitl->isCollidingLeft()) {
  
        mGameState.xochitl->resetMovement();
    }

    if (mGameState.sun->isActive()) {

        if (miteDead) {

            {
                totalEnemies--;
                miteDeathTimer -= deltaTime;

                mGameState.sun->setSpeed(0); 
                mGameState.sun->setDirection(DOWN);
                mGameState.sun->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

                if (miteDeathTimer <= 0.0f) {
                    mGameState.sun->deactivate();
                    miteDead = false;
                }
            }
        } else {
            mGameState.sun->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

            if (gSlash->isActive() && gSlash->checkCollision(mGameState.sun) && (!miteHurt)) {

                miteHealth -=1;
                miteHurt = true;

                if (miteHealth <=0) {
                    mGameState.sun->deactivate();
                    miteDead= true;
                    mGameState.sun->setSpeed(0); 
                    mGameState.sun->setDirection(DOWN);
                }
            }
        }

            if (mGameState.sun->checkCollision(mGameState.xochitl)) {
                if (!(mGameState.xochitl->isInvincible())) {
                    lives--;
                    mGameState.xochitl->setInvincible();
                }

                if (lives == 0) mGameState.nextSceneID=3;
            }

    }

    if (!(mGameState.xochitl->isSlashing())) {
        miteHurt = false; 
    }
    

    
    
    // slash

    // check if slash touches spider
    if (gSlash->isActive() && gSlash->checkCollision(mGameState.spider)){
        // if touches and gslash is active then deactivate teh slash and shield the target this case the spider

        gSlash->deactivate();
        showShield(mGameState.spider); 
    }

    // smash

    if (mGameState.spider->isActive() && mGameState.xochitl->isSmashing() && mGameState.xochitl->checkCollision(mGameState.spider) && !(spiderHurt)) { // spider is active, bee is smashing, and collising with spider? 
        spiderHealth -=1; 
        spiderHurt = true; 
        if (spiderHealth <= 0) {
            spiderDead = true;
            spiderDeathTimer = 3.0f;

            spiderAttacking = false;
            spiderWebbed = false;

            mGameState.spiderWeb->deactivate();

            mGameState.spider->setSpeed(0);
            mGameState.spider->setDirection(DOWN);
            
            
        }
    }

    if (!(mGameState.xochitl->isSmashing())) {
        spiderHurt = false; 
    }

    if (spiderDead) {
        totalEnemies--;
        spiderDeathTimer -= deltaTime;

        spiderAttacking = false;

        mGameState.spider->setSpeed(0);
        mGameState.spider->setDirection(DOWN);
        mGameState.spider->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);
        mGameState.spiderWeb->deactivate();

        if (spiderDeathTimer <= 0) {
            mGameState.spider->deactivate();
            mGameState.spiderWeb->deactivate();
            spiderDead = false;
        }


    }


    // making spider attack soon 

    if (!spiderDead) {
       
    if (spiderAttackSpeed > 0) {
        spiderAttackSpeed -= deltaTime;
    }

    // spider is moving and not attacking but now spider should attack 
    if (mGameState.spider->isActive() && !(spiderAttacking) && (spiderAttackSpeed <= 0.0f) ) {
        spiderAttacking = true;  
        spiderAttackTimer = 0.5f;  // how long to be animating 
        mGameState.spider->setSpeed(0); // stop moving the spider 
        spiderFacingRight = (mGameState.spider->getDirection() == RIGHT);
        mGameState.spider->setDirection(UP); // shoot web animation
    }
        //spiders attacking so shoot web
    if (spiderAttacking) {
        spiderAttackTimer -= deltaTime; 
        mGameState.spider->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        if (!(spiderWebbed)) { //spider didn't shoot web then shoot 
            shootWeb(mGameState.spiderWeb, mGameState.spider, spiderFacingRight); // from main
            spiderWebbed = true;
        }
        

        // spider attack time is done so reset 
        if (spiderAttackTimer <= 0.0f) {
            spiderAttacking = false;
            spiderAttackSpeed = 5.0;
            mGameState.spider->setSpeed(55); // start moving the spider again
            spiderWebbed = false;          

            bool directionGuess = (rand()%2 == 0);

            if (directionGuess) mGameState.spider->moveRight();
            else mGameState.spider->moveLeft();
        }
    } else {
        mGameState.spider->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);
    }

    if (mGameState.spiderWeb->isActive()) { //spider web is spawned
        mGameState.spiderWeb->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr,0);

        if (mGameState.spiderWeb->checkCollision(mGameState.xochitl)) { // web collieds with player 
            mGameState.spiderWeb->deactivate();
            
            if (!(mGameState.xochitl->isInvincible())) { // check for player already didnt take damage
                lives -= 1;
                mGameState.xochitl->setInvincible(); 
            }

            if (lives == 0) {
                mGameState.nextSceneID=3;
            }
        }

        if (mGameState.spiderWeb->isCollidingRight() ) {
            mGameState.spiderWeb->resetMovement();
            mGameState.spiderWeb->moveUp();
            mGameState.spiderWeb->moveLeft();
        }

        if (mGameState.spiderWeb->isCollidingLeft()) {
            mGameState.spiderWeb->resetMovement();
            mGameState.spiderWeb->moveUp();
            mGameState.spiderWeb->moveRight();
        }


        if (mGameState.spiderWeb->getPosition().y < mOrigin.y - 600.0f) {
            mGameState.spiderWeb->deactivate();
        }

    }

    }
    
    if (totalEnemies <= 0 ) {
        mGameState.jar->activate();
    }

    if (mGameState.jar->checkCollision(mGameState.xochitl) && (totalEnemies <= 0)) {mGameState.nextSceneID = 1;}

}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.xochitl->render();
    
    mGameState.sun->render();
 
    mGameState.spider->render();


    mGameState.map->render();

    if (mGameState.spiderWeb->isActive()) mGameState.spiderWeb->render();

    if(mGameState.jar->isActive()) {mGameState.jar->render();}
    
}

void LevelA::shutdown()
{
    delete mGameState.xochitl;
    mGameState.xochitl = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    delete mGameState.sun;
    mGameState.sun = nullptr;


    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.dashSound);
    UnloadSound(mGameState.buzzing);
    mGameState.jumpSound = {};
}
