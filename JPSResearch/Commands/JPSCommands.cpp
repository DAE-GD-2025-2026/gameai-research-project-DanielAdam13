#include "JPSCommands.h"
#include "JPSApp.h"

void jps::ToggleWallCommand::OnClick(float mouseX, float mouseY, float deltaTime)
{
	int cx;
	int cy;

	if (!MouseToCell(mouseX, mouseY, cx, cy))
		return;

	const Cell clicked{ cx, cy };

	// Don't allow toggling the start/goal cell into a wall
	if (clicked == m_pApp->GetStart() || clicked == m_pApp->GetGoal())
		return;

	
}
	

void jps::SetStartCommand::OnClick(float mouseX, float mouseY, float deltaTime)
{
}

void jps::SetGoalCommand::OnClick(float mouseX, float mouseY, float deltaTime)
{
}
