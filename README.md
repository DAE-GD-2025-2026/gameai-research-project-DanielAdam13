Created a Grid Renderer Component and attached it to a dedicated single game object pushed and updated on a scene.
-------------------------------------
The Grid Renderer only used the Render() method and inside it:
1. Draws background
2. Draws walls as filled rectangles
3. Draws expanded cells
4. Draws all grid lines
5. DRAWS THE PATH LINE
6. Draws filled rects for start and goal
-------------------------------------
The JPS app creates and assigns the grid, start and goal to the Render Component and computes the path.
This happens ONCE by simply building a test level with walls and calling the ASTAR ALGORITHM foe the path calculation


Created a base JPS mouse command class from which specific JPS game command are derived. OnClick() is the thing that defines them.
-------------------------------------
In JPSApp:
1. Created a GridLayout struct that holds origin and cell size info. This is needed for the mouse commands.
2. Created public setting methods which will be toggled by ImGui.
3. Created an empty GO playing the role of an input target for all commands.


Implemented a MouseToCell() protected method in the base JPS Mouse Command that gets mouse input coordinates and returns the x/y coordinates in the array for a Cell.
Method also returns if this succeeded -> false if cell is NOT in bounds of the GRID.