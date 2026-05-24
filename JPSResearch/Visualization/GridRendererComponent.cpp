#include "GridRendererComponent.h"

#include "Renderer.h"

namespace jps
{
    namespace
    {
        // Colors
        constexpr SDL_Color kBackground{ 245, 245, 248, 255 };
        constexpr SDL_Color kGridLine{ 210, 210, 215, 255 };
        constexpr SDL_Color kWall{ 40,  40,  50, 255 };
        constexpr SDL_Color kExpanded{ 180, 200, 255, 140 };
        constexpr SDL_Color kPath{ 60, 140, 240, 255 };
        constexpr SDL_Color kStart{ 60, 200,  90, 255 };
        constexpr SDL_Color kGoal{ 230,  80,  80, 255 };
    }
}

jps::GridRendererComponent::GridRendererComponent(ge::GameObject* owner)
    :Component::Component(owner)
{
}

void jps::GridRendererComponent::RenderComponent() const
{
    if (!m_pGrid)
        return;

    SDL_Renderer* r{ ge::Renderer::GetInstance().GetSDLRenderer() };
    if (!r)
        return;

    const int width{ m_pGrid->GetWidth() };
    const int height{ m_pGrid->GetHeight() };

    // --- 1. Background panel behind the grid ---
    SDL_FRect panel{
        m_OriginX, m_OriginY,
        static_cast<float>(width) * m_CellSize, static_cast<float>(height) * m_CellSize
    };
    SDL_SetRenderDrawColor(r, kBackground.r, kBackground.g, kBackground.b, kBackground.a);
    SDL_RenderFillRect(r, &panel);

    // --- 2. Walls ---
    for (int y{}; y < height; ++y)
    {
        for (int x{}; x < width; ++x)
        {
            if (!m_pGrid->IsWalkable(x, y))
            {
                // Filled
                DrawCell(r, x, y, kWall, true);
            }
        }
    }

    // --- 3. Expanded Cells ---
    /*for (const Cell& c : m_Expanded)
    {
        DrawCell(r, c.x, c.y, kExpanded, true);
    }*/

    // --- 4. Grid lines ---
    SDL_SetRenderDrawColor(r, kGridLine.r, kGridLine.g, kGridLine.b, kGridLine.a);
    for (int x{}; x <= width; ++x)
    {
        const float px{ m_OriginX + static_cast<float>(x) * m_CellSize };
        SDL_RenderLine(r, 
            px, m_OriginY, px, m_OriginY + static_cast<float>(height) * m_CellSize);
    }
    for (int y{}; y <= height; ++y)
    {
        const float py{ m_OriginY + static_cast<float>(y) * m_CellSize };
        SDL_RenderLine(r, 
            m_OriginX, py, m_OriginX + static_cast<float>(width) * m_CellSize, py);
    }

    // --- 5. Path line through cell centres ---
    DrawPathLine(r);

    // --- 6. Start and goal markers ---
    if (m_HasStart && m_pGrid->InBounds(m_Start))
    {
        DrawCell(r, m_Start.x, m_Start.y, kStart, true);
    }
    if (m_HasGoal && m_pGrid->InBounds(m_Goal))
    {
        DrawCell(r, m_Goal.x, m_Goal.y, kGoal, true);
    }  
}

void jps::GridRendererComponent::DrawCell(SDL_Renderer* r, int x, int y, SDL_Color color, bool filled) const
{
    SDL_FRect rect{
    m_OriginX + static_cast<float>(x) * m_CellSize,
    m_OriginY + static_cast<float>(y) * m_CellSize,
    m_CellSize,
    m_CellSize };
    
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    if (filled)
        SDL_RenderFillRect(r, &rect);
    else
        SDL_RenderRect(r, &rect);

}

void jps::GridRendererComponent::DrawPathLine(SDL_Renderer* r) const
{
    if (m_Path.size() < 2) 
        return;

    SDL_SetRenderDrawColor(r, kPath.r, kPath.g, kPath.b, kPath.a);

    // Draw a line through the centre of each cell in the path.
    const float half{ m_CellSize * 0.5f };
    for (size_t i{ 1 }; i < m_Path.size(); ++i)
    {
        const Cell& a{ m_Path[i - 1] };
        const Cell& b{ m_Path[i] };

        const float ax{ m_OriginX + static_cast<float>(a.x) * m_CellSize + half };
        const float ay{ m_OriginY + static_cast<float>(a.y) * m_CellSize + half };
        const float bx{ m_OriginX + static_cast<float>(b.x) * m_CellSize + half };
        const float by{ m_OriginY + static_cast<float>(b.y) * m_CellSize + half };

        SDL_RenderLine(r, ax, ay, bx, by);
    }
}
