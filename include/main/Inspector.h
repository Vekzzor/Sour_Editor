#pragma once
#include "ECS/ECS.h"
class Inspector : public System
{
public:
	Inspector();
	~Inspector();
	void ListLiveEntites(EditorTools& _tools);
	void InspectEntity(Entity _entity);
	Entity selectedEntity;
private:
	
};

// Idea to add component configurations:
// NEW: check refl-cpp for reflection
// Find components
// look what primitive data types it contains
// * for floats/ints/doubles, etc: create slider combined with text input
// * bool crossmarks
// * chars/strings : text input 
// * pointers/references : further research required