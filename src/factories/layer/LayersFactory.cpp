#include "LayersFactory.h"

#include <filesystem>

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
    std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(scriptPath);
    std::string normalizedPath = canonicalPath.string();

    std::unique_ptr<Layer> layer = loadLayerTemplateFromLua(normalizedPath);
    if (!layer) {
      return nullptr;
    }

    std::string name = layer->getName();
    if (logsManager.checkAndLogError(name.empty(), "Layer name is empty after loading from Lua: " + scriptPath)) {
      return nullptr;
    }

    layerTemplates[name] = std::move(layer);
    layerScriptPaths[name] = normalizedPath;
    return cloneLayer(name);
  }

  bool LayersFactory::hasLayerTemplate(const std::string& layerName) const {
    return layerTemplates.find(layerName) != layerTemplates.end();
  }

  std::unique_ptr<Layer> LayersFactory::cloneLayer(const std::string& layerName) {
    auto it = layerTemplates.find(layerName);
    if (logsManager.checkAndLogError(it == layerTemplates.end(), "Layer template not found: " + layerName)) {
      return nullptr;
    }

    const Layer& tmpl = *it->second;
    auto clone = std::make_unique<Layer>(tmpl.getName(), tmpl.getCategory());
    clone->setVisible(tmpl.isVisible());
    clone->setActive(tmpl.isActive());
    clone->setInteractable(tmpl.isInteractable());
    clone->setFollowCamera(tmpl.followsCamera());
    return clone;
  }

  std::unique_ptr<Layer> LayersFactory::cloneLayerFromPath(const std::string& scriptPath) {
    std::filesystem::path canonicalRequested = std::filesystem::weakly_canonical(scriptPath);
    std::string normalizedRequested = canonicalRequested.string();

    for (const auto& [name, path] : layerScriptPaths) {
      std::filesystem::path storedCanonical = std::filesystem::weakly_canonical(path);
      if (storedCanonical == canonicalRequested || path == normalizedRequested) {
        return cloneLayer(name);
      }
    }
    logsManager.logError("Layer template for path '" + scriptPath + "' not found.");
    return nullptr;
  }

  std::unique_ptr<Layer> LayersFactory::loadLayerTemplateFromLua(const std::string& scriptPath) {
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