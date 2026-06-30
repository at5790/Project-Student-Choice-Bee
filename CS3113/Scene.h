#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState

{ // had to set to nullptr because segfault 
    Entity *xochitl = nullptr;
    Entity *sun = nullptr; //proj 3
    Entity *fakeBee = nullptr; // proj 2 
    Entity *slime = nullptr; 

    Entity *spider = nullptr; 
    Entity *spiderWeb = nullptr;

    Entity *queenBee = nullptr;

    Entity *jar = nullptr;

    
    Map    *map = nullptr;

    Music bgm = {} ;
    Sound jumpSound = {};

    Sound dashSound = {};

    Sound death = {};

    //added for buzzing

    Sound buzzing = {}; 

    int nextSceneID = 5;
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState   getState()           const { return mGameState;       }
    Vector2     getOrigin()          const { return mOrigin;          }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif // SCENE_H
