#include "Voronoi_Tessellation.h"
#include "pch.h"
#include <set>
void Voronoi_Tessellation::CreateSeedPoints(std::vector<sf::Vector2f>& _polygon,
											std::vector<HALF_EDGE::HE_Vertex*>& _seed_points)
{
	if(!_polygon.empty())
	{
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;


		_seed_points.reserve(_polygon.size() + SEEDS + 4);
		for (auto& vertex : _polygon)
		{
			minX = std::min(minX, vertex.x);
			maxX = std::max(maxX, vertex.x);
			minY = std::min(minY, vertex.y);
			maxY = std::max(maxY, vertex.y);
			//BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(vertex, BWpoints.size()));
		}
		for(size_t i = 0; i < SEEDS; i++)
		{
			//float x = CornerMin.x + float(rand() % (int)(CornerMax.x - CornerMin.x));
			//float y = CornerMin.y + float(rand() % (int)(CornerMax.y - CornerMin.y));

			float x = minX + float(rand() % (int)(maxX - minX));
			float y = minY + float(rand() % (int)(maxY - minY));
			//x		= CornerMax.x - CornerMin.x;
			//y		= CornerMax.y - CornerMin.y;
			_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(x, y, _seed_points.size()));
		}

		
		sf::Vector2f min(minX, minY);
		sf::Vector2f max(maxX, maxY);
		sf::Vector2f Center = (min + max) * 0.5f;
		sf::Vector2f minMax = (max - min) * 2.0f;
		//sf::Vector2f Center		= (CornerMin + CornerMax) * 0.5f;
		//sf::Vector2f minMax		= (CornerMax - CornerMin) * 2.0f;
		sf::Vector2f starTop	= {Center.x, Center.y - minMax.y};
		sf::Vector2f starLeft   = {Center.x - minMax.x, Center.y};
		sf::Vector2f starRight  = {Center.x + minMax.x, Center.y};
		sf::Vector2f starBottom = {Center.x, Center.y + minMax.y};

		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, _seed_points.size()));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, _seed_points.size()));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, _seed_points.size()));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, _seed_points.size()));
	}
}

void Voronoi_Tessellation::CreateSeedPoint(std::vector<sf::Vector2f>& _polygon,
											std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
											sf::Vector2f _point)
{
	if(!_polygon.empty())
	{
		_seed_points.reserve(_polygon.size() + 4);

		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;

		for (auto& vertex : _polygon)
		{
			minX = std::min(minX, vertex.x);
			maxX = std::max(maxX, vertex.x);
			minY = std::min(minY, vertex.y);
			maxY = std::max(maxY, vertex.y);
			_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(vertex, _seed_points.size()));
		}

		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_point, _seed_points.size()));

		/*BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(sf::Vector2f(0,0)-point, BWpoints.size()));*/

		sf::Vector2f min(minX, minY);
		sf::Vector2f max(maxX, maxY);
		sf::Vector2f Center		= (min + max) * 0.5f;
		sf::Vector2f minMax		= (max - min) * 2.0f;
		sf::Vector2f starTop	= {Center.x, Center.y - minMax.y};
		sf::Vector2f starLeft   = {Center.x - minMax.x, Center.y};
		sf::Vector2f starRight  = {Center.x + minMax.x, Center.y};
		sf::Vector2f starBottom = {Center.x, Center.y + minMax.y};

		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, _seed_points.size()));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, _seed_points.size()));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, _seed_points.size()));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, _seed_points.size()));
	}
}

void Voronoi_Tessellation::CreateSeedPoints(std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
											sf::Vector2f _corner_max,
											sf::Vector2f _corner_min)
{
	if(_seed_points.empty())
	{
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_corner_min, _seed_points.size())); //TopLeft
		_seed_points.push_back(
			DBG_NEW HALF_EDGE::HE_Vertex(_corner_max.x, _corner_min.y, _seed_points.size())); //TopRight
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_corner_max, _seed_points.size())); //BottomRight
		_seed_points.push_back(
			DBG_NEW HALF_EDGE::HE_Vertex(_corner_min.x, _corner_max.y, _seed_points.size())); //BottomLeft
	}

	for(size_t i = 0; i < SEEDS; i++)
	{
		float x = _corner_min.x + float(rand() % (int)(_corner_max.x - _corner_min.x));
		float y = _corner_min.y + float(rand() % (int)(_corner_max.y - _corner_min.y));
		_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(x, y, _seed_points.size()));
	}

	sf::Vector2f center		= (_corner_min + _corner_max) * 0.5f;
	sf::Vector2f minMax		= (_corner_max - _corner_min) * 2.0f;
	sf::Vector2f starTop	= {center.x, center.y - minMax.y};
	sf::Vector2f starLeft   = {center.x - minMax.x, center.y};
	sf::Vector2f starRight  = {center.x + minMax.x, center.y};
	sf::Vector2f starBottom = {center.x, center.y + minMax.y};

	_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, _seed_points.size()));
	_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, _seed_points.size()));
	_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, _seed_points.size()));
	_seed_points.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, _seed_points.size()));
}

void Voronoi_Tessellation::CreateVoronoiPolyBodies(const std::vector<sf::Vector2f>& _polygon_hull,
												   std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
												   std::vector<HALF_EDGE::HE_Face*>& _face_list,
												   std::vector<Mesh2D>& _polygons,
												   std::vector<Entity>& _entities,
												   PhysicsSystem* _system)
{
	Coordinator& coordinator = gCoordinator.GetInstance();

	std::vector<VoronoiEdge*> allVoronoiEdges;
	for(size_t i = 0; i < _face_list.size(); i++)
	{
		HALF_EDGE::HE_Edge* e1 = _face_list[i]->edge;
		HALF_EDGE::HE_Edge* e2 = e1->next;
		HALF_EDGE::HE_Edge* e3 = e2->next;

		sf::Vector2f* v1 = e1->vert->point;
		sf::Vector2f* v2 = e2->vert->point;
		sf::Vector2f* v3 = e3->vert->point;

		sf::Vector2f voronoiVertex = _face_list[i]->circumCenter;

		addVoronoiEdge(e1, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e2, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e3, voronoiVertex, allVoronoiEdges);
	}

	std::vector<VoronoiCell> voronoiCells;
	voronoiCells.reserve(_seed_points.size());
	for(int i = 0; i < allVoronoiEdges.size(); i++)
	{
		VoronoiEdge* e = allVoronoiEdges[i];
		e->v1.x		   = roundf(e->v1.x);
		e->v1.y		   = roundf(e->v1.y);
		e->v2.x		   = roundf(e->v2.x);
		e->v2.y		   = roundf(e->v2.y);
		int index = -1;
		//Find the position in the list of all cells that includes this site
		for(int k = 0; k < voronoiCells.size(); k++)
		{
			if(e->site == voronoiCells[k].site)
			{
				index = k;
				break;
			}
		}

		int cellPos = index;
		//No cell was found so we need to create a new cell
		if(cellPos == -1)
		{
			VoronoiCell newCell(e->site);

			voronoiCells.push_back(newCell);

			voronoiCells.back().edges.push_back(e);
			//voronoiCells.back().vertices.emplace(sf::Vector2f(0,0));
			//voronoiCells.back().vertices.insert(e->v2);
		}
		else
		{
			bool inserted = false;
			for (auto i = 0; i != voronoiCells[cellPos].edges.size() && !inserted; i++)
			{

				if(voronoiCells[cellPos].edges[i]->v2 == e->v1)
				{
					voronoiCells[cellPos].edges.insert(
						voronoiCells[cellPos].edges.begin() + ((i + 1) % voronoiCells[cellPos].edges.size()), e);
					inserted = true;
				}
				else if(voronoiCells[cellPos].edges[i]->v1 == e->v2)
				{
					voronoiCells[cellPos].edges.insert(voronoiCells[cellPos].edges.begin() + i, e);
					inserted = true;
				}
			}
			if(!inserted)
			voronoiCells[cellPos].edges.push_back(e);
		}
	}

	_polygons.reserve(_seed_points.size());
	std::vector<Entity> entities;
	entities.reserve(_seed_points.size());

	//Get AABB
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	for(int i = 0; i < _seed_points.size() - 4; i++)
	{
		minX = std::min(minX, _seed_points[i]->point->x);
		maxX = std::max(maxX, _seed_points[i]->point->x);
		minY = std::min(minY, _seed_points[i]->point->y);
		maxY = std::max(maxY, _seed_points[i]->point->y);
	}

	const float dx		 = maxX - minX;
	const float dy		 = maxY - minY;
	const float deltaMax = std::max(dx, dy);
	const float midx	 = (minX + maxX) * 0.5f;
	const float midy	 = (minY + maxY) * 0.5f;

	float halfwidthX = (dx * 0.5f) + 1.0f;
	float halfwidthY = (dy * 0.5f) + 1.0f;

	/*
	ClipLines_LiangBarsky(midx - halfwidthX,
										 midx + halfwidthX,
										 midy + halfwidthY,
										 midy - halfwidthY,
	*/
	// ClipLines_CyrusBeck(test, 4,

	//auto less =[](sf::Vector2f& a, sf::Vector2f& b) 
	//{
	//	if(a.x - center.x >= 0 && b.x - center.x < 0)
	//		return true;
	//	if(a.x - center.x < 0 && b.x - center.x >= 0)
	//		return false;
	//	if(a.x - center.x == 0 && b.x - center.x == 0)
	//	{
	//		if(a.y - center.y >= 0 || b.y - center.y >= 0)
	//			return a.y > b.y;
	//		return b.y > a.y;
	//	}

	//	// compute the cross product of vectors (center -> a) x (center -> b)
	//	int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
	//	if(det < 0)
	//		return true;
	//	if(det > 0)
	//		return false;

	//	// points a and b are on the same line from the center
	//	// check which point is closer to the center
	//	int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
	//	int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
	//	return d1 > d2;
	//};
	//

	//for(auto& cell : voronoiCells) //Sort vertices counterclockwise
	//{
	//	sf::Vector2f p1(*cell.site);
	//	std::vector<float> angles;
	//	std::sort(cell.edges.begin(), cell.edges.end(), )



	//	for (int i = 0; i < cell.edges.size(); i++)
	//	{
	//		sf::Vector2f p2(cell.edges[i]->v1);
	//		sf::Vector2f p3(cell.edges[i]->v2);

	//		(Math.toDegrees(Math.atan2(a.x - p1.x, a.y - p1.y)) + 360) % 360)
	//		(Math.toDegrees(Math.atan2(b.x - p1.x, b.y - p1.y)) + 360) % 360;
	//		(a1 - a2)
	//	}
	//
	//}
	
	for(auto& cell : voronoiCells)
	{
		sf::Color color(rand() % 200, rand() % 200, rand() % 200);
		
		Mesh2D polygon;
		sf::Vector2f p1(*cell.site);
		p1.x = int(p1.x);
		p1.y = int(p1.y);
		*cell.site = p1;
		std::vector<sf::Vector2f> vertices;
		for(auto it = cell.edges.begin(); it != cell.edges.end(); it++)
		{
			vertices.push_back((*it)->v1);
		}

		Voronoi_Tessellation::SuthHodgClip(vertices, _polygon_hull);
		//Voronoi_Tessellation::SuthHodgClip_VE(cell.edges, _OrigPoly);

		//for(int i = 0; i < cell.edges.size(); i++)
		//{
		//	sf::Vector2f p2(cell.edges[i]->v2);
		//	p2.x = int(p2.x);
		//	p2.y = int(p2.y);
		//	sf::Vector2f p3(cell.edges[i]->v1);
		//	p3.x = int(p3.x);
		//	p3.y = int(p3.y);
		//	sf::Vertex vertex[3];
		//	vertex[0]		= p1;
		//	vertex[0].color = color;
		//	vertex[1]		= p2;
		//	vertex[1].color = color;
		//	vertex[2]		= p3;
		//	vertex[2].color = color;
		//	sf::Vector2f v1Result;
		//	sf::Vector2f v2Result;
		//	bool draw = false;
		//	if(ClipLines_CyrusBeck(
		//		   test, 4, p2, p3, v1Result, v2Result))
		//	{
		//		if(v2Result != vertex[1].position)
		//		{
		//			
		//		}
		//		draw			   = true;
		//		vertex[1].position = v1Result;
		//		vertex[2].position = v2Result;
		//	}
		//	/*	if(ClipLines_LiangBarsky(midx - halfwidthX,
		//							 midx + halfwidthX,
		//							 midy + halfwidthY,
		//							 midy - halfwidthY,
		//				   vertex[1].position,
		//				   vertex[2].position,
		//				   v1Result,
		//				   v2Result))
		//	{
		//		sf::Vector2f v1ResultN;
		//		sf::Vector2f v2ResultN;
		//		if(v1Result != vertex[1].position)
		//		{
		//			if(ClipLines_LiangBarsky(midx - halfwidthX,
		//									 midx + halfwidthX,
		//									 midy + halfwidthY,
		//									 midy - halfwidthY,
		//						   vertex[0].position,
		//						   vertex[1].position,
		//						   v1ResultN,
		//						   v2ResultN))
		//			{
		//				sf::Vertex nVertex;
		//				nVertex		  = p1;
		//				nVertex.color = color;
		//				polygon.m_vertices.push_back(nVertex);
		//				nVertex		  = v1Result;
		//				nVertex.color = color;
		//				polygon.m_vertices.push_back(nVertex);
		//				nVertex		  = v2ResultN;
		//				nVertex.color = color;
		//				polygon.m_vertices.push_back(nVertex);
		//			}
		//		}
		//		if(v2Result != vertex[2].position)
		//		{
		//			if(ClipLines_LiangBarsky(midx - halfwidthX,
		//									 midx + halfwidthX,
		//									 midy + halfwidthY,
		//									 midy - halfwidthY,
		//						   vertex[0].position,
		//						   vertex[2].position,
		//						   v1ResultN,
		//						   v2ResultN))
		//			{
		//				sf::Vertex nVertex;
		//				nVertex		  = p1;
		//				nVertex.color = color;
		//				polygon.m_vertices.push_back(nVertex);
		//				nVertex		  = v2Result;
		//				nVertex.color = color;
		//				polygon.m_vertices.push_back(nVertex);
		//				nVertex		  = v2ResultN;
		//				nVertex.color = color;
		//				polygon.m_vertices.push_back(nVertex);
		//			}
		//		}
		//		vertex[1].position = v1Result;
		//		vertex[2].position = v2Result;
		//	}
		//	else
		//	{
		//		bool passed = false;
		//		if(ClipLines_LiangBarsky(midx - halfwidthX,
		//								 midx + halfwidthX,
		//								 midy + halfwidthY,
		//								 midy - halfwidthY,
		//					   vertex[0].position,
		//					   vertex[2].position,
		//					   v1Result,
		//					   v2Result))
		//		{
		//			vertex[0].position = v1Result;
		//			vertex[2].position = v2Result;
		//			passed			 = true;
		//		}
		//		if(ClipLines_LiangBarsky(midx - halfwidthX,
		//								 midx + halfwidthX,
		//								 midy + halfwidthY,
		//								 midy - halfwidthY,
		//					   vertex[0].position,
		//					   vertex[1].position,
		//					   v1Result,
		//					   v2Result))
		//		{
		//			vertex[0].position = v1Result;
		//			vertex[1].position = v2Result;
		//			passed			 = true;
		//		}
		//		if(!passed)
		//			draw = false;
		//	}*/
		//	//if(ClipLines_CyrusBeck(
		//	//	   test, 4, vertex[1].position, vertex[2].position, v1Result, v2Result))
		//	//{
		//	//	sf::Vector2f v1ResultN;
		//	//	sf::Vector2f v2ResultN;
		//	//	if(v1Result != vertex[1].position)
		//	//	{
		//	//		if(ClipLines_CyrusBeck( test, 4,
		//	//								 vertex[0].position,
		//	//								 vertex[1].position,
		//	//								 v1ResultN,
		//	//								 v2ResultN))
		//	//		{
		//	//			sf::Vertex nVertex;
		//	//			nVertex		  = p1;
		//	//			nVertex.color = color;
		//	//			polygon.m_vertices.push_back(nVertex);
		//	//			nVertex		  = v1Result;
		//	//			nVertex.color = color;
		//	//			polygon.m_vertices.push_back(nVertex);
		//	//			nVertex		  = v2ResultN;
		//	//			nVertex.color = color;
		//	//			polygon.m_vertices.push_back(nVertex);
		//	//		}
		//	//	}
		//	//	draw = true;
		//	//	vertex[1].position = v1Result;
		//	//	vertex[2].position = v2Result;
		//	//}
		//	//if(ClipLines_CyrusBeck(
		//	//	   test, 4, vertex[0].position, vertex[1].position, v1Result, v2Result))
		//	//{
		//	//	if(v2Result == vertex[1].position)
		//	//	{
		//	//		draw = true;
		//	//	}
		//	//	if(v2Result == v1Result) //PARALLEL WITH
		//	//	{
		//	//		if(vertex[1].position.x == vertex[2].position.x)
		//	//			vertex[1].position.y = vertex[0].position.y;
		//	//		else
		//	//			vertex[1].position.x = vertex[0].position.x;
		//	//	}
		//	//}
		//	//if(ClipLines_CyrusBeck(
		//	//	   test, 4, vertex[0].position, vertex[2].position, v1Result, v2Result))
		//	//{
		//	//	if(v2Result == vertex[2].position)
		//	//	{
		//	//		draw = true;
		//	//	}
		//	//	if(v2Result == v1Result)
		//	//	{
		//	//		if(vertex[2].position.x == vertex[1].position.x)
		//	//			vertex[2].position.y = vertex[0].position.y;
		//	//		else
		//	//			vertex[2].position.x = vertex[0].position.x;
		//	//	}
		//	//}
		//	
		//	if(draw)
		//	{
		//		polygon.m_vertices.push_back(vertex[0]);
		//		polygon.m_vertices.push_back(vertex[1]);
		//		polygon.m_vertices.push_back(vertex[2]);
		//	}
		//}
		if (!vertices.empty())
		{
			polygon.primitive_type = sf::PrimitiveType::TriangleFan;
			polygon.vertices.push_back(sf::Vertex(vertices[0], color));
			for(int i = 1; i < vertices.size(); i++)
			{
				sf::Vertex vertex[3];
				vertex[0]		= vertices[0];
				vertex[0].color = color;
				vertex[1]		= vertices[i];
				vertex[1].color = color;
				vertex[2]		= vertices[(i) % vertices.size()];
				vertex[2].color = color;

				//polygon.m_vertices.push_back(vertex[0]);
				polygon.vertices.push_back(vertex[1]);
				//polygon.m_vertices.push_back(vertex[2]);
			}
		}
		
		if(polygon.vertices.size() != 0)
		{
			//SITE POSITION: sf::Vector2f(cell.site->x, cell.site->y) - sf::Vector2f(polygon.m_center.getRadius(), polygon.m_center.getRadius())
			entities.push_back(coordinator.CreateEntity());
			coordinator.AddComponent(entities.back(), Live{});
			coordinator.AddComponent(entities.back(), polygon);

			float2 minBound = {FLT_MAX, FLT_MAX};
			float2 maxBound = {-FLT_MAX, -FLT_MAX};
			for(auto& vertex : polygon.vertices)
			{
				sf::Vector2f pos = vertex.position;
				minBound[0]		 = std::min(pos.x, minBound[0]);
				minBound[1]		 = std::min(pos.y, minBound[1]);
				maxBound[0]		 = std::max(pos.x, maxBound[0]);
				maxBound[1]		 = std::max(pos.y, maxBound[1]);
			}
			coordinator.AddComponent(entities.back(), Selectable{minBound, maxBound});

			sf::Transformable transform;
			transform.setOrigin({minBound[0] + (maxBound[0] - minBound[0]) / 2,
								 minBound[1] + (maxBound[1] - minBound[1]) / 2});
			////PLACEHOLDER///////////////////
			transform.setScale(100.0f,100.0f);
			/////////////////////////////////
			sf::Vector2f test;
			test.x = transform.getScale().x * transform.getOrigin().x;
			test.y = transform.getScale().y * transform.getOrigin().y;
			transform.setPosition(test);
			coordinator.AddComponent(entities.back(), transform);
			_polygons.push_back(polygon);
		}
	}
	_polygons.shrink_to_fit();
	entities.shrink_to_fit();

	auto less = [](const b2Vec2& a, const b2Vec2& b) {
		return a.x < b.x || (a.x == b.x && a.y > b.y);
	};

	auto cross = [](const b2Vec2& O, const b2Vec2& A, const b2Vec2& B) {
		return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
	};

	for(auto entity : entities)
	{
		auto& polygon = coordinator.GetComponent<Mesh2D>(entity);
		size_t n = polygon.vertices.size(), k = 0;
		std::vector<b2Vec2> points(polygon.vertices.size());
		for(size_t i = 0; i < n; i++)
		{
			//Set vertex relative to entity origin
			sf::Vector2f OrgScale =
				coordinator.GetComponent<sf::Transformable>(entity).getOrigin();
			OrgScale.x *= coordinator.GetComponent<sf::Transformable>(entity).getScale().x;
			OrgScale.y *= coordinator.GetComponent<sf::Transformable>(entity).getScale().y;
			sf::Vector2f p = coordinator.GetComponent<sf::Transformable>(entity).getTransform() *
				polygon.vertices[i].position;
			points[i] = b2Vec2(p.x, p.y);
		}

		{
			std::vector<bool> remove(points.size(), false);
			for(size_t i = 0; i < points.size(); i++)
			{
				for(size_t k = i; k < points.size(); k++)
				{
					if(i == k)
					{
						continue;
					}
					if(points[i] == points[k])
					{
						remove[k] = true;
					}
				}
			}
			auto is_duplicate = [&](auto const& e) { return remove[&e - &points[0]]; };
			erase_where(points, is_duplicate);
			remove.clear();
		}

		//std::sort(points.begin(), points.end(), less);

		//{
		//	std::vector<bool> remove(points.size(), false);
		//	for (int i = 0; i < points.size() - 1; i++)
		//	{
		//		if (b2DistanceSquared(points[i], points[i + 1]) < ((0.5f * b2_linearSlop) * (0.5f * b2_linearSlop)))
		//		{
		//			remove[i] = true;
		//		}
		//	}
		//	auto is_too_close = [&](auto const& e) { return remove[&e - &points[0]]; };
		//	erase_where(points, is_too_close);
		//	remove.clear();
		//}
		n = points.size();
		std::vector<b2Vec2> hull(n * 2);
		for(size_t i = 0; i < n; ++i)
		{
			while(k >= 2 && cross(hull[k - 2], hull[k - 1], points[i]) <= 0)
				k--;
			hull[k++] = points[i];
		}

		for(size_t i = n - 1, t = k + 1; i > 0; --i)
		{
			while(k >= t && cross(hull[k - 2], hull[k - 1], points[i - 1]) <= 0)
				k--;
			hull[k++] = points[i - 1];
		}
		hull.resize(k - 1);

		//{
		//	std::vector<bool> remove(hull.size(), false);
		//	for (size_t i = 0; i < hull.size(); i++)
		//	{
		//		for (size_t k = i; k < hull.size(); k++)
		//		{
		//			if (i == k) {
		//				continue;
		//			}
		//			if (hull[i] == hull[k])
		//			{
		//				remove[k] = true;
		//			}
		//		}
		//	}
		//	auto is_duplicate = [&](auto const& e) { return remove[&e - &hull[0]]; };
		//	erase_where(hull, is_duplicate);
		//	remove.clear();
		//}
		//{
		//	std::vector<bool> remove(hull.size(), false);
		//	for (int i = 0; i < hull.size() - 1; i++)
		//	{
		//		if (b2DistanceSquared(hull[i], hull[i + 1]) < ((0.5f * b2_linearSlop) * (0.5f * b2_linearSlop)))
		//		{
		//			remove[i] = true;
		//		}
		//	}
		//	auto is_too_close = [&](auto const& e) { return remove[&e - &hull[0]]; };
		//	erase_where(hull, is_too_close);
		//	remove.clear();
		//}

		for(size_t i = 0; i < points.size(); i++)
		{
			points[i].x /= kPixelToMeterRatio;
			points[i].y /= kPixelToMeterRatio;
		}
		//Render(window, sfHull, points);
		_system->CreateMeshCollider(entity, b2BodyType::b2_dynamicBody);
	}

	_entities.insert(_entities.end(), entities.begin(), entities.end());
	entities.clear();

	for(auto& face : _face_list)
	{
		delete face;
	}
	_face_list.clear();
	for(auto& edge : allVoronoiEdges)
	{
		delete edge;
	}
	allVoronoiEdges.clear();
	voronoiCells.clear();
}


void Voronoi_Tessellation::CreateVoronoiPolyBodies(const std::vector<sf::Vector2f>& _polygon_hull,
												   std::vector<HALF_EDGE::HE_Vertex*>& _seed_points,
												   std::vector<HALF_EDGE::HE_Face*>& _face_list,
												   PhysicsSystem* _system)
{
	Coordinator& coordinator = gCoordinator.GetInstance();

	std::vector<VoronoiEdge*> allVoronoiEdges;
	for(size_t i = 0; i < _face_list.size(); i++)
	{
		HALF_EDGE::HE_Edge* e1 = _face_list[i]->edge;
		HALF_EDGE::HE_Edge* e2 = e1->next;
		HALF_EDGE::HE_Edge* e3 = e2->next;

		sf::Vector2f* v1 = e1->vert->point;
		sf::Vector2f* v2 = e2->vert->point;
		sf::Vector2f* v3 = e3->vert->point;

		sf::Vector2f voronoiVertex = _face_list[i]->circumCenter;

		addVoronoiEdge(e1, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e2, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e3, voronoiVertex, allVoronoiEdges);
	}

	std::vector<VoronoiCell> voronoiCells;
	voronoiCells.reserve(_seed_points.size());
	for(int i = 0; i < allVoronoiEdges.size(); i++)
	{
		VoronoiEdge* e = allVoronoiEdges[i];
		e->v1.x		   = roundf(e->v1.x);
		e->v1.y		   = roundf(e->v1.y);
		e->v2.x		   = roundf(e->v2.x);
		e->v2.y		   = roundf(e->v2.y);
		int index	  = -1;
		//Find the position in the list of all cells that includes this site
		for(int k = 0; k < voronoiCells.size(); k++)
		{
			if(e->site == voronoiCells[k].site)
			{
				index = k;
				break;
			}
		}

		int cellPos = index;
		//No cell was found so we need to create a new cell
		if(cellPos == -1)
		{
			VoronoiCell newCell(e->site);

			voronoiCells.push_back(newCell);

			voronoiCells.back().edges.push_back(e);
			//voronoiCells.back().vertices.emplace(sf::Vector2f(0,0));
			//voronoiCells.back().vertices.insert(e->v2);
		}
		else
		{
			bool inserted = false;
			for(auto i = 0; i != voronoiCells[cellPos].edges.size() && !inserted; i++)
			{

				if(voronoiCells[cellPos].edges[i]->v2 == e->v1)
				{
					voronoiCells[cellPos].edges.insert(
						voronoiCells[cellPos].edges.begin() +
							((i + 1) % voronoiCells[cellPos].edges.size()),
						e);
					inserted = true;
				}
				else if(voronoiCells[cellPos].edges[i]->v1 == e->v2)
				{
					voronoiCells[cellPos].edges.insert(voronoiCells[cellPos].edges.begin() + i, e);
					inserted = true;
				}
			}
			if(!inserted)
				voronoiCells[cellPos].edges.push_back(e);
		}
	}

	std::vector<Entity> entities;
	entities.reserve(_seed_points.size());

	//Get AABB
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	for(int i = 0; i < _seed_points.size() - 4; i++)
	{
		minX = std::min(minX, _seed_points[i]->point->x);
		maxX = std::max(maxX, _seed_points[i]->point->x);
		minY = std::min(minY, _seed_points[i]->point->y);
		maxY = std::max(maxY, _seed_points[i]->point->y);
	}

	const float dx		 = maxX - minX;
	const float dy		 = maxY - minY;
	const float deltaMax = std::max(dx, dy);
	const float midx	 = (minX + maxX) * 0.5f;
	const float midy	 = (minY + maxY) * 0.5f;

	float halfwidthX = (dx * 0.5f) + 1.0f;
	float halfwidthY = (dy * 0.5f) + 1.0f;

	for(auto& cell : voronoiCells)
	{
		sf::Color color(rand() % 200, rand() % 200, rand() % 200);

		Mesh2D polygon;
		sf::Vector2f p1(*cell.site);
		p1.x	   = int(p1.x);
		p1.y	   = int(p1.y);
		*cell.site = p1;
		std::vector<sf::Vector2f> vertices;
		for(auto it = cell.edges.begin(); it != cell.edges.end(); it++)
		{
			vertices.push_back((*it)->v1);
		}

		Voronoi_Tessellation::SuthHodgClip(vertices, _polygon_hull);

		if(!vertices.empty())
		{
			std::vector<bool> remove(vertices.size(), false);
			for(size_t i = 0; i < vertices.size(); i++)
			{
				for(size_t k = i; k < vertices.size(); k++)
				{
					if(i == k)
					{
						continue;
					}
					if(vertices[i] == vertices[k])
					{
						remove[k] = true;
					}
				}
			}
			auto is_duplicate = [&](auto const& e) { return remove[&e - &vertices[0]]; };
			erase_where(vertices, is_duplicate);
			remove.clear();

			polygon.primitive_type = sf::PrimitiveType::TriangleFan;
			polygon.vertices.push_back(sf::Vertex(vertices[0], color));
			for(int i = 1; i < vertices.size(); i++)
			{
				polygon.vertices.push_back(sf::Vertex(vertices[i], color));
			}
		}

		if(polygon.vertices.size() != 0)
		{
			entities.push_back(coordinator.CreateEntity());
			coordinator.AddComponent(entities.back(), Live{});
			coordinator.AddComponent(entities.back(), polygon);

			float2 minBound = {FLT_MAX, FLT_MAX};
			float2 maxBound = {-FLT_MAX, -FLT_MAX};
			for(auto& vertex : polygon.vertices)
			{
				sf::Vector2f pos = vertex.position;
				minBound[0]		 = std::min(pos.x, minBound[0]);
				minBound[1]		 = std::min(pos.y, minBound[1]);
				maxBound[0]		 = std::max(pos.x, maxBound[0]);
				maxBound[1]		 = std::max(pos.y, maxBound[1]);
			}
			coordinator.AddComponent(entities.back(), Selectable{minBound, maxBound});

			sf::Transformable transform;
			transform.setOrigin({minBound[0] + (maxBound[0] - minBound[0]) / 2,
								 minBound[1] + (maxBound[1] - minBound[1]) / 2});
			////PLACEHOLDER///////////////////
			transform.setScale(100.0f, 100.0f);
			/////////////////////////////////
			sf::Vector2f test;
			test.x = transform.getScale().x * transform.getOrigin().x;
			test.y = transform.getScale().y * transform.getOrigin().y;
			transform.setPosition(test);
			coordinator.AddComponent(entities.back(), transform);
		}
	}
	entities.shrink_to_fit();

	for(auto entity : entities)
	{
		_system->CreateMeshCollider(entity, b2BodyType::b2_dynamicBody);
	}

	for(auto& face : _face_list)
	{
		delete face;
	}
	_face_list.clear();
	for(auto& edge : allVoronoiEdges)
	{
		delete edge;
	}
	allVoronoiEdges.clear();
	voronoiCells.clear();
}


void Voronoi_Tessellation::addVoronoiEdge(HALF_EDGE::HE_Edge* _edge,
										  sf::Vector2f _center,
										  std::vector<VoronoiEdge*>& _voronoi_edges)
{
	//Ignore if this edge has no neighboring triangle
	if(_edge->twin == nullptr)
		return;

	//Get the circumcenter of the neighbor
	HALF_EDGE::HE_Edge* eNeighbor	  = _edge->twin;
	sf::Vector2f voronoiVertexNeighbor = eNeighbor->face->circumCenter;

	//Create a new voronoi edge between the voronoi vertices
	_voronoi_edges.push_back(new VoronoiEdge(_center, voronoiVertexNeighbor, _edge->prev->vert->point));
}

bool Voronoi_Tessellation::ClipLines_LiangBarsky(
	double _edgeLeft,
	double _edgeRight,
	double _edgeBottom,
	double _edgeTop, // Define the x/y clipping values for the border.
	sf::Vector2f _src0,
	sf::Vector2f _src1, // Define the start and end points of the line.
	sf::Vector2f& _clip0,
	sf::Vector2f& _clip1) // The output values, so declare these outside.
{

	double t0	 = 0.0;
	double t1	 = 1.0;
	double xdelta = _src1.x - _src0.x;
	double ydelta = _src1.y - _src0.y;
	double p, q, r;

	for(int edge = 0; edge < 4; edge++)
	{ // Traverse through left, right, bottom, top edges.
		if(edge == 0)
		{
			p = -xdelta;
			q = -(_edgeLeft - _src0.x);
		}
		if(edge == 1)
		{
			p = xdelta;
			q = (_edgeRight - _src0.x);
		}
		if(edge == 2)
		{
			p = ydelta;
			q = (_edgeBottom - _src0.y);
		}
		if(edge == 3)
		{
			p = -ydelta;
			q = -(_edgeTop - _src0.y);
		}
		r = q / p;
		if(p == 0 && q < 0)
			return false; // Don't draw line at all. (parallel line outside)

		if(p < 0)
		{
			if(r > t1)
				return false; // Don't draw line at all.
			else if(r > t0)
				t0 = r; // Line is clipped!
		}
		else if(p > 0)
		{
			if(r < t0)
				return false; // Don't draw line at all.
			else if(r < t1)
				t1 = r; // Line is clipped!
		}
	}

	_clip0 = {_src0.x + float(t0 * xdelta), _src0.y + float(t0 * ydelta)};
	_clip1 = {_src0.x + float(t1 * xdelta), _src0.y + float(t1 * ydelta)};

	return true; // (clipped) line is drawn
}

bool Voronoi_Tessellation::ClipLines_CyrusBeck(
	std::pair<int, int> _vertices[],
	int _n,
	sf::Vector2f _src0,
	sf::Vector2f _src1,
	sf::Vector2f& _clip0,
	sf::Vector2f& _clip1) 
	//arg 1: Bounds, aka the original polygon, line to clip. Returns the clipped line
{
	// Function to take dot product
	auto dot = [](std::pair<int, int> p0, std::pair<int, int> p1) -> int {
		return p0.first * p1.first + p0.second * p1.second;
	};

	// Function to calculate the max from a vector of floats
	auto max = [](std::vector<float> t) -> float {
		float maximum = INT_MIN;
		for(int i = 0; i < t.size(); i++)
			if(t[i] > maximum)
				maximum = t[i];
		return maximum;
	};

	// Function to calculate the min from a vector of floats
	auto min = [](std::vector<float> t) -> float {
		float minimum = INT_MAX;
		for(int i = 0; i < t.size(); i++)
			if(t[i] < minimum)
				minimum = t[i];
		return minimum;
	};

	// Normals initialized dynamically(can do it statically also, doesn't matter)
	std::pair<int, int>* normal = new std::pair<int, int>[_n];

	// Calculating the normals
	for(int i = 0; i < _n; i++)
	{
		normal[i].second = _vertices[(i + 1) % _n].first - _vertices[i].first;
		normal[i].first  = _vertices[i].second - _vertices[(i + 1) % _n].second;
	}

	// Calculating P1 - P0
	std::pair<int, int> P1_P0 = std::make_pair(_src1.x - _src0.x, _src1.y - _src0.y);

	// Initializing all values of P0 - PEi
	std::pair<int, int>* P0_PEi = new std::pair<int, int>[_n];

	// Calculating the values of P0 - PEi for all edges
	for(int i = 0; i < _n; i++)
	{
		// Calculating PEi - P0, so that the
		// denominator won't have to multiply by -1
		P0_PEi[i].first = _vertices[i].first - _src0.x;

		// while calculating 't'
		P0_PEi[i].second = _vertices[i].second - _src0.y;
	}

	int *numerator = new int[_n], *denominator = new int[_n];

	// Calculating the numerator and denominators
	// using the dot function
	for(int i = 0; i < _n; i++)
	{
		numerator[i]   = dot(normal[i], P0_PEi[i]);
		denominator[i] = dot(normal[i], P1_P0);
	}

	// Initializing the 't' values dynamically
	float* t = new float[_n];

	// Making two vectors called 't entering'
	// and 't leaving' to group the 't's
	// according to their denominators
	std::vector<float> tE, tL;

	// Calculating 't' and grouping them accordingly
	for(int i = 0; i < _n; i++)
	{
		t[i] = (float)(numerator[i]) / (float)(denominator[i]);
		if(denominator[i] > 0)
			tE.push_back(t[i]);
		else
			tL.push_back(t[i]);
	}

	// Initializing the final two values of 't'
	float temp[2];

	// Taking the max of all 'tE' and 0, so pushing 0
	tE.push_back(0.f);
	temp[0] = max(tE);

	// Taking the min of all 'tL' and 1, so pushing 1
	tL.push_back(1.f);
	temp[1] = min(tL);

	// Entering 't' value cannot be
	// greater than exiting 't' value,
	// hence, this is the case when the line
	// is completely outside
	if(temp[0] > temp[1])
	{
		_clip0 = sf::Vector2f(-1, -1);
		_clip1 = sf::Vector2f(-1, -1);
		return false;
	}

	// Calculating the coordinates in terms of x and y
	_clip0.x = int((float)_src0.x + (float)P1_P0.first * (float)temp[0]);
	_clip0.y = int((float)_src0.y + (float)P1_P0.second * (float)temp[0]);
	_clip1.x = int((float)_src0.x + (float)P1_P0.first * (float)temp[1]);
	_clip1.y = int((float)_src0.y + (float)P1_P0.second * (float)temp[1]);
	
	return true;
}

void Voronoi_Tessellation::SuthHodgClip(std::vector<sf::Vector2f>& _polygon,
										const std::vector<sf::Vector2f>& _clipper_vertices)
{
	//i and k are two consecutive indexes
	for(int i = 0; i < _clipper_vertices.size(); i++)
	{
		int k = (i + 1) % _clipper_vertices.size(); 

		// We pass the current array of vertices, it's size
		// and the end points of the selected clipper line
		ClipPoly(_polygon, _clipper_vertices[i], _clipper_vertices[k]);
	}
}

void Voronoi_Tessellation::ClipPoly(std::vector<sf::Vector2f>& _polygon,
									sf::Vector2f _clip0,
									sf::Vector2f _clip1)
{
	int x1				 = _clip0.x;
	int y1				 = _clip0.y;
	int x2				 = _clip1.x;
	int y2				 = _clip1.y;

	const int MAX_POINTS = 20; 
	std::vector<sf::Vector2f> new_points;
	new_points.reserve(MAX_POINTS);

	auto x_intersect = [](int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) -> int
	{
		int num = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
		int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		return num / den; 
	};

	auto y_intersect = [](int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) -> int
	{
		int num = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
		int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		return num / den; 
	};

	for(int i = 0; i < _polygon.size(); i++)
	{
		int k  = (i + 1) % _polygon.size(); 
		int ix = _polygon[i].x, iy = _polygon[i].y;
		int kx = _polygon[k].x, ky = _polygon[k].y;

		
		int i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1); 

		int k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1); 

		// Case 1 : When both points are inside
		if(i_pos < 0 && k_pos < 0)
		{
			//Only second point is added
			new_points.push_back(sf::Vector2f(kx, ky));
		} 
		// Case 2: When only first point is outside
		else if(i_pos >= 0 && k_pos < 0)
		{
			// Point of intersection with edge
			// and the second point is added
			new_points.push_back(sf::Vector2f(x_intersect(x1, y1, x2, y2, ix, iy, kx, ky), 
				y_intersect(x1, y1, x2, y2, ix, iy, kx, ky)));

			new_points.push_back(sf::Vector2f(kx, ky));
		} 
		// Case 3: When only second point is outside 
		else if(i_pos < 0 && k_pos >= 0)
		{
			//Only point of intersection with edge is added
			new_points.push_back(sf::Vector2f(x_intersect(x1, y1, x2, y2, ix, iy, kx, ky),
											  y_intersect(x1, y1, x2, y2, ix, iy, kx, ky)));
		} 
		// Case 4: When both points are outside
		else
		{
			//No points are added
		} 

	}
	///////TODO///////////////////////////////////////
	//TRACK EDGE PAIRS ///////////////////////////////
	//WILL REDUCE THE AMMOUNT OF TRIANGLES TO CREATE//
	//////////////////////////////////////////////////

	// Copying new points into original array
	_polygon	= new_points;
}