#pragma once
#include "ECS/ECS.h"
#include "PhysicsSystem.h"
class Grid : public sf::Drawable, public sf::Transformable
{
public:
	unsigned int zoom_level_ = 3;
	unsigned int max_level_threshold_ = pow(10, zoom_level_);
	unsigned int min_level_threshold_ = pow(10, zoom_level_ - 1);
	std::vector<sf::Vertex> vertices_;
	sf::PrimitiveType primitive_type_ =
		sf::Lines; //10x10 grids with 8 levels ---------- show two (three?) levels at a time
	void AddGridLevel(int _level);
	Grid();
	Grid(const sf::View& _view);
	void Init(const sf::View& _view);
	~Grid();
	void UpdateGrid(const sf::View& _view);

private:
	virtual void draw(sf::RenderTarget& _target, sf::RenderStates _states) const
	{
		_states.transform *= getTransform();
		_target.draw(vertices_.data(), vertices_.size(), primitive_type_, _states);
	}
	void CreateGrid();
	sf::Transform transformation_;
	sf::Texture texture_;
	const int kMaxGridOffset_ = 9000000;
};

enum class Tool
{
	kSelectBox,
	kAddBox,
	kTransformGizmo,
	kVoronoiFracture,
};

enum class GizmoMode
{
	kNone,
	kTranslate,
	kRotate,
	kScale
};

enum class Axis
{
	kNone, kX, kY, kBoth
};

enum class GizmoHandleAnchor
{
	kNone, 
	kTop, 
	kBottom, 
	kLeft, 
	kRight, 
	kCenter
};

///////////////TRANSFORMATION GIZMOS///////////////

struct TranslateGizmoShape
{
	GizmoMode gizmo_type = GizmoMode::kTranslate;
	Axis axis = Axis::kNone;
	sf::Vector2f half_length;
};

struct ScaleGizmoShape
{
	sf::ConvexShape gizmo_shape;
	GizmoMode gizmo_type = GizmoMode::kScale;
	GizmoHandleAnchor anchor_type = GizmoHandleAnchor::kNone;
	sf::Vector2f half_length;
};

//Move the scene
class HandTool
{

};
//Move GameObjects
class TranslateTool
{
public:

	static Axis AxisIsPressed(Entity _entity, sf::Vector2f _size, sf::Vector2f _MousePos, bool _held)
	{
		Coordinator& coordinator = gCoordinator.GetInstance();
		auto& transform = coordinator.GetComponent<sf::Transformable>(_entity);
		Axis result = Axis::kNone;

		if (_held)
		{
			float scale = 0.000005*_size.x;
			float width = 70 * scale;
			float height = 10000 * scale;
			float offset = 80 * scale;

			sf::Vector2f center = transform.getPosition();
			if (offset + fabs(_MousePos.y - center.y) < width * 5 && offset + fabs(_MousePos.x - center.x - height / 2) < height / 2)
				result = Axis::kX;
			else if (offset + fabs(_MousePos.x - center.x) < width * 5 && offset + fabs(_MousePos.y - center.y + height / 2) < height / 2)
				result = Axis::kY;
		}
		return result;
	}

	static void GizmoManipulate(sf::RenderWindow& _window, Entity _entity, sf::Vector2f _size, sf::Vector2f _MousePos, sf::Vector2f _DeltaMousePos, Axis _AxisHeld)
	{
		Coordinator& coordinator = gCoordinator.GetInstance();
		auto& transform = coordinator.GetComponent<sf::Transformable>(_entity);
		auto& select = coordinator.GetComponent<Selectable>(_entity);

		sf::RenderStates states;
		states.transform *= transform.getTransform();
		sf::ConvexShape rect;
		rect.setOutlineColor(sf::Color::Yellow);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(40.0);
		rect.setPointCount(4);

		sf::Vector2f pos(select.aabb_lower_bounds[0], select.aabb_lower_bounds[1]);
		rect.setPoint(0, pos);
		pos = sf::Vector2f(select.aabb_lower_bounds[0], select.aabb_upper_bounds[1]);
		rect.setPoint(1, pos);
		pos = sf::Vector2f(select.aabb_upper_bounds[0], select.aabb_upper_bounds[1]);
		rect.setPoint(2, pos);
		pos = sf::Vector2f(select.aabb_upper_bounds[0], select.aabb_lower_bounds[1]);
		rect.setPoint(3, pos);
		_window.draw(rect, states);

		sf::Vector2f center = transform.getPosition();
		rect.setOutlineThickness(0.0);
		rect.setPosition(transform.getPosition());
		float scale = _size.x*0.000005;
		float width = 70 * scale;
		float height = 10000 * scale;
		float offset = 80 * scale;
		//rect.setScale(scale, scale);

		//X-AXIS
		rect.setFillColor(sf::Color::Red);
		pos = sf::Vector2f(offset, width);
		rect.setPoint(0, pos);
		pos = sf::Vector2f(offset, -width);
		rect.setPoint(1, pos);
		pos = sf::Vector2f(height, -width);
		rect.setPoint(2, pos);
		pos = sf::Vector2f(height, width);
		rect.setPoint(3, pos);

		if (offset + fabs(_MousePos.y - center.y) < width * 5 && offset + fabs(_MousePos.x - center.x - height / 2) < (height / 2))
			rect.setFillColor(sf::Color::Yellow);
		_window.draw(rect);

		//Y-AXIS
		rect.setFillColor(sf::Color::Green);
		pos = sf::Vector2f(width, -offset);
		rect.setPoint(0, pos);
		pos = sf::Vector2f(-width, -offset);
		rect.setPoint(1, pos);
		pos = sf::Vector2f(-width, -height);
		rect.setPoint(2, pos);
		pos = sf::Vector2f(width, -height);
		rect.setPoint(3, pos);
		if (offset + fabs(_MousePos.x - center.x) < width * 5 && offset + fabs(_MousePos.y - center.y + height / 2) < (height / 2))
			rect.setFillColor(sf::Color::Yellow);
		_window.draw(rect);

		if (_AxisHeld == Axis::kX)
			transform.move(sf::Vector2f(_DeltaMousePos.x - _MousePos.x, 0));
		else if (_AxisHeld == Axis::kY)
			transform.move(sf::Vector2f(0, _DeltaMousePos.y - _MousePos.y));
	}
};
//Rotate GameObjects
class RotateTool
{

};
//Scale GameObjects
class ScaleTool
{

};
//Move/Rotate/Scale 2D Objects <- Only use this tool for SFML?
class RectTool
{
	TranslateGizmoShape translate_handle_;
	std::array<ScaleGizmoShape,4> scale_handle_;

	std::vector<int> active_indices_;
public:
	RectTool()
	{

	}
	void CreateHandles(Entity _entity, sf::Vector2f _size)
	{
		Coordinator& coordinator = gCoordinator.GetInstance();
		auto& transform = coordinator.GetComponent<sf::Transformable>(_entity);
		auto& select = coordinator.GetComponent<Selectable>(_entity);

		sf::Vector2f center= transform.getOrigin();
		
		sf::Vector2f uB(select.aabb_upper_bounds[0], select.aabb_upper_bounds[1]);
		sf::Vector2f lB(select.aabb_lower_bounds[0], select.aabb_lower_bounds[1]);
		uB = transform.getTransform()*uB;
		lB = transform.getTransform()*lB;

		float halfwidth = ((select.aabb_upper_bounds[0] - select.aabb_lower_bounds[0]) / 2);
		float halfheight = ((select.aabb_upper_bounds[1] - select.aabb_lower_bounds[1]) / 2);

		float viewScaleX = 0.0005/transform.getScale().x;
		float viewScaleY = 0.0005/transform.getScale().y;
		//ImGui::SliderFloat("Test", &viewScale, 0.00000f, 0.01000f, "%.6f");
		for (int i = 0; i < 4; i++)
		{
			float rectHW = _size.x*viewScaleX* (1 - (i % 2)) + halfwidth * (i % 2); //Either 500 or halfwidth
			float rectHH = _size.x*viewScaleY* (i%2) + halfheight * (1 - (i % 2)); //Either 500 or halfheight
			scale_handle_[i].gizmo_shape.setFillColor(sf::Color::Red);
			scale_handle_[i].gizmo_shape.setPointCount(4);
			scale_handle_[i].half_length = sf::Vector2f(rectHW, rectHH);
			scale_handle_[i].gizmo_shape.setPoint(0, sf::Vector2f(-rectHW, rectHH));
			scale_handle_[i].gizmo_shape.setPoint(1, sf::Vector2f(-rectHW, -rectHH));
			scale_handle_[i].gizmo_shape.setPoint(2, sf::Vector2f(rectHW, -rectHH));
			scale_handle_[i].gizmo_shape.setPoint(3, sf::Vector2f(rectHW, rectHH));
		}
		scale_handle_[0].gizmo_shape.setPosition(sf::Vector2f(center.x + halfwidth, center.y));
		scale_handle_[1].gizmo_shape.setPosition(sf::Vector2f(center.x, center.y + halfheight));
		scale_handle_[2].gizmo_shape.setPosition(sf::Vector2f(center.x - halfwidth, center.y));
		scale_handle_[3].gizmo_shape.setPosition(sf::Vector2f(center.x, center.y - halfheight));
		scale_handle_[0].anchor_type = GizmoHandleAnchor::kRight;
		scale_handle_[1].anchor_type = GizmoHandleAnchor::kBottom;
		scale_handle_[2].anchor_type = GizmoHandleAnchor::kLeft;
		scale_handle_[3].anchor_type = GizmoHandleAnchor::kTop;

	}
	void HandleIsHeld(Entity _entity, sf::Vector2f _size, sf::Vector2f _MousePos, bool _held)
	{
		if (_held)
		{
			Coordinator& coordinator = gCoordinator.GetInstance();
			auto& transform = coordinator.GetComponent<sf::Transformable>(_entity);
			auto& select = coordinator.GetComponent<Selectable>(_entity);

			sf::Vector2f center = transform.getOrigin();
			sf::Vector2f localMPos = transform.getInverseTransform()*_MousePos;
			for (int i = 0; i < 4; i++)
			{
				const sf::Vector2f rectC = scale_handle_[i].gizmo_shape.getPosition();
				const sf::Vector2f halfLengths = scale_handle_[i].half_length;
				sf::Vector2f lengthToCenter = localMPos - rectC;
				if (fabs(lengthToCenter.x) < halfLengths.x && fabs(lengthToCenter.y) < halfLengths.y)
					active_indices_.push_back(i);
			}
		}	
	}

	void HandleReleased()
	{
		active_indices_.clear();
	}

	void GizmoManipulate(sf::RenderWindow& _window, Entity _entity, sf::Vector2f _size, sf::Vector2f _MousePos, sf::Vector2f _OldMousePos, bool _held)
	{
		Coordinator& coordinator = gCoordinator.GetInstance();
		auto& transform = coordinator.GetComponent<sf::Transformable>(_entity);
		auto& select = coordinator.GetComponent<Selectable>(_entity);

		sf::Vector2f center = transform.getOrigin();
		sf::Vector2f localMPos = transform.getInverseTransform()*_MousePos;
		sf::Vector2f localOMPos = transform.getInverseTransform()*_OldMousePos;

		float scale = _size.x*0.000005;
		float halfwidth = (select.aabb_upper_bounds[0] - select.aabb_lower_bounds[0])/2;
		float halfheight = (select.aabb_upper_bounds[1] - select.aabb_lower_bounds[1])/2;
		float offset = 80 * scale;

		sf::Vector2f lengthToCenter = localMPos - center;
		if (_held && active_indices_.empty() && fabs(lengthToCenter.x) < halfwidth && fabs(lengthToCenter.y) < halfheight)
			transform.move(_OldMousePos - _MousePos);

		for (auto index : active_indices_)
		{
			sf::Vector2f localDelta((localOMPos - center) - (localMPos - center));
			sf::Vector2f scaleDelta(1,1);
			sf::Vector2f newPos(0,0);
			
			
			sf::Vector2f length;
			switch (scale_handle_[index].anchor_type)
			{
			case GizmoHandleAnchor::kRight:
				scaleDelta.x += localDelta.x / halfwidth;
				length.x = (halfwidth * transform.getScale().x*(scaleDelta.x)) - (halfwidth* transform.getScale().x);
				break;
			case GizmoHandleAnchor::kLeft:
				scaleDelta.x += -localDelta.x / halfwidth;
				length.x =  (halfwidth* transform.getScale().x) - (halfwidth * transform.getScale().x*(scaleDelta.x));
				break;
			case GizmoHandleAnchor::kTop:
				scaleDelta.y += -localDelta.y / halfheight;
				length.y = (halfheight* transform.getScale().y) - (halfheight * transform.getScale().y*(scaleDelta.y));
				break;
			case GizmoHandleAnchor::kBottom:
				scaleDelta.y += localDelta.y / halfheight;
				length.y = (halfheight * transform.getScale().y*(scaleDelta.y)) - (halfheight* transform.getScale().y);
				break; 
			}

			float rot = transform.getRotation() * (b2_pi / 180);
			newPos.x = (cosf(rot) * length.x) - (sinf(rot) * length.y);
			newPos.y = (sinf(rot) * length.x) + (cosf(rot) * length.y);

			transform.move(newPos);
			transform.scale(scaleDelta);
		}

		for (auto& shape : scale_handle_)
		{ 
			lengthToCenter = localMPos-shape.gizmo_shape.getPosition();
			if (fabs(lengthToCenter.x) < shape.half_length.x && fabs(lengthToCenter.y) < shape.half_length.y)
				shape.gizmo_shape.setFillColor(sf::Color::Yellow);
			else
				shape.gizmo_shape.setFillColor(sf::Color::Red);

			_window.draw(shape.gizmo_shape, transform.getTransform());
		}
	}
};

//Move ViewPort ex: while right mouse button held, use WASD to move viewport
class ViewMoveTool
{

};
//Zoom ViewPort ex: hold alt, move mouse up or down to zoom
class ViewZoomTool
{

};

class EditorTools
{
private:
	sf::Vector2f drag_start_;
	sf::Vector2f drag_end_;
	sf::ConvexShape object_selection_;

public:
	EditorTools();
	~EditorTools();
	void UpdateCurrentTool();

	void StartDrag(sf::Vector2f _pos);
	sf::Vector2f GetDragStart()
	{
		return this->drag_start_;
	};
	sf::ConvexShape SelectionBox(sf::Vector2f _pos);
	void CreateObjectSelectedBorder();
	sf::ConvexShape& GetSelectedObjectBorder()
	{
		return this->object_selection_;
	};
	void AddBox(b2World& _world, PhysicsSystem _system, std::vector<Mesh2D>& _boxes, sf::Vector2f _pos);
	Grid grid_;
	Tool current_tool_ = Tool::kSelectBox;
	GizmoMode current_gizmo_ = GizmoMode::kTranslate;
	Axis gizmo_axis_held_ = Axis::kNone;
	Entity selected_entity_ = -1;
	RectTool rect_tool_;
};


