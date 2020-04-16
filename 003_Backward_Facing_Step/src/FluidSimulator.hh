#ifndef __FLUID_SIMULATOR_H__
#define __FLUID_SIMULATOR_H__


#include "FileReader.hh"
#include "Types.hh"
#include "StaggeredGrid.hh"
#include "SORSolver.hh"
#include <iostream>
#include <string>

class FluidSimulator
{
  public:
      FluidSimulator( const FileReader & conf );

      /// Simulates a given time-length
      void simulate             ( real duration              );
      void simulateTimeStepCount( unsigned int nrOfTimeSteps );


      // Getter functions for the internally stored StaggeredGrid
            StaggeredGrid & grid()       { return grid_; }
      const StaggeredGrid & grid() const { return grid_; }
            SORSolver & solver()         { return solver_; }
      const SORSolver & solver()   const { return solver_; }


      //Functions other
      real umaxval();
      real vmaxval();
      void checkcondition (std::string boundary, bool flag);


  private:
      void computeFG();
      //Other funcitons
      void composeRHS();
      void determineNextDT(real const & limit);
      void setboundary();
      int checkboundary(std::string boundary);
      void boundaryE(int boundary);
      void boundaryW(int boundary);
      void boundaryN(int boundary);
      void boundaryS(int boundary);
      void updateVelocities();
      void refreshBoundaries();
      void flagfill();
      void correctBoundary();


      //Variables
      StaggeredGrid grid_;
      SORSolver solver_;
      real gx,gy,Re,U_init,V_init,P_init,dt,gamma;
      real tau;
      unsigned int timesteps,writestep;
      std::string namen;
      std::string bE, bW, bN, bS;
      real bEU, bWU, bNU, bSU;
      int bouE, bouW,bouN,bouS;
      bool flagE = false,flagW = false,flagN = false,flagS = false;




};



#endif
