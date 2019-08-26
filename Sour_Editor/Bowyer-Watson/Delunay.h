#pragma once
#include "..\Data Structures/DCELHE.h"
constexpr double eps = 1e-4;

typedef std::pair< HALF_EDGE::HE_Vertex*, HALF_EDGE::HE_Vertex*> edge;
class Triangle;
class Delunay
{
public:
	static void Triangulate(std::vector<HALF_EDGE::HE_Vertex*>& points, std::vector<Triangle>& _triangles);
	static bool equalEdge(edge e1, edge e2);
private:
};

struct DCircle {
	float x, y, radius;
	DCircle() = default;
};

class Triangle
{
private:
	bool cmpf(float A, float B, float epsilon = 0.005f) const
	{
		return (fabs(A - B) < epsilon);
	}
	bool cmpfVec(const sf::Vector2f& A, const sf::Vector2f& B) const
	{
		return (cmpf(A.x, B.x) && cmpf(A.y, B.y));
	}
public:
	edge e[3];
	HALF_EDGE::HE_Vertex* v1;
	HALF_EDGE::HE_Vertex* v2;
	HALF_EDGE::HE_Vertex* v3;
	DCircle circle;
	bool isBad = false;
	int lul = 0;
	Triangle(HALF_EDGE::HE_Vertex* _v1, HALF_EDGE::HE_Vertex* _v2, HALF_EDGE::HE_Vertex* _v3)
	  : v1{ _v1 },
		v2{ _v2 },
		v3{ _v3 },
		e{ {_v2, _v1}, { _v3, _v2 }, { _v1, _v3 } },
		circle{}
	{
		/*if (this->orientation() == 2)
			this->flipOrientation();*/

		sf::Vector2f* p1 = v1->point;
		sf::Vector2f* p2 = v2->point;
		sf::Vector2f* p3 = v3->point;

		const float ax = p2->x - p1->x;
		const float ay = p2->y - p1->y;
		const float bx = p3->x - p1->x;
		const float by = p3->y - p1->y;

		const float m = p2->x * p2->x - p1->x * p1->x + p2->y * p2->y - p1->y * p1->y;
		const float u = p3->x * p3->x - p1->x * p1->x + p3->y * p3->y - p1->y * p1->y;
		const float s = 1. / (2. * (ax * by - ay * bx));

		this->circle.x = ((p3->y - p1->y) * m + (p1->y - p2->y) * u) * s;
		this->circle.y = ((p1->x - p3->x) * m + (p2->x - p1->x) * u) * s;

		const float dx = p1->x - this->circle.x;
		const float dy = p1->y - this->circle.y;
		this->circle.radius = dx * dx + dy * dy;
	}
	~Triangle()
	{
		 v1	= nullptr;
		 v2	= nullptr;
		 v3	= nullptr;
	}
	void operator=(const Triangle &t)
	{
		this->e[0] = t.e[0];
		this->e[1] = t.e[1];
		this->e[2] = t.e[2];

		this->v1 = t.v1;
		this->v2 = t.v2;
		this->v3 = t.v3;

		this->circle = t.circle;
	}
	bool operator==(const Triangle &t)
	{
		return (Delunay::equalEdge(this->e[0], t.e[0]) && Delunay::equalEdge(this->e[1], t.e[1]) && Delunay::equalEdge(this->e[2], t.e[2]));
	}
	sf::Vector2f CalculateCircleCenter()
	{
		sf::Vector2f center;
		sf::Vector2f* p1 = v1->point;
		sf::Vector2f* p2 = v2->point;
		sf::Vector2f* p3 = v3->point;

		float ma = (p2->y - p1->y) / (p2->x - p1->x);
		float mb = (p3->y - p2->y) / (p3->x - p2->x);

		center.x = (ma * mb * (p1->y - p3->y) + mb * (p1->x + p2->x) - ma * (p2->x + p3->x)) / (2 * (mb - ma));

		center.y = (-1 / ma) * (center.x - (p1->x + p2->x) / 2) + (p1->y + p2->y) / 2;

		return center;
	}
	int orientation()
	{
		int val = (v2->point->y - v1->point->y) * (v3->point->x - v2->point->x) -
			(v2->point->x - v1->point->x) * (v3->point->y - v2->point->y);

		if (val == 0) return 0;  // colinear 

		return (val > 0) ? 1 : 2; // clock or counterclock wise 
	}

	int edgeOrientation()
	{
		int val = 0;
		if (e[0].second == e[1].first && e[1].second == e[2].first && e[2].second == e[0].first)
		{
			val = 1;
		}
		else if (e[0].second == e[2].first && e[2].second == e[1].first && e[1].second == e[0].first)
		{
			val = 2;
		}

		return val;
	}

	float counterClockwiseResult()
	{
		float result = (v2->point->x - v1->point->x) * (v3->point->y - v1->point->y) -
			(v3->point->x - v1->point->x) * (v2->point->y - v1->point->y);
		return result;
	}
	void flipOrientation()
	{
		std::swap(v1, v2);
	}
	bool re_OrderEdges()
	{
		std::swap(v1, v2);

		e[0].first = v2;
		e[0].second = v1;

		e[1].first = v3;
		e[1].second = v2;

		e[2].first = v1;
		e[2].second = v3;

		return this->orientation() == 1;
	}

	bool IsCounterClockwise(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3)
	{
		float result = (point2.x - point1.x) * (point3.y - point1.y) -
			(point3.x - point1.x) * (point2.y - point1.y);
		return result > 0;
	}
};

template<class Container, class F>
auto erase_where(Container& c, F&& f, int offset = 0)
{
	return c.erase(std::remove_if(c.begin()+offset,
		c.end(),
		std::forward<F>(f)),
		c.end());
}


