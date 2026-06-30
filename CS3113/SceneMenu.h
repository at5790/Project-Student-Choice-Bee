#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "Scene.h"

class SceneMenu : public Scene
{ 
public:
    SceneMenu();
    SceneMenu(Vector2 origin, const char *bgHexCode);

    virtual void initialise() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;

};

#endif // SCENEMENU_H