#include "Array.hh"
#include "FileReader.hh"
#include "Debug.hh"
#include "StaggeredGrid.hh"
#include "SORSolver.hh"
#include "FluidSimulator.hh"
#include "lodepng.h"
#include "GrayScaleImage.hh"

#include <iostream>
#include <vector>
#include <fstream>

int main(int argc, char** argv)
{
    CHECK_MSG(argc == 2, "Problem with Parameter File");

    //File Reader functions to be called
    std::cout << "Reading File: " << std::endl;

    FileReader f1;
    f1.readFile(argv[1]);


    std::cout << "Starting Fluid Simulator: " << std::endl;


    FluidSimulator FS(f1);
    if (f1.findparameters("timesteps"))
        FS.simulateTimeStepCount(f1.getIntParameter("timesteps"));
    else if (f1.findparameters("duration"))
        FS.simulate(f1.getRealParameter("duration"));
    else
        CHECK_MSG (0, "Parameters Duration or timesteps not found in file");





    std::cout << std::endl << "Fluid Simulator Ended" << std::endl;
    std::cout << "Program Completed: Press ENTER to end The Program" << std::endl;
    std::cin.get();


   return 0;
}
