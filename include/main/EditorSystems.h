#pragma once
#include "ECS/ECS.h"
class SelectableSystem : public System
{
public:
	SelectableSystem();
	~SelectableSystem();
	void findEntities(std::vector<Entity>& intersections, float2 point);

private:
	void dac_bp_Test(std::vector<std::pair<Selectable, Entity>>& selectables,
					 const std::vector<std::pair<Selectable, Entity>>::iterator start,
					 const std::vector<std::pair<Selectable, Entity>>::iterator end,
					 std::vector<Entity>& intersections,
					 float2 point);
	void narrowphase_Test(std::vector<Entity>& intersections, float2 point);
	//void dac_bp_Test(Coordinator& coordinator, const std::set<Entity>::iterator start, const std::set<Entity>::iterator end, std::vector<Entity>& intersections, float2 point);
};
