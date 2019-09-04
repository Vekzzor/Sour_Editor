// PhysicsDestructionBox2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
//#include "imgui/imgui.h"
//#include "imgui/imgui-SFML.h"
EditorTools tools;
b2World world(b2Vec2(0.0f, 9.82f));
static const int PPM = 100;
sf::ContextSettings settings(0,0,16);
sf::RenderWindow window(sf::VideoMode(2560, 1440), "Voronoi Tesselation", sf::Style::Default, settings);
static const int MINSIZE = 0; //300
static const int MAXSIZE = 100000; //400
static const float CENTER = float(MINSIZE + (MAXSIZE / 2));
static const int SEEDS = 1;



class MyQueryCallback : public b2QueryCallback {
public:
	std::vector<b2Body*> foundBodies;

	bool ReportFixture(b2Fixture* fixture) {
		foundBodies.push_back(fixture->GetBody());
		return true;//keep going to find all fixtures in the query area
	}
};

void applyBlastImpulse(b2Body* body, b2Vec2 blastCenter, b2Vec2 applyPoint, float blastPower) {
	b2Vec2 blastDir = applyPoint - blastCenter;
	float distance = blastDir.Normalize();
	//ignore bodies exactly at the blast point - blast direction is undefined
	if (distance == 0)
		return;
	float invDistance = 1 / distance;
	float impulseMag = blastPower * invDistance * invDistance;
	body->ApplyLinearImpulse(impulseMag * blastDir, applyPoint,true);
}

class PhysicsShapeDraw : public b2Draw
{
public:
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		int count = vertexCount;
		sf::ConvexShape convex;
		convex.setPointCount(count);

		// define the points
		for (int i = 0; i < count; i++)
		{
			convex.setPoint(i, sf::Vector2f(vertices[i].x*pixelToMeterRatio, vertices[i].y*pixelToMeterRatio));
		}
		/*convex.setPoint(0, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[0].x*150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[0].y*150));
		convex.setPoint(1, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[1].x * 150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[1].y * 150));
		convex.setPoint(2, sf::Vector2f(static_cast<const b2PolygonShape*>(shape)->m_vertices[2].x * 150, -static_cast<const b2PolygonShape*>(shape)->m_vertices[2].y * 150));
		*/
		convex.setFillColor(sf::Color(218, 165, 32));
		window.draw(convex);
	
	}
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
		
	}
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
		
	}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {}
	void DrawTransform(const b2Transform& xf) {}
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) 
	{
		sf::CircleShape point;
		point.setPosition(p.x - size, p.y - size);
		point.setRadius(size);
		window.draw(point);
	}
};



struct VoronoiEdge
{
	VoronoiEdge(sf::Vector2f _v1, sf::Vector2f _v2, sf::Vector2f* _site) : v1{ _v1 }, v2{ _v2 }, site{ _site }{};
	sf::Vector2f v1;
	sf::Vector2f v2;
	sf::Vector2f* site;
};

struct VoronoiCell
{
	sf::Vector2f* site;
	std::vector<VoronoiEdge*> edges;

	VoronoiCell(sf::Vector2f* sitePos) : site{ sitePos } {};
};

void addVoronoiEdge(HALF_EDGE::HE_Edge* e, sf::Vector2f center, std::vector<VoronoiEdge*>& voronoiEdges)
{
	//Ignore if this edge has no neighboring triangle
	if (e->twin == nullptr)
	{
		return;
	}
	//Get the circumcenter of the neighbor
	HALF_EDGE::HE_Edge* eNeighbor = e->twin;
	sf::Vector2f voronoiVertexNeighbor = eNeighbor->face->circumCenter;


	//Create a new voronoi edge between the voronoi vertices
	voronoiEdges.push_back(DBG_NEW
		VoronoiEdge(center, voronoiVertexNeighbor, e->prev->vert->point));
}

void CreateSeedPoints(std::vector<HALF_EDGE::HE_Vertex*>& BWpoints, float _MaxSize, float _MinSize)
{
	if (BWpoints.empty())
	{
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_MinSize, _MinSize, BWpoints.size())); //TopLeft
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_MaxSize + _MinSize, _MinSize, BWpoints.size())); //TopRight
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_MaxSize + _MinSize, _MaxSize + _MinSize, BWpoints.size())); //BottomRight
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(_MinSize, _MaxSize + _MinSize, BWpoints.size())); //BottomLeft
	}

	//int points = rand() % 10 +1;
	//for (size_t i = 0; i < SEEDS; i++)
	//{
	//	float x = float(rand() % (int)_MaxSize + _MinSize);
	//	float y = float(rand() % (int)_MaxSize + _MinSize);
	//	BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(x, y, BWpoints.size()));
	//}

	//Add Star Shape
	{
		float Center = float((_MinSize + _MaxSize) / 2);
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(Center,Center, BWpoints.size()));
		float minMax = _MaxSize * 2;
		sf::Vector2f starTop = { Center, Center - minMax };
		sf::Vector2f starLeft = { Center - minMax, Center };
		sf::Vector2f starRight = { Center + minMax, Center };
		sf::Vector2f starBottom = { Center, Center + minMax };

		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, BWpoints.size()));
	}
}

void CreateSeedPoints(std::vector<HALF_EDGE::HE_Vertex*>& BWpoints, sf::Vector2f CornerMax, sf::Vector2f CornerMin)
{

	if (BWpoints.empty())
	{
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(CornerMin, BWpoints.size())); //TopLeft
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(CornerMax.x, CornerMin.y, BWpoints.size())); //TopRight
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(CornerMax, BWpoints.size())); //BottomRight
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(CornerMin.x, CornerMax.y, BWpoints.size())); //BottomLeft
	}

	for (size_t i = 0; i < SEEDS; i++)
	{
		float x = CornerMin.x+ float(rand() % (int) (CornerMax.x - CornerMin.x));
		float y = CornerMin.y+ float(rand() % (int) (CornerMax.y - CornerMin.y));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(x, y, BWpoints.size()));
	}
	//float x1 = float(rand() % (int)CornerMax.x + CornerMin.x);
	//float y1 = float(rand() % (int)CornerMax.y + CornerMin.y);
	//float x2 = float(rand() % (int)50000.0f + 0.0f);
	//float y2= float(rand()% (int)50000.0f + 0.0f);


	{
		sf::Vector2f Center = (CornerMin + CornerMax) *0.5f;
		//BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(Center, BWpoints.size()));

		sf::Vector2f minMax = (CornerMax- CornerMin) * 2.0f;
		sf::Vector2f starTop = { Center.x, Center.y - minMax.y };
		sf::Vector2f starLeft = { Center.x - minMax.x, Center.y };
		sf::Vector2f starRight = { Center.x + minMax.x, Center.y };
		sf::Vector2f starBottom = { Center.x, Center.y + minMax.y };

		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starTop, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starLeft, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starRight, BWpoints.size()));
		BWpoints.push_back(DBG_NEW HALF_EDGE::HE_Vertex(starBottom, BWpoints.size()));
	}
}
bool LiangBarsky(double edgeLeft, double edgeRight, double edgeBottom, double edgeTop,   // Define the x/y clipping values for the border.
	sf::Vector2f src0, sf::Vector2f src1,                 // Define the start and end points of the line.
	sf::Vector2f &clip0, sf::Vector2f &clip1)         // The output values, so declare these outside.
{

	double t0 = 0.0;    double t1 = 1.0;
	double xdelta = src1.x - src0.x;
	double ydelta = src1.y - src0.y;
	double p, q, r;

	for (int edge = 0; edge < 4; edge++) {   // Traverse through left, right, bottom, top edges.
		if (edge == 0) { p = -xdelta;    q = -(edgeLeft - src0.x); }
		if (edge == 1) { p = xdelta;     q = (edgeRight - src0.x); }
		if (edge == 2) { p = ydelta;    q = (edgeBottom - src0.y); }
		if (edge == 3) { p = -ydelta;     q = -(edgeTop - src0.y); }
		r = q / p;
		if (p == 0 && q < 0) return false;   // Don't draw line at all. (parallel line outside)

		if (p < 0) {
			if (r > t1) return false;         // Don't draw line at all.
			else if (r > t0) t0 = r;            // Line is clipped!
		}
		else if (p > 0) {
			if (r < t0) return false;      // Don't draw line at all.
			else if (r < t1) t1 = r;         // Line is clipped!
		}
	}

	clip0 = { src0.x + float(t0 * xdelta) , src0.y + float(t0 * ydelta) };
	clip1 = { src0.x + float(t1 * xdelta) , src0.y + float(t1 * ydelta) };

	return true;        // (clipped) line is drawn
}


void dac_bp_Test(const std::vector<Polygon>& bounds, const std::vector<int>::iterator start, const std::vector<int>::iterator end, std::vector<int>& intersections, float2 point);

void InputActions();

void CreateVoronoiPolyBodies(std::vector<HALF_EDGE::HE_Vertex*>& _BWpoints, std::vector<HALF_EDGE::HE_Face*>& _faceList, std::vector<Polygon>& _polygons);
static void funcPointTest()
{
	std::cout << (InputMapping::Alt | InputMapping::Control | InputMapping::Shift);
}
int main()
{

	PhysicsShapeDraw debugDraw;
	world.SetDebugDraw(&debugDraw);
	debugDraw.SetFlags(b2Draw::e_shapeBit);

	std::vector<HALF_EDGE::HE_Vertex*> BWpoints;
	CreateSeedPoints(BWpoints, sf::Vector2f(40000,30000), sf::Vector2f(0, 0));
	//CreateSeedPoints(BWpoints,50000, 0);

	std::vector<HALF_EDGE::HE_Face*> faceList;
	Delunay::Triangulate(BWpoints, faceList);
	std::vector<Polygon> polygons;
	CreateVoronoiPolyBodies(BWpoints, faceList, polygons);

	for (auto &point : BWpoints)
	{
		delete point;
	}
	BWpoints.clear();


	double timer = 0;
	
	sf::Vector2f oldPos;
	bool moving = false;
	float zoom = 5;

	sf::View view = window.getDefaultView();
	//view.setCenter(sf::Vector2f(CENTER, CENTER));
	view.zoom(zoom);
	window.setView(view);

	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	static const double timeStep = 1.0 / 60.0;

	b2BodyDef myBodyDef;
	myBodyDef.type = b2_staticBody; //this will be a static body
	myBodyDef.position.Set(0, (150000) / pixelToMeterRatio); //set the starting position
	myBodyDef.angle = 0;
	b2Body* staticBody = world.CreateBody(&myBodyDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(100.0f, 1.0f);

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	staticBody->CreateFixture(&boxFixtureDef);

	sf::ConvexShape plane;
	plane.setPointCount(boxShape.m_count);
	for (int i = 0; i < plane.getPointCount(); i++)
		plane.setPoint(i, sf::Vector2f(boxShape.m_vertices[i].x * pixelToMeterRatio, boxShape.m_vertices[i].y * pixelToMeterRatio));
	plane.setOrigin(0,0);
	plane.setPosition(staticBody->GetPosition().x * pixelToMeterRatio, staticBody->GetPosition().y * pixelToMeterRatio);
	plane.setFillColor(sf::Color(0,200,200));

	sf::RectangleShape grid(window.getDefaultView().getSize());
	grid.setOutlineColor(sf::Color::Blue);
	grid.setOutlineThickness(-1);

	tools.grid.Init(view);
	std::vector<Polygon> boxes;
	std::deque<Polygon> trashedBoxes;
	boxes.insert(boxes.end(), polygons.begin(), polygons.end());
	sf::ConvexShape selection;
	sf::ConvexShape selectedObject;

	bool holding = false;
	bool runningPhysics = false;
	ImGui::SFML::Init(window);
	sf::Clock deltaClock;
	ImGuiIO& io = ImGui::GetIO();
	
	window.setFramerateLimit(288);
	window.setVerticalSyncEnabled(true);
	bool onePressed = false;
	bool renderPhysics = false;
	InputMapping* IM_Instance = InputMapping::Instance();
	IM_Instance->addFunctionToKey(std::bind(&funcPointTest),sf::Keyboard::Q, InputMapping::KeyState::Pressed, InputMapping::Shift | InputMapping::Alt);
	IM_Instance->addFunctionToKey([&]() { window.close(); }, sf::Keyboard::Escape);
	IM_Instance->addFunctionToKey([&]() { boxes[0].m_body->SetLinearVelocity({ 100,0 }); }, sf::Keyboard::D);
	IM_Instance->addFunctionToKey([&]() { boxes[0].m_body->SetLinearVelocity({ -100,0 }); }, sf::Keyboard::A);
	//Undo Box Creation
	IM_Instance->addFunctionToKey([&]() 
	{
		if(!boxes.empty())
		{
			/*for(auto& fixture : boxes.back().m_fixtures)
				boxes.back().m_body->DestroyFixture(fixture);
			boxes.back().m_fixtures.clear();*/
			boxes.back().m_body->SetActive(false);
			trashedBoxes.push_back(boxes.back());
			boxes.pop_back();
		}
	}, sf::Keyboard::Z, InputMapping::KeyState::Pressed, InputMapping::Control);

	//Redo Box Creation
	IM_Instance->addFunctionToKey([&]()
	{
		if (!trashedBoxes.empty())
		{
			boxes.push_back(trashedBoxes.back());
			//boxes.back().m_body->ResetMassData();
			//boxes.back().createPhysicsBody();
			boxes.back().m_body->SetActive(true);
			trashedBoxes.pop_back();
		}
	}, sf::Keyboard::Y, InputMapping::KeyState::Pressed, InputMapping::Control);

	IM_Instance->addFunctionToMouseKey([&]()
	{
		const std::pair<int, int> mousePos = IM_Instance->getMousePos();
		moving = true;
		oldPos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second));
	}, sf::Mouse::Button::Middle, InputMapping::KeyState::Pressed);

	IM_Instance->addFunctionToMouseKey([&](){ moving = false; }, sf::Mouse::Button::Middle, InputMapping::KeyState::Released);

	IM_Instance->addFunctionToMouseKey([&]()
	{
		const std::pair<int, int> mousePos = IM_Instance->getMousePos();
		int offset = pow(10, tools.grid.zoomLevel - 2);
		sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);
		sf::Vector2f pos2 = sf::Vector2f(pos.x / offset, pos.y / offset);

		tools.startDrag(sf::Vector2f(round(pos2.x)*offset, round(pos2.y)*offset));
		holding = true;
	}, sf::Mouse::Button::Left, InputMapping::KeyState::Pressed);


	IM_Instance->addFunctionToMouseKey([&]()
	{
		const std::pair<int, int> mousePos = IM_Instance->getMousePos();
		selection = sf::ConvexShape();

		if (tools.currentTool() == Tool::AddBox)
		{
			int offset = pow(10, tools.grid.zoomLevel - 2);
			sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);
			sf::Vector2f pos2 = sf::Vector2f(pos.x / offset, pos.y / offset);
			pos = sf::Vector2f(round(pos2.x)*offset, round(pos2.y)*offset);
			//tools.addBox(world, boxes, sf::Vector2f(round(pos2.x)*offset, round(pos2.y)*offset));
			trashedBoxes.clear();

			if (pos.x != tools.getDragStart().x && pos.y != tools.getDragStart().y)
			{
				sf::Vector2f maxPoint = { std::max(tools.getDragStart().x, pos.x), std::max(tools.getDragStart().y, pos.y) };
				sf::Vector2f minPoint = { std::min(tools.getDragStart().x, pos.x), std::min(tools.getDragStart().y, pos.y) };
				std::vector<HALF_EDGE::HE_Vertex*> points;
				CreateSeedPoints(points, maxPoint, minPoint);

				std::vector<HALF_EDGE::HE_Face*> fList;
				Delunay::Triangulate(points, fList);
				std::vector<Polygon> polys;
				CreateVoronoiPolyBodies(points, fList, polys);

				for (auto &point : points)
				{
					delete point;
				}
				points.clear();
				boxes.insert(boxes.end(), polys.begin(), polys.end());
			}
		}
		else if(tools.currentTool() == Tool::SelectBox)
		{
			sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second), view);

			std::vector<int> indices;
			for (size_t i = 0; i < boxes.size(); i++)
				indices.push_back(i);
			std::vector<int> intersections;
			dac_bp_Test(boxes, indices.begin(), indices.end(), intersections, { pos.x,pos.y });

			if (!intersections.empty())
			{
				tools.setSelectedObject(&boxes[intersections[0]]);
				tools.createObjectSelectedBorder();
				/*auto float2Conversion = [](float2 value){ return sf::Vector2f(value[0], value[1]); };
				selectedObject = tools.objectSelected(float2Conversion(boxes[intersections[0]].AABB_lowerBounds), float2Conversion(boxes[intersections[0]].AABB_upperBounds));*/
			}
		}
		holding = false;
	}, sf::Mouse::Button::Left, InputMapping::KeyState::Released);

	while (window.isOpen())
	{
		EventHandler::Instance()->handleEvents(window);
		
		const std::pair<int, int> mousePos = IM_Instance->getMousePos();
		if (IM_Instance->isMouseMoving())
		{
			const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second));
			if (holding)
			{
				int offset = pow(10, tools.grid.zoomLevel - 2);
				sf::Vector2f pos2 = sf::Vector2f(newPos.x / offset, newPos.y / offset);
				selection = tools.selectionBox(sf::Vector2f(round(pos2.x)*offset, round(pos2.y)*offset));
			}

			// Ignore mouse movement unless a button is pressed (see above)
			if (moving)
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
		if (IM_Instance->isMouseScrolling() && !moving) // Ignore the mouse wheel unless we're not moving
		{
			const float mWSD = IM_Instance->getMouseScrollDelta();
			// Determine the scroll direction and adjust the zoom level
			// Again, you can swap these to invert the direction
			if (mWSD <= -1)
				zoom = std::min(1000.f, zoom + tools.grid.zoomLevel);
			else if (mWSD >= 1)
				zoom = std::max(10.0f, zoom - tools.grid.zoomLevel);

			// Update our view
			view.setSize(window.getDefaultView().getSize()); // Reset the size
			view.zoom(zoom); // Apply the zoom level (this transforms the view)
			window.setView(view);
		}

		static bool selectable[2]{ true, false };
		bool popped[2] = { false };
		ImGui::SFML::Update(window, deltaClock.getElapsedTime());
		ImGui::Begin("Hello, world!");
		ImGui::Text("MousePos = %f, %f", window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second)).x, window.mapPixelToCoords(sf::Vector2i(mousePos.first, mousePos.second)).y);
		
		ImGui::PushStyleColor(ImGuiCol_Button, { 1,0,0,1 });
		if (selectable[1])
		{
			ImGui::PopStyleColor();
			popped[0] = true;
		}
		if (ImGui::Button("Select Box"))
		{
			// do stuff
			selectable[0] = true;
			selectable[1] = false;
			tools.currentTool(Tool::SelectBox);
		}
		if (!popped[0])
			ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Button, { 1,0,0,1 });
		if (selectable[0])
		{
			ImGui::PopStyleColor();
			popped[1] = true;
		}
		if (ImGui::Button("Create Rectangle"))
		{ 
			selectable[0] = false;
			selectable[1] = true;
			tools.currentTool(Tool::AddBox);
			tools.setSelectedObject(nullptr);
			// do stuff
		}
		if (!popped[1])
			ImGui::PopStyleColor();

		
		ImGui::Checkbox("Physics", &runningPhysics);
		ImGui::Checkbox("Rendering Physics", &renderPhysics);
		ImGui::SliderFloat("Zoom", &zoom, 1.0f, 1000.0f);

		
		ImGui::End();

		if (tools.getSelectedObject() != nullptr)
		{
			Polygon* object = tools.getSelectedObject();
			b2Vec2 pos = object->m_body->GetPosition();
			
			ImGui::Begin("SelectedObject");
			ImGui::SliderFloat("X", &pos.x, -10, 10.0f);
			ImGui::SliderFloat("Y", &pos.y, -10, 10.0f);
			if (pos != object->m_body->GetPosition())
			{
				object->m_body->SetTransform(pos, object->m_body->GetAngle());
				object->m_body->SetAwake(true);
			}
			ImGui::End();
		}
		
		// Update our view
		view.setSize(window.getDefaultView().getSize()); // Reset the size
		view.zoom(zoom); // Apply the zoom level (this transforms the view)
		window.setView(view);

		timer += deltaClock.restart().asSeconds();
		if (runningPhysics && timer >= timeStep)
		{
			world.Step(timeStep, velocityIterations, positionIterations);
			timer -= timeStep;
		}
		window.clear({ 0,0,0 });
		tools.grid.updateGrid(view);
		tools.updateCurrentTool();
		window.draw(tools.grid);
		
	/*	for (auto& polygon : polygons)
		{
			polygon.update();
		}*/
		window.draw(plane);
		
		/*for (int i = 0; i < polygons.size(); i++)
		{
			window.draw(polygons[i]);
		}*/
		
		for (auto& box : boxes)
		{
			box.update();
			window.draw(box);
		}
		window.draw(selection);
		window.draw(tools.getSelectedObjectBorder());
		if(renderPhysics)
			world.DrawDebugData();

		/*for (int i = 0; i < polygons.size(); i++)
		{
			b2Vec2 localCenter = polygons[i].m_body->GetPosition();
			sf::CircleShape cent;
			cent.setRadius(200);
			cent.setPosition(sf::Vector2f(localCenter.x, localCenter.y));
			window.draw(cent);
		}*/
		ImGui::SFML::Render(window);
		window.display();
	}

	
	ImGui::SFML::Shutdown();
}

void InputActions()
{
	InputMapping* IM_Instance = InputMapping::Instance();
	if (!IM_Instance->EmptyKeyPressTransistion())
	{
		sf::Event::KeyEvent trans = InputMapping::Instance()->getKeyPressTransition(sf::Keyboard::Escape);

		if (IM_Instance->getKeyPressTransition(sf::Keyboard::Escape).code != sf::Keyboard::Unknown)
			window.close();

		if (IM_Instance->getKeyPressTransition(sf::Keyboard::Num1).code != sf::Keyboard::Unknown)
		{
		}
		if (IM_Instance->getKeyPressTransition(sf::Keyboard::Num2).code != sf::Keyboard::Unknown)
		{
		}
	}
}

void Render(sf::RenderWindow& window, std::vector<sf::Vector2f>& points, std::vector<b2Vec2>& allVertices)
{
	bool pause = false;
	double timer = 0;
	sf::Event event;
	while (timer < 3)
	{
		auto start = std::chrono::system_clock::now();
		window.clear();
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				else if (event.key.code == sf::Keyboard::Space)
				{
					pause = 1;
					window.setTitle("Paused: " + std::to_string(pause));
				}
				else if (event.key.code == sf::Keyboard::BackSpace)
				{
					pause = 0;
					window.setTitle("Paused: " + std::to_string(pause));
					start = std::chrono::system_clock::now();
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					timer = 3;
				}
				break;
			}
		}
		if (!pause)
		{
			for (auto &point : allVertices)
			{
				sf::CircleShape vertex;
				vertex.setRadius(500);
				vertex.setPosition(sf::Vector2f(point.x,point.y) - sf::Vector2f(500, 500));
				vertex.setFillColor({ 0,255,0 });
				window.draw(vertex);
			}
			std::vector<sf::Vertex> test;
			for (auto &point : points)
			{
				sf::Vertex vert;
				vert.position = point;
				test.push_back(vert);

				sf::CircleShape vertex;
				vertex.setRadius(500);
				vertex.setPosition(point - sf::Vector2f(500, 500));
				vertex.setFillColor({ 255,0,0 });
				window.draw(vertex);
			}
			window.draw(test.data(), test.size(), sf::LineStrip);
			sf::Vertex vert[2];
			vert[0].position = test.back().position;
			vert[1].position = test.front().position;

			window.draw(vert, 2, sf::Lines);
			window.display();
			std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
			timer += elapsed_seconds.count();
		}

	}
	timer = 0;
}

void CreateVoronoiPolyBodies(std::vector<HALF_EDGE::HE_Vertex*>& _BWpoints, std::vector<HALF_EDGE::HE_Face*>& _faceList, std::vector<Polygon>& _polygons)
{
	/*sf::ContextSettings settings(0, 0, 16);
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Paused: 0", sf::Style::Default, settings);
	float zoom = 70;

	sf::View view = window.getDefaultView();
	view.setCenter(sf::Vector2f(14000, 14000));
	view.zoom(zoom);
	window.setView(view);*/

	std::vector<VoronoiEdge*> allVoronoiEdges;
	for (size_t i = 0; i < _faceList.size(); i++)
	{
		HALF_EDGE::HE_Edge* e1 = _faceList[i]->edge;
		HALF_EDGE::HE_Edge* e2 = e1->next;
		HALF_EDGE::HE_Edge* e3 = e2->next;

		sf::Vector2f* v1 = e1->vert->point;
		sf::Vector2f* v2 = e2->vert->point;
		sf::Vector2f* v3 = e3->vert->point;

		sf::Vector2f voronoiVertex = _faceList[i]->circumCenter;

		addVoronoiEdge(e1, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e2, voronoiVertex, allVoronoiEdges);
		addVoronoiEdge(e3, voronoiVertex, allVoronoiEdges);
	}

	std::vector<VoronoiCell> voronoiCells;
	voronoiCells.reserve(_BWpoints.size());
	for (int i = 0; i < allVoronoiEdges.size(); i++)
	{
		VoronoiEdge* e = allVoronoiEdges[i];

		int index = -1;
		//Find the position in the list of all cells that includes this site
		for (int k = 0; k < voronoiCells.size(); k++)
		{
			if (e->site == voronoiCells[k].site)
			{
				index = k;
				break;
			}
		}

		int cellPos = index;
		//No cell was found so we need to create a new cell
		if (cellPos == -1)
		{
			VoronoiCell newCell(e->site);

			voronoiCells.push_back(newCell);

			voronoiCells.back().edges.push_back(e);
		}
		else
		{
			voronoiCells[cellPos].edges.push_back(e);
		}
	}


	
	std::vector<std::vector<b2Vec2>> physicsPolygons;
	physicsPolygons.reserve(_BWpoints.size());
	_polygons.resize(_BWpoints.size());
	int counter = 0;

	//Get AABB
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	for (int i = 0; i < _BWpoints.size()-4;i++)
	{
		minX = std::min(minX, _BWpoints[i]->point->x);
		maxX = std::max(maxX, _BWpoints[i]->point->x);
		minY = std::min(minY, _BWpoints[i]->point->y);
		maxY = std::max(maxY, _BWpoints[i]->point->y);
	}

	const float dx = maxX - minX;
	const float dy = maxY - minY;
	const float deltaMax = std::max(dx, dy);
	const float midx = (minX + maxX)*0.5f;
	const float midy = (minY + maxY)*0.5f;

	float halfwidthX = (dx*0.5f) + 1.0f;
	float halfwidthY = (dy*0.5f) + 1.0f;

	for (auto &cell : voronoiCells)
	{
		sf::Color color(rand() % 200, rand() % 200, rand() % 200);
		std::map<std::pair<int, int>, sf::Vector2f> box2DVertices;
		std::vector<b2Vec2> physicsVertices;
		Polygon polygon(world);
		sf::Vector2f polyCenter;
		int vertCount = 0;
		for (int i = 0; i < cell.edges.size(); i++)
		{
			sf::Vector2f p3(cell.edges[i]->v1);
			sf::Vector2f p2(cell.edges[i]->v2);
			sf::Vector2f p1(*cell.site);
			sf::Vertex test[3];
			test[0] = p1;
			test[0].color = color;
			test[1] = p2;
			test[1].color = color;
			test[2] = p3;
			test[2].color = color;

			sf::Vector2f v1Result;
			sf::Vector2f v2Result;
			bool draw = true;
			//float halfwidth = float(MAXSIZE / 2) + 1.0f;
			

			/*CENTER - halfwidth, CENTER + halfwidth,
			CENTER + halfwidth, CENTER - halfwidth*/

			/*midx - halfwidthX, midx + halfwidthX,
			midy + halfwidthY, midy - halfwidthY,*/
			if (LiangBarsky(midx - halfwidthX, midx + halfwidthX,
				midy + halfwidthY, midy - halfwidthY,
				test[1].position, test[2].position, v1Result, v2Result))
			{
				sf::Vector2f v1ResultN;
				sf::Vector2f v2ResultN;
				if (v1Result != test[1].position)
				{
					if (LiangBarsky(midx - halfwidthX, midx + halfwidthX,
						midy + halfwidthY, midy - halfwidthY,
						test[0].position, test[1].position, v1ResultN, v2ResultN))
					{
						sf::Vertex nVertex;
						nVertex = p1;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v1Result;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v2ResultN;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						polyCenter += v1Result;
						polyCenter += v2ResultN;
						vertCount += 2;

						physicsVertices.push_back(b2Vec2((v1Result.x / pixelToMeterRatio), (v1Result.y / pixelToMeterRatio)));
						physicsVertices.push_back(b2Vec2((v2ResultN.x / pixelToMeterRatio), (v2ResultN.y / pixelToMeterRatio)));
					}


				}
				if (v2Result != test[2].position)
				{
					if (LiangBarsky(midx - halfwidthX, midx + halfwidthX,
						midy + halfwidthY, midy - halfwidthY,
						test[0].position, test[2].position, v1ResultN, v2ResultN))
					{
						sf::Vertex nVertex;
						nVertex = p1;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v2Result;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						nVertex = v2ResultN;
						nVertex.color = color;
						polygon.m_vertices.push_back(nVertex);

						polyCenter += v2Result;
						polyCenter += v2ResultN;
						vertCount += 2;

						physicsVertices.push_back(b2Vec2((v2Result.x / pixelToMeterRatio), (v2Result.y / pixelToMeterRatio)));
						physicsVertices.push_back(b2Vec2((v2ResultN.x / pixelToMeterRatio), (v2ResultN.y / pixelToMeterRatio)));
					}


				}

				test[1].position = v1Result;
				test[2].position = v2Result;
			}
			else
			{
				bool passed = false;
				if (LiangBarsky(midx - halfwidthX, midx + halfwidthX,
					midy + halfwidthY, midy - halfwidthY,
					test[0].position, test[2].position, v1Result, v2Result))
				{
					test[0].position = v1Result;
					test[2].position = v2Result;
					passed = true;
				}
				if (LiangBarsky(midx - halfwidthX, midx + halfwidthX,
					midy + halfwidthY, midy - halfwidthY,
					test[0].position, test[1].position, v1Result, v2Result))
				{
					test[0].position = v1Result;
					test[1].position = v2Result;
					passed = true;
				}
				if (!passed)
					draw = false;
			}

			if (draw)
			{
				polyCenter += test[1].position;
				polyCenter += test[2].position;
				vertCount += 2;
				polygon.m_vertices.push_back(test[0]);
				polygon.m_vertices.push_back(test[1]);
				polygon.m_vertices.push_back(test[2]);
				physicsVertices.push_back(b2Vec2((test[1].position.x / pixelToMeterRatio), (test[1].position.y / pixelToMeterRatio)));
				physicsVertices.push_back(b2Vec2((test[2].position.x / pixelToMeterRatio), (test[2].position.y / pixelToMeterRatio)));
			}
		}

		if (polygon.m_vertices.size() != 0)
		{
			polyCenter.x /= vertCount;
			polyCenter.y /= vertCount;
			polygon.m_center.setPosition(sf::Vector2f(cell.site->x, cell.site->y) - sf::Vector2f(polygon.m_center.getRadius(), polygon.m_center.getRadius()));
			polygon.setOrigin(polyCenter);
			polygon.setPosition(polyCenter);

			physicsPolygons.push_back(physicsVertices);
			_polygons[counter] = polygon;
			counter++;
		}
	}

	_polygons.resize(counter);
	_polygons.shrink_to_fit();
	physicsPolygons.shrink_to_fit();
	for (auto &physPol : physicsPolygons)
	{
		std::vector<bool> remove(physPol.size(), false);
		{
			for (size_t i = 0; i < physPol.size(); i++)
			{
				for (size_t k = i; k < physPol.size(); k++)
				{
					if (i == k) {
						continue;
					}
					if (physPol[i] == physPol[k])
					{
						remove[k] = true;
					}
				}
			}
			auto is_duplicate = [&](auto const& e) { return remove[&e - &physPol[0]]; };
			erase_where(physPol, is_duplicate);
			remove.clear();
		}
	}
	counter = 0;

	auto less = [](const b2Vec2 & a, const b2Vec2 & b)
	{
		return a.x < b.x || (a.x == b.x && a.y > b.y);
	};

	auto cross = [](const b2Vec2 &O, const b2Vec2 &A, const b2Vec2 &B)
	{
		return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
	};

	for (auto& polygon : _polygons)
	{
		size_t n = polygon.m_vertices.size(),  k = 0;
		
		std::vector<b2Vec2> points(polygon.m_vertices.size());
		for (size_t i = 0; i < n; i++)
		{
			points[i] = b2Vec2(polygon.m_vertices[i].position.x, polygon.m_vertices[i].position.y);
		}

		{
			std::vector<bool> remove(points.size(), false);
			for (size_t i = 0; i < points.size(); i++)
			{
				for (size_t k = i; k < points.size(); k++)
				{
					if (i == k) {
						continue;
					}
					if (points[i] == points[k])
					{
						remove[k] = true;
					}
				}
			}
			auto is_duplicate = [&](auto const& e) { return remove[&e - &points[0]]; };
			erase_where(points, is_duplicate);
			remove.clear();
		}
		
		std::sort(points.begin(), points.end(), less);
		
		//{
		//	std::vector<bool> remove(points.size(), false);
		//	for (int i = 0; i < points.size() - 1; i++)
		//	{
		//		if (b2DistanceSquared(points[i], points[i + 1]) < ((0.5f * b2_linearSlop) * (0.5f * b2_linearSlop)))
		//		{
		//			remove[i] = true;
		//		}
		//	}
		//	auto is_too_close = [&](auto const& e) { return remove[&e - &points[0]]; };
		//	erase_where(points, is_too_close);
		//	remove.clear();
		//}
		n = points.size();
		std::vector<b2Vec2> hull(n * 2);
		for (size_t i = 0; i < n; ++i) {
			while (k >= 2 && cross(hull[k - 2], hull[k - 1], points[i]) <= 0) k--;
			hull[k++] = points[i];
		}

		for (size_t i = n - 1, t = k + 1; i > 0; --i) {
			while (k >= t && cross(hull[k - 2], hull[k - 1], points[i - 1]) <= 0) k--;
			hull[k++] = points[i - 1];
		}
		hull.resize(k-1);

		//{
		//	std::vector<bool> remove(hull.size(), false);
		//	for (size_t i = 0; i < hull.size(); i++)
		//	{
		//		for (size_t k = i; k < hull.size(); k++)
		//		{
		//			if (i == k) {
		//				continue;
		//			}
		//			if (hull[i] == hull[k])
		//			{
		//				remove[k] = true;
		//			}
		//		}
		//	}
		//	auto is_duplicate = [&](auto const& e) { return remove[&e - &hull[0]]; };
		//	erase_where(hull, is_duplicate);
		//	remove.clear();
		//}
		//{
		//	std::vector<bool> remove(hull.size(), false);
		//	for (int i = 0; i < hull.size() - 1; i++)
		//	{
		//		if (b2DistanceSquared(hull[i], hull[i + 1]) < ((0.5f * b2_linearSlop) * (0.5f * b2_linearSlop)))
		//		{
		//			remove[i] = true;
		//		}
		//	}
		//	auto is_too_close = [&](auto const& e) { return remove[&e - &hull[0]]; };
		//	erase_where(hull, is_too_close);
		//	remove.clear();
		//}

		


		std::vector<sf::Vector2f> sfHull(hull.size());
		for (size_t i = 0; i < hull.size(); i++)
		{
			//sfHull[i] = reinterpret_cast<sf::Vector2f&>(hull[i]);
			hull[i].x /= pixelToMeterRatio;
			hull[i].y /= pixelToMeterRatio;
			//sfHull[i].x *= pixelToMeterRatio;
			//sfHull[i].y *= pixelToMeterRatio;
			//b2Hull[i].x /= pixelToMeterRatio;
			//b2Hull[i].y /= pixelToMeterRatio;
		}
		//Render(window, sfHull, points);
		polygon.createPhysicsBody(hull);
		counter++;
	}

	

	for (auto &face : _faceList)
	{
		delete face;
	}
	_faceList.clear();
	for (auto &edge : allVoronoiEdges)
	{
		delete edge;
	}
	allVoronoiEdges.clear();
	voronoiCells.clear();
}


// Divide and Conquer Broadphase
void dac_bp_Test(const std::vector<Polygon>& bounds, const std::vector<int>::iterator start, const std::vector<int>::iterator end, std::vector<int>& intersections , float2 point)
{
	const int BRUTE_FORCE_THRESH = 32;

	if ((end - start) < BRUTE_FORCE_THRESH)
	{
		// Brute force check the remaining pairs in this interval
		for (auto i = start; i != end; i++) //Update To do a more fine grained intersection (Convex Hull)
		{
			if ((bounds[*i].AABB_lowerBounds[0] < point[0] && point[0] < bounds[*i].AABB_upperBounds[0]) && (bounds[*i].AABB_lowerBounds[1] < point[1] && point[1] < bounds[*i].AABB_upperBounds[1]))
			{
				intersections.push_back(*i);
			}
		}
	}
	else
	{
		// Compute bounds of all boxes in this interval
		float2 pmin = { FLT_MAX, FLT_MAX }, pmax = { -FLT_MAX, -FLT_MAX };
		for (auto i = start; i != end; i++)
		{
			pmin = std::min(pmin, bounds[*i].AABB_lowerBounds);
			pmax = std::max(pmax, bounds[*i].AABB_upperBounds);
		}

		// Choose the partition axis and partition location
		float2 size = { pmax[0] - pmin[0], pmax[1] - pmin[1] };
		int axis = (size[1] > size[0]);
		float split = (pmin[axis] + pmax[axis]) * 0.5f;

		// Partition boxes left and recurse
		auto mid_left = partition(start, end, [split, axis, bounds](int i) { return bounds[i].AABB_lowerBounds[axis] <= split; });

		dac_bp_Test(bounds, start, mid_left, intersections, point);

		// Partition boxes right and recurse
		auto mid_right = partition(start, end, [split, axis, bounds](int i) { return bounds[i].AABB_upperBounds[axis] >= split; });

		dac_bp_Test(bounds, start, mid_right, intersections, point);
	}
}


// USAGE
// Build a vector containing bounds of all objects
//std::vector<Polygon> bounds = { /* fill with bounds of objects */ };
//
//// Initialize indices
//std::vector<int> indices;
//for (size_t i = 0; i < bounds.size(); i++)
//	indices.push_back(i);
//
//// Get all colliding pairs!
//std::set<std::pair<int, int>> pairs;
//dac_bp(bounds, indices.start(), indices.end(), pairs);