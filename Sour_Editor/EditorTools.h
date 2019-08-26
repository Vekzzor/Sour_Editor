#pragma once
#include "Polygon.h"

class Grid : public sf::Drawable, public sf::Transformable
{
public:
	unsigned int zoomLevel = 3;
	unsigned int maxLevelThreshold = pow(10, zoomLevel);
	unsigned int minLevelThreshold = pow(10, zoomLevel - 1);
	std::vector<sf::Vertex> m_vertices;
	sf::PrimitiveType m_type = sf::Lines; //10x10 grids with 8 levels ---------- show two (three?) levels at a time
	void addGridLevel(int level);
	Grid();
	Grid(const sf::View& view);
	void Init(const sf::View& view);
	~Grid();
	void updateGrid(const sf::View& view);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the entity's transform -- combine it with the one that was passed by the caller
		states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

		// apply the texture
		//states.texture = &m_texture;
		// you may also override states.shader or states.blendMode if you want
		// draw the vertex array
		target.draw(m_vertices.data(), m_vertices.size(), m_type, states);
	}
	void createGrid();
	sf::Transform m_transformation;
	sf::Texture m_texture;
	const int MAX_GRID_OFFSET = 9000000;
};

enum Tool { SelectBox, AddBox };
class EditorTools
{
private:
	Tool cTool = AddBox;
	sf::Vector2f dragStart;
	sf::Vector2f dragEnd;
	sf::ConvexShape objectSelection;
	Polygon* selectedObject = nullptr;
public:
	EditorTools();
	~EditorTools();
	void updateCurrentTool();
	const Tool currentTool() { return this->cTool; };
	void currentTool(Tool tool) { this->cTool = tool; };
	void inspectSelectedObject();
	/*void createGrid();
	void updateGrid();
	void changeGridPrecision();*/
	void startDrag(sf::Vector2f pos);
	sf::Vector2f getDragStart() { return this->dragStart; };
	sf::ConvexShape selectionBox(sf::Vector2f pos);
	void createObjectSelectedBorder();
	sf::ConvexShape& getSelectedObjectBorder() { return this->objectSelection; };
	void setSelectedObject(Polygon* poly) { this->selectedObject = poly; };
	Polygon* getSelectedObject() { return this->selectedObject; };
	void addBox(b2World& world, std::vector<Polygon>& _boxes, sf::Vector2f pos);
	Grid grid;
	
};

