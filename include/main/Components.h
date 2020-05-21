#pragma once
#include <array>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "Box2D/box2d.h"
#include <float.h>
#include <boost/variant.hpp>
#include <functional>
#include <boost/preprocessor.hpp>
#include <boost/type_traits.hpp>
#include <boost/ref.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/bind.hpp>
#include <boost/type_index.hpp>
#include <cctype>

#define EXPAND(...) __VA_ARGS__
#define EAT(...)

// Retrieve the type
#define DETAIL_TYPEOF_HEAD(x, ...) EXPAND x
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),

#define DETAIL_TYPEOF_INT2(tuple) DETAIL_TYPEOF_HEAD tuple 
#define DETAIL_TYPEOF_INT(...) DETAIL_TYPEOF_INT2((__VA_ARGS__))

#define TYPEOF(x) DETAIL_TYPEOF_INT(DETAIL_TYPEOF_PROBE x,)
// Strip off the type
#define STRIP(x) EAT x
// Show the type without parenthesis
#define PAIR(x) EXPAND x

#define CREATE_COMPONENT_CONSTRUCTOR(...) \
struct ComponentConstructor \
{	\
	static void CreateComponent(const char* _compName, Entity _entity) \
	{ \
		BOOST_PP_SEQ_FOR_EACH_I(CREATE_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
		{}\
	};\
};

#define CREATE_EACH(r, data, i, x) \
if(_compName == typeid(x).name()) \
{ \
	gCoordinator.GetInstance().AddComponent(_entity, x()); \
} \
else 

#define COMPONENTS Live, Selectable, Mesh2D, PhysicsObject, sf::Transformable, Viewport

typedef std::array<float, 2> float2;

struct Live
{
	Live() {};
};

struct Viewport
{
	sf::View view;
};

struct Selectable
{
	Selectable() {};
	Selectable(float2 _minBounds, float2 _maxBounds) : aabb_lower_bounds(_minBounds), aabb_upper_bounds(_maxBounds) {};
	~Selectable() {};
	float2 aabb_lower_bounds;
	float2 aabb_upper_bounds;
};

struct Mesh2D
{
	Mesh2D() {};
	~Mesh2D() {
		vertices.clear();
		if (texture != NULL)
		{
			delete texture;
		}
	};
	std::vector<sf::Vertex> vertices;
	sf::PrimitiveType primitive_type = sf::Triangles;
	bool is_rendering_mesh = true;

	const sf::Texture* texture = nullptr;
	sf::IntRect        texture_rect;
};

//#UNUSED
struct VoronoiGeometryDebug
{
	sf::CircleShape center;
	bool render_site = false;
};

struct PhysicsObject
{
	PhysicsObject() {};
	~PhysicsObject() {
		body = nullptr;
		fixtures.clear();
	};
	b2Body* body = nullptr;
	std::vector<b2Fixture*> fixtures;
};

typedef boost::variant<Live, Selectable, Mesh2D, PhysicsObject, sf::Transformable> variant_type;
template<typename T> variant_type createInstance() {
	return variant_type(T());
}
typedef std::map<const char*, std::function<variant_type()> > map_type;


CREATE_COMPONENT_CONSTRUCTOR(COMPONENTS)
#undef CREATE_COMPONENT_CONSTRUCTOR


class GameObject
{
	Entity entity;
public:
	GameObject() 
	{
		auto& coordinator = gCoordinator.GetInstance();
		entity = coordinator.CreateEntity();
		coordinator.AddComponent<Live>(entity, Live{});
		coordinator.AddComponent<sf::Transformable>(entity, sf::Transformable{});
	}
	~GameObject()
	{
		gCoordinator.DestroyEntity(entity);
	}
};

class PhysicsObjectInspector
{
public:
	static void ShowData(Entity _entity)
	{
		if (ImGui::TreeNodeEx("PhysicsObject", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& comp = gCoordinator.GetInstance().GetComponent<PhysicsObject>(_entity);
			bool enabled = comp.body->IsEnabled();
			ImGui::Checkbox("Enabled", &enabled);
			comp.body->SetEnabled(enabled);
			
			b2Vec2 vec = comp.body->GetPosition();
			float x = vec.x;
			float y = vec.y;
			ImGui::PushItemWidth(100);
			ImGui::Text("Mass"); ImGui::SameLine();
			ImGui::Text("%f", comp.body->GetMass());
			ImGui::Text("Position");
			ImGui::Text("X"); ImGui::SameLine();
			ImGui::DragFloat("##PhysPosX", &x, 100, -FLT_MAX, FLT_MAX); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine();
			ImGui::DragFloat("##PhysPosY", &y, 100, -FLT_MAX, FLT_MAX);

			ImGui::TreePop();
		}
	}
};

class MeshInspector
{
public:
	static void ShowData(Entity _entity)
	{
		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& comp = gCoordinator.GetInstance().GetComponent<Mesh2D>(_entity);
			ImGui::Text("Vertices: %i", comp.vertices.size());
			ImGui::TreePop();
		}
	}
};

// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) {
		return std::isspace(c);
	}));
}
class ComponentCreator
{
public:
	static void ShowComponents(Entity _entity)
	{
		std::vector<const char*> names;
		gCoordinator.GetInstance().getComponentNames(names);
		static int item_current = 0;
		ImGui::Text("Components"); ImGui::SameLine();
		ImGui::SetNextItemWidth(-1);
		ImGui::Combo("##Components", &item_current, names.data(), names.size());
		
		ImGui::Indent(ImGui::GetWindowWidth() - 117.0f);
		if (ImGui::Button("AddComponent", ImVec2(100, 0.0f))) 
		{ 
			ComponentConstructor::CreateComponent(names[item_current], _entity); 
		}
	}
};

class TransformInspector
{
public:
	static void ShowData(Entity _entity)
	{
		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& comp = gCoordinator.GetInstance().GetComponent<sf::Transformable>(_entity);
			ImVec2 vec = comp.getPosition();
			float x = vec.x;
			float y = vec.y;
			ImGui::PushItemWidth(100);
			ImGui::Text("Position");
			ImGui::Text("X"); ImGui::SameLine();
			ImGui::DragFloat("##PosX", &x, 100, -FLT_MAX, FLT_MAX); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine();
			ImGui::DragFloat("##PosY", &y, 100, -FLT_MAX, FLT_MAX);
			comp.setPosition(x, y);

			vec = comp.getOrigin();
			x = vec.x;
			y = vec.y;
			ImGui::Text("Origin");
			ImGui::Text("X"); ImGui::SameLine();
			ImGui::DragFloat("##OriginX", &x, 100, -FLT_MAX, FLT_MAX); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine();
			ImGui::DragFloat("##OriginY", &y, 100, -FLT_MAX, FLT_MAX);
			comp.setOrigin(x, y);

			vec = comp.getScale();
			x = vec.x;
			y = vec.y;
			ImGui::Text("Scale");
			ImGui::Text("X"); ImGui::SameLine();
			ImGui::DragFloat("##ScaleX", &x, 0.1f, -FLT_MAX, FLT_MAX); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine();
			ImGui::DragFloat("##ScaleY", &y, 0.1f, -FLT_MAX, FLT_MAX);
			comp.setScale(x, y);

			float rot = comp.getRotation();
			ImGui::Text("Rotation");
			ImGui::DragFloat("##Rotation", &rot, 1, 0, 360);
			comp.setRotation(std::fmodf(rot, 360));

			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
	}
};
