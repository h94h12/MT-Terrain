CS184 Final Project Poster Session: 
Procedural Terrain Generation
Steven Hewitt cs184-eu -- Sean Han cs184-cb -- Huiming Han cs184-dc
public github repo at: https://github.com/h94h12/MT-Terrain

Youtube video link:



https://www.youtube.com/watch?v=YRJ77O6KQiA




Abstract/Description
Our project uses isosurface extraction through marching tetrahedrons to render pseudo-random density functions in the form of a procedurally generated landscape.  The density function originates from a Perlin Noise-based heightmap and can be varied through user input.  The user can specify different island shapes corresponding to implicit equations of closed curves, specify the cube size for the marching tetrahedrons algorithm, explore the landscape, change the position of the sun, print the screen with different effects, and input a filename to write to when printing. that users can print the screen buffer to. The landscape contains clouds generated through perlin noise, an ocean, a skybox, optional fog, and a rising/setting sun.


Input
* -<shape> x y:  adds the <shape> to the terrain.  List of shapes: circle, butterfly, cycloid, links, ampersand, cruciform, clover.  A user can specify none, or many of these shapes.
* -filename <name>: sets the filename where the .png’s are written to <name>#.png
* -grid x y z:  specifies the max x, y, z for the grid.  Recommended: -grid 10 5 10
* -noise #: specifies the amount of noise for the perlin noise for the terrain.  [1, 50]


Controls
* Mouse left-click: zoom
* Mouse right-click: rotate camera
* Shift + [F]: enable/disable Fog
*                [f]: change fog decay function
* Shift + [P]: print screen buffer to .png file
* [1]: quickly rewind time
* [2]: rewind time
* [3]: fast-foward time
* [4]: quickly fast-forward time
