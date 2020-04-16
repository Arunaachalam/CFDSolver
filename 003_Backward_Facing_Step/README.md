----------------------------------------------------------------------
A brief description of how to run the code and its functionalities
----------------------------------------------------------------------

===================
Solver Type
===================
This is a normalized direct Navier-Stokes discretisation, and works well for laminar cases. (Please donot use the solver for Reynolds number over 3000, the grid refinement is very large and it is not meaningful to use this solver for such cases.)


=======================
Makefile
=======================
The code can be compiled for three scenarios
1. GCC		 - C++17, No checks or logs on kernel, Works fast
2. GCCdebug	 - C++17, For error checking prints out residuals and other error checking implemented in the code, FOR DEBUG USE ONLY (Slows the execution)
3. ICC		 - C++11, compiles for Intel compilers (not recommended) 

Please compile the code based on your requirement.


==============
Input File
==============
The input to the file (Reynolds number, Grid, Domain size, SOR parameters, Write) can be edited in the file "backstep.par".

*The equations are normalized, Re is the only changable parameter other than the domain

================================================================
HOW TO RUN THE CODE
================================================================
1. Edit the "backstep.par" file for your required input
2. Edit the "Makefile" for the required implementation
3. run command: make					---- Compile the code
4. run command: ./CFDSolver backstep.par			---- Run the code
5. run command: paraview				---- Visualizing results
6. run command: make clean				---- Deletes everything except build and source code
7. run command: make distclean				---- Deletes everything except source code


=================================================================
RESULTS PROVIDED
=================================================================
Comparecodes.jpg	-	Streamlines showing vortex generation at different Reynolds number
Output.png		-	Displays png for the geometry profile

=============================================
Brief code explanation .hh and .cc files
=============================================
Array		- Creates the arrays used
FileReader	- Reads and maps the input files "backstep.par"
StaggeredGrid	- Creates discretized staggered grid
SORSolver	- Linear pressure solver SOR method
VTKWriter	- Output file written in vtk format for post-processing
FluidSimulator	- NS discretization and implementation
Debug		- Error checking classes and implementation
Types		- Enum types used in program
lodepng		- PNG image loader (not implemented)
GrayScaleImage	- Output image of the geometry

