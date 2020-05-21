 #pragma once
#include <array>
#include <assert.h>
#include <bitset>
#include <queue>
#include <set>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include"../../extern/refl-cpp/refl.hpp"
class IComponentArray;
template <typename T>
class ComponentArray;
class EntityManager;
class ComponentManager;
class SystemManager;
class Coordinator;

extern Coordinator gCoordinator;
// A simple type alias
using Entity = std::uint32_t;
// Used to define the size of arrays later on
static const Entity MAX_ENTITIES = 1000;

// A simple type alias
using ComponentType = std::uint8_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager (seen later)
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.

class System
{
public:
	std::set<Entity> entities_;
};

