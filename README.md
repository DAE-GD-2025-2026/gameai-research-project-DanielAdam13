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