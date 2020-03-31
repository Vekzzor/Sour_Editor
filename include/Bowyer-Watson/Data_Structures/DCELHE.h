#pragma once
#include <SFML/Graphics.hpp>

namespace HALF_EDGE
{
	static int RefCount = 0;
	struct HE_Edge;
	//struct Site;
	//Site
	struct HE_Vertex 
	{
		HE_Vertex(int x, int y, int index = -1);
		HE_Vertex(sf::Vector2f* vector, int index = -1);
		HE_Vertex(sf::Vector2f vector, int index = -1);
		~HE_Vertex();
		sf::Vector2f getCoordinates();
		bool operator==(const HE_Vertex &other)const;
		HE_Edge *edge = nullptr;  /* rep->tail == this */
		sf::Vector2f* point = nullptr;
		int arrayIndex = -1;
	};

	struct HE_Face 
	{
		HE_Face();
		~HE_Face();
		HE_Edge *edge = nullptr;  /* rep->left == this */
		sf::Vector2f circumCenter;
		float radius;
	};

	struct HE_Edge
	{
		HE_Edge();
		HE_Edge(HE_Vertex* _vert, HE_Face* _face = nullptr) : vert{ _vert }, face{ _face } {this->refCount = ++RefCount; };
		~HE_Edge();
		void setTwin(HE_Edge* newTwin);
		void setNext(HE_Edge* newNext);
		void setPrev(HE_Edge* newPrev);
		void clearEdge();
		HE_Edge *next = nullptr;  /* next->prev == this */
		HE_Edge *prev = nullptr;  /* prev->next == this */
		HE_Edge *twin = nullptr;  /* twin->twin == this */
		HE_Face *face = nullptr;  /* prev->left == left && next->left == left */
		HE_Vertex* vert = nullptr;
		int refCount = -1;
	};

	//Traversing the edges around a face
	_inline void getFaceEdges(std::vector<HE_Edge*>& edgeList,HE_Face* face)
	{
		HE_Edge* start_Edge = face->edge;
		HE_Edge* half_edge = start_Edge;
		do
		{
			//Collect edge;
			edgeList.push_back(half_edge);
			half_edge = half_edge->next;
		} while (half_edge != start_Edge);

	}

	_inline void getFaceVertices(std::vector<HE_Vertex*>& vertexList, HE_Face* face)
	{
		HE_Edge* start_Edge = face->edge;
		HE_Edge* half_edge = start_Edge;
		do
		{
			//Collect vertex;
			vertexList.push_back(half_edge->vert);
			half_edge = half_edge->next;
		} while (half_edge != start_Edge);
	}

	_inline	bool getPolygon(std::vector<sf::Vector2f>& siteList, HE_Vertex* vertex)
	{
		bool hasNeighbor = true;
		HE_Edge* start_Edge = vertex->edge;
		HE_Edge* half_edge = start_Edge;
		do
		{
			if (half_edge->twin == nullptr)
			{
				hasNeighbor = false;
				break;
			}
			//Collect edge
			siteList.push_back(half_edge->face->circumCenter);
			half_edge = half_edge->twin->next;
		} while (half_edge != start_Edge);
		return hasNeighbor;
	}

	//Traversing the edges around a vertex
	_inline void getVertexEdges(HE_Vertex* vertex)
	{
		HE_Edge* start_Edge = vertex->edge;
		HE_Edge* half_edge = start_Edge;
		do
		{
			//Collect edge
			half_edge = half_edge->twin->next;
		} while (half_edge != start_Edge);
	}

	_inline void getVertexFaces(HE_Vertex* vertex)
	{
		HE_Edge* start_Edge = vertex->edge;
		HE_Edge* half_edge = start_Edge;
		do
		{
			//Collect edge
			half_edge = half_edge->twin->next;
		} while (half_edge != start_Edge);
	}

	_inline HE_Vertex* vertexNext(HE_Edge* half_edge)
	{
		return half_edge->twin->next->vert;
	}
	_inline HE_Vertex* vertexPrev(HE_Edge* half_edge)
	{
		return half_edge->prev->twin->vert;
	}

	_inline int orientation(HE_Vertex* v1, HE_Vertex* v2, HE_Vertex* v3)
	{
		int val = (v2->point->y - v1->point->y) * (v3->point->x - v2->point->x) -
			(v2->point->x - v1->point->x) * (v3->point->y - v2->point->y);

		if (val == 0) return 0;  // colinear 

		return (val > 0) ? 1 : 2; // clock or counterclock wise 
	}
}