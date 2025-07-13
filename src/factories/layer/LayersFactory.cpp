#include "LayersFactory.h"

#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"

namespace Project::Factories {
  using Project::Layers::Layer;
  using Project::Layers::LayerCategory;
  using Project::Layers::LayerCategoryResolver;

  namespace Keys = Project::Libraries::Keys;

  LayersFactory::LayersFactory(Project::Utilities::LogsManager& logsManager)
    : logsManager(logsManager) {}

  std::unique_ptr<Layer> LayersFactory::createLayerFromLua(const std::string& scriptPath) {
    Project::Utilities::LuaStateWrapper lua(logsManager);
    if (!lua.loadScript(scriptPath)) {
      logsManager.logError("Failed to load layer script: " + scriptPath);
      return nullptr;
    }

    std::string name = lua.getGlobalString(Keys::LAYER_NAME);
    std::string categoryStr = lua.getGlobalString(Keys::LAYER_CATEGORY, Project::Libraries::Categories::Layers::CUSTOM);
    LayerCategory category = LayerCategoryResolver::resolve(categoryStr);

    auto layer = std::make_unique<Layer>(name, category);
    
    bool active = lua.getGlobalBoolean(Keys::ACTIVE, true);
    bool follow = lua.getGlobalBoolean(Keys::FOLLOW_CAMERA, layer->followsCamera());
    bool interact = lua.getGlobalBoolean(Keys::INTERACTABLE, layer->isInteractable());
    bool visible = lua.getGlobalBoolean(Keys::VISIBLE, true);

    layer->setVisible(visible);
    layer->setActive(active);
    layer->setFollowCamera(follow);
    layer->setInteractable(interact);

    return layer;
  }
}