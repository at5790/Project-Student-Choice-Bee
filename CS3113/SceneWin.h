#ifndef SCENEWIN_H
#define SCENEWIN_H

#include "Scene.h"

class SceneWin : public Scene
{ 
public:
    SceneWin();
    SceneWin(Vector2 origin, const char *bgHexCode);

    virtual void initialise() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;

};

#endif // SCENEWIN_H