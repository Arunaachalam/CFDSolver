#ifndef STAGGERED_GRID_HH
#define STAGGERED_GRID_HH


#include "Types.hh"
#include "Array.hh"
#include "FileReader.hh"
#include "Debug.hh"
#include "GrayScaleImage.hh"

#include <vector>
#include <string>

//*******************************************************************************************************************
/*! Class for storing all arrays required for the simulation
*
* For now it only contains an array for the pressure and another array for the
* right hand side of the pressure equation.
* In following assignments this will be extended and also contain
* arrays for x/y velocity and some arrays for intermediate values.
*
* Feel free to add member functions or variables to this class, but please don't
* delete or rename any existing functions, since future skeletons rely on these functions.
*
*/
//*******************************************************************************************************************
class StaggeredGrid
{
public:
   // Constructors to manually create staggered grid
   StaggeredGrid ( int xSize, int ySize, real dx, real dy ); // TODO implement!

   // Constructor to create a staggered grid from a parsed configuration file
   StaggeredGrid ( const FileReader & configuration );       // TODO implement!


   // Getters / Setters for member variables
   Array & p()    { return p_;    }
   Array & rhs()  { return rhs_;  }
   Array & u()    { return u_;    }
   Array & v()    { return v_;    }
   Array & f()    { return f_;    }
   Array & g()    { return g_;    }

   const Array & p()   const { return p_;   }
   const Array & rhs() const { return rhs_; }
   const Array & u()   const { return u_;   }
   const Array & v()   const { return v_;   }
   const Array & f()   const { return f_;   }
   const Array & g()   const { return g_;   }

   real dx() const { return dx_; }
   real dy() const { return dy_; }
   real xSize() const { return xSize_; }
   real ySize() const { return ySize_; }
   real Rectx1() const { return Rectx1_; }
   real Rectx2() const { return Rectx2_; }
   real Recty1() const { return Recty1_; }
   real Recty2() const { return Recty2_; }
   real Circx() const { return Circx_; }
   real Circy() const { return Circy_; }
   real Circr() const { return Circr_; }
   real rows() const { return rows_; }
   real column() const { return column_; }
   real xlength() const { return xlength_; }
   real ylength() const { return ylength_; }
   std::string name() const { return name_; }

   //Flag field
   inline bool  isFluid ( int i ,int j );
   inline int getnumFluid();
   void createRectangle (real x1, real y1, real x2, real y2);
   void createCircle (real x, real y, real r);
   void setCellToObstacle (int x, int y);
   void wrappedaccessu();
   void wrappedaccessv();
   void wrappedaccessp();
   void wrappedaccessf();
   void wrappedaccessg();

   inline Direction isObstacleField (int i, int j);
   void createObstacleEnumField ();
   void printobs();
   void fill(bool value);
   void print ();
   void imagewrite();

protected:
   Array p_;   //< pressure field
   Array rhs_; //< right hand side of the pressure equation
   Array u_;    //< Velocity field in X dir
   Array v_;    //< Velocity field in y dir
   Array f_;    //< Intermediate velocity at x dir
   Array g_;    //< Intermediate velocity at y dir

   real dx_;   //< distance between two grid points in x direction
   real dy_;   //< distance between two grid points in y direction
   real xSize_;
   real ySize_;
   real Rectx1_,Rectx2_, Recty1_, Recty2_;
   real Circx_, Circy_ ,Circr_;
   real rows_, column_;
   real xlength_, ylength_;

   std::vector <bool> flag;
   std::vector <Direction> obstacleflag;
   std::string name_;


};

//===================================================================================================================
//
//  Inline Access Operators and Sizes
//
//===================================================================================================================

inline bool  StaggeredGrid::isFluid ( int i ,int j )
{
    return flag[(j*(xSize_+2))+i];
}

inline Direction StaggeredGrid::isObstacleField(int i, int j)
{
    return obstacleflag[(j*(xSize_+2))+i];
}

inline int StaggeredGrid::getnumFluid()
{
    int totalnumber = 0;
    for (int i = 1; i < xSize_+1; i++)
    for (int j = 1; j < ySize_+1; j++)
    {
        if (isFluid(i,j) == true)
            totalnumber++;
    }

    return totalnumber;
}



#endif //STAGGERED_GRID_HH

