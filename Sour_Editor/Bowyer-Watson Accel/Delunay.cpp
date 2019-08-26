#include "Delunay.h"
#include "..\pch.h"

void Render(sf::RenderWindow& window, std::vector<HALF_EDGE::HE_Face*>& faces)
{
	bool pause = false;
	double timer = 0;
	sf::Event event;
	while (timer < 3)
	{
		auto start = std::chrono::system_clock::now();
		window.clear();
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				else if (event.key.code == sf::Keyboard::Space)
				{
					pause = 1;
					window.setTitle("Paused: " + std::to_string(pause));
				}
				else if (event.key.code == sf::Keyboard::BackSpace)
				{
					pause = 0;
					window.setTitle("Paused: " + std::to_string(pause));
					start = std::chrono::system_clock::now();
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					timer = 3;
				}
				break;
			}
		}
		if (!pause)
		{
			for (auto &face : faces)
			{
				sf::CircleShape point;
				point.setRadius(face->radius);
				point.setPosition(sf::Vector2f(face->circumCenter.x, face->circumCenter.y) - sf::Vector2f(face->radius, face->radius));
				point.setOutlineThickness(1.0f);
				point.setOutlineColor({ 255,0,255 });
				point.setFillColor(sf::Color::Transparent);
				window.draw(point);

				point = sf::CircleShape();
				point.setRadius(2);
				point.setPosition(sf::Vector2f(face->circumCenter.x, face->circumCenter.y) - sf::Vector2f(2, 2));
				point.setFillColor({ 0,0,255 });
				window.draw(point);
				sf::Vertex test[6];
				test[0].position = *face->edge->vert->point;
				test[1].position = *face->edge->twin->vert->point;

				test[2].position = *face->edge->next->vert->point;
				test[3].position = *face->edge->next->twin->vert->point;

				test[4].position = *face->edge->prev->vert->point;
				test[5].position = *face->edge->prev->twin->vert->point;
				window.draw(test, 6, sf::Lines);
				point = sf::CircleShape();
				point.setRadius(2);
				point.setPosition(*face->edge->vert->point - sf::Vector2f(2, 2));
				point.setFillColor({ 255,0,0 });
				window.draw(point);
				point = sf::CircleShape();
				point.setRadius(2);
				point.setPosition(*face->edge->next->vert->point - sf::Vector2f(2, 2));
				point.setFillColor({ 255,0,0 });
				window.draw(point);
				point = sf::CircleShape();
				point.setRadius(2);
				point.setPosition(*face->edge->prev->vert->point - sf::Vector2f(2, 2));
				point.setFillColor({ 255,0,0 });
				window.draw(point);
			}
			window.display();
			std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
			timer += elapsed_seconds.count();
		}

	}
	timer = 0;
}
void Delunay::Triangulate(std::vector<HALF_EDGE::HE_Vertex*>& points, std::vector<HALF_EDGE::HE_Face*>& faces)
{
	sf::ContextSettings settings(0, 0, 16);
	//sf::RenderWindow window(sf::VideoMode(1000, 1000), "Paused: 0", sf::Style::Default, settings);
	float zoom = 5;

	/*sf::View view = window.getDefaultView();
	view.setCenter(sf::Vector2f(500, 500));
	view.zoom(zoom);
	window.setView(view);*/
	
	HALF_EDGE::HE_Edge* e1 = nullptr;
	HALF_EDGE::HE_Edge* e2 = nullptr;
	HALF_EDGE::HE_Edge* e3 = nullptr;

	HALF_EDGE::HE_Face* f = nullptr;

	HALF_EDGE::HE_Vertex* v1 = nullptr;
	HALF_EDGE::HE_Vertex* v2 = nullptr;
	HALF_EDGE::HE_Vertex* v3 = nullptr;

	if (faces.empty())
	{
		// Determinate the super triangle
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;

		for (HALF_EDGE::HE_Vertex* pt : points)
		{
			minX = std::min(minX, pt->point->x);
			maxX = std::max(maxX, pt->point->x);
			minY = std::min(minY, pt->point->y);
			maxY = std::max(maxY, pt->point->y);
		}

		const float dx = maxX - minX;
		const float dy = maxY - minY;
		const float deltaMax = std::max(dx, dy);
		const float midx = (minX + maxX)*0.5f;
		const float midy = (minY + maxY)*0.5f;

		v1 = new  HALF_EDGE::HE_Vertex(midx - 20 * deltaMax, midy - deltaMax, -1);
		v2 = new  HALF_EDGE::HE_Vertex(midx, midy + 20 * deltaMax, -2);
		v3 = new  HALF_EDGE::HE_Vertex(midx + 20 * deltaMax, midy - deltaMax, -3);

		e1 = new HALF_EDGE::HE_Edge(v1);
		v1->edge = e1;
		e2 = new HALF_EDGE::HE_Edge(v2);
		v2->edge = e2;
		e3 = new HALF_EDGE::HE_Edge(v3);
		v3->edge = e3;

		e1->setNext(e2);
		e2->setNext(e3);
		e3->setNext(e1);

		e1->setTwin(new HALF_EDGE::HE_Edge(v3));
		e2->setTwin(new HALF_EDGE::HE_Edge(v1));
		e3->setTwin(new HALF_EDGE::HE_Edge(v2));

		e1->twin->setNext(e3->twin);
		e2->twin->setNext(e1->twin);
		e3->twin->setNext(e2->twin);

		f = new HALF_EDGE::HE_Face();
		f->edge = e1;
		e1->face = f;
		e2->face = f;
		e3->face = f;

		e1->twin->face = f;
		e2->twin->face = f;
		e3->twin->face = f;

		generateCircumCircle(f);
		faces.push_back(f);
	}
		
	double circumCheckTime = 0;
	double dublicateEdgesTime = 0;
	double triangleCreationTime = 0;
	double pushBackTime = 0;

	for (HALF_EDGE::HE_Vertex* const pt : points)
	{
		std::vector<HALF_EDGE::HE_Edge*> edges;
		std::map<HALF_EDGE::HE_Face*, bool> intersections;

		findIntersections(pt, faces, intersections, edges);
		
		
		auto is_bad = [&](auto const& face)
		{ 
			std::map<HALF_EDGE::HE_Face*, bool>::iterator it = intersections.find(face);
			return (it != intersections.end()) ? it->second : false;
		};
		erase_where(faces, is_bad);

		/* Delete duplicate edges. */
		std::vector<bool> remove(edges.size(), false);

		for (auto it1 = edges.begin(); it1 != edges.end(); ++it1) {
			for (auto it2 = edges.begin(); it2 != edges.end(); ++it2) {
				if (it1 == it2) {
					continue;
				}
				if (equalEdge(*it1, *it2)) 
				{	
					//exchangeEdges(*it1, *it2);
					remove[std::distance(edges.begin(), it1)] = true;
					remove[std::distance(edges.begin(), it2)] = true;
				}
			}
		}
		auto is_duplicate = [&](auto const& e) { return remove[&e - &edges[0]]; };
		erase_where(edges, is_duplicate);

		/* Update triangulation. */
		{
			std::map< std::pair<int,int>, HALF_EDGE::HE_Edge* > edgeList;
			for (auto edge : edges)
			{
				HALF_EDGE::HE_Face* face = new HALF_EDGE::HE_Face();
				edge->face = face;
				face->edge = edge;

				std::pair<int,int> edge1 = std::make_pair(edge->vert->arrayIndex, pt->arrayIndex);
				std::pair<int,int> edge2 = std::make_pair(pt->arrayIndex, edge->twin->vert->arrayIndex);
				edgeList[edge1] = new HALF_EDGE::HE_Edge(pt, face);
				edgeList[edge1]->setPrev(edge);

				edgeList[edge2] = new HALF_EDGE::HE_Edge(edge->twin->vert, face);
				edgeList[edge2]->setPrev(edgeList[edge1]);
				edgeList[edge2]->setNext(edge);

				std::pair<unsigned int, unsigned int> otherPair = std::make_pair(pt->arrayIndex, edge->vert->arrayIndex);
				if (edgeList.find(otherPair) != edgeList.end())
					edgeList[edge1]->setTwin(edgeList[otherPair]);
				
				otherPair = std::make_pair(edge->twin->vert->arrayIndex, pt->arrayIndex);

				if (edgeList.find(otherPair) != edgeList.end())
					edgeList[edge2]->setTwin(edgeList[otherPair]);

				generateCircumCircle(face);
				faces.push_back(face);
			}
		}

#if 0
		Render(window, faces);
#endif
	}

	auto is_part_STriangle = [&](HALF_EDGE::HE_Face* const& face) {
		return (face->edge->vert->arrayIndex< 0 || face->edge->next->vert->arrayIndex < 0 || face->edge->prev->vert->arrayIndex < 0); };
	erase_where(faces, is_part_STriangle);

	delete v1,v1,v3;
	delete e1,e2,e3;
	delete f;
}

bool Delunay::equalEdge(HALF_EDGE::HE_Edge * e1, HALF_EDGE::HE_Edge * e2)
{
	return e1->twin == e2;
}

void Delunay::exchangeEdges(HALF_EDGE::HE_Edge * e1, HALF_EDGE::HE_Edge * e2)
{
	e1->prev->setNext(e2->next);
	e2->prev->setNext(e1->next);

	delete e1;
	delete e2;
	e1 = nullptr;
	e2 = nullptr;
}

void Delunay::generateCircumCircle(HALF_EDGE::HE_Face * face)
{
	HALF_EDGE::HE_Edge* edge = face->edge;
	sf::Vector2f* p1 = edge->vert->point;
	sf::Vector2f* p2 = edge->prev->vert->point;
	sf::Vector2f* p3 = edge->next->vert->point;

	const float ax = p2->x - p1->x;
	const float ay = p2->y - p1->y;
	const float bx = p3->x - p1->x;
	const float by = p3->y - p1->y;

	const float m = p2->x * p2->x - p1->x * p1->x + p2->y * p2->y - p1->y * p1->y;
	const float u = p3->x * p3->x - p1->x * p1->x + p3->y * p3->y - p1->y * p1->y;
	const float s = 1. / (2. * (ax * by - ay * bx));

	face->circumCenter.x = ((p3->y - p1->y) * m + (p1->y - p2->y) * u) * s;
	face->circumCenter.y = ((p1->x - p3->x) * m + (p2->x - p1->x) * u) * s;

	const float dx = p1->x - face->circumCenter.x;
	const float dy = p1->y - face->circumCenter.y;
	face->radius = dx * dx + dy * dy;
}

bool Delunay::findIntersections(HALF_EDGE::HE_Vertex* pt, std::vector<HALF_EDGE::HE_Face*> faces, std::map<HALF_EDGE::HE_Face*, bool>& intersections, std::vector<HALF_EDGE::HE_Edge*>& edges)
{
	for (HALF_EDGE::HE_Face* t : faces)
	{
		const auto dist = (t->circumCenter.x - pt->point->x) * (t->circumCenter.x - pt->point->x) +
			(t->circumCenter.y - pt->point->y) * (t->circumCenter.y - pt->point->y);
		if ((dist - t->radius) <= eps)
		{
			//Pushing bad triangle
			intersections[t] = true;
			HALF_EDGE::getFaceEdges(edges, t);
			/*HALF_EDGE::HE_Edge * start_Edge = t->edge;
			HALF_EDGE::HE_Edge * half_edge = start_Edge;
			do
			{

				Delunay::neighborIntersections(pt, half_edge->twin, intersections, edges);
				half_edge = half_edge->next;
			} while (half_edge != start_Edge);
			break;*/
		}
		else
		{
			intersections[t] = false;
			//does not contain in its circumcenter
		}
		
	}
	
	return false;
}

void Delunay::neighborIntersections(HALF_EDGE::HE_Vertex * pt, HALF_EDGE::HE_Edge * edge, std::map<HALF_EDGE::HE_Face*, bool>& intersections, std::vector<HALF_EDGE::HE_Edge*>& edges)
{
	HALF_EDGE::HE_Face* t = edge->face;
	if (intersections.find(t) != intersections.end())
		return;

	const auto dist = (t->circumCenter.x - pt->point->x) * (t->circumCenter.x - pt->point->x) +
		(t->circumCenter.y - pt->point->y) * (t->circumCenter.y - pt->point->y);
	if ((dist - t->radius) <= eps)
	{
		intersections[t] = true;
		HALF_EDGE::getFaceEdges(edges, t);
		HALF_EDGE::HE_Edge * start_Edge = edge;
		HALF_EDGE::HE_Edge * half_edge = start_Edge;
		do
		{

			Delunay::neighborIntersections(pt, half_edge->twin, intersections, edges);
			half_edge = half_edge->next;
		} while (half_edge != start_Edge);
	}
	else
	{
		intersections[t] = false;
	}
}

#if 0
HALF_EDGE::HE_Edge* start_Edge = edges[0];
//Set vertices
auto edgeNext = new HALF_EDGE::HE_Edge(start_Edge->twin->vert);
edgeNext->vert->edge = edgeNext;
auto edgePrev = new HALF_EDGE::HE_Edge(pt);
edgePrev->vert->edge = edgePrev;

//Bind new edges
HALF_EDGE::HE_Edge* lastEdge = edgePrev;

edgeNext->setNext(edgePrev);


HALF_EDGE::HE_Edge* half_edge = start_Edge->next;
start_Edge->setNext(edgeNext);

do
{

	//Set vertices
	edgeNext = new HALF_EDGE::HE_Edge(half_edge->twin->vert);
	pt->edge = edgeNext;
	edgePrev = new HALF_EDGE::HE_Edge(pt);
	half_edge->twin->vert->edge = edgePrev;

	//Bind new edges
	edgePrev->setTwin(half_edge->prev->next);
	edgePrev->setNext(half_edge);
	edgeNext->setNext(edgePrev);
	//Next edge
	HALF_EDGE::HE_Edge* temp = half_edge;
	half_edge = half_edge->next;
	//Link the previous edge
	half_edge->prev->setNext(edgeNext);
} while (half_edge != start_Edge);

lastEdge->setNext(start_Edge);
edgeNext->setTwin(start_Edge->prev);

assert(start_Edge == start_Edge->next->next->next);
assert(start_Edge == start_Edge->prev->prev->prev);
assert(start_Edge == start_Edge->prev->next);
assert(start_Edge == start_Edge->next->prev);

start_Edge = pt->edge;
half_edge = start_Edge;
do
{
	//Collect edge
	HALF_EDGE::HE_Face* face = new HALF_EDGE::HE_Face();
	face->edge = half_edge;
	half_edge->face = face;
	half_edge->next->face = face;
	half_edge->prev->face = face;

	//if (HALF_EDGE::orientation(half_edge->vert, half_edge->next->vert, half_edge->prev->vert) == 2)
	//{

	//}


	generateCircumCircle(face);
	faces.push_back(std::make_pair(face, false));
	half_edge = half_edge->next->twin;
} while (half_edge != start_Edge);

#endif