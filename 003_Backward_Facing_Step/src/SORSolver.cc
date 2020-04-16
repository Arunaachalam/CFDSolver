#include "SORSolver.hh"

#include <iostream>
#include <cmath>
#include <math.h>
#include <random>
#include <vector>
#include <string>



//
///////////////////////////////////////////////////////////////////////////////////

//Constructors

///////////////////////////////////////////////////////////////////////////////////

SORSolver::SORSolver (double omega, double epsilon, double maxiteration, int xmaximum, int ymaximum)
{
    omg = omega;
    eps = epsilon;
    itermax = maxiteration;
    imax_ = xmaximum;
    jmax_ = ymaximum;

    CHECK_MSG(imax_ > 1, "ERROR: imax Value should be Greater than or equal to 1");
    CHECK_MSG(jmax_ > 1, "ERROR: jmax Value should be Greater than or equal to 1");

    PROGRESS ("REQUIRED VARIABLES FOR PRESSURE SOLVER SET")
}

SORSolver::SORSolver ( const FileReader & configuration )
{
    omg = configuration.getRealParameter("omg");
    eps = configuration.getRealParameter("eps");
    itermax = configuration.getIntParameter("itermax");
    imax_ = configuration.getIntParameter("imax");
    jmax_ = configuration.getIntParameter("jmax");
    checkfreq = configuration.getIntParameter("checkfrequency");
    normfreq = configuration.getIntParameter("normalizationfrequency");
    CHECK_MSG(imax_ > 1, "ERROR: imax Value should be Greater than or equal to 1");
    CHECK_MSG(jmax_ > 1, "ERROR: jmax Value should be Greater than or equal to 1");

    PROGRESS ("REQUIRED VARIABLES FOR PRESSURE SOLVER SET")
}

//
//////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////


bool SORSolver::solve(StaggeredGrid & grid)
{
    PROGRESS ("SOLVER START")
    //Solver
    //Initialise solver loop variable
    int loopvar = 0;
    double residual = 100.0;
    real grid_dxdy = (omg) * (1.0/((2.0/(grid.dx()*grid.dx()))+(2.0/(grid.dy()*grid.dy()))));
    real grid_dx = grid.dx()*grid.dx();
    real grid_dy = grid.dy()*grid.dy();


    //Start of While loop // loop at 44 starts to spike up
    while(loopvar < itermax && residual > eps)
    {
    loopvar++;
    grid.wrappedaccessp();
    //Differential Formula calculation of P(i,j)
    for (int i = 1; i < imax_+1; i++)
    {
    for (int j = 1; j < jmax_+1; j++)
    {
        if(grid.isFluid (i,j))
        {
        grid.p().operator()(i,j) =( (1-omg) * (grid.p().operator()(i,j)) ) +
                                   ( (grid_dxdy) *
                                   ( ((grid.p().operator()(i-1,j) + grid.p().operator()(i+1,j))/(grid_dx)) +
                                     ((grid.p().operator()(i,j-1) + grid.p().operator()(i,j+1))/(grid_dy)) -
                                      grid.rhs().operator()(i,j)) );
        }
    }
    }

    //Residual Calculation
    if (loopvar % checkfreq == 0)
    {
    residual = 0;
    std::vector <double> res;
    for (int i = 1; i < imax_+1; i++)
    for (int j = 1; j < jmax_+1; j++)
    {
        double temp = 0;
        double calcs = 0;

        if (grid.isFluid(i,j))
        calcs = ((grid.p().operator()(i-1,j)-(2*grid.p().operator()(i,j))+grid.p().operator()(i+1,j))/(grid.dx()*grid.dx())) +
                        ((grid.p().operator()(i,j-1)-(2*grid.p().operator()(i,j))+grid.p().operator()(i,j+1))/(grid.dy()*grid.dy()));

        temp  = (grid.rhs().operator()(i,j) - calcs);
        res.push_back(temp);
    }

        for(unsigned int it =  0; it < res.size(); it++)
        {
            residual += ((res[it]*res[it])/(grid.getnumFluid()));
        }

        residual = pow(residual,0.5);

    PROGRESS("At Iteration " + std::to_string(loopvar) + " the Residual is " + std::to_string(residual))
    }

    //Normalise the pressure
    if ((loopvar % normfreq) == 0)
    {
	real calc = 0;
	for (int i = 1; i < imax_+1; i++)
	for (int j = 1; j < jmax_+1; j++)
        {
		calc += grid.p().operator()(i,j);
    	}
	real temp = calc /(grid.getnumFluid());
	for (int i = 1; i < imax_+1; i++)
	for (int j = 1; j < jmax_+1; j++)
		grid.p().operator()(i,j) -= temp;
    }

    //Boundary conditions Bottom
    for(int i = 0;i < imax_+2; i++)
    {
        int j = 0;
        if (grid.isFluid(i,j))
        grid.p().operator()(i,j) = grid.p().operator()(i,j+1);
    }
    //Boundary conditions Top
    for(int i = 0; i < imax_+2; i++)
    {
        int j = jmax_+1;
        if(grid.isFluid(i,j))
        grid.p().operator()(i,j) = grid.p().operator()(i,j-1);

    }
    //Boundary conditions Left
    for(int j = 0; j < jmax_+2; j++)
    {
        int i = 0;
        if(grid.isFluid(i,j))
        grid.p().operator()(i,j) = grid.p().operator()(i+1,j);
    }
    //Boundary Conditions right
    for(int j = 0; j < jmax_+2; j++)
    {
        int i = imax_+1;
        if(grid.isFluid(i,j))
        grid.p().operator() (i,j) = grid.p().operator()(i-1,j);
    }
    //Set all other values as zero
    for(int i = 0; i < imax_+2; i++)
    for(int j = 0; j < jmax_+2; j++)
        if(!grid.isFluid(i,j))
            grid.p().operator()(i,j) = 0;

    }


    PROGRESS ("SOLVER END")

    if (loopvar < itermax)
        return true;
    else
        return false;
}


void SORSolver::print()
{
    std::cout << "omg" << '\t' << omg << std::endl;
    std::cout << "eps" << '\t' << eps << std::endl;
    std::cout << "itermax" << '\t' << itermax << std::endl;
    std::cout << "checkfrequency" << '\t' << checkfreq << std::endl;
    std::cout << "normalizationfrequency" << '\t' << normfreq << std::endl;

    PROGRESS ("PRINT ALL PARAMETERS IN PRESSURE SOLVER")
}
