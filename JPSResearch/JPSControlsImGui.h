#pragma once
#include "ImGuiScene.h"
#include "Pathfinding/Heuristics.h"

namespace jps
{
	class JPSApp;

	class JPSControlsImGui final : public ge::ImGuiInstance
	{
	public:
		explicit JPSControlsImGui(JPSApp* app);
		~JPSControlsImGui() override = default;

		void UpdateImGui(float) override {}
		void RenderImGui() override;

	private:
		JPSApp* m_App; // Cached ref

		int m_AlgorithmIndex{ 0 };  // 0 = A*, 1 = JPS

		int m_HeuristicIndex{ 2 };  // 0=Manhattan 1=Euclidean 2=Octile 3=Chebyshev
		bool m_AllowCornerCutting{ false };
		bool m_ShowExpanded{ true };
		bool m_InterpolateJPSPath{ false };

		// For the regenerate grid controls
		int m_RandomDensityPct{ 25 }; 
	};

	namespace imguiHelpers
	{
		static inline constexpr const char* kAlgorithmNames[] = { "A*", "JPS" };

		static inline constexpr const char* kHeuristicNames[] = { "Manhattan", "Euclidean", "Octile", "Chebyshev" };

		heuristics::Fn HeuristicForIndex(int i) noexcept;
	}
}