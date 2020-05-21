#pragma once
#include <imgui-SFML.h>
#include "Data_Structures/DCELHE.h"
#include "Components.h"
#include "PhysicsSystem.h"

struct VoronoiEdge
{
	VoronoiEdge(sf::Vector2f _v1, sf::Vector2f _v2, sf::Vector2f* _site)
		: v1{ _v1 }
		, v2{ _v2 }
	, site{ _site } {};
	sf::Vector2f v1;
	sf::Vector2f v2;
	sf::Vector2f* site;
};

struct PEdge
{
	PEdge(sf::Vector2f _v1, sf::Vector2f _v2)
		: v1{ _v1 }
		, v2{ _v2 } {};
	sf::Vector2f v1;
	sf::Vector2f v2;
};


struct VoronoiCell
{
	sf::Vector2f* site;
	//std::unordered_set<sf::Vector2f> vertices;
	std::vector<VoronoiEdge*> edges;

	VoronoiCell(sf::Vector2f* _sitePos)
		: site{ _sitePos } {};
};

static const int SEEDS = 10;

class Voronoi_Tessellation
{
private:
	static void addVoronoiEdge(HALF_EDGE::HE_Edge* _e, sf::Vector2f _center, std::vector<VoronoiEdge*>& _voronoiEdges);

	static bool ClipLines_LiangBarsky(
		double _edgeLeft,
		double _edgeRight,
		double _edgeBottom,
		double _edgeTop, // Define the x/y clipping values for the border.
		sf::Vector2f _src0,
		sf::Vector2f _src1, // Define the start and end points of the line.
		sf::Vector2f& _clip0,
		sf::Vector2f& _clip1);
	static bool Voronoi_Tessellation::ClipLines_CyrusBeck(
		std::pair<int, int> _vertices[],
		int _n,
		sf::Vector2f _src0,
		sf::Vector2f _src1,
		sf::Vector2f& _clip0,
		sf::Vector2f& _clip1); //arg 1: Bounds, aka the original polygon, line to clip. Returns the clipped line

	static void SuthHodgClip(std::vector<sf::Vector2f>& _poly, const std::vector<sf::Vector2f>& _clipper_vertices);
	static void ClipPoly(std::vector<sf::Vector2f>& _poly, sf::Vector2f _clip0, sf::Vector2f _clip1);

	static void SuthHodgClip_VE(std::vector<PEdge>& _poly, const std::vector<sf::Vector2f>& _clipper_ertices);
	static void ClipPoly_VE(std::vector<PEdge>& _poly, sf::Vector2f _clip0, sf::Vector2f _clip1);
public:
	static void Voronoi_Tessellation::CreateSeedPoint(std::vector<sf::Vector2f>& _polygon,
		std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
		sf::Vector2f _point);
	static void CreateSeedPoints(std::vector<sf::Vector2f>& _polygon, std::vector<HALF_EDGE::HE_Vertex*>& _seed_points);
	static void CreateSeedPoints(std::vector<HALF_EDGE::HE_Vertex*>& _seed_points, sf::Vector2f _corner_max, sf::Vector2f _corner_min);
	static void CreateVoronoiPolyBodies(
		const std::vector<sf::Vector2f>& _polygon_hull,
		std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
		std::vector<HALF_EDGE::HE_Face*>& _face_list,
		std::vector<Mesh2D>& _polygons,
		std::vector<Entity>& _entities,
		PhysicsSystem *_system);

	static void Voronoi_Tessellation::CreateVoronoiPolyBodies(const std::vector<sf::Vector2f>& _polygon_hull,
		std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
		std::vector<HALF_EDGE::HE_Face*>& _face_list,
		PhysicsSystem* _system);
};

