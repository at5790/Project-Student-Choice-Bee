#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction    { LEFT, UP, RIGHT, DOWN               }; // For walking but now for activity
enum Abilities    { HOVERING, SLASHING, SMASHING, DASHING, TAKEOFF, CRAWLING, STANDING, INVINCIBLE};
enum EntityStatus { ACTIVE, INACTIVE                    };
enum EntityType   { PLAYER, BLOCK, PLATFORM, NPC, EMPTY };
enum AIType       { WANDERER, FOLLOWER, LERPER , Patroller, Flyer               };
enum AIState      { WALKING, IDLE, FOLLOWING            };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D   mTexture;
    TextureType mTextureType;
    Vector2     mSpriteSheetDimensions;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int>                      mAnimationIndices;
    Direction                             mDirection;
    bool                                  mFacingRight = true;
    int                                   mFrameSpeed;

    int   mCurrentFrameIndex = 0;
    float mAnimationTime     = 0.0f;

        // for animation

    std::map<Abilities, std::vector<int>> mAbilityAtlas;
    std::vector<int>                      mAbilityIndices;
    Abilities                             mAbility = HOVERING;

    int   mAbilityFrameIndex = 0;
    float mAbilityTime     = 0.0f;

    bool  mIsJumping    = false;
    float mJumpingPower = 0.0f;

    // added for the bee

    bool mIsDashing = false;
    float mDashingPower = 4000.0f;

    bool mIsSlashing = false;

    bool mIsSmashing = false;

    bool mIsTakingOff = false;
    bool mTouchingGround = false;

    bool mIsCrawling = false;

    bool mIsInvincible = false; 


    int   mSpeed;

    
    float mAngle; 
    //from project 1 
    float flyerAngle = 0;
    Vector2 flyerOrigin = { 0.0f, 0.0f};

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;

    AIType  mAIType;
    AIState mAIState;

    float mLerpFactor = 0.1f;

    bool isColliding(Entity *other) const;

    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionY(Map *map);

    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionX(Map *map);

    void resetColliderFlags()
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);
        
    void animateAbility(float deltaTime);


    void AIActivate(Entity *target, float deltaTime); // from project 3
    void AIWander();
    void AIFollow(Entity *target);


    void AILerp(Entity *target, float deltaTime); // proj 3

    void AIFly(float deltaTime); 

    // added for facing left
    // original plan was to reverse the spritesheet but it didnt work apparently raylib if u negate width it reads from right to left so used buillt in direction 



public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 8;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(
        Vector2     position,
        Vector2     scale,
        const char *textureFilepath,
        EntityType  entityType
    );
    Entity(
        Vector2                               position,
        Vector2                               scale,
        const char                           *textureFilepath,
        TextureType                           textureType,
        Vector2                               spriteSheetDimensions,
        std::map<Direction, std::vector<int>> animationAtlas,
        EntityType                            entityType
    );
    ~Entity();

    void update(
        float   deltaTime,
        Entity *player,
        Map    *map,
        Entity *collidableEntities,
        int     collisionCheckCount
    );
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    void jump()       { mIsJumping     = true;     }

    // added moves  for the bee player 
    // ability frame index and time had to be reset. Correct bools set to true and from Atlas the right index chosen
    void dash() { mIsDashing = true; mAbility = DASHING; mAbilityIndices = mAbilityAtlas.at(DASHING); mAbilityFrameIndex =0; mAbilityTime = 0.0f; }

    void slash() {mIsSlashing = true; mAbility = SLASHING; mAbilityIndices = mAbilityAtlas.at(SLASHING); mAbilityFrameIndex =0; mAbilityTime = 0.0f;}

    void smash() {mIsSmashing = true; mAbility = SMASHING; mAbilityIndices = mAbilityAtlas.at(SMASHING); mAbilityFrameIndex =0; mAbilityTime = 0.0f;} 

    void crawl()  { if ( (mAbility == SLASHING) || (mAbility == SMASHING)  || (mAbility == DASHING) || (mAbility == TAKEOFF)) return; if (mAbility != CRAWLING) {mIsCrawling = true; mAbility = CRAWLING; mAbilityIndices = mAbilityAtlas.at(CRAWLING); mAbilityFrameIndex =0; mAbilityTime = 0.0f;}  }

    void stopCrawling() { if (mAbility == CRAWLING )  {mIsCrawling = false; mAbility = HOVERING; mAbilityIndices = mAbilityAtlas.at(HOVERING); mAbilityFrameIndex =0; mAbilityTime = 0.0f;}}

    void takeOff() { if ( (mAbility == SLASHING) || (mAbility == SMASHING)  || (mAbility == DASHING) ) return; mIsTakingOff = true; mAbility = TAKEOFF; mAbilityIndices = mAbilityAtlas.at(TAKEOFF); mAbilityFrameIndex =0; mAbilityTime = 0.0f;} 

    void setInvincible() {mIsInvincible= true; mAbility = INVINCIBLE; mAbilityIndices = mAbilityAtlas.at(INVINCIBLE); mAbilityFrameIndex =0; mAbilityTime = 0.0f;} 

    void activate()   { mEntityStatus  = ACTIVE;   }
    void deactivate() { mEntityStatus  = INACTIVE; }
    void displayCollider();

    bool isActive() { return mEntityStatus == ACTIVE; }

    void moveUp()    { mMovement.y = -1;    }
    void moveDown()  { mMovement.y =  1; mDirection = DOWN; }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT; mFacingRight = false; }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; mFacingRight = true;}

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mColliderDimensions;    }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    bool        isJumping()                const { return mIsJumping;             }

    float getDashingPower() const {return mDashingPower; }

    Abilities getAbility() const { return mAbility;}

    bool isFacingRight() const { return mFacingRight; }

    bool isDashing() const {return mIsDashing; }

    bool isSlashing() const {return mIsSlashing; }

    bool isSmashing() const {return mIsSmashing; }

    bool isTakingOff() const {return mIsTakingOff;}
    bool isTouchingGround() const {return mTouchingGround;}

    bool isCrawling() const {return mIsCrawling;}

    bool isInvincible() const {return mIsInvincible; }


    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    EntityType  getEntityType()            const { return mEntityType;            }
    AIType      getAIType()                const { return mAIType;                }
    AIState     getAIState()               const { return mAIState;               }

    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }
    // added
    bool isCollidingRight() const {return mIsCollidingRight;}
    bool isCollidingLeft() const {return mIsCollidingLeft;}

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }
    std::map<Abilities, std::vector<int>> getAbilityAtlas() const { return mAbilityAtlas; } // added for abilities 

    void setAbilityAtlas(std::map<Abilities, std::vector<int>> newAbilityAtlas) {
        mAbilityAtlas = newAbilityAtlas;
        mAbility = HOVERING; 
        mAbilityIndices = mAbilityAtlas.at(HOVERING);
    }

    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { mAcceleration = newAcceleration;         }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions)
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions)
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(float newSpeed)
        { mSpeed = newSpeed;                       }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setJumpingPower(float newJumpingPower)
        { mJumpingPower = newJumpingPower;         }

    void setDashingPower( float newDashingPower)
    {
        mDashingPower = newDashingPower;
    }

    void setTouchingGround( bool bit ) {  mTouchingGround = bit ;}

    void setAngle(float newAngle)
        { mAngle = newAngle;                       }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                }
    void setDirection(Direction newDirection)
    {
        mDirection = newDirection;

        if (mTextureType == ATLAS) mAnimationIndices = mAnimationAtlas.at(mDirection);
    }
    void setAIState(AIState newState)
        { mAIState = newState;                     }
    void setAIType(AIType newType)
        { mAIType = newType; 
      
        
        if (newType == Flyer) {
            flyerOrigin = mPosition;
            flyerAngle = 0.0f;
        }
                              }

    // ––– TODO: Add a setLerpFactor setter and a getLerpFactor getter here –––

    void setLerpFactor(float newFactor) { mLerpFactor = newFactor; }
    float getLerpFactor()           { return mLerpFactor; }

    //Copied from Project 2 Pong, now used to win/lose condition 
    bool        checkCollision(Entity *other) const {return isColliding(other);}

    //added

    void idle() {

        if (mAbility == DASHING || mAbility == SLASHING || mAbility == SMASHING || mAbility == TAKEOFF || mAbility == INVINCIBLE) {return;}

        if (mDirection == DOWN) {
            if (mFacingRight) setDirection(RIGHT); 
            else setDirection(LEFT);

        
        }
        
        // deciding which idling to use
        if (mTouchingGround) {
            if (mAbility != STANDING) {
                mAbility = STANDING;
                mAbilityIndices = mAbilityAtlas.at(STANDING); 
                mAbilityFrameIndex = 0; 
                mAbilityTime = 0.0f;
            }
        }
        else 
        {   
            if (mAbility != HOVERING) {
                mAbility = HOVERING;
                mAbilityIndices = mAbilityAtlas.at(HOVERING); 
                mAbilityFrameIndex = 0; 
                mAbilityTime = 0.0f;
            }
        }
    }
    
    


};

#endif // ENTITY_H
