#include "JPSMouseCommand.h"

jps::JPSAppMouseCommand::JPSAppMouseCommand(JPSApp* app)
	:m_pApp{ app }
{
}

bool jps::JPSAppMouseCommand::MouseToCell(float mouseX, float mouseY, int& outX, int& outY) const
{
	if (!m_pApp)
		return false;

	
}
