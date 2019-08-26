#include "Delunay.h"
#include "..\pch.h"
void Delunay::Triangulate(std::vector<HALF_EDGE::HE_Vertex*>& points, std::vector<Triangle>& triangles)
{
	sf::ContextSettings settings(0, 0, 16);
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Paused: 0", sf::Style::Default, settings);
	float zoom = 5;

	sf::View view = window.getDefaultView();
	view.setCenter(sf::Vector2f(500, 500));
	view.zoom(zoom);
	window.setView(view);
	bool pause = false;
	double timer = 0;

	HALF_EDGE::HE_Vertex* p1 = nullptr;
	HALF_EDGE::HE_Vertex* p2 = nullptr;
	HALF_EDGE::HE_Vertex* p3 = nullptr;

	if (triangles.empty())
	{
		// Determinate the super triangle
		float minX = points[0]->point->x;
		float minY = points[0]->point->y;
		float maxX = minX;
		float maxY = minY;

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

		p1 = new  HALF_EDGE::HE_Vertex(midx - 20 * deltaMax, midy - deltaMax);
		p2 = new  HALF_EDGE::HE_Vertex(midx, midy + 20 * deltaMax);
		p3 = new  HALF_EDGE::HE_Vertex(midx + 20 * deltaMax, midy - deltaMax);

		triangles.push_back({ p1, p2, p3 });
	}
		

	double circumCheckTime = 0;
	double dublicateEdgesTime = 0;
	double triangleCreationTime = 0;
	double pushBackTime = 0;
	for (HALF_EDGE::HE_Vertex* const pt : points)
	{
		std::vector<edge> edges;
			
		for (Triangle& t : triangles)
		{
			const auto dist = (t.circle.x - pt->point->x) * (t.circle.x - pt->point->x) +
				(t.circle.y - pt->point->y) * (t.circle.y - pt->point->y);
			if ((dist - t.circle.radius) <= eps)
			{
				//Pushing bad triangle
				t.isBad = true;
				for (int i = 0; i < 3; i++)
					edges.push_back(t.e[i]);		
			}
			else
			{
				t.isBad = false;
				//does not contain in its circumcenter
			}
		}
		auto is_bad = [&](auto const& tri) { return tri.isBad; };
		erase_where(triangles, is_bad);

		/* Delete duplicate edges. */
		std::vector<bool> remove(edges.size(), false);

		for (auto it1 = edges.begin(); it1 != edges.end(); ++it1) {
			for (auto it2 = edges.begin(); it2 != edges.end(); ++it2) {
				if (it1 == it2) {
					continue;
				}
				if (equalEdge(*it1, *it2)) {
					remove[std::distance(edges.begin(), it1)] = true;
					remove[std::distance(edges.begin(), it2)] = true;
				}
			}
		}
		auto is_duplicate = [&](auto const& e) { return remove[&e - &edges[0]]; };
		erase_where(edges, is_duplicate);

		/* Update triangulation. */
		for (const edge &e : edges)
		{
			triangles.push_back(Triangle(e.first, e.second, pt));
		}
#if 1
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
					break;
				}
			}
			if (!pause)
			{
				for (auto &triangle : triangles)
				{
					sf::CircleShape point;
					point.setRadius(triangle.circle.radius);
					point.setPosition(sf::Vector2f(triangle.circle.x, triangle.circle.y) - sf::Vector2f(triangle.circle.radius, triangle.circle.radius));
					point.setOutlineThickness(1.0f);
					point.setOutlineColor({ 255,0,255 });
					point.setFillColor(sf::Color::Transparent);
					window.draw(point);

					point = sf::CircleShape();
					point.setRadius(2);
					point.setPosition(sf::Vector2f(triangle.circle.x, triangle.circle.y) - sf::Vector2f(2, 2));
					point.setFillColor({ 0,0,255 });
					window.draw(point);
					sf::Vertex test[3];
					test[0].position = *triangle.v1->point;
					test[1].position = *triangle.v2->point;
					test[2].position = *triangle.v3->point;
					window.draw(test, 3, sf::LineStrip);
					point = sf::CircleShape();
					point.setRadius(2);
					point.setPosition(*triangle.v1->point- sf::Vector2f(2, 2));
					point.setFillColor({ 255,0,0 });
					window.draw(point);
					point = sf::CircleShape();
					point.setRadius(2);
					point.setPosition(*triangle.v2->point - sf::Vector2f(2, 2));
					point.setFillColor({ 255,0,0 });
					window.draw(point);
					point = sf::CircleShape();
					point.setRadius(2);
					point.setPosition(*triangle.v3->point - sf::Vector2f(2, 2));
					point.setFillColor({ 255,0,0 });
					window.draw(point);
				}
				window.display();
				std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
				timer += elapsed_seconds.count();
			}
			
		}
		timer = 0;
#endif
	}

	auto is_part_STriangle = [&](Triangle const& tri) {
		return (tri.v1->arrayIndex == -1 || tri.v2->arrayIndex == -1 || tri.v3->arrayIndex == -1); };

	erase_where(triangles, is_part_STriangle);

	delete p1;
	delete p2;
	delete p3;

	for (auto & t : triangles)
	{
		if (t.orientation() == 2)
			t.flipOrientation();
	}
#if 0
	sf::Event event;
	while (timer < 10)
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
					pause = !pause;
					window.setTitle("Paused: " + pause);
				}
				break;
			}
		}
		for (auto &triangle : triangles)
		{
			sf::CircleShape point;
			point.setRadius(triangle.circle.radius);
			point.setPosition(sf::Vector2f(triangle.circle.x, triangle.circle.y) - sf::Vector2f(triangle.circle.radius, triangle.circle.radius));
			point.setOutlineThickness(1.0f);
			point.setOutlineColor({ 255,0,255 });
			point.setFillColor(sf::Color::Transparent);
			window.draw(point);

			point = sf::CircleShape();
			point.setRadius(2);
			point.setPosition(sf::Vector2f(triangle.circle.x, triangle.circle.y) - sf::Vector2f(2, 2));
			point.setFillColor({ 0,0,255 });
			window.draw(point);
			sf::Vertex test[3];
			test[0].position = *triangle.v1->point;
			test[1].position = *triangle.v2->point;
			test[2].position = *triangle.v3->point;
			window.draw(test, 3, sf::LineStrip);
		}
		window.display();
		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
		timer += elapsed_seconds.count();
	}
	window.close();
#endif
}

bool Delunay::equalEdge(edge e1, edge e2)
{
	return ((e2.first == e1.first && e2.second == e1.second) ||
			(e2.first == e1.second && e2.second == e1.first));
}
