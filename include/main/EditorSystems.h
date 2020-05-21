#pragma once
#include "ECS/ECS.h"
#include "Components.h"
#include "PhysicsSystem.h"
class SelectableSystem : public System
{
public:
	SelectableSystem();
	~SelectableSystem();
	void findEntities(std::vector<Entity>& _intersections, float2 _point);

private:
	void dac_bp_Test(const std::vector<std::pair<Selectable, Entity>>& _selectables,
					 const std::vector<int>::iterator _start,
					 const std::vector<int>::iterator _end,
					 std::vector<Entity>& _intersections,
					 const float2& _point);
	void narrowphase_Test(std::vector<Entity>& _intersections, float2 _point);
	//void dac_bp_Test(Coordinator& coordinator, const std::set<Entity>::iterator start, const std::set<Entity>::iterator end, std::vector<Entity>& intersections, float2 point);

	//WARNING: NOT TESTED ON DIFFERENT PRIMITIVE TYPES
	static bool pointTriangleIntersection(sf::Vector2f _p, sf::Vector2f _q0, sf::Vector2f _q1, sf::Vector2f _q2)
	{
		auto dot = [](sf::Vector2f p, sf::Vector2f q) {return p.x*q.x + p.y*q.y; };
		auto v0 = _q2 - _q0;
		auto v1 = _q1 - _q0;
		auto v2 = _p - _q0;

		auto dot00 = dot(v0, v0);
		auto dot01 = dot(v0, v1);
		auto dot02 = dot(v0, v2);
		auto dot11 = dot(v1, v1);
		auto dot12 = dot(v1, v2);

		auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return (u >= 0) && (v >= 0) && (u + v < 1);
	}
};

class VoronoiGenerationTool
{
public:
	VoronoiGenerationTool();
	~VoronoiGenerationTool();
	void addSeed(sf::Vector2f _point);
	void clearSeeds();
	void GenerateVoronoiCells(const std::vector<sf::Vector2f>& _clip_polygon, PhysicsSystem* _system);
private:
	void FinalizeSeeds(const std::vector<sf::Vector2f>& _clip_polygon);
	std::vector<HALF_EDGE::HE_Vertex*> seeds_;
};