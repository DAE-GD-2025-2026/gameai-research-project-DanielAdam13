#include "JPSMouseCommand.h"
#include "JPSApp.h"

jps::JPSAppMouseCommand::JPSAppMouseCommand(JPSApp* app)
	:m_pApp{ app }
{
}

bool jps::JPSAppMouseCommand::MouseToCell(float mouseX, float mouseY, int& outX, int& outY) const
{
	if (!m_pApp)
		return false;

	const auto layout{ m_pApp->GetLayout() };
	const float relativeX{ mouseX - layout.originX };
	const float relativeY{ mouseY - layout.originY };

	if (relativeX < 0.f || relativeY < 0.f)
		return false;

	const int cellX{ static_cast<int>(relativeX / layout.cellSize) };
	const int cellY{ static_cast<int>(relativeY / layout.cellSize) };

	if (!m_pApp->GetGrid().InBounds(cellX, cellY))
		return false;

	outX = cellX;
	outY = cellY;
	return true;
}
