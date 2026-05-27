# Introduction to the Idea

Jump Point Search is an algorithm that improves upon normal A* pathfinding.
At its core, A* expands its search by doing the simplest thing possible: adding a node's immediate neighbours to the set of what to examine next.
JPS tries to skip over some nodes that are not valuable to look at directly.



# The Algorithm

At the base of JPS sits the fundamental rules of Neighbour Prunning applied during each search:
One rule is applied to straight steps while the other - for diagonal.
The main idea behind both cases is to remove/prune a set of immediate neighbours around a node by trying to prove
that a PATH from the parent node of the currently one exists to each neighbour while the path does not invlolve the current node.

![Research image](/resources/NeighbourPrunning.png?raw=true "Neighbour Prunning") Neighbour Prunning

Node X is being expanded with the arrow signifying the direction it has been reached from its parent. 
In both cases we can prune/remove the greyed out neighbours as these can be reached optimally from the neighbour without going through X.
The remaining nodes AFTER prunning are called "Natural Neighbours" of the node. These are the ones marked with white on the image above.

    Ideally the cheapest and most efficient approach would be to only process the set of Natural Neighbours but this is not always the case.

When an obstacle is a part of the immediate neighbours, a [0 ≤  k ≤ 2] amount of additional nodes from the neighbour need to be considered.
These new nodes are named "Forced Neighbours" of the current node.

![Research image](/resources/ForcedNeighbours.png?raw=true "Forced Neighbours") Forced Neighbours

On the image above the Black node signifying the obstacle is the reason for the new highlighted in red circle nodes to be called Forced Neighbours.
This is because each optimal path from the parent of X to these nodes is blocked.

    Both rules mentioned above are not generated individually for each node on the path. Instead, we recursively prune the set of neighbours around any such node.
    This is the main idea behind how we JUMP OVER all nodes that can be reached optimally from a certain node(start node or previous Jump Point).

This recursion is stopped when:
- we reach an obstacle
- we find a so called "Jump Point successor"

### Explaining it more into detail:

![Research image](/resources/CoreIdeaNeigbours.png?raw=true "Neighbour Examination") Neighbour Examination

The Green Node is the current node we want to expand. Looking back, let's say that the arrow coming horiontally behind it is 
the direction it has been reached from its parent(0). The nodes (1) and (2) are immediate neighbours of the green node that we
can immediately see that are not worth checking since they can be accessed easily by the parent(0). 
Then we go to (3) and (4) which are the same case but moving diagonally. Then come (5) and (6) - the path reaching them DOES NOT NEED to
go through the green node which means they can be prunned(see later). This leaves us with only one node worth checking from the Green Node - the one to its right.
The direction worth checking is marked with the white arrow.

Another exception assumption:

![Research image](/resources/JumpBlocked.png?raw=true "Forced Neighbours") If the way is blocked as we jump to the right, we can disregard the ENTIRE jump. This is because we've already assumed that the paths above and below have already been handled by other AND we haven't stopped because of a Forced Diagonal Neighbour. We only care about what's immediately to our right which means there's nowhere else to go.


# Implementation Details

1) Created a Grid Renderer Component and attached it to a dedicated single game object pushed and updated on a scene.
-------------------------------------
The Grid Renderer only used the Render() method and inside it:
- Draws background
- Draws walls as filled rectangles
- Draws expanded cells
- Draws all grid lines
- DRAWS THE PATH LINE
- Draws filled rects for start and goal
-------------------------------------
The JPS app creates and assigns the grid, start and goal to the Render Component and computes the path.
This happens ONCE by simply building a test level with walls and calling the ASTAR ALGORITHM foe the path calculation


2) Created a base JPS mouse command class from which specific JPS game command are derived. OnClick() is the thing that defines them.
-------------------------------------
In JPSApp:
- Created a reusable GridLayout struct that holds origin and cell size info. This is located here since it is used by Commands, ImGui, rendering...
- Created public setting methods which will be toggled by ImGui.
- Created an empty GO playing the role of an input target for all commands.


3) Implemented a MouseToCell() protected method in the base JPS Mouse Command that gets mouse input coordinates and returns the x/y coordinates in the array for a Cell.
Method also returns if this succeeded -> false if cell is NOT in bounds of the GRID.


4) Implemented 3 new JPS Specific Commands. All of them directly modify the Grid via the App itself.
MOST IMPORTANTLY, the app Recomputes the PATH every time one of these is called:
- ToggleWall - Calls the app's ToggleWall method. Command returns early if clicked cell is outside grid or is start/end.
- SetStart - Calls the app's SetStartCell. Command return early if clicked cell is outside grid or is a wall.
- SetGoal - Same as SetStart but for the goal cell.
-------------------------------------
The JPS App's called functions themselves simply:
- Call functions from the Grid, modifying the state of the clicked CELL.
- Call the Grid Renderer to update cells.
- MOST IMPORTANTLY, it Recomputes the PATH every time one of these is called.


5) Created a ImGui instance scene which updates data of the App which is then distributed to the Grid Renderer and modifies the Grid itself. The ImGui also displays the STATS, read from the app which are cached form the AStar Path computations.
-------------------------------------
ImGui does:
1. Sets Heuristic function from an index. Octile is default (best for JPS).
2. Visualization toggle for Expanded Cells
3. Toggle for Generating/Clearing Walls
4. DISPLAYS THE Algorithm Stats
-------------------------------------
Everything supported from the ImGui is followed by a RecomputePath call from the app!


6) Created a GO "handle" playing the role of an input target for the commands.
The mouse commands are bound to the Input Manager:
- Left: Walls
- Right: Start Cell
- Middle: Goal Cell
-------------------------------------
The Stats calculated from the path computing algorithm(AStar for now) is now stored as a member variable which is read from the IMGUI instance and displayed as text.