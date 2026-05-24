#include "JPSCommands.h"
#include "JPSApp.h"
#include "Pathfinding/Grid.h"

void jps::ToggleWallCommand::OnClick(float mouseX, float mouseY, float)
{
	int cx;
	int cy;

	if (!MouseToCell(mouseX, mouseY, cx, cy))
		return;

	const Cell clicked{ cx, cy };

	// Don't allow toggling the start/goal cell into a wall
	if (clicked == m_pApp->GetStart() || clicked == m_pApp->GetGoal())
		return;

	m_pApp->ToggleWall(clicked);
}

void jps::SetStartCommand::OnClick(float mouseX, float mouseY, float)
{
	int cx;
	int cy;
	
	if (!MouseToCell(mouseX, mouseY, cx, cy))
		return;

	const Cell clicked{ cx, cy };

	// Can't start in a wall
	if (!m_pApp->GetGrid().IsWalkable(clicked)) 
		return;  

	// Start shouldn't be goal
	if (clicked == m_pApp->GetGoal()) 
		return;        

	m_pApp->SetStartCell(clicked);
}

void jps::SetGoalCommand::OnClick(float mouseX, float mouseY, float)
{
	int cx;
	int cy;

	if (!MouseToCell(mouseX, mouseY, cx, cy))
		return;

	const Cell clicked{ cx, cy };

	// Can't start in a wall
	if (!m_pApp->GetGrid().IsWalkable(clicked))
		return;

	// Start shouldn't be goal
	if (clicked == m_pApp->GetStart())
		return;

	m_pApp->SetGoalCell(clicked);
}
