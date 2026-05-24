#include "JPSControlsImGui.h"
#include "JPSApp.h"
#include "Pathfinding/Heuristics.h"

#include <imgui.h>

using namespace jps;

jps::JPSControlsImGui::JPSControlsImGui(JPSApp* app)
	:m_App{ app }
{
}

void jps::JPSControlsImGui::RenderImGui()
{
	if (!m_App)
		return;

	ImGui::Begin("JPS Demo Controls");

	// -----------------------------------------------------------------
	// Algorithm settings
	// -----------------------------------------------------------------
	ImGui::TextUnformatted("Algorithm");
	ImGui::Separator();

	if (ImGui::Combo("Heuristic", &m_HeuristicIndex, imguiHelpers::kHeuristicNames, IM_ARRAYSIZE(imguiHelpers::kHeuristicNames)))
	{
		m_App->SetHeuristic(imguiHelpers::HeuristicForIndex(m_HeuristicIndex));
	}

	if (ImGui::Checkbox("Allow corner cutting", &m_AllowCornerCutting))
	{
		m_App->SetAllowCornerCutting(m_AllowCornerCutting);
	}

	// -----------------------------------------------------------------
	// Visualization toggles
	// -----------------------------------------------------------------
	ImGui::Spacing();
	ImGui::TextUnformatted("Visualization");
	ImGui::Separator();

	if (ImGui::Checkbox("Show expanded cells", &m_ShowExpanded))
	{
		m_App->SetShowExpandedCells(m_ShowExpanded);
	}

	// -----------------------------------------------------------------
	// Grid manipulation
	// -----------------------------------------------------------------
	ImGui::Spacing();
	ImGui::TextUnformatted("Grid");
	ImGui::Separator();

	ImGui::SliderInt("Random density %", &m_RandomDensityPct, 0, 60);
	
	if (ImGui::Button("Regenerate random walls"))
	{
		m_App->GenerateRandomWalls(m_RandomDensityPct);
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear walls"))
	{
		m_App->ClearWalls();
	}

	// -----------------------------------------------------------------
	// Stats from the last search — pulled live from the app
	// -----------------------------------------------------------------
	ImGui::Spacing();
	ImGui::TextUnformatted("Last Search");
	ImGui::Separator();

	const SearchStats& s{ m_App->GetLastStats() };
	const bool found{ m_App->LastSearchFound() };

	ImGui::Text("Found:      %s", found ? "yes" : "no");
	ImGui::Text("Path length: %d cells", s.pathLength);
	ImGui::Text("Path cost:   %.3f", s.pathCost);
	ImGui::Text("Expanded:    %d", s.nodesExpanded);
	ImGui::Text("Generated:   %d", s.nodesGenerated);

	// -----------------------------------------------------------------
	// Controls Usage help
	// -----------------------------------------------------------------
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Mouse controls"))
	{
		ImGui::BulletText("Left click  - toggle wall");
		ImGui::BulletText("Right click - set start");
		ImGui::BulletText("Middle click - set goal");
	}

	ImGui::End();
}

heuristics::Fn jps::imguiHelpers::HeuristicForIndex(int i) noexcept
{
	switch (i)
	{
	case 0:
		return &heuristics::Manhattan;
	case 1:
		return &heuristics::Euclidean;
	case 2:
		return &heuristics::Octile;
	case 3:
		return &heuristics::Chebyshev;
	default:
		return &heuristics::Octile;
	}
}
