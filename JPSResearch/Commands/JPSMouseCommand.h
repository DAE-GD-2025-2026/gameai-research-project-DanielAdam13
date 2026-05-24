#pragma once
#include "Commands/MouseCommand.h"

namespace jps
{
	class JPSApp;

	class JPSAppMouseCommand : public ge::MouseCommand
	{
	public:
		explicit JPSAppMouseCommand(JPSApp* app);

	protected:
		JPSApp* m_pApp; // Non-owning ref

		// Convert pixel coords to a cell. Returns false if outside the grid.
		bool MouseToCell(float mouseX, float mouseY, int& outX, int& outY) const;

	private:
	};
}