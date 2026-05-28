# Introduction to the Idea

Jump Point Search is an algorithm that improves upon normal A* pathfinding.
At its core, A* expands its search by doing the simplest thing possible: adding a node's immediate neighbours to the set of what to examine next.
JPS tries to skip over some nodes that are not valuable to look at directly.

    JPS is NOT a different search algorithm - it's a different neighbour function plugged into the same A* skeleton. This means:
    - same open list
    - same closed list
    - same g/f costs
    - same parent pointers

### The two main differences are:
1. Pruned neighbours - instead of generating and wasting all 8 immediate neighbors of a node, we generate only the "Natural" and "Forced" Neighbours(see below)
2. Jumping - for each direction you'd normally step in, we apply RECURSIVE Jump(see below) until we reach a node of interest. Then we put ONLY
that node in the open list - skipping every cell in between.

This means JPS is more memory efficient than A*. The nodes in between the Jump Points(see below) DO get examined, but they're never processed. 
This is the main difference between JPS and A*.


# The Algorithm

At the base of JPS sits the fundamental rules of Neighbour Pruning applied during each search:
One rule is applied to straight steps while the other - for diagonal.
The main idea behind both cases is to remove/prune a set of immediate neighbours around a node by trying to prove
that a PATH from the parent node of the current one exists to each neighbour while the path does not involve the current node.

(This implementation uses the no-corner-cutting variant (Harabor & Grastien 2012), online with no preprocessing)

![Research image](/resources/NeighbourPrunning.png?raw=true "Neighbour Pruning") Neighbour Pruning

Node X is being expanded with the arrow signifying the direction it has been reached from its parent. 
In both cases we can prune/remove the greyed out neighbours as these can be reached optimally from the neighbour without going through X.
The remaining nodes AFTER pruning are called "Natural Neighbours" of the node. These are the ones marked with white on the image above.

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

![Research image](/resources/TheRecursionJump.png?raw=true "The Recursion Jump") The Recursion Jump

On the image above we recursively apply the Diagonal Pruning rule and see that node Y is an excelent Jump Point candidate.
The important part is the dashed lines - if any of the horizontal or vertical recursions DID identify a node as a Jump Point, 
the diagonal jump would have stopped and put lower on the priority queue.
Another thing - node W is a forced neighbour of X on this example.


# Explaining it more into detail:

![Research image](/resources/CoreIdeaNeigbours.png?raw=true "Neighbour Examination") Neighbour Examination

The Green Node is the current node we want to expand. Looking back, let's say that the arrow coming horiontally behind it is 
the direction it has been reached from its parent(0). The nodes (1) and (2) are immediate neighbours of the green node that we
can immediately see that are not worth checking since they can be accessed easily by the parent(0). 
Then we go to (3) and (4) which are the same case but moving diagonally. Then come (5) and (6) - the path reaching them DOES NOT NEED to
go through the green node which means they can be prunned(see later). This leaves us with only one node worth checking from the Green Node - the one to its right.
The direction worth checking is marked with the white arrow.

Another exception assumption:

<img src="/resources/JumpBlocked.png?raw=true" alt="Jump Blocked" align="left" width="300" />

If the way is blocked as we jump to the right, we can disregard the ENTIRE jump.
This is because we've already assumed that the paths above and below have already
been handled by other branches, and we haven't stopped because of a Forced Diagonal
Neighbour. We only care about what's immediately to our right, which means there's
nowhere else to go.

<br clear="all" />

# A* versus JPS

As mentioned before, the only difference between A* and JPS is the neighbour calculations and the jumping algorithm. 

The FACT that JPS expands to way less nodes is real and it is shown on the table below. The expanded cells are the ones highlighted in blue:

<table>
  <tr>
    <td><img src="/resources/Example_AStar.png?raw=true" alt="A*" /></td>
    <td><img src="/resources/Example_JPS.png?raw=true" alt="JPS" /></td>
  </tr>
  <tr>
    <td align="center">A*</td>
    <td align="center">JPS</td>
  </tr>
</table>

If you've read thoroughly, you might say "Yes, but the highlighted cells for JPS and not ALL of the nodes that the algorithm processes".
And you would be correct - on the JPS image you can see that the Scanned(JPS Only) field displays the actual number of the "Processed" nodes - 1136 for JPS, 0 for A*.

However, let's clear something up: Scanned cells are NOT as expensive as Expanded cells.
The scanned nodes do not apply any heap operations and no node-record writes, they only go through a few cheap conditions checks:
- Is the cell walkable?
- Does it have forced neighbours(which is a combination of 2-4 IsWalkable checks)
- An integer add to advance the Neighbour cell index

What happens when A* expands a cell:
- A priority queue pop which shifts down all elements in the heap.
- A loop over UP TO 8 neighbours and calculating g-cost, heuristic and updating the Record of nodes.

## Comparison table - A* vs JPS

A benchmark is run through different map types:
- Empty: 0% walls
- Very Sparse - 5% of the grid is walls
- Sparse - 10% of the grid is walls
- Dense - 30% is walls

<table>
  <tr>
    <td><img src="/resources/Comp_Table.png?raw=true" alt="Table" /></td>
    <td><img src="/resources/Comp_Stats.png?raw=true" alt="Table" /></td>
  </tr>
</table>

The benchmark generates a random grid map and runs both A* and JPS storing both their expanded cells and in the case of JPS - scanned nodes. 

For every test, both A* and JPS successfully find the shortest path to the goal - same cost, same length.

# Conclusion

    Jump Point Search extends A* through two additions - neighbour pruning and recursive jumping - that together let it skip over regions of the grid that A* would expand cell by cell.

    The benchmarks also clarify why JPS is faster, and it is not simply "less work". On open maps JPS frequently scans more cells than A* expands, yet still runs faster, because a scanned cell costs only a few array reads while an expanded cell costs a priority-queue pop, a heuristic evaluation, and several node-record updates.

    Jump Point Search is therefore best suited to large, sparse, uniform-cost grids - a condition needed in many game maps.


# Implementation Details

### First, the generic A* skeleton needs to be created.
1) Created a Grid Renderer Component and attached it to a dedicated single game object pushed and updated on a scene.

The Grid Renderer only used the Render() method and inside it:
- Draws background
- Draws walls as filled rectangles
- Draws expanded cells
- Draws all grid lines
- DRAWS THE PATH LINE
- Draws filled rects for start and goal.

The JPS app creates and assigns the grid, start and goal to the Render Component and computes the path.
This happens ONCE by simply building a test level with walls and calling the ASTAR ALGORITHM for the path calculation.

2) In JPSApp:
- Created a reusable GridLayout struct that holds origin and cell size info. This is located here since it is used by Commands, ImGui, rendering...
- Created public setting methods which will be toggled by ImGui.
- Created an empty GO playing the role of an input target for all commands.

### Then comes User Feedback via ImGui
3) Created a base JPS mouse command class from which specific JPS game command are derived. OnClick() is the thing that defines them.

Implemented a MouseToCell() protected method in the base JPS Mouse Command that gets mouse input coordinates and returns the x/y coordinates in the array for a Cell.
Method also returns if this succeeded -> false if cell is NOT in bounds of the GRID.

4) Implemented 3 new JPS Specific Commands. All of them directly modify the Grid via the App itself.
MOST IMPORTANTLY, the app Recomputes the PATH every time one of these is called:
- ToggleWall - Calls the app's ToggleWall method. Command returns early if clicked cell is outside grid or is start/end.
- SetStart - Calls the app's SetStartCell. Command return early if clicked cell is outside grid or is a wall.
- SetGoal - Same as SetStart but for the goal cell.

5) The JPS App's called functions themselves simply:
- Call functions from the Grid, modifying the state of the clicked CELL.
- Call the Grid Renderer to update cells.
- MOST IMPORTANTLY, it Recomputes the PATH every time one of these is called.

6) Created a ImGui instance scene which updates data of the App which is then distributed to the Grid Renderer and modifies the Grid itself. The ImGui also displays the STATS, read from the app which are cached form the AStar Path computations.
ImGui does:
- Sets Heuristic function from an index. Octile is default (best for JPS).
- Visualization toggle for Expanded Cells
- Toggle for Generating/Clearing Walls
- DISPLAYS THE Algorithm Stats.

Everything supported from the ImGui is followed by a RecomputePath call from the app!

7) Created a Game Object "handle" playing the role of an input target for the commands.
The mouse commands are bound to the Input Manager:
- Left: Walls
- Right: Start Cell
- Middle: Goal Cell
The Stats calculated from the path computing algorithm(AStar for now) is now stored as a member variable which is read from the IMGUI instance and displayed as text.

##  From this point on the specific JPS logic is implemented in the codebase.

### 8. JPS Algorithm Implementation itself:
    It is similar to A* at places, the only parts that are different are the neighbour calculations. The algorithm does:
1. Returns early if start/end are out of bounds or not walkable.
2. Pushes the start node to the Node Records and on the OPEN LIST. Increment the nodes Generated stat number.
3. Inside the open List loop, checks if the current node is already in closed or with the same FCost and returns early if so.
4. Marks the current node as expanded and checks if it is the GOAL. If so, the whole algorithm returns the result and stops. The Jump Points are reconstructed from the Node Record, the Path is calculated along with its cost.
Depending on whether the current node is the Starting or any other node, PRUNED NEIGHBOUR directions are calculated by
Jumping across all possible 8 directions from the start of the current node. 
It then calls the Jump function and IF:
- Jump Point is not a dead end
- Jump Point is not already in closed
- Jump Point is MORE OPTIMAL than the G Cost already needed to reach it

,it pushes the Jump Point on the open list and marks it as generated.
5. Starts calling Jump again but in the already Natural Neighbour directions. Same Jump Point logic as before, if it is optimal, it is pushed on the open list and marked as generated.

### User Feedback for Jump Point Search:
9) Implemented the following JPS logic in the form of helpers which will be used in the FindPath method:
- HasForcedNeighbours - two versions - for hor/vert and for diagonal. The logic covers conditions for when a forced neighbour can occur depending on neighbour obstacles from the incoming direction.
- JUMP - the created method returns the index of a successful jump point, else it returns -1. This method covers the 3 difference variations of a jump point - goal, forced neighbour, Recursion on the hor/vert axes BEFORE going diagonally.
- GetPruneDirections - Outputs the NATURAL NEIGHBOUR directions from the currently checked node. The whole method revolves around the specific forced neighbour conditions.
- InterpolatePath - similarly to Reconstructing the Jump Points, uses the jump points to generate the path between them - the DENSE path.
- SumPathCost - returns the total sum of the jump point to point - the SPARSE path.

10) Added Getters/Setters in the JPS App which directly change a variable for the JPS Algorithm -Show Interpolated Dense Path. Algorithm is now switchable from the app. 

For JPS, the ImGui logic makes the following changes:
- First, a Combo box to switch between A* and JPS is added.
- The checkbox for corner cutting is DISABLED for JPS
- Checkbox for Interpolated JPS Path is added, which is disabled during A* of course

# References Section

### Visual Explanations

- https://zerowidth.com/2013/a-visual-explanation-of-jump-point-search/#try-it-out
- https://harablog.wordpress.com/2011/09/07/jump-point-search/

### Documentation Papers

- https://harabor.net/data/papers/harabor-grastien-aaai11.pdf

### Implementation Guides

- https://gamedev.net/tutorials/programming/artificial-intelligence/jump-point-search-fast-a-pathfinding-for-uniform-cost-grids-r4220/
