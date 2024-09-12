#include "SceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "DebugTestScene.h"
#include"ResultScene.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName){

    std::unique_ptr<IScene> newScene = nullptr;

    if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }
    if (sceneName == "Title") {
        newScene = std::make_unique<TitleScene>();
    }
    if (sceneName == "Debug") {
        newScene = std::make_unique<DebugTestScene>();
    }
    if (sceneName == "Result") {
        newScene = std::make_unique<ResultScene>();
    }

    return newScene;
}
