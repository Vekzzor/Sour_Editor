#include "EditorTools.h"
#include "pch.h"

EditorTools::EditorTools()
{
	object_selection_.setOutlineColor(sf::Color::Yellow);
	object_selection_.setFillColor(sf::Color::Transparent);
}

EditorTools::~EditorTools() {}

void EditorTools::UpdateCurrentTool()
{
	if(current_tool_ == Tool::kSelectBox)
	{
		float offset = pow(10, grid_.zoom_level_ - 2) / 10;
		if(selected_entity_ != -1)
		{
			Coordinator& coordinator = gCoordinator.GetInstance();
			Mesh2D& poly	= coordinator.GetComponent<Mesh2D>(selected_entity_);
			sf::Transformable& transform =
				coordinator.GetComponent<sf::Transformable>(selected_entity_);
			object_selection_.setPosition(transform.getPosition());
			object_selection_.setOutlineThickness(-offset / 5);
			object_selection_.setRotation(transform.getRotation());
		}
	}
}

void EditorTools::StartDrag(sf::Vector2f _pos)
{
	drag_start_ = _pos;
}

sf::ConvexShape EditorTools::SelectionBox(sf::Vector2f _pos)
{
	float offset = pow(10, grid_.zoom_level_ - 2) / 10;
	sf::ConvexShape shape;
	shape.setPointCount(4);
	shape.setPoint(0, this->drag_start_);
	shape.setPoint(1, sf::Vector2f(_pos.x, this->drag_start_.y));
	shape.setPoint(2, _pos);
	shape.setPoint(3, sf::Vector2f(this->drag_start_.x, _pos.y));
	shape.setOutlineThickness(-offset / 5);
	shape.setOutlineColor(sf::Color::Red);
	shape.setFillColor(sf::Color::Transparent);
	return shape;
}

void EditorTools::CreateObjectSelectedBorder()
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	if (coordinator.HasComponent<Mesh2D>(selected_entity_))
	{
		Mesh2D& poly = coordinator.GetComponent<Mesh2D>(selected_entity_);
		float offset = pow(10, grid_.zoom_level_ - 2) / 10;
		object_selection_.setPointCount(poly.vertices.size());
		for(int i = 0; i < poly.vertices.size(); i++)
		{
			object_selection_.setPoint(i, poly.vertices[i].position);
		}
	}
}

void EditorTools::AddBox(b2World& _world,
						 PhysicsSystem _system,
						 std::vector<Mesh2D>& _boxes,
						 sf::Vector2f _pos)
{
	if(_pos.x != this->drag_start_.x && _pos.y != this->drag_start_.y)
	{
		Mesh2D shape;
		sf::Vertex vertices[6];
		vertices[0].position = this->drag_start_;
		vertices[0].color	= sf::Color::Cyan;
		vertices[1].position = sf::Vector2f(_pos.x, this->drag_start_.y);
		vertices[1].color	= sf::Color::Cyan;
		vertices[2].position = _pos;
		vertices[2].color	= sf::Color::Cyan;
		vertices[3].position = _pos;
		vertices[3].color	= sf::Color::Cyan;
		vertices[4].position = sf::Vector2f(this->drag_start_.x, _pos.y);
		vertices[4].color	= sf::Color::Cyan;
		vertices[5].position = this->drag_start_;
		vertices[5].color	= sf::Color::Green;
		for(auto& vertex : vertices)
			shape.vertices.push_back(vertex);
		//_system.CreatePhysicsBodyFromPolygon(world, shape);
		_boxes.push_back(shape);
	}
}

void Grid::AddGridLevel(int _level) {}

Grid::Grid() {}

Grid::Grid(const sf::View& _view)
{
	CreateGrid();
}

void Grid::Init(const sf::View& _view)
{
	CreateGrid();
}

Grid::~Grid()
{
	vertices_.clear();
}

void Grid::UpdateGrid(const sf::View& _view)
{
	float offset = pow(10, zoom_level_) / 10;
	sf::Vector2f pos(_view.getCenter().x / offset, _view.getCenter().y / offset);
	this->setPosition(round(pos.x) * offset - (offset * 6), round(pos.y) * offset - (offset * 6));

	if(_view.getSize().x >= max_level_threshold_)
	{
		zoom_level_++;
		vertices_.clear();
		min_level_threshold_ = max_level_threshold_;
		max_level_threshold_ = pow(10, zoom_level_);
		CreateGrid();
	}
	else if(_view.getSize().x < min_level_threshold_)
	{
		zoom_level_--;
		vertices_.clear();
		max_level_threshold_ = min_level_threshold_;
		min_level_threshold_ = pow(10, zoom_level_ - 1);
		CreateGrid();
	}
	for(int i = 0; i < 480; i++)
	{
		vertices_[i].color.a = std::max(5.0f, 150 - (_view.getSize().x / max_level_threshold_) * 140);
	}
}

void Grid::CreateGrid()
{
	float offset = pow(10, zoom_level_ - 1) / 10;
	for(int i = 0; i < 120; i++)
	{
		sf::Vertex v1;
		sf::Vertex v2;
		v1.color = sf::Color(255, 255, 255, 100);
		v2.color = sf::Color(255, 255, 255, 100);
		//Horizontal
		v1.position.x = i * offset;
		v1.position.y = -kMaxGridOffset_;
		v2.position.x = i * offset;
		v2.position.y = kMaxGridOffset_;
		vertices_.push_back(v1);
		vertices_.push_back(v2);

		//Vertical
		v1.position.x = -kMaxGridOffset_;
		v1.position.y = i * offset;
		v2.position.x = kMaxGridOffset_;
		v2.position.y = i * offset;
		vertices_.push_back(v1);
		vertices_.push_back(v2);
	}

	offset = pow(10, zoom_level_) / 10;
	for(int i = 0; i < 120; i++)
	{
		sf::Vertex v1;
		sf::Vertex v2;
		v1.color = sf::Color(255, 255, 255, 180);
		v2.color = sf::Color(255, 255, 255, 180);
		//Horizontal
		v1.position.x = i * offset;
		v1.position.y = -kMaxGridOffset_;
		v2.position.x = i * offset;
		v2.position.y = kMaxGridOffset_;
		vertices_.push_back(v1);
		vertices_.push_back(v2);

		//Vertical
		v1.position.x = -kMaxGridOffset_;
		v1.position.y = i * offset;
		v2.position.x = kMaxGridOffset_;
		v2.position.y = i * offset;
		vertices_.push_back(v1);
		vertices_.push_back(v2);
	}

	offset = pow(10, zoom_level_ + 1) / 10;
	for(int i = 0; i < 120; i++)
	{
		sf::Vertex v1;
		sf::Vertex v2;
		v1.color = sf::Color(255, 255, 255, 255);
		v2.color = sf::Color(255, 255, 255, 255);
		//Horizontal
		v1.position.x = i * offset;
		v1.position.y = -kMaxGridOffset_;
		v2.position.x = i * offset;
		v2.position.y = kMaxGridOffset_;
		vertices_.push_back(v1);
		vertices_.push_back(v2);

		//Vertical
		v1.position.x = -kMaxGridOffset_;
		v1.position.y = i * offset;
		v2.position.x = kMaxGridOffset_;
		v2.position.y = i * offset;
		vertices_.push_back(v1);
		vertices_.push_back(v2);
	}
}
