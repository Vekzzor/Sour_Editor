#include "EditorSystems.h"
#include "pch.h"

SelectableSystem::SelectableSystem() {}

SelectableSystem::~SelectableSystem() {}

void SelectableSystem::findEntities(std::vector<Entity>& _intersections, float2 _point)
{
	std::vector<Selectable> bounds;
	Coordinator& coordinator = gCoordinator.GetInstance();
	std::vector<std::pair<Selectable, Entity>> selectables;
	for(auto const& entity : entities_)
	{
		selectables.push_back(std::make_pair(coordinator.GetComponent<Selectable>(entity), entity));
		sf::Transform translation =
			gCoordinator.GetInstance().GetComponent<sf::Transformable>(entity).getTransform();
		sf::Vector2f pos(selectables.back().first.aabb_lower_bounds[0],
						 selectables.back().first.aabb_lower_bounds[1]);
		pos = translation * pos;
		pos											 = pos - sf::Vector2f(_point[0], _point[1]);
		pos.x /= kPixelToMeterRatio, pos.y /= kPixelToMeterRatio;
		selectables.back().first.aabb_lower_bounds[0] = pos.x;
		selectables.back().first.aabb_lower_bounds[1] = pos.y;

		pos = translation * sf::Vector2f(selectables.back().first.aabb_upper_bounds[0],
							   selectables.back().first.aabb_upper_bounds[1]);
		pos = pos - sf::Vector2f(_point[0], _point[1]);
		pos.x /= kPixelToMeterRatio, pos.y /= kPixelToMeterRatio;
		selectables.back().first.aabb_upper_bounds[0] = pos.x;
		selectables.back().first.aabb_upper_bounds[1] = pos.y;
	}

	//selectables.push_back(std::make_pair(Selectable{{-100000, -100000}, {100000, 100000}}, 99));

	std::vector<int> indices;
	for(size_t i = 0; i < selectables.size(); i++)
		indices.push_back(i);

	dac_bp_Test(selectables, indices.begin(), indices.end(), _intersections, _point);
	narrowphase_Test(_intersections, _point);
}

static int MidLeftCount = 0;
static int MidRightCount = 0;
void SelectableSystem::dac_bp_Test(const std::vector<std::pair<Selectable, Entity>>& _selectables,
								   const std::vector<int>::iterator _start,
								   const std::vector<int>::iterator _end,
								   std::vector<Entity>& intersections, const float2& _point)
{
	const int BRUTE_FORCE_THRESH = 32;
	int partitionSize			 = _end - _start;
	if((_end - _start) < BRUTE_FORCE_THRESH)
	{
		// Brute force check the remaining pairs in this interval
		for(auto it = _start; it != _end;
			it++) //Update To do a more fine grained intersection (Convex Hull)
		{
			if((_selectables[*it].first.aabb_lower_bounds[0] < 0 &&
				0 < _selectables[*it].first.aabb_upper_bounds[0]) &&
			   (_selectables[*it].first.aabb_lower_bounds[1] < 0 &&
				0 < _selectables[*it].first.aabb_upper_bounds[1]))
			{
				intersections.push_back(_selectables[*it].second);
			}
		}

		int i = 0;
	}
	else
	{
		// Compute bounds of all boxes in this interval
		float2 pmin = {FLT_MAX, FLT_MAX}, pmax = {-FLT_MAX, -FLT_MAX};
		for (auto it = _start; it != _end; it++)
		{
			pmin = std::min(pmin, _selectables[*it].first.aabb_lower_bounds);
			pmax = std::max(pmax, _selectables[*it].first.aabb_upper_bounds);
		}
		// Choose the partition axis and partition location
		float2 size = {pmax[0] - pmin[0], pmax[1] - pmin[1]};
		bool axis	= (size[1] > size[0]);
		float split = (pmin[axis] + pmax[axis]) * 0.5f;

		// Partition boxes left and recurse
		auto mid_left =
			std::partition(_start, _end, [split, axis, _selectables](int i) {
			return _selectables[i].first.aabb_lower_bounds[axis] <= split;
			});

		MidLeftCount++;
		dac_bp_Test(_selectables, _start, mid_left, intersections, _point);

		// Partition boxes right and recurse
		auto mid_right = 
			std::partition(_start, _end, [split, axis, _selectables](int i) {
			return _selectables[i].first.aabb_upper_bounds[axis] >= split;
			});

		MidRightCount++;
		dac_bp_Test(_selectables, _start, mid_right, intersections,_point);
	}
}

bool onSegment(float2 p, float2 q, float2 r)
{
	if(q[0] <= std::max(p[0], r[0]) && q[0] >= std::min(p[0], r[0]) &&
	   q[1] <= std::max(p[1], r[1]) && q[1] >= std::min(p[1], r[1]))
		return true;
	return false;
}

int orientation(float2 p, float2 q, float2 r)
{
	int val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1]);

	if(val == 0)
		return 0; // colinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

bool doIntersect(float2 p1, float2 q1, float2 p2, float2 q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if(o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if(o1 == 0 && onSegment(p1, p2, q1))
		return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if(o2 == 0 && onSegment(p1, q2, q1))
		return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if(o3 == 0 && onSegment(p2, p1, q2))
		return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if(o4 == 0 && onSegment(p2, q1, q2))
		return true;

	return false; // Doesn't fall in any of the above cases
}

void SelectableSystem::narrowphase_Test(std::vector<Entity>& _intersections, float2 _point)
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	//float2 endPoint			 = {point[0] + 1000000.0f, point[1]};

	auto false_positive = [&coordinator, _point](Entity entity) {
		
		if(coordinator.HasComponent<PhysicsObject>(entity))
		{
			auto& physObj = coordinator.GetComponent<PhysicsObject>(entity);
			for(b2Fixture* f = physObj.body->GetFixtureList(); f; f = f->GetNext())
			{
				//do something with the fixture 'f'
				if(f->TestPoint({_point[0] / kPixelToMeterRatio, _point[1] / kPixelToMeterRatio}))
					return false;
			}

		}
		else if(coordinator.HasComponent<Mesh2D>(entity))
		{
			auto& vertices = coordinator.GetComponent<Mesh2D>(entity).vertices;
			sf::Vector2f localPoint(_point[0], _point[1]);
			localPoint = coordinator.GetComponent<sf::Transformable>(entity).getInverseTransform()*localPoint;
			for(int i = 0; i < vertices.size(); i+=2)
			{
				if(pointTriangleIntersection(localPoint,
											 vertices[i % vertices.size()].position,
											 vertices[(i + 1) % vertices.size()].position,
											 vertices[(i + 2) % vertices.size()].position))
					return false;
			}
		}
		return true;
	};
	

	erase_where(_intersections, false_positive);
	//for(auto it = intersections.begin(); it != intersections.end(); it++)
	//{
	//	int count	 = 0;
	//	auto& polygon = coordinator.GetComponent<Polygon>(*it);

	//	for(int i = 0; i < polygon.m_vertices.size(); i = i + 2)
	//	{
	//		auto p = polygon.getTransform() * polygon.m_vertices[i].position;
	//		auto q = polygon.getTransform() *
	//				 polygon.m_vertices[(i + 1) % polygon.m_vertices.size()].position;

	//		if(doIntersect({p.x, p.y}, {q.x, q.y}, point, endPoint))
	//		{
	//			count++;
	//		}
	//	}

	//	if(count% 2 == 0 && count != 0)
	//	{
	//		intersections.erase(it);
	//		if (it != intersections.begin())
	//			it--;
	//		/*if(intersections.size() == 1)
	//			break;*/
	//	}
	//}
}

//void SelectableSystem::dac_bp_Test(Coordinator& coordinator, const std::set<Entity>::iterator start, const std::set<Entity>::iterator end, std::vector<Entity>& intersections, float2 point)
//{
//	const int BRUTE_FORCE_THRESH = 32;
//	if ((end - start) < BRUTE_FORCE_THRESH)
//	{
//		// Brute force check the remaining pairs in this interval
//		for (auto i = start; i != end; i++) //Update To do a more fine grained intersection (Convex Hull)
//		{
//			auto& selectable = coordinator.GetComponent<Selectable>(*i);
//			if ((selectable.AABB_lowerBounds[0] < point[0] && point[0] < selectable.AABB_upperBounds[0]) && (selectable.AABB_lowerBounds[1] < point[1] && point[1] < selectable.AABB_upperBounds[1]))
//			{
//				intersections.push_back(*i);
//			}
//		}
//	}
//	else
//	{
//		// Compute bounds of all boxes in this interval
//		float2 pmin = { FLT_MAX, FLT_MAX }, pmax = { -FLT_MAX, -FLT_MAX };
//		for (auto i = start; i != end; i++)
//		{
//			auto& selectable = coordinator.GetComponent<Selectable>(*i);
//			pmin = std::min(pmin, selectable.AABB_lowerBounds);
//			pmax = std::max(pmax, selectable.AABB_upperBounds);
//		}
//
//		// Choose the partition axis and partition location
//		float2 size = { pmax[0] - pmin[0], pmax[1] - pmin[1] };
//		int axis = (size[1] > size[0]);
//		float split = (pmin[axis] + pmax[axis]) * 0.5f;
//
//		// Partition boxes left and recurse
//		auto mid_left = partition(start, end, [split, axis, &coordinator](Entity i) { return coordinator.GetComponent<Selectable>(i).AABB_lowerBounds[axis] <= split; });
//
//		dac_bp_Test(coordinator, start, mid_left, intersections, point);
//
//		// Partition boxes right and recurse
//		auto mid_right = partition(start, end, [split, axis, &coordinator](Entity i) { return coordinator.GetComponent<Selectable>(i).AABB_upperBounds[axis] >= split; });
//
//		dac_bp_Test(coordinator, start, mid_right, intersections, point);
//	}
//}

VoronoiGenerationTool::VoronoiGenerationTool() {}

VoronoiGenerationTool::~VoronoiGenerationTool() {}

void VoronoiGenerationTool::addSeed(sf::Vector2f _point) 
{
	seeds_.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_point, seeds_.size()));
}

void VoronoiGenerationTool::clearSeeds()
{
	for(auto& point : seeds_)
		delete point;
	seeds_.clear();
}

void VoronoiGenerationTool::GenerateVoronoiCells(const std::vector<sf::Vector2f>& _clip_polygon,
												 PhysicsSystem* _system)
{
	FinalizeSeeds(_clip_polygon);
	std::vector<HALF_EDGE::HE_Face*> faceList;
	Delunay::Triangulate(seeds_, faceList);
	Voronoi_Tessellation::CreateVoronoiPolyBodies(
		_clip_polygon, seeds_, faceList, _system);
	clearSeeds();
}

void VoronoiGenerationTool::FinalizeSeeds(const std::vector<sf::Vector2f>& _clip_polygon)
{
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	for(auto vertex : seeds_) //Change to _ClipPolygon if not working
	{
		minX = std::min(minX, vertex->point->x);
		maxX = std::max(maxX, vertex->point->x);
		minY = std::min(minY, vertex->point->y);
		maxY = std::max(maxY, vertex->point->y);
	}

	sf::Vector2f min(minX, minY);
	sf::Vector2f max(maxX, maxY);
	sf::Vector2f Center		= (min + max) * 0.5f;
	sf::Vector2f minMax		= (max - min) * 2.0f;
	sf::Vector2f starTop	= {Center.x, Center.y - minMax.y};
	sf::Vector2f starLeft   = {Center.x - minMax.x, Center.y};
	sf::Vector2f starRight  = {Center.x + minMax.x, Center.y};
	sf::Vector2f starBottom = {Center.x, Center.y + minMax.y};

	seeds_.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, seeds_.size()));
	seeds_.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, seeds_.size()));
	seeds_.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, seeds_.size()));
	seeds_.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, seeds_.size()));
}
