#include "pch.h"
#include "EditorTools.h"


EditorTools::EditorTools()
{
	
	objectSelection.setOutlineColor(sf::Color::Yellow);
	objectSelection.setFillColor(sf::Color::Transparent);
}


EditorTools::~EditorTools()
{
}

void EditorTools::updateCurrentTool()
{
	if (cTool == Tool::SelectBox)
	{
		float offset = pow(10, grid.zoomLevel - 2) / 10;
		if (selectedObject != nullptr)
		{
			//auto float2Conversion = [](float2 value) { return sf::Vector2f(value[0], value[1]); };
			//sf::Vector2f lowerBound = float2Conversion(selectedObject->AABB_lowerBounds);
			//sf::Vector2f upperBound = float2Conversion(selectedObject->AABB_upperBounds);


			
			/*objectSelection.setPoint(0, lowerBound + sf::Vector2f(-offset / 5, -offset / 5));
			objectSelection.setPoint(1, sf::Vector2f(lowerBound.x, upperBound.y) + sf::Vector2f(-offset / 5, offset / 5));
			objectSelection.setPoint(2, upperBound + sf::Vector2f(offset / 5, offset / 5));
			objectSelection.setPoint(3, sf::Vector2f(upperBound.x, lowerBound.y) + sf::Vector2f(offset / 5, -offset / 5));*/
			objectSelection.setPosition(selectedObject->getPosition());
			objectSelection.setOutlineThickness(-offset / 5);
			//objectSelection.setScale(sf::Vector2f(1.05f, 1.05f));
			objectSelection.setRotation(selectedObject->getRotation());
		}
	}
}

void EditorTools::inspectSelectedObject()
{
	ImGuiIO& io = ImGui::GetIO();
}

void EditorTools::startDrag(sf::Vector2f pos)
{
	dragStart = pos;
}

sf::ConvexShape EditorTools::selectionBox(sf::Vector2f pos)
{
	float offset = pow(10, grid.zoomLevel-2) / 10;
	sf::ConvexShape shape;
	shape.setPointCount(4);
	shape.setPoint(0, this->dragStart);
	shape.setPoint(1, sf::Vector2f(pos.x, this->dragStart.y));
	shape.setPoint(2, pos);
	shape.setPoint(3, sf::Vector2f(this->dragStart.x, pos.y));
	shape.setOutlineThickness(-offset/5);
	shape.setOutlineColor(sf::Color::Red);
	shape.setFillColor(sf::Color::Transparent);
	return shape;
}

void EditorTools::createObjectSelectedBorder()
{
	float offset = pow(10, grid.zoomLevel - 2) / 10;
	objectSelection.setPointCount(selectedObject->m_vertices.size());
	int i = 0;
	for (auto &vertex : selectedObject->m_vertices)
	{
		objectSelection.setPoint(i, vertex.position);
		i++;
	}
}

void EditorTools::addBox(b2World& world, std::vector<Polygon>& _boxes, sf::Vector2f pos)
{
	if (pos.x != this->dragStart.x && pos.y != this->dragStart.y)
	{
		Polygon shape(world);
		sf::Vertex vertices[6];
		vertices[0].position = this->dragStart;
		vertices[0].color = sf::Color::Cyan;
		vertices[1].position = sf::Vector2f(pos.x, this->dragStart.y);
		vertices[1].color = sf::Color::Cyan;
		vertices[2].position = pos;
		vertices[2].color = sf::Color::Cyan;
		vertices[3].position = pos;
		vertices[3].color = sf::Color::Cyan;
		vertices[4].position = sf::Vector2f(this->dragStart.x, pos.y);
		vertices[4].color = sf::Color::Cyan;
		vertices[5].position = this->dragStart;
		vertices[5].color = sf::Color::Green;
		for (auto& vertex : vertices)
			shape.m_vertices.push_back(vertex);
		shape.AABB_lowerBounds = { this->dragStart.x,this->dragStart.y };
		shape.AABB_upperBounds = { pos.x, pos.y };
		shape.createPhysicsBody();

		_boxes.push_back(shape);
	}	
}

void Grid::addGridLevel(int level)
{

}

Grid::Grid()
{
}

Grid::Grid(const sf::View& view)
{
	createGrid();
}

void Grid::Init(const sf::View & view)
{
	createGrid();
}

Grid::~Grid()
{
	m_vertices.clear();
}

void Grid::updateGrid(const sf::View & view)
{
	float offset = pow(10, zoomLevel) / 10;
	sf::Vector2f pos(view.getCenter().x / offset, view.getCenter().y / offset);
	this->setPosition(round(pos.x)*offset-(offset*6), round(pos.y)*offset - (offset * 6));

	if (view.getSize().x >= maxLevelThreshold)
	{
		zoomLevel++;
		m_vertices.clear();
		minLevelThreshold = maxLevelThreshold;
		maxLevelThreshold = pow(10, zoomLevel);
		createGrid();
	}

	else if (view.getSize().x < minLevelThreshold)
	{
		zoomLevel--;
		m_vertices.clear();
		maxLevelThreshold = minLevelThreshold;
		minLevelThreshold = pow(10, zoomLevel-1);
		createGrid();
	}
	for (int i = 0; i < 480; i++)
	{
		m_vertices[i].color.a = std::max(5.0f, 150 - (view.getSize().x / maxLevelThreshold) * 140);
	}
	//for (int i = 220; i < 440; i++)
	//{
	//	m_vertices[i].color.a = std::max(5.0f, 150 - (view.getSize().x / maxLevelThreshold) * 150);
	//}
}

void Grid::createGrid()
{
	float offset = pow(10,zoomLevel-1) / 10;
	for (int i = 0; i < 120; i++)
	{
		sf::Vertex v1;
		sf::Vertex v2;
		v1.color = sf::Color(255, 255, 255, 100);
		v2.color = sf::Color(255, 255, 255, 100);
		//Horizontal
		v1.position.x = i * offset;
		v1.position.y = -MAX_GRID_OFFSET;
		v2.position.x = i * offset;
		v2.position.y = MAX_GRID_OFFSET;
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);

		//Vertical
		v1.position.x = -MAX_GRID_OFFSET;
		v1.position.y = i * offset;
		v2.position.x = MAX_GRID_OFFSET;
		v2.position.y = i * offset;
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);
	}

	offset = pow(10, zoomLevel) / 10;
	for (int i = 0; i < 120; i++)
	{
		sf::Vertex v1;
		sf::Vertex v2;
		v1.color = sf::Color(255, 255, 255, 180);
		v2.color = sf::Color(255, 255, 255, 180);
		//Horizontal
		v1.position.x = i * offset;
		v1.position.y = -MAX_GRID_OFFSET;
		v2.position.x = i * offset;
		v2.position.y = MAX_GRID_OFFSET;
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);

		//Vertical
		v1.position.x = -MAX_GRID_OFFSET;
		v1.position.y = i * offset;
		v2.position.x = MAX_GRID_OFFSET;
		v2.position.y = i * offset;
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);
	}

	offset = pow(10, zoomLevel + 1) / 10;
	for (int i = 0; i < 120; i++)
	{
		sf::Vertex v1;
		sf::Vertex v2;
		v1.color = sf::Color(255, 255, 255, 255);
		v2.color = sf::Color(255, 255, 255, 255);
		//Horizontal
		v1.position.x = i * offset;
		v1.position.y = -MAX_GRID_OFFSET;
		v2.position.x = i * offset;
		v2.position.y = MAX_GRID_OFFSET;
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);

		//Vertical
		v1.position.x = -MAX_GRID_OFFSET;
		v1.position.y = i * offset;
		v2.position.x = MAX_GRID_OFFSET;
		v2.position.y = i * offset;
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);
	}
}
