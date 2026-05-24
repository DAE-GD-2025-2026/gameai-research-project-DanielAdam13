#include "JPSApp.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"
#include "Visualization/GridRendererComponent.h"
#include "JPSControlsImGui.h"

#include "Commands/JPSCommands.h"

#include <utility>
#include <vector>

namespace jps
{
	namespace
	{
		constexpr int kGridWidth{ 30 };
		constexpr int kGridHeight{ 30 };
	}
}

using namespace jps;

void JPSApp::Load()
{
	// 1. Build the Grid
	m_Grid = std::make_unique<Grid>(kGridWidth, kGridHeight);
	BuildTestGrid();

	// 2. Create scene + Game Object holding the Grid Renderer
	ge::Scene& scene{ ge::SceneManager::GetInstance().CreateScene("JPSDemo") };
	ge::SceneManager::GetInstance().SwitchToSceneWithName("JPSDemo");

	auto rendererObject = std::make_unique<ge::GameObject>("GO_GridRenderer");
	m_GridRenderer = rendererObject->AddComponent<GridRendererComponent>(rendererObject.get());
	m_GridRenderer->SetLayout(m_Layout.originX, m_Layout.originY, m_Layout.cellSize);
	m_GridRenderer->SetGrid(m_Grid.get());
	m_GridRenderer->SetStart(m_Start);
	m_GridRenderer->SetGoal(m_Goal);
	scene.Add(std::move(rendererObject));

	// 3. ImGui control panel.
	scene.AddImGuiScene(std::make_unique<JPSControlsImGui>(this));

	// 4. Create the input target Game Object. It's just a handle so we can call Unbind later...
	auto inputObject = std::make_unique<ge::GameObject>("GO_JPSInputTarget");
	m_InputTarget = inputObject.get();
	scene.Add(std::move(inputObject));

	BindInputCommands(); // !!!

	// 5. Run the initial search and hand the result to the renderer
	RecomputePath();

}

void jps::JPSApp::ToggleWall(Cell c)
{
	if (!m_Grid->InBounds(c))
		return;

	// If walkable -> become wall, if wall -> become walkable
	const bool currentlyWalkable{ m_Grid->IsWalkable(c) };
	m_Grid->SetBlocked(c, currentlyWalkable);

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::SetStartCell(Cell c)
{
	m_Start = c;
	if (m_GridRenderer)
		m_GridRenderer->SetStart(c);

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::SetGoalCell(Cell c)
{
	m_Goal = c;
	if (m_GridRenderer)
		m_GridRenderer->SetGoal(c);

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::SetHeuristic(heuristics::Fn h)
{
	m_AStar.SetHeuristic(h);

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::SetAllowCornerCutting(bool allow)
{
	m_AStar.SetAllowCornerCutting(allow);

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::SetShowExpandedCells(bool show)
{
	m_ShowExpandedCells = show;

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::GenerateRandomWalls(int densityPercent)
{

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::ClearWalls()
{
	if (!m_Grid) return;

	const int W{ m_Grid->GetWidth() };
	const int H{ m_Grid->GetHeight() };

	// Just set every single cell to walkable
	for (int y{}; y < H; ++y)
		for (int x{}; x < W; ++x)
			m_Grid->SetBlocked(x, y, false);

	// --------------------------------
	// Recompute!
	// --------------------------------
	RecomputePath();
}

void jps::JPSApp::BuildTestGrid()
{
	for (int y{ 0 }; y < 20; ++y) m_Grid->SetBlocked(15, y, true);
	for (int x{ 5 }; x < 25; ++x) m_Grid->SetBlocked(x, 22, true);
	m_Grid->SetBlocked(25, 25, true);
	m_Grid->SetBlocked(26, 25, true);
	m_Grid->SetBlocked(25, 26, true);
}

void jps::JPSApp::BindInputCommands()
{
	auto& input{ ge::ServiceLocator::GetInputManager() };

	// Left: wall
	// Right: start
	// Middle: goal
	input.BindMouseCommand(ge::InputManager::MouseButton::Left,
		std::make_unique<ToggleWallCommand>(this));
	input.BindMouseCommand(ge::InputManager::MouseButton::Right,
		std::make_unique<SetStartCommand>(this));
	input.BindMouseCommand(ge::InputManager::MouseButton::Middle,
		std::make_unique<SetGoalCommand>(this));
}

void jps::JPSApp::RecomputePath()
{
	if (!m_Grid || !m_GridRenderer) 
		return;

	const SearchResult result{ m_AStar.FindPath(*m_Grid, m_Start, m_Goal) };

	m_LastStats = result.stats;
	m_LastSearchFound = result.Found();

	m_GridRenderer->SetPath(result.path);
	m_GridRenderer->SetExpandedCells(m_ShowExpandedCells ? result.expandedCells : std::vector<Cell>{});
}
