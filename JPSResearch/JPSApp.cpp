#include "JPSApp.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Visualization/GridRendererComponent.h"

#include <utility>

namespace jps
{
	namespace
	{
		constexpr int kGridWidth{ 30 };
		constexpr int kGridHeight{ 30 };
		constexpr float kCellSize{ 24.f };
		constexpr float kOriginX{ 40.f };
		constexpr float kOriginY{ 40.f };
	}
}

using namespace jps;

void JPSApp::Load()
{
	// 1. Build the Grid
	m_Grid = std::make_unique<Grid>(kGridWidth, kGridHeight);
	BuildTestGrid();

	// 2. Create scene + Game Object for the Grid Renderer
	ge::Scene& scene{ ge::SceneManager::GetInstance().CreateScene("JPSDemo") };
	ge::SceneManager::GetInstance().SwitchToSceneWithName("JPSDemo");

	auto rendererObject = std::make_unique<ge::GameObject>("GridRenderer");
	m_pRenderer = rendererObject->AddComponent<GridRendererComponent>(rendererObject.get());
	m_pRenderer->SetLayout(kOriginX, kOriginY, kCellSize);
	m_pRenderer->SetGrid(m_Grid.get());
	m_pRenderer->SetStart(m_Start);
	m_pRenderer->SetGoal(m_Goal);

	scene.Add(std::move(rendererObject));

	// 3. Run the initial search and hand the result to the renderer
	RecomputePath();

}

void JPSApp::Update(float)
{
}

void JPSApp::FixedUpdate(float)
{
}

void jps::JPSApp::BuildTestGrid()
{
	for (int y{}; y < 20; ++y)
	{
		m_Grid->SetBlocked(15, y, true);
	}

	for (int x{5}; x < 25; ++x)
	{
		m_Grid->SetBlocked(x, 22, true);
	}

	m_Grid->SetBlocked(25, 25, true);
	m_Grid->SetBlocked(26, 25, true);
	m_Grid->SetBlocked(25, 26, true);

}

void jps::JPSApp::RecomputePath()
{
	if (!m_Grid || !m_pRenderer) 
		return;

	const SearchResult result{ m_AStar.FindPath(*m_Grid, m_Start, m_Goal) };

	m_pRenderer->SetPath(result.path);
}
