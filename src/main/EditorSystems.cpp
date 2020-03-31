#include "EditorSystems.h"
#include "pch.h"

SelectableSystem::SelectableSystem() {}

SelectableSystem::~SelectableSystem() {}

void SelectableSystem::findEntities(std::vector<Entity>& intersections, float2 point)
{
	std::vector<Selectable> bounds;
	Coordinator& coordinator = gCoordinator.GetInstance();
	std::vector<std::pair<Selectable, Entity>> selectables;
	for(auto const& entity : mEntities)
	{
		selectables.push_back(std::make_pair(coordinator.GetComponent<Selectable>(entity), entity));
	}

	dac_bp_Test(selectables, selectables.begin(), selectables.end(), intersections, point);
	narrowphase_Test(intersections, point);
}

void SelectableSystem::dac_bp_Test(std::vector<std::pair<Selectable, Entity>>& selectables,
								   const std::vector<std::pair<Selectable, Entity>>::iterator start,
								   const std::vector<std::pair<Selectable, Entity>>::iterator end,
								   std::vector<Entity>& intersections,
								   float2 point)
{
	const int BRUTE_FORCE_THRESH = 12;
	if((end - start) < BRUTE_FORCE_THRESH)
	{
		// Brute force check the remaining pairs in this interval
		for(auto i = start; i != end;
			i++) //Update To do a more fine grained intersection (Convex Hull)
		{
			if((i->first.AABB_lowerBounds[0] < point[0] &&
				point[0] < i->first.AABB_upperBounds[0]) &&
			   (i->first.AABB_lowerBounds[1] < point[1] && point[1] < i->first.AABB_upperBounds[1]))
			{
				intersections.push_back(i->second);
			}
		}
	}
	else
	{
		// Compute bounds of all boxes in this interval
		float2 pmin = {FLT_MAX, FLT_MAX}, pmax = {-FLT_MAX, -FLT_MAX};
		for(auto i = start; i != end; i++)
		{
			pmin = std::min(pmin, i->first.AABB_lowerBounds);
			pmax = std::max(pmax, i->first.AABB_upperBounds);
		}

		// Choose the partition axis and partition location
		float2 size = {pmax[0] - pmin[0], pmax[1] - pmin[1]};
		int axis	= (size[1] > size[0]);
		float split = (pmin[axis] + pmax[axis]) * 0.5f;

		// Partition boxes left and recurse
		auto mid_left =
			partition(start, end, [split, axis, selectables](std::pair<Selectable, Entity> i) {
				return i.first.AABB_lowerBounds[axis] <= split;
			});

		dac_bp_Test(selectables, start, mid_left, intersections, point);

		// Partition boxes right and recurse
		auto mid_right =
			partition(start, end, [split, axis, selectables](std::pair<Selectable, Entity> i) {
				return i.first.AABB_upperBounds[axis] >= split;
			});

		dac_bp_Test(selectables, start, mid_right, intersections, point);
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

void SelectableSystem::narrowphase_Test(std::vector<Entity>& intersections, float2 point)
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	//float2 endPoint			 = {point[0] + 1000000.0f, point[1]};

	auto false_positive = [&coordinator, point](Entity entity) {
		auto& polygon = coordinator.GetComponent<Polygon>(entity);
		for(b2Fixture* f = polygon.m_body->GetFixtureList(); f; f = f->GetNext())
		{
			//do something with the fixture 'f'
			if(f->TestPoint({point[0] / pixelToMeterRatio, point[1] / pixelToMeterRatio}))
				return false;
		}

		return true;
	};
	erase_where(intersections, false_positive);
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
