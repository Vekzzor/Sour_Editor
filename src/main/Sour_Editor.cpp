// PhysicsDestructionBox2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

EditorTools tools;
sf::ContextSettings settings(0, 0, 16);
sf::RenderWindow
	window(sf::VideoMode(2560, 1440), "Voronoi Tesselation", sf::Style::Default, settings);

Coordinator& coordinator						   = gCoordinator.GetInstance();
std::shared_ptr<RenderSystem> renderSystem		   = NULL;
std::shared_ptr<PhysicsSystem> physicsSystem	   = NULL;
std::shared_ptr<SelectableSystem> selectableSystem = NULL;
std::shared_ptr<Inspector> inspectorSystem		   = NULL;

class MyQueryCallback : public b2QueryCallback
{
public:
	std::vector<b2Body*> foundBodies;

	bool ReportFixture(b2Fixture* fixture)
	{
		foundBodies.push_back(fixture->GetBody());
		return true; //keep going to find all fixtures in the query area
	}
};

void applyBlastImpulse(b2Body* body, b2Vec2 blastCenter, b2Vec2 applyPoint, float blastPower)
{
	b2Vec2 blastDir = applyPoint - blastCenter;
	float distance  = blastDir.Normalize();
	//ignore bodies exactly at the blast point - blast direction is undefined
	if(distance == 0)
		return;
	float invDistance = 1 / distance;
	float impulseMag  = blastPower * invDistance * invDistance;
	body->ApplyLinearImpulse(impulseMag * blastDir, applyPoint, true);
}

class PhysicsShapeDraw : public b2Draw
{
public:
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		int count = vertexCount;
		sf::ConvexShape convex;
		convex.setPointCount(count);

		sf::VertexArray vertArr;
		vertArr.setPrimitiveType(sf::PrimitiveType::LineStrip);
		// define the points
		sf::Vertex vertex;
		vertex.color = sf::Color::Green;
		for(int i = 0; i < count; i++)
		{
			vertex.position =
				sf::Vector2f(vertices[i].x * kPixelToMeterRatio, vertices[i].y * kPixelToMeterRatio);
			vertArr.append(vertex);
		}
		vertex.position =
			sf::Vector2f(vertices[0].x * kPixelToMeterRatio, vertices[0].y * kPixelToMeterRatio);
		vertArr.append(vertex);

		window.draw(vertArr);
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		DrawPolygon(vertices, vertexCount, color);
		//int count = vertexCount;
		//sf::ConvexShape convex;
		//convex.setPointCount(count);

		//// define the points
		//for(int i = 0; i < count; i++)
		//{
		//	convex.setPoint(
		//		i,
		//		sf::Vector2f(vertices[i].x * pixelToMeterRatio, vertices[i].y * pixelToMeterRatio));
		//}
		///*convex.setPoint(0, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[0].x*150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[0].y*150));
		//convex.setPoint(1, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[1].x * 150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[1].y * 150));
		//convex.setPoint(2, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[2].x * 150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[2].y * 150));
		//*/
		//convex.setOutlineThickness(-0.05 * pixelToMeterRatio);
		//convex.setOutlineColor(sf::Color(218, 165, 32));
		//convex.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
		//window.draw(convex);
	}
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {}
	void
	DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
	{}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
	{
		sf::VertexArray vertices;
		vertices.setPrimitiveType(sf::PrimitiveType::Lines);
		sf::Vertex vertex(sf::Vector2f(p1.x, p1.y), sf::Color(color.r, color.g, color.b, color.a));
		vertices.append(vertex);
		vertex.position = sf::Vector2f(p2.x, p2.y);
		window.draw(vertices);
	}
	void DrawTransform(const b2Transform& xf) {}
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color)
	{
		sf::CircleShape point;
		point.setPosition(p.x - size, p.y - size);
		point.setRadius(size);
		window.draw(point);
	}
};
void InputActions();

static void funcPointTest()
{
	std::cout << (InputMapping::kAlt | InputMapping::kControl | InputMapping::kShift);
}

static void RegisterSystems()
{
	renderSystem	 = coordinator.RegisterSystem<RenderSystem>();
	physicsSystem	= coordinator.RegisterSystem<PhysicsSystem>();
	selectableSystem = coordinator.RegisterSystem<SelectableSystem>();
	inspectorSystem  = coordinator.RegisterSystem<Inspector>();

	Signature signature;
	signature.set(coordinator.GetComponentType<Live>());
	coordinator.SetSystemSignature<Inspector>(signature);
	signature.reset();
	signature.set(coordinator.GetComponentType<Live>());
	signature.set(coordinator.GetComponentType<sf::Transformable>());
	signature.set(coordinator.GetComponentType<PhysicsObject>());
	//signature.set(coordinator.GetComponentType<Mesh2D>());
	//signature.set(coordinator.GetComponentType<Selectable>());
	coordinator.SetSystemSignature<PhysicsSystem>(signature);

	signature.reset();
	signature.set(coordinator.GetComponentType<Live>());
	signature.set(coordinator.GetComponentType<sf::Transformable>());
	signature.set(coordinator.GetComponentType<Mesh2D>());
	//signature.set(coordinator.GetComponentType<Selectable>());
	coordinator.SetSystemSignature<RenderSystem>(signature);

	signature.reset();
	signature.set(coordinator.GetComponentType<Live>());
	signature.set(coordinator.GetComponentType<Selectable>());
	signature.set(coordinator.GetComponentType<sf::Transformable>());
	//signature.set(coordinator.GetComponentType<PhysicsObject>());
	coordinator.SetSystemSignature<SelectableSystem>(signature);
}

int main()
{
// enable memleak detection
#ifdef _DEBUG
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpFlag |=
		_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF; // Check heap alloc and dump mem leaks at exit
	_CrtSetDbgFlag(tmpFlag);
	assert(!errno);
#endif
	coordinator.Init();
	coordinator.RegisterComponent<Live>();
	coordinator.RegisterComponent<sf::Transformable>();
	coordinator.RegisterComponent<PhysicsObject>();
	coordinator.RegisterComponent<Mesh2D>();
	coordinator.RegisterComponent<Selectable>();

	RegisterSystems();

	PhysicsShapeDraw debugDraw;
	physicsSystem->SetDebugDraw(&debugDraw);
	debugDraw.SetFlags(b2Draw::e_shapeBit);

	std::vector<sf::Vector2f> HE_Polygon;

	float radius = 100;
	float vertexCount = 30;
	for (int k = 0; k < 20; k++)
	{
		for(int i = 0; i < vertexCount; i++)
		{
			float angle = 360 / vertexCount;
			sf::Vector2f pos;
			pos.x = (300 * k) + std::roundf(radius * cosf((-i * angle) * (b2_pi / 180)));
			pos.y = std::roundf(radius * sinf((-i * angle) * (b2_pi / 180)));
			HE_Polygon.push_back(pos);
		}

		//CLOCKWISE ORDER

		//sf::Vector2f CornerMax(400, 300);
		//sf::Vector2f CornerMin(10, 10);
		//HE_Polygon.push_back(CornerMin); //BottomLeft

		//HE_Polygon.push_back(sf::Vector2f(CornerMin.x - 100, (CornerMin.y + CornerMax.y) * 0.5)); //Left

		//HE_Polygon.push_back(sf::Vector2f(CornerMin.x, CornerMax.y)); //TopLeft

		//HE_Polygon.push_back(sf::Vector2f((CornerMin.x + CornerMax.x) * 0.5,CornerMax.y + 100)); //Top

		//HE_Polygon.push_back(CornerMax); //TopRight

		//HE_Polygon.push_back(sf::Vector2f(CornerMax.x+100, (CornerMin.y + CornerMax.y)*0.5)); // Right

		//HE_Polygon.push_back(sf::Vector2f(CornerMax.x, CornerMin.y)); //BottomRight

		//HE_Polygon.push_back(sf::Vector2f((CornerMin.x + CornerMax.x) * 0.5, CornerMin.y - 100)); // Bottom
		VoronoiGenerationTool VGT;

		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;

		for(auto& vertex : HE_Polygon)
		{
			minX = std::min(minX, vertex.x);
			maxX = std::max(maxX, vertex.x);
			minY = std::min(minY, vertex.y);
			maxY = std::max(maxY, vertex.y);
			//BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(vertex, BWpoints.size()));
		}
		for(size_t i = 0; i < SEEDS + 2; i++)
		{
			float x = minX + float(rand() % (int)(maxX - minX));
			float y = minY + float(rand() % (int)(maxY - minY));
			//x		= CornerMax.x - CornerMin.x;
			//y		= CornerMax.y - CornerMin.y;
			VGT.addSeed(sf::Vector2f(x, y));
		}
		VGT.GenerateVoronoiCells(HE_Polygon, physicsSystem.get());
		HE_Polygon.clear();
	}
	
	

	//std::vector<HALF_EDGE::HE_Vertex*> BWpoints;
	//Voronoi_Tessellation::CreateSeedPoints(HE_Polygon,BWpoints);

	//std::vector<HALF_EDGE::HE_Face*> faceList;
	//Delunay::Triangulate(BWpoints, faceList);
	std::vector<Entity> entities;
	//std::vector<Mesh2D> polygons;
	//Voronoi_Tessellation::CreateVoronoiPolyBodies(
	//	HE_Polygon, BWpoints, faceList, polygons, entities, physicsSystem.get());

	//for(auto& point : BWpoints)
	//	delete point;
	//BWpoints.clear();

	double timer = 0;

	sf::Vector2f oldPos;
	bool moving = false;
	float zoom  = 5;

	sf::View view = window.getDefaultView();
	view.zoom(zoom);
	window.setView(view);

	// GROUND PLANE

	for (int i = 0; i < 10; i++)
	{
		Entity planeEntity = coordinator.CreateEntity();
		coordinator.AddComponent<Live>(planeEntity, Live{});

		Mesh2D planeMesh;
		float2 minBound  = {FLT_MAX, FLT_MAX};
		float2 maxBound  = {-FLT_MAX, -FLT_MAX};
		planeMesh.primitive_type = sf::PrimitiveType::TriangleFan;
		{
			sf::Vertex v(sf::Vector2f(1.0f, 1.0f), sf::Color(0, 100, 200));
			planeMesh.vertices.push_back(v);
			v.position = sf::Vector2(1.0f, -1.0f);
			planeMesh.vertices.push_back(v);
			v.position = sf::Vector2(-1.0f, -1.0f);
			planeMesh.vertices.push_back(v);
			v.position = sf::Vector2(-1.0f, 1.0f);
			planeMesh.vertices.push_back(v);

			for(auto& vertex : planeMesh.vertices)
			{
				sf::Vector2f pos = vertex.position;
				minBound[0]		 = std::min(pos.x, minBound[0]);
				minBound[1]		 = std::min(pos.y, minBound[1]);
				maxBound[0]		 = std::max(pos.x, maxBound[0]);
				maxBound[1]		 = std::max(pos.y, maxBound[1]);
			}
		}
		coordinator.AddComponent<Selectable>(planeEntity, Selectable{minBound, maxBound});

		sf::Transformable planeTrans;
		planeTrans.setOrigin({minBound[0] + (maxBound[0] - minBound[0]) / 2,
							  minBound[1] + (maxBound[1] - minBound[1]) / 2});
		planeTrans.setScale(10000, 100);

		sf::Vector2f test;
		test.x = planeTrans.getScale().x * planeTrans.getOrigin().x;
		test.y = planeTrans.getScale().y * planeTrans.getOrigin().y;
		planeTrans.setPosition(sf::Vector2f(i * 2 * kPixelToMeterRatio, 15 * kPixelToMeterRatio) + test);
		//planeTrans.setPosition(0, 150000);

		coordinator.AddComponent<sf::Transformable>(planeEntity, planeTrans);
		coordinator.AddComponent<Mesh2D>(planeEntity, planeMesh);

		physicsSystem->CreateMeshCollider(planeEntity, b2BodyType::b2_staticBody);
	}
	

	// GRID
	sf::RectangleShape grid(window.getDefaultView().getSize());
	grid.setOutlineColor(sf::Color::Blue);
	grid.setOutlineThickness(-1);

	tools.grid_.Init(view);
	std::deque<Entity> trashedEntities;

	sf::ConvexShape selection;
	sf::ConvexShape selectedObject;

	bool holding		= false;
	bool runningPhysics = false;
	ImGui::SFML::Init(window);
	sf::Clock deltaClock;
	ImGuiIO& io = ImGui::GetIO();

	window.setFramerateLimit(288);
	window.setVerticalSyncEnabled(true);
	bool onePressed			  = false;
	bool renderPhysics		  = false;
	InputMapping* IM_Instance = InputMapping::Instance();
	IM_Instance->AddFunctionToKey(std::bind(&funcPointTest),
								  sf::Keyboard::Q,
								  InputMapping::KeyState::kPressed,
								  InputMapping::kShift | InputMapping::kAlt);
	IM_Instance->AddFunctionToKey([&]() { window.close(); },
								  sf::Keyboard::Escape,
								  InputMapping::kPressed,
								  InputMapping::kShift);
	IM_Instance->AddFunctionToKey([&]() { tools.selected_entity_ = -1; }, sf::Keyboard::Escape);

	IM_Instance->AddFunctionToKey(
		[&]() {
			if(tools.selected_entity_ != -1)
				coordinator.GetComponent<PhysicsObject>(tools.selected_entity_)
					.body->SetLinearVelocity({10, 0});
		},
		sf::Keyboard::D);
	IM_Instance->AddFunctionToKey(
		[&]() {
			if(tools.selected_entity_ != -1)
				coordinator.GetComponent<PhysicsObject>(tools.selected_entity_)
					.body->SetLinearVelocity({-10, 0});
		},
		sf::Keyboard::A);
	//Undo Box Creation
	IM_Instance->AddFunctionToKey(
		[&]() {
			if(!entities.empty())
			{
				auto& physObj = coordinator.GetComponent<PhysicsObject>(entities.back());
				coordinator.RemoveComponent<Live>(entities.back());
				physObj.body->SetEnabled(false);
				trashedEntities.push_back(entities.back());
				entities.pop_back();
			}
		},
		sf::Keyboard::Z,
		InputMapping::KeyState::kPressed,
		InputMapping::kControl);

	//Redo Box Creation
	IM_Instance->AddFunctionToKey(
		[&]() {
			if(!trashedEntities.empty())
			{
				coordinator.AddComponent(trashedEntities.back(), Live{});
				auto& physObj = coordinator.GetComponent<PhysicsObject>(trashedEntities.back());
				physObj.body->SetEnabled(true);

				entities.push_back(trashedEntities.back());
				trashedEntities.pop_back();
			}
		},
		sf::Keyboard::Y,
		InputMapping::KeyState::kPressed,
		InputMapping::kControl);

	IM_Instance->AddFunctionToMouseKey(
		[&]() {
			const std::pair<int, int> mousePos = IM_Instance->GetMousePos();
			moving							   = true;
			oldPos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second));
		},
		sf::Mouse::Button::Middle,
		InputMapping::KeyState::kPressed);

	IM_Instance->AddFunctionToMouseKey(
		[&]() { moving = false; }, sf::Mouse::Button::Middle, InputMapping::KeyState::kReleased);

	IM_Instance->AddFunctionToMouseKey(
		[&]() {
			const std::pair<int, int> mousePos = IM_Instance->GetMousePos();
			sf::Vector2f pos =
				window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);
			if(tools.current_tool_ == Tool::kAddBox)
			{
				int offset		  = pow(10, tools.grid_.zoom_level_ - 2);
				sf::Vector2f pos2 = sf::Vector2f(pos.x / offset, pos.y / offset);

				tools.StartDrag(sf::Vector2f(round(pos2.x) * offset, round(pos2.y) * offset));
			}
			if(tools.selected_entity_ != -1)
			{
				tools.gizmo_axis_held_ = TranslateTool::AxisIsPressed(
					tools.selected_entity_, window.getView().getSize(), pos, true);
				tools.rect_tool_.HandleIsHeld(tools.selected_entity_, window.getView().getSize(), pos, true);
			}
			holding = true;
		},
		sf::Mouse::Button::Left,
		InputMapping::KeyState::kPressed);

	IM_Instance->AddFunctionToMouseKey(
		[&]() {
			const std::pair<int, int> mousePos = IM_Instance->GetMousePos();
			selection						   = sf::ConvexShape();

			if(tools.current_tool_ == Tool::kAddBox)
			{
				int offset = pow(10, tools.grid_.zoom_level_ - 2);
				sf::Vector2f pos =
					window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);
				sf::Vector2f pos2 = sf::Vector2f(pos.x / offset, pos.y / offset);
				pos				  = sf::Vector2f(round(pos2.x) * offset, round(pos2.y) * offset);
				//DELETE ALL ENTITIES LEFT
				for(const auto& entity : trashedEntities)
				{
					if(coordinator.HasComponent<PhysicsObject>(entity))
						physicsSystem->DestroyPhysicsBody(entity);

					coordinator.DestroyEntity(entity);
				}
				trashedEntities.clear();

				if(pos.x != tools.GetDragStart().x && pos.y != tools.GetDragStart().y)
				{
					sf::Vector2f maxPoint = {std::max(tools.GetDragStart().x, pos.x),
											 std::max(tools.GetDragStart().y, pos.y)};
					sf::Vector2f minPoint = {std::min(tools.GetDragStart().x, pos.x),
											 std::min(tools.GetDragStart().y, pos.y)};
					std::vector<sf::Vector2f> autoLol;
					autoLol.push_back(minPoint); //TopLeft
					autoLol.push_back(sf::Vector2f(minPoint.x, maxPoint.y)); //BottomLeft
					autoLol.push_back(maxPoint); //BottomRight
					autoLol.push_back(sf::Vector2f(maxPoint.x, minPoint.y)); //TopRight
					
					std::vector<HALF_EDGE::HE_Vertex*> points;
					Voronoi_Tessellation::CreateSeedPoints(autoLol,points);

					std::vector<HALF_EDGE::HE_Face*> fList;
					Delunay::Triangulate(points, fList);
					
					std::vector<Mesh2D> polys;
					Voronoi_Tessellation::CreateVoronoiPolyBodies(
						autoLol,
						points, fList, polys, entities, physicsSystem.get());

					for(auto& point : points)
						delete point;

					points.clear();
					autoLol.clear();
				}
			}
			else if(tools.current_tool_ == Tool::kSelectBox)
			{
				sf::Vector2f pos =
					window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);
				std::vector<Entity> entities2;
				selectableSystem->findEntities(entities2, {pos.x, pos.y});
				if(!entities2.empty())
				{
					tools.selected_entity_ = entities2.back();
					tools.CreateObjectSelectedBorder();
				}
			}
			else if (tools.current_tool_ == Tool::kVoronoiFracture)
			{
				sf::Vector2f pos =
					window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);
			}

			tools.gizmo_axis_held_ = Axis::kNone;
			tools.rect_tool_.HandleReleased();
			holding			  = false;
		},
		sf::Mouse::Button::Left,
		InputMapping::KeyState::kReleased);

	while(window.isOpen())
	{
		EventHandler::Instance()->HandleEvents(window);
		//INPUT HANDLING
		////////////////////////////////////////////////////////////////
		const std::pair<int, int> mousePos = IM_Instance->GetMousePos();
		if(IM_Instance->IsMouseMoving())
		{
			const sf::Vector2f newPos =
				window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second));
			if(holding)
			{
				int offset		  = pow(10, tools.grid_.zoom_level_ - 2);
				sf::Vector2f pos2 = sf::Vector2f(newPos.x / offset, newPos.y / offset);
				if(tools.current_tool_ == Tool::kAddBox)
					selection = tools.SelectionBox(
						sf::Vector2f(round(pos2.x) * offset, round(pos2.y) * offset));
			}

			// Ignore mouse movement unless a button is pressed (see above)
			if(moving)
			{
				// Determine the new position in world coordinates
				// Determine how the cursor has moved
				// Swap these to invert the movement direction
				const sf::Vector2f deltaPos = oldPos - newPos;

				// Move our view accordingly and update the window
				view.setCenter(view.getCenter() + deltaPos);
				window.setView(view);

				// Save the new position as the old one
				// We're recalculating this, since we've changed the view
				oldPos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second));
			}
		}
		if(IM_Instance->IsMouseScrolling() &&
		   !moving) // Ignore the mouse wheel unless we're not moving
		{
			const float mWSD = IM_Instance->GetMouseScrollDelta();
			// Determine the scroll direction and adjust the zoom level
			// Again, you can swap these to invert the direction
			if(mWSD <= -1)
				zoom = std::min(1000.f, zoom + tools.grid_.zoom_level_);
			else if(mWSD >= 1)
				zoom = std::max(10.0f, zoom - tools.grid_.zoom_level_);

			// Update our view
			view.setSize(window.getDefaultView().getSize()); // Reset the size
			view.zoom(zoom); // Apply the zoom level (this transforms the view)
			window.setView(view);
		}

		//IMGUI UI
		////////////////////////////////////////////////////////////////
		static bool selectable[2]{true, false};
		bool popped[2] = {false};
		ImGui::SFML::Update(window, deltaClock.getElapsedTime());
		ImGui::Begin("Hello, world!");
		ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
		ImGui::Text("Living Entities: %i", coordinator.getEntityCount());
		ImGui::Text("MousePos = %f, %f",
					window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second)).x,
					window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second)).y);

		ImGui::PushStyleColor(ImGuiCol_Button, {1, 0, 0, 1});
		if(selectable[1])
		{
			ImGui::PopStyleColor();
			popped[0] = true;
		}
		if(ImGui::Button("Select Box"))
		{
			selectable[0] = true;
			selectable[1] = false;
			tools.current_tool_   = Tool::kSelectBox;
		}
		if(!popped[0])
			ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Button, {1, 0, 0, 1});
		if(selectable[0])
		{
			ImGui::PopStyleColor();
			popped[1] = true;
		}
		if(ImGui::Button("Create Rectangle"))
		{
			selectable[0]		 = false;
			selectable[1]		 = true;
			tools.current_tool_			 = Tool::kAddBox;
			tools.selected_entity_ = -1;
		}
		if(!popped[1])
			ImGui::PopStyleColor();

		ImGui::Checkbox("Physics", &runningPhysics);
		ImGui::Checkbox("Rendering Physics", &renderPhysics);
		ImGui::DragFloat("Zoom", &zoom, 1.0f, 1.0f, 1000.0f);
		inspectorSystem->ListLiveEntites(tools);
		ImGui::End();

		if(tools.selected_entity_ != -1)
			inspectorSystem->InspectEntity(tools.selected_entity_);

		ImGui::ShowDemoWindow(false);
		// Update our view
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoom); // Apply the zoom level (this transforms the view)
		window.setView(view);

		//PHYSICS AND DRAWING
		////////////////////////////////////////////////////////////////
		timer += deltaClock.restart().asSeconds();
		if(runningPhysics && timer >= physicsSystem->getTimeStep())
		{
			physicsSystem->WorldStep();
			timer -= physicsSystem->getTimeStep();
		}
		//if(runningPhysics)
		physicsSystem->Update();
		window.clear({100, 100, 100});
		tools.grid_.UpdateGrid(view);
		//tools.updateCurrentTool();
		window.draw(tools.grid_);

		renderSystem->DrawMeshes(window);
		window.draw(selection);
		//window.draw(tools.getSelectedObjectBorder());
		if(tools.selected_entity_ != -1 && coordinator.HasComponent<Mesh2D>(tools.selected_entity_))
		{
			renderSystem->DrawWireframeMesh(window, tools.selected_entity_);
			auto& dPos = IM_Instance->GetMousePosDelta();
			//TranslateTool::GizmoManipulate(
			//	window,
			//	tools.selectedEntity,
			//	window.getView().getSize(),
			//	window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second)),
			//	window.mapPixelToCoords(
			//		sf::Vector2i(mousePos.first + dPos.first, mousePos.second + dPos.second)),
			//	tools.GizmoIsHeld);
			tools.rect_tool_.CreateHandles(tools.selected_entity_, window.getView().getSize());
			tools.rect_tool_.GizmoManipulate(
				window,
				tools.selected_entity_,
				window.getView().getSize(),
				window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second)),
				window.mapPixelToCoords(
					sf::Vector2i(mousePos.first + dPos.first, mousePos.second + dPos.second)), holding);
		}

		if(renderPhysics)
			physicsSystem->WorldDebugDraw();

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	window.close();
}

void InputActions()
{
	InputMapping* IM_Instance = InputMapping::Instance();
	if(!IM_Instance->EmptyKeyPressTransistion())
	{
		sf::Event::KeyEvent trans =
			InputMapping::Instance()->GetKeyPressTransition(sf::Keyboard::Escape);

		if(IM_Instance->GetKeyPressTransition(sf::Keyboard::Escape).code != sf::Keyboard::Unknown)
			window.close();

		if(IM_Instance->GetKeyPressTransition(sf::Keyboard::Num1).code != sf::Keyboard::Unknown)
		{
		}
		if(IM_Instance->GetKeyPressTransition(sf::Keyboard::Num2).code != sf::Keyboard::Unknown)
		{
		}
	}
}

void Render(sf::RenderWindow& window,
			std::vector<sf::Vector2f>& points,
			std::vector<b2Vec2>& allVertices)
{
	bool pause   = false;
	double timer = 0;
	sf::Event event;
	while(timer < 3)
	{
		auto start = std::chrono::system_clock::now();
		window.clear();
		while(window.pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if(event.key.code == sf::Keyboard::Escape)
					window.close();
				else if(event.key.code == sf::Keyboard::Space)
				{
					pause = 1;
					window.setTitle("Paused: " + std::to_string(pause));
				}
				else if(event.key.code == sf::Keyboard::BackSpace)
				{
					pause = 0;
					window.setTitle("Paused: " + std::to_string(pause));
					start = std::chrono::system_clock::now();
				}
				else if(event.key.code == sf::Keyboard::Right)
				{
					timer = 3;
				}
				break;
			}
		}
		if(!pause)
		{
			for(auto& point : allVertices)
			{
				sf::CircleShape vertex;
				vertex.setRadius(500);
				vertex.setPosition(sf::Vector2f(point.x, point.y) - sf::Vector2f(500, 500));
				vertex.setFillColor({0, 255, 0});
				window.draw(vertex);
			}
			std::vector<sf::Vertex> test;
			for(auto& point : points)
			{
				sf::Vertex vert;
				vert.position = point;
				test.push_back(vert);

				sf::CircleShape vertex;
				vertex.setRadius(500);
				vertex.setPosition(point - sf::Vector2f(500, 500));
				vertex.setFillColor({255, 0, 0});
				window.draw(vertex);
			}
			window.draw(test.data(), test.size(), sf::LineStrip);
			sf::Vertex vert[2];
			vert[0].position = test.back().position;
			vert[1].position = test.front().position;

			window.draw(vert, 2, sf::Lines);
			window.display();
			std::chrono::duration<double> elapsed_seconds =
				std::chrono::system_clock::now() - start;
			timer += elapsed_seconds.count();
		}
	}
	timer = 0;
}

//// Divide and Conquer Broadphase
//void dac_bp_Test(const std::vector<VoronoiGeometry>& bounds,
//				 const std::vector<int>::iterator start,
//				 const std::vector<int>::iterator end,
//				 std::vector<int>& intersections,
//				 float2 point)
//{
//	const int BRUTE_FORCE_THRESH = 32;
//	if((end - start) < BRUTE_FORCE_THRESH)
//	{
//		// Brute force check the remaining pairs in this interval
//		for(auto i = start; i != end;
//			i++) //Update To do a more fine grained intersection (Convex Hull)
//		{
//			if((bounds[*i].AABB_lowerBounds[0] < point[0] &&
//				point[0] < bounds[*i].AABB_upperBounds[0]) &&
//			   (bounds[*i].AABB_lowerBounds[1] < point[1] &&
//				point[1] < bounds[*i].AABB_upperBounds[1]))
//			{
//				intersections.push_back(*i);
//			}
//		}
//	}
//	else
//	{
//		// Compute bounds of all boxes in this interval
//		float2 pmin = {FLT_MAX, FLT_MAX}, pmax = {-FLT_MAX, -FLT_MAX};
//		for(auto i = start; i != end; i++)
//		{
//			pmin = std::min(pmin, bounds[*i].AABB_lowerBounds);
//			pmax = std::max(pmax, bounds[*i].AABB_upperBounds);
//		}
//
//		// Choose the partition axis and partition location
//		float2 size = {pmax[0] - pmin[0], pmax[1] - pmin[1]};
//		int axis	= (size[1] > size[0]);
//		float split = (pmin[axis] + pmax[axis]) * 0.5f;
//
//		// Partition boxes left and recurse
//		auto mid_left = partition(start, end, [split, axis, bounds](int i) {
//			return bounds[i].AABB_lowerBounds[axis] <= split;
//		});
//
//		dac_bp_Test(bounds, start, mid_left, intersections, point);
//
//		// Partition boxes right and recurse
//		auto mid_right = partition(start, end, [split, axis, bounds](int i) {
//			return bounds[i].AABB_upperBounds[axis] >= split;
//		});
//
//		dac_bp_Test(bounds, start, mid_right, intersections, point);
//	}
//}

// USAGE
// Build a vector containing bounds of all objects
//std::vector<VoronoiGeometry> bounds = { /* fill with bounds of objects */ };
//
//// Initialize indices
//std::vector<int> indices;
//for (size_t i = 0; i < bounds.size(); i++)
//	indices.push_back(i);
//
//// Get all colliding pairs!
//std::set<std::pair<int, int>> pairs;
//dac_bp(bounds, indices.start(), indices.end(), pairs);