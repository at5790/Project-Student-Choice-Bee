#ifndef SCENEGAMEOVER_H
#define SCENEGAMEOVER_H

#include "Scene.h"

class SceneGameOver : public Scene
{ 
public:
    SceneGameOver();
    SceneGameOver(Vector2 origin, const char *bgHexCode);

    virtual void initialise() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;

};

#endif // SCENEGAMEOVER_H