#include "DCELHE.h"
using namespace HALF_EDGE;

HE_Vertex::HE_Vertex(int x, int y, int index)
{
	point = new sf::Vector2f(x, y);
	arrayIndex = index;
}
HE_Vertex::HE_Vertex(sf::Vector2f* vector, int index)
{
	point = vector;
	arrayIndex = index;
}
HE_Vertex::HE_Vertex(sf::Vector2f vector, int index)
{
	point = new sf::Vector2f(vector);
	arrayIndex = index;
}
HE_Vertex::~HE_Vertex()
{
	delete point;
}
sf::Vector2f HE_Vertex::getCoordinates()
{
	return sf::Vector2f(*point);
}

bool HALF_EDGE::HE_Vertex::operator==(const HE_Vertex & other) const
{
	return (*other.point == *point);
}


HE_Face::HE_Face() {}
HALF_EDGE::HE_Face::~HE_Face()
{
	/*if (this->edge)
		this->edge->face = nullptr;
	this->edge = nullptr;*/
};

HE_Edge::HE_Edge() { this->refCount = ++RefCount; }
HALF_EDGE::HE_Edge::~HE_Edge()
{
	//clearEdge();
}
void HE_Edge::setTwin(HE_Edge* newTwin) {
	this->twin = newTwin;
	newTwin->twin = this;
};
void HE_Edge::setNext(HE_Edge* newNext) {
	this->next = newNext;
	newNext->prev = this;
};
void HE_Edge::setPrev(HE_Edge* newPrev) {
	this->prev = newPrev;
	newPrev->next = this;
}
void HALF_EDGE::HE_Edge::clearEdge()
{
	if(this->next)
		this->next->prev = nullptr;
	if(this->prev)
		this->prev->next = nullptr;
	if (this->twin)
		this->twin->twin = nullptr;
	this->next = nullptr;
	this->prev = nullptr;
	this->twin = nullptr;
};