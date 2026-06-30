#include "Entity.h"

Entity::Entity()
    : mPosition{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f },
      mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
      mScale{ DEFAULT_SIZE, DEFAULT_SIZE },
      mColliderDimensions{ DEFAULT_SIZE, DEFAULT_SIZE },
      mTexture{}, mTextureType{ SINGLE }, mAngle{ 0.0f },
      mSpriteSheetDimensions{}, mDirection{ RIGHT },
      mAnimationAtlas{ {} }, mAnimationIndices{}, mFrameSpeed{ 0 },
      mEntityType{ EMPTY }
{}

Entity::Entity(
    Vector2     position,
    Vector2     scale,
    const char *textureFilepath,
    EntityType  entityType
)
    : mPosition{ position }, mVelocity{ 0.0f, 0.0f },
      mAcceleration{ 0.0f, 0.0f }, mScale{ scale }, mMovement{ 0.0f, 0.0f },
      mColliderDimensions{ scale }, mTexture{ LoadTexture(textureFilepath) },
      mTextureType{ SINGLE }, mDirection{ RIGHT }, mAnimationAtlas{ {} },
      mAnimationIndices{}, mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED },
      mAngle{ 0.0f }, mEntityType{ entityType }
{}

Entity::Entity(
    Vector2                               position,
    Vector2                               scale,
    const char                           *textureFilepath,
    TextureType                           textureType,
    Vector2                               spriteSheetDimensions,
    std::map<Direction, std::vector<int>> animationAtlas,
    EntityType                            entityType
)
    : mPosition{ position }, mVelocity{ 0.0f, 0.0f },
      mAcceleration{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f }, mScale{ scale },
      mColliderDimensions{ scale }, mTexture{ LoadTexture(textureFilepath) },
      mTextureType{ ATLAS }, mSpriteSheetDimensions{ spriteSheetDimensions },
      mAnimationAtlas{ animationAtlas }, mDirection{ RIGHT },
      mAnimationIndices{ animationAtlas.at(RIGHT) },
      mFrameSpeed{ DEFAULT_FRAME_SPEED }, mAngle{ 0.0f },
      mSpeed{ DEFAULT_SPEED }, mEntityType{ entityType }
{}

Entity::~Entity()
{
    UnloadTexture(mTexture);
}

void Entity::checkCollisionY(
    Entity *collidableEntities, int collisionCheckCount
)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            // STEP 3: "Unclip" ourselves from the other entity, and zero our
            //         vertical velocity.
            if (mVelocity.y > 0)
            {
                mPosition.y        -= yOverlap;
                mVelocity.y         = 0;
                mIsCollidingBottom  = true;
            }
            else if (mVelocity.y < 0)
            {
                mPosition.y     += yOverlap;
                mVelocity.y      = 0;
                mIsCollidingTop  = true;

                if (collidableEntity->mEntityType == BLOCK)
                    collidableEntity->deactivate();
            }
        }
    }
}

void Entity::checkCollisionX(
    Entity *collidableEntities, int collisionCheckCount
)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            // When standing on a platform, we're always slightly overlapping
            // it vertically due to gravity, which causes false horizontal
            // collision detections. So the solution I found is only resolve X
            // collisions if there's significant Y overlap, preventing the
            // platform we're standing on from acting like a wall.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(
                xDistance - (mColliderDimensions.x / 2.0f) -
                (collidableEntity->mColliderDimensions.x / 2.0f)
            );

            if (mVelocity.x > 0)
            {
                mPosition.x       -= xOverlap;
                mVelocity.x        = 0;
                mIsCollidingRight  = true;

                if (mAIType == Patroller) {
                    moveLeft();
                }
            }
            else if (mVelocity.x < 0)
            {
                mPosition.x      += xOverlap;
                mVelocity.x       = 0;
                mIsCollidingLeft  = true;

                if (mAIType == Patroller) {
                    moveRight();
                }
            }
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe = { mPosition.x,
                               mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f),
                               mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f),
                               mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = {
        mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomLeftProbe  = { mPosition.x - (mColliderDimensions.x / 2.0f),
                                 mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe = { mPosition.x + (mColliderDimensions.x / 2.0f),
                                 mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) &&
        mVelocity.y < 0.0f)
    {
        mPosition.y     += yOverlap; // push down
        mVelocity.y      = 0.0f;
        mIsCollidingTop  = true;
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) &&
        mVelocity.y > 0.0f)
    {
        mPosition.y        -= yOverlap; // push up
        mVelocity.y         = 0.0f;
        mIsCollidingBottom  = true;
    }
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe  = { mPosition.x - (mColliderDimensions.x / 2.0f),
                                 mPosition.y };
    Vector2 rightCentreProbe = { mPosition.x + (mColliderDimensions.x / 2.0f),
                                 mPosition.y };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if (map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) &&
        mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x       -= xOverlap * 1.01f; // push left
        mVelocity.x        = 0.0f;
        mIsCollidingRight  = true;

        if (mAIType == Patroller) moveLeft(); 
    }

    // COLLISION ON LEFT (moving left)
    if (map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) &&
        mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x      += xOverlap * 1.01f; // push right
        mVelocity.x       = 0.0f;
        mIsCollidingLeft  = true;

        if (mAIType == Patroller) moveRight(); 
    }
}

bool Entity::isColliding(Entity *other) const
{
    if (!other->isActive() || other == this) return false;

    float xDistance =
        fabs(mPosition.x - other->getPosition().x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance =
        fabs(mPosition.y - other->getPosition().y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime        += deltaTime;
    float secondsPerFrame  = 1.0f / mFrameSpeed;

    if (mAnimationTime >= secondsPerFrame)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::animateAbility(float deltaTime) {

    if (mAbilityIndices.empty()) return; 

    mAbilityTime        += deltaTime;
    float secondsPerFrame  = 1.0f / mFrameSpeed;

    if (mAbilityTime >= secondsPerFrame)
    {
        mAbilityTime = 0.0f;

        mAbilityFrameIndex++;

        if (mAbilityFrameIndex >= mAbilityIndices.size()) {
            if (mAbility == HOVERING || mAbility == CRAWLING || mAbility == STANDING) {
                mAbilityFrameIndex = 0 ; 
            }
            
            else {

                mAbility = HOVERING; 
                mAbilityIndices = mAbilityAtlas.at(HOVERING); 
                mAbilityFrameIndex = 0 ; 
                mAbilityTime = 0.0f;

                mIsDashing = false;
                mIsSlashing = false;
                mIsSmashing = false; 
                mIsTakingOff = false;
                mIsInvincible = false;

            }
        }
    }

    
}

void Entity::AIWander() { moveLeft(); }

void Entity::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 250.0f)
            mAIState = WALKING;
        break;

    case WALKING:
        if (mPosition.x > target->getPosition().x)
            moveLeft();
        else
            moveRight();
    
    


    default:
        break;
    }
}

// now included lerper from proj 2  and Bouncer
void Entity::AIActivate(Entity *target, float deltaTime)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;

    case LERPER:
        AILerp(target, deltaTime);
        break;

    case Flyer:
        AIFly(deltaTime);
        break;

  
    default:
        break;
    }

}

//proj 3

void Entity::AILerp(Entity *target, float deltaTime)
{   // position' = position + (target position - position) * lerp factor * delta time
    
    switch (mAIState) {
        case IDLE:
            if (Vector2Distance(mPosition, target->getPosition()) < 350 ) {
                mAIState = FOLLOWING;
                
            }
            break;
            
        case WALKING:

        case FOLLOWING:
            mPosition.x = mPosition.x + (target->getPosition().x - mPosition.x) * mLerpFactor * deltaTime ;
            mPosition.y = mPosition.y + (target->getPosition().y - mPosition.y) * mLerpFactor * deltaTime ; 
    }
}
//

void Entity::AIFly(float deltaTime) {

    //from Project 1

    flyerAngle += (mSpeed/ 3.0f  * deltaTime);

    mPosition.x = flyerOrigin.x + 400  * sin(flyerAngle);
    mPosition.y = flyerOrigin.y + 100 * sin(2 * flyerAngle);


}

void Entity::update(
    float   deltaTime,
    Entity *player,
    Map    *map,
    Entity *collidableEntities,
    int     collisionCheckCount


)
{
    if (mEntityStatus == INACTIVE) return;

    if (mEntityType == NPC) AIActivate(player, deltaTime);

    resetColliderFlags();

    mVelocity.x = mMovement.x * mSpeed;
    
    if (mMovement.y != 0.0f) {mVelocity.y = mMovement.y * mSpeed;} else { mVelocity.y = 5.0f;}


    // ––––– JUMPING ––––– //
    if (mIsJumping)
    {
        mIsJumping   = false;
        mVelocity.y -= mJumpingPower;
    }

    if (mIsDashing) {
        mIsDashing = false;
        if (mDirection == LEFT) {
           mVelocity.x -= mDashingPower; 
        } 

        if (mDirection == RIGHT) {
            mVelocity.x += mDashingPower;
        }
        
    }

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(map);
    // for bool touching bottom
    mTouchingGround = mIsCollidingBottom; 

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(map);

    if (mTextureType == ATLAS) // changed so that keeps moving becuase bee && GetLength(mMovement) != 0 && mIsCollidingBottom)
        {animate(deltaTime); animateAbility(deltaTime);}

    


}

void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
    case SINGLE:
        textureArea = { // top-left corner
                        0.0f,
                        0.0f,

                        // bottom-right corner (of texture)
                        static_cast<float>(mTexture.width),
                        static_cast<float>(mTexture.height)
        };
        break;
    case ATLAS:
    {
        int frameIndex = 0;

        bool usingAbility = (mAbility == DASHING || mAbility == SLASHING || mAbility == SMASHING || mAbility == TAKEOFF || mAbility == CRAWLING || mAbility == STANDING || mAbility == INVINCIBLE);

        bool isIdle = mMovement.x == 0.0f && mMovement.y == 0.0f; 


        if ( usingAbility && !mAbilityIndices.empty()){ frameIndex = mAbilityIndices[mAbilityFrameIndex]; }
        else  if (isIdle && !mAbilityIndices.empty() ){
            frameIndex = mAbilityIndices[mAbilityFrameIndex];
        }
        else { frameIndex = mAnimationIndices[mCurrentFrameIndex];}

        textureArea = getUVRectangle(
            &mTexture,
            frameIndex,
            mSpriteSheetDimensions.x,
            mSpriteSheetDimensions.y
        );
        break;
    }

    default:
        break;
    }

    Rectangle destinationArea = { mPosition.x,
                                  mPosition.y,
                                  static_cast<float>(mScale.x),
                                  static_cast<float>(mScale.y) };

    Vector2 originOffset = { static_cast<float>(mScale.x) / 2.0f,
                             static_cast<float>(mScale.y) / 2.0f };


    // for going left since sprite sheet only had going right 

    if ( ((mEntityType == PLAYER) || (mEntityType == NPC) || (mEntityType == EMPTY) ) && mFacingRight == 0 ) {
        textureArea.width *= -1; 
    }

    DrawTexturePro(
        mTexture, textureArea, destinationArea, originOffset, mAngle, WHITE
    );

    // displayCollider();
}

void Entity::displayCollider()
{
    Rectangle colliderBox = { mPosition.x - mColliderDimensions.x / 2.0f,
                              mPosition.y - mColliderDimensions.y / 2.0f,
                              mColliderDimensions.x,
                              mColliderDimensions.y };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}
