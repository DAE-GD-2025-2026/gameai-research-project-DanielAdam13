#pragma once
#include "JPSMouseCommand.h"
#include "Commands/MouseCommand.h"

namespace jps
{
    // Left-click: toggle the wall state of the clicked cell
	class ToggleWallCommand final : public JPSAppMouseCommand
	{
	public:
		ToggleWallCommand(JPSApp* app)
            :JPSAppMouseCommand::JPSAppMouseCommand(app)
        {
        }

	protected:
		virtual void OnClick(float mouseX, float mouseY, float deltaTime) override;
		
	};

    // Right-click: move the start cell
    class SetStartCommand final : public JPSAppMouseCommand
    {
    public:
        SetStartCommand(JPSApp* app)
            :JPSAppMouseCommand::JPSAppMouseCommand(app)
        {
        }

    protected:
        void OnClick(float mouseX, float mouseY, float deltaTime) override;
    };

    // Middle-click: move the goal cell.
    class SetGoalCommand final : public JPSAppMouseCommand
    {
    public:
        SetGoalCommand(JPSApp* app)
            :JPSAppMouseCommand::JPSAppMouseCommand(app)
        {
        }

    protected:
        void OnClick(float mouseX, float mouseY, float deltaTime) override;
    };

}