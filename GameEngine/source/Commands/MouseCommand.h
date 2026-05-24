#pragma once
#include "Command.h"

namespace ge
{
	class MouseCommand : public ge::Command
	{
	public:
		MouseCommand() = default;
		virtual ~MouseCommand() override = default;

		// No derived classes overriding, they should override the protected OnClick method
		void Execute(float deltaTime) final;

	protected:
		virtual void OnClick(float mouseX, float mouseY, float deltaTime) = 0;

	};
}