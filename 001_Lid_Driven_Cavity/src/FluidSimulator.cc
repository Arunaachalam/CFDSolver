#include "FluidSimulator.hh"
#include "VTKWriter.hh"
#include <iostream>
#include <cmath>
#include <algorithm>


//
///////////////////////////////////////////////////////////////////////////////////

//Constructors

///////////////////////////////////////////////////////////////////////////////////

FluidSimulator::FluidSimulator( const FileReader & conf )
:grid_(conf),solver_(conf)
{
    gx = conf.getRealParameter("GX");
    gy = conf.getRealParameter("GY");
    Re = conf.getRealParameter("Re");
    U_init = conf.getRealParameter("U_INIT");
    V_init = conf.getRealParameter("V_INIT");
    P_init = conf.getRealParameter("P_INIT");
    dt = conf.getRealParameter("dt");
    gamma = conf.getRealParameter("gamma");
    timesteps = conf.getIntParameter("timesteps");
    writestep = conf.getIntParameter("outputinterval");
    namen = conf.getStringParameter("name");
    tau = conf.getRealParameter("safetyfactor");
    bE = conf.findstring("boundary_condition_E");
    bW = conf.findstring("boundary_condition_W");
    bN = conf.findstring("boundary_condition_N");
    bS = conf.findstring("boundary_condition_S");

    flagE = conf.findparameters("boundary_velocity_E");
    flagW = conf.findparameters("boundary_velocity_W");
    flagN = conf.findparameters("boundary_velocity_N");
    flagS = conf.findparameters("boundary_velocity_S");

    if (flagE == true)
        bEU = conf.getRealParameter("boundary_velocity_E");
    if (flagW == true)
        bWU = conf.getRealParameter("boundary_velocity_W");
    if (flagN == true)
        bNU = conf.getRealParameter("boundary_velocity_N");
    if (flagS == true)
        bSU = conf.getRealParameter("boundary_velocity_S");

    checkcondition (bE,flagE);
    checkcondition (bW,flagW);
    checkcondition (bN,flagN);
    checkcondition (bS,flagS);

    PROGRESS("ALL DATAS FOR FLUID SIMULATOR READ")
}

//
//////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////

void FluidSimulator::simulateTimeStepCount( unsigned int nrOfTimeSteps )
{
    grid().p().fill(P_init);
    grid().u().fill(U_init);
    grid().v().fill(V_init);
    grid().rhs().fill(0.0);
    setboundary();
    int check = 0;
    unsigned int beginn = nrOfTimeSteps;
    VTKWriter V(grid_,namen,true,true);

    while (nrOfTimeSteps > 0)
    {
        determineNextDT(nrOfTimeSteps);
        refreshBoundaries();
        if ((check) % writestep == 0 || (nrOfTimeSteps == beginn))
        {
            V.write();
            check = 0;
        }
        check++;
        computeFG();
        composeRHS();
        solver().solve(grid_);
        updateVelocities();
        nrOfTimeSteps--;
    }
    refreshBoundaries();
    V.write();

}


void FluidSimulator::simulate(real duration)
{
    grid().p().fill(P_init);
    grid().u().fill(U_init);
    grid().v().fill(V_init);
    grid().rhs().fill(0.0);
    setboundary();
    VTKWriter V(grid_,namen,true,true);
    real beginn= duration;
    int check = 0;

    while (duration > 0)
    {
        refreshBoundaries();
        if ((check) % writestep == 0 || (duration == beginn))
        {
            V.write();
            check = 0;
        }
        check++;
        computeFG();
        composeRHS();
        solver().solve(grid_);
        updateVelocities();
        computeFG();
        duration -= dt;
    }
    refreshBoundaries();
    V.write();
}





void FluidSimulator::computeFG()
{

real grid_dx = 1/grid().dx();
real gamma_dx = gamma/grid().dx();
real grid_dy = 1/grid().dy();
real gamma_dy = gamma/grid().dy();
real powx = pow(grid().dx(),2);
real powy = pow(grid().dy(),2);


for (int i = 1; i < solver().imax()+1; i++)
{
for (int j = 1; j < solver().jmax()+1; j++)
{
    real du2_dx = ( (grid_dx)*(pow(((grid().u().operator()(i,j)+grid().u().operator()(i+1,j))/2),2)-pow(((grid().u().operator()(i-1,j)+grid().u().operator()(i,j))/2),2)) ) +
                    ( (gamma_dx) * ( ((abs(grid().u().operator()(i,j)+grid().u().operator()(i+1,j)))*(grid().u().operator()(i,j)-grid().u().operator()(i+1,j))/4.0) -
                      ((abs(grid().u().operator()(i-1,j)+grid().u().operator()(i,j)))*(grid().u().operator()(i-1,j)-grid().u().operator()(i,j))/4.0) ) );

    real dv2_dy = ( (grid_dy)*(pow(((grid().v().operator()(i,j)+grid().v().operator()(i,j+1))/2),2)-pow(((grid().v().operator()(i,j-1)+grid().v().operator()(i,j))/2),2)) ) +
                    ( (gamma_dy) * ( ((abs(grid().v().operator()(i,j)+grid().v().operator()(i,j+1)))*(grid().v().operator()(i,j)-grid().v().operator()(i,j+1))/4.0) -
                      ((abs(grid().v().operator()(i,j-1)+grid().v().operator()(i,j)))*(grid().v().operator()(i,j-1)-grid().v().operator()(i,j))/4.0) ) );

    real d2u_dx2 = (grid().u().operator()(i+1,j)-(2*grid().u().operator()(i,j))+grid().u().operator()(i-1,j))/(powx);

    real d2u_dy2 = (grid().u().operator()(i,j+1)-(2*grid().u().operator()(i,j))+grid().u().operator()(i,j-1))/(powy);

    real d2v_dx2 = (grid().v().operator()(i+1,j)-(2*grid().v().operator()(i,j))+grid().v().operator()(i-1,j))/(powx);

    real d2v_dy2 = (grid().v().operator()(i,j+1)-(2*grid().v().operator()(i,j))+grid().v().operator()(i,j-1))/(powy);

    real duv_dy = ( (grid_dy)*( ((((grid().v().operator()(i,j)+grid().v().operator()(i+1,j))*(grid().u().operator()(i,j)+grid().u().operator()(i,j+1)))/4.0)-(((grid().v().operator()(i,j-1)+grid().v().operator()(i+1,j-1))*(grid().u().operator()(i,j-1)+grid().u().operator()(i,j)))/4.0))))+
                    ( (gamma_dy) * ((((abs(grid().v().operator()(i,j)+grid().v().operator()(i+1,j))*(grid().u().operator()(i,j)-grid().u().operator()(i,j+1))))/4.0)-(((abs(grid().v().operator()(i,j-1)+grid().v().operator()(i+1,j-1)))*(grid().u().operator()(i,j-1)-grid().u().operator()(i,j)))/4.0)) );

    real duv_dx = ( (grid_dx)*( ((((grid().u().operator()(i,j)+grid().u().operator()(i,j+1))*(grid().v().operator()(i,j)+grid().v().operator()(i+1,j)))/4.0)-(((grid().u().operator()(i-1,j)+grid().u().operator()(i-1,j+1))*(grid().v().operator()(i-1,j)+grid().v().operator()(i,j)))/4.0)))) +
                    ( (gamma_dx) * ((((abs(grid().u().operator()(i,j)+grid().u().operator()(i,j+1))*(grid().v().operator()(i,j)-grid().v().operator()(i+1,j))))/4.0)-(((abs(grid().u().operator()(i-1,j)+grid().u().operator()(i-1,j+1)))*(grid().v().operator()(i-1,j)-grid().v().operator()(i,j)))/4.0)) );

    grid().f().operator()(i,j) = (grid().u().operator()(i,j)) + ( dt*(((1/Re)*(d2u_dx2 + d2u_dy2)) - du2_dx - duv_dy + gx) );
    grid().g().operator()(i,j) = (grid().v().operator()(i,j)) + ( dt*(((1/Re)*(d2v_dx2 + d2v_dy2)) - duv_dx - dv2_dy + gy) );
}
}

//Set Boundary

for (int j = 1; j < solver().jmax()+1;j++)
{
    grid().f().operator ()(0,j) = grid().u().operator ()(0,j);
    grid().f().operator ()(solver().imax(),j) = grid().u().operator ()(solver().imax(),j);
}

for (int i = 1; i < solver().imax()+1; i++)
{
    grid().g().operator ()(i,0) = grid().v().operator ()(i,0);
    grid().g().operator ()(i,solver().jmax()) = grid().v().operator ()(i,solver().jmax());

}

}

void FluidSimulator::composeRHS()
{
    for (int i = 1; i < solver().imax()+1; i++)
    {
    for (int j = 1; j < solver().jmax()+1; j++)
    {
        grid().rhs().operator()(i,j) = (1/dt) * (((grid().f().operator()(i,j) - grid().f().operator()(i-1,j))/grid().dx())+((grid().g().operator()(i,j) - grid().g().operator()(i,j-1))/grid().dy()));
    }
    }
}

void FluidSimulator::determineNextDT(real const & limit)
{
    if (limit > 0)
    {
        real xx = (Re/2.0)*(1.0/((1/(grid().dx()*grid().dx())+(1/(grid().dx()*grid().dx())))));
        real yy = grid().dx()/umaxval();
        real zz = grid().dy()/vmaxval();

        dt = tau * std::min(xx,std::min(yy,zz));

    }
}

real FluidSimulator::umaxval()
{
    real maxi = 0;
    for (int i = 0; i < grid().u().rows(); i++)
    {
    for (int j = 0 ;j < grid().u().column(); j++)
        maxi = (maxi>abs(grid().u().operator()(i,j))) ? maxi : abs(grid().u().operator()(i,j));
    }
    return maxi;
}

real FluidSimulator::vmaxval()
{
    real maxi = 0;
    for (int i = 0; i < grid().v().rows(); i++)
    {
    for (int j = 0; j < grid().v().column(); j++)
        maxi = (maxi>abs(grid().v().operator()(i,j))) ? maxi : abs(grid().v().operator()(i,j));
    }
    return maxi;
}

void FluidSimulator::setboundary()
{
    bouE = checkboundary(bE);
    bouW = checkboundary(bW);
    bouN = checkboundary(bN);
    bouS = checkboundary(bS);
    if (bouE == 0 && flagE == true)
        bouE = 3;
    if (bouW == 0 && flagW == true)
        bouW = 3;
    if (bouN == 0 && flagN == true)
        bouN = 3;
    if (bouS == 0 && flagS == true)
        bouS = 3;

}

int FluidSimulator::checkboundary(std::string boundary)
{
    if (boundary == "noslip")
        return 0;
    else if (boundary == "freeslip")
        return 1;
    else if (boundary == "outflow")
        return 2;
    else if (boundary == "inflow")
        return 3;
    else
        return 0;
}

void FluidSimulator::refreshBoundaries()
{
    boundaryE (bouE);
    boundaryW (bouW);
    boundaryN (bouN);
    boundaryS (bouS);
}

void FluidSimulator::updateVelocities()
{
    real dt_dx = dt/grid().dx();
    real dt_dy = dt/grid().dy();

    for (int i = 1; i < solver().imax()+1; i++)
    for (int j = 1; j < solver().jmax()+1; j++)
    {
        grid().u().operator()(i,j) = grid().f().operator()(i,j) - (dt_dx * (grid().p().operator()(i+1,j)-grid().p().operator()(i,j)));
    }
    for (int i = 1; i < grid().v().rows()-1; i++)
    for (int j = 1; j < grid().v().column()-1; j++)
    {
        grid().v().operator()(i,j) = grid().g().operator()(i,j) - (dt_dy * (grid().p().operator()(i,j+1)-grid().p().operator()(i,j)));
    }

}


void FluidSimulator::checkcondition(std::string boundary, bool flag)
{
    CHECK_MSG(!(boundary == "outflow" && flag == true), "ERROR: Outflow Condition wrong. Initialized Outflow with Velocity");
}

void FluidSimulator::boundaryE (int boundary)
{
    switch (boundary)
    {
    case 0:     for(int j = 1; j < solver().jmax(); j++)
                    grid().u().operator()(solver().imax(),j) = 0;
                for (int j = 1; j < solver().jmax(); j++)
                    grid().v().operator()(solver().imax()+1,j) = -grid().v().operator()(solver().imax(),j);
                break;

    case 1:  for (int j = 1; j < solver().jmax(); j++)
            {
                grid().u().operator ()(solver().imax(),j) = 0;
                grid().v().operator()(solver().imax()+1,j) = grid().v().operator()(solver().imax(),j);
            }
                break;

    case 2:  for (int j = 1; j < solver().jmax(); j++)
              {     grid().u().operator()(solver().imax(),j) = grid().u().operator()(solver().imax()-1,j);
                    grid().v().operator()(solver().imax()+1,j) = grid().v().operator()(solver().imax(),j);
              }
              break;

    case 3:  for (int j = 1; j < solver().jmax()+2; j++)
              {     grid().u().operator()(solver().imax(),j) = bEU;
                    grid().u().operator()(solver().imax()-1,j) = grid().u().operator()(solver().imax(),j);
              }
             for (int j = 1; j < solver().jmax()+1;j++)
                    grid().v().operator ()(solver().imax()+1,j) = - grid().v().operator ()(solver().imax(),j);
              break;
    }
}

void FluidSimulator::boundaryW (int boundary)
{
    switch (boundary)
    {
    case 0:     for(int j = 1; j < solver().jmax();j++)
                    grid().u().operator()(0,j) = 0;
                for (int j = 1; j < solver().jmax(); j++)
                    grid().v().operator()(0,j) = -grid().v().operator()(1,j);
                break;

    case 1:  for (int j = 1; j < solver().jmax(); j++)
        {
                grid().u().operator ()(0,j) = 0;
                grid().v().operator()(0,j) = grid().v().operator()(1,j);
        }
             break;

    case 2:  for (int j = 1; j < solver().jmax(); j++)
                {   grid().u().operator()(0,j) = grid().u().operator()(1,j);
                    grid().v().operator()(0,j) = grid().v().operator()(1,j);
                }
             break;

    case 3:  for (int j = 0; j < solver().jmax()+2; j++)
                {   grid().u().operator()(0,j) = bWU;
                    grid().u().operator()(1,j) = grid().u().operator()(0,j);
                }
            for(int j = 0; j < solver().jmax()+1; j++)
                grid().v().operator ()(0,j) = -grid().v().operator ()(1,j);
            break;
    }
}

void FluidSimulator::boundaryN (int boundary)
{
    switch (boundary)
    {
    case 0:     for (int i = 1; i < solver().imax()+1; i++)
                    grid().v().operator()(i,solver().jmax()) = 0;
                for (int i = 1; i < solver().imax(); i++)
                    grid().u().operator()(i,solver().jmax()+1) = -grid().u().operator()(i,solver().jmax());
                break;

    case 1:  for (int i = 1; i < solver().imax()+1; i++)
                grid().v().operator ()(i,solver().jmax()) = 0;
             for (int i = 1; i < solver().imax(); i++)
                grid().u().operator()(i,solver().jmax()+1) = grid().u().operator()(i,solver().jmax());
             break;

    case 2:  for (int i = 1; i < solver().imax()+1; i++)
                    grid().v().operator()(i,solver().jmax()) = grid().v().operator()(i,solver().jmax()-1);
             for (int i = 1; i < solver().imax(); i++)
                    grid().u().operator()(i,solver().jmax()+1) = grid().u().operator()(i,solver().jmax());
             break;

    case 3:  for (int i = 0; i < solver().imax()+1; i++)
              {     grid().u().operator()(i,solver().jmax()+1) = bNU;
            grid().u().operator()(i,solver().jmax()) = grid().u().operator()(i,solver().jmax()+1);
              }
             for (int i = 0; i < solver().imax()+1; i++)
                    grid().v().operator ()(i,solver().jmax()) = -grid().v().operator ()(i,solver().jmax()-1);
             break;
    }

}

void FluidSimulator::boundaryS (int boundary)
{
    switch (boundary)
    {
    case 0:     for (int i = 1; i < solver().imax()+1; i++)
                    grid().v().operator()(i,0) = 0;
                for (int i = 1; i < solver().imax(); i++)
                    grid().u().operator()(i,0) = -grid().u().operator()(i,1);
                break;

    case 1:  for (int i = 1; i < solver().imax()+1; i++)
                grid().v().operator ()(i,0) = 0;
             for (int i = 1; i < solver().imax(); i++)
                grid().u().operator()(i,0) = grid().u().operator()(i,1);
             break;

    case 2:  for (int i = 1; i < solver().imax()+1; i++)
                  grid().v().operator()(i,0) = grid().v().operator()(i,1);
             for (int i = 1; i < solver().imax(); i++)
                    grid().u().operator()(i,0) = grid().u().operator()(i,1);
             break;

    case 3:  for (int i = 0; i < solver().imax()+1; i++)
             {     grid().u().operator()(i,0) = bNU;
                   grid().u().operator()(i,1) = grid().u().operator()(i,0);
             }
             for (int i = 0; i < solver().imax()+1; i++)
                grid().v().operator ()(i,0) = -grid().v().operator ()(i,1);
             break;
    }
}

