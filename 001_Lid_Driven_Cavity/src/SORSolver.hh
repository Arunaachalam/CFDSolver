#ifndef SOR_SOLVER_HH
#define SOR_SOLVER_HH

#include "StaggeredGrid.hh"
#include "Debug.hh"


class SORSolver
{
public:
   // Constructor to manually create SORSolver
   SORSolver ( double omega, double epsilon, double maxiteration,int xmaximum, int ymaximum);

   // Constructor to create a SORSolver from a parsed configuration file
   SORSolver ( const FileReader & configuration );


   // solve the pressure equation on the staggered grid
   bool solve( StaggeredGrid & grid );

   //Print Parameters
   void print();

   //getter and setter functions
   int imax() const { return imax_; }
   int jmax() const { return jmax_; }


private:
   // TODO add solver parameters here as member
   double omg;
   double eps;
   double itermax;
   int checkfreq;
   int normfreq;
   int imax_;
   int jmax_;


};






#endif //SOR_SOLVER_HH




