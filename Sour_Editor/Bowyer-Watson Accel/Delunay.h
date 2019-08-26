#pragma once
#include "..\Data Structures/DCELHE.h"
constexpr double eps = 1e-4;
class Delunay
{
public:
	static void Triangulate(std::vector<HALF_EDGE::HE_Vertex*>& points, std::vector<HALF_EDGE::HE_Face*>& faces);
private:
	static bool equalEdge(HALF_EDGE::HE_Edge* e1, HALF_EDGE::HE_Edge* e2);
	static void exchangeEdges(HALF_EDGE::HE_Edge* e1, HALF_EDGE::HE_Edge* e2);
	static void generateCircumCircle(HALF_EDGE::HE_Face* face);
	static bool findIntersections(HALF_EDGE::HE_Vertex* pt, std::vector<HALF_EDGE::HE_Face*> faces, std::map<HALF_EDGE::HE_Face*, bool>& intersections, std::vector<HALF_EDGE::HE_Edge*>& edges);
	static void neighborIntersections(HALF_EDGE::HE_Vertex* pt, HALF_EDGE::HE_Edge * edge, std::map<HALF_EDGE::HE_Face*, bool>& intersections, std::vector<HALF_EDGE::HE_Edge*>& edges);
};

template<class Container, class F>
auto erase_where(Container& c, F&& f, int offset = 0)
{
	return c.erase(std::remove_if(c.begin()+offset,
		c.end(),
		std::forward<F>(f)),
		c.end());
}

