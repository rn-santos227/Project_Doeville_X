#include "LuaGameStateBindings.h"

#include <cmath>
#include <string>
#include <unordered_set>

#include <SDL.h>

#include "assets/AssetsManager.h"
#include "assets/map_asset/MapAsset.h"
#include "assets/tile_asset/TileAsset.h"
#include "components/ComponentType.h"
#include "components/camera_component/CameraComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "entities/ChunkSize.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/asset/AssetsFactory.h"
#include "handlers/tile/TileHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"
#include "platform/Platform.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"

namespace Project::Bindings::LuaBindings {
  using Project::States::GameState;
  using Project::States::GameStateManager;
  using Project::Entities::EntitiesManager;
  using Project::Entities::Entity;
  using Project::Assets::AssetsManager;
  using Project::Assets::MapAsset;
  using Project::Assets::TileAsset;
  using Project::Factories::AssetsFactory;
  using Project::Factories::EntitiesFactory;
  using Project::Handlers::TileHandler;
  using Project::Platform::Platform;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  static AssetsManager* assetsManagerPtr = nullptr;
  static AssetsFactory* assetsFactoryPtr = nullptr;
}
