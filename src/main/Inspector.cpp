#include "Inspector.h"
#include "pch.h"

Inspector::Inspector() {}

Inspector::~Inspector() {}

void Inspector::ListLiveEntites(EditorTools& _tools)
{
	// Rounded border
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("EntityList", ImVec2(0, 260), true, ImGuiWindowFlags_MenuBar);
	ImGui::Columns(2);
	if(ImGui::BeginMenuBar())
	{
		ImGui::Text("Entities");
		ImGui::EndMenuBar();
	}
	for(auto const& entity : entities_)
	{
		char buf[32];
		sprintf(buf, "%03d", entity);
		if (ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f)))
		{
			_tools.selected_entity_ = entity;
			_tools.CreateObjectSelectedBorder();
		}
		ImGui::NextColumn();
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void Inspector::InspectEntity(Entity _entity) 
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
	ImGui::Begin("SelectedObject");
	if(coordinator.HasComponent<Live>(_entity))
		ImGui::Text("WE LIVE BOYS");
	if(coordinator.HasComponent<PhysicsObject>(_entity))
		PhysicsObjectInspector::ShowData(_entity);
	if(coordinator.HasComponent<sf::Transformable>(_entity))
		TransformInspector::ShowData(_entity);
	if(coordinator.HasComponent<Mesh2D>(_entity))
		MeshInspector::ShowData(_entity);
	ComponentCreator::ShowComponents(_entity);
	//ImGui::PopStyleVar();
	ImGui::End();
}
