#include "StaggeredGrid.hh"
#include "Array.hh"
#include "FileReader.hh"

#include <iostream>
#include <omp.h>
#include <cmath>
#include <math.h>

//
///////////////////////////////////////////////////////////////////////////////////

//Constructors

///////////////////////////////////////////////////////////////////////////////////

StaggeredGrid::StaggeredGrid(int xSize, int ySize, real dx, real dy)
:p_(xSize+2,ySize+2),rhs_(xSize+2,ySize+2),u_(xSize+1,ySize+2),v_(xSize+2,ySize+1),f_(xSize+1,ySize+2),g_(xSize+2,ySize+1)
{

	dx_ = dx/xSize;
	dy_ = dy/ySize;
	flag.resize(xSize*ySize);
    PROGRESS ("STAGGERED GRIDS CREATED")
}

StaggeredGrid::StaggeredGrid ( const FileReader & configuration )
:p_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+2), rhs_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+2),u_(configuration.getIntParameter("imax")+1,configuration.getIntParameter("jmax")+2),v_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+1),f_(configuration.getIntParameter("imax")+1,configuration.getIntParameter("jmax")+2),g_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+1)
{

    dx_ = configuration.getRealParameter("xlength")/configuration.getIntParameter("imax");
    dy_ = configuration.getRealParameter("ylength")/configuration.getIntParameter("jmax");
    xSize_ = configuration.getRealParameter("imax");
    ySize_ = configuration.getRealParameter("jmax");
    xlength_ = configuration.getRealParameter("xlength");
    ylength_ = configuration.getRealParameter("ylength");
    name_ = configuration.getStringParameter("name");
    if (configuration.findparameters("RectangleX1"))
    {
    Rectx1_ = configuration.getRealParameter("RectangleX1");
    Rectx2_ = configuration.getRealParameter("RectangleX2");
    Recty1_ = configuration.getRealParameter("RectangleY1");
    Recty2_ = configuration.getRealParameter("RectangleY2");
    }
    else if (configuration.findparameters("CircleX"))
    {
    Circx_ = configuration.getRealParameter("CircleX");
    Circy_ = configuration.getRealParameter("CircleY");
    Circr_ = configuration.getRealParameter("CircleR");
    }

    flag.resize((xSize_+2)*(ySize_+2));
    obstacleflag.resize((xSize_+2)*(ySize_+2));


    PROGRESS ("STAGGERED GRIDS CREATED")

}


//===================================================================================================================
//
//  Convenience Functions
//
//===================================================================================================================

void StaggeredGrid::fill( bool value )
{

   int i = (xSize_+2)*(ySize_+2);
   flag.assign(i,value);
   PROGRESS ("FILL ALL ELEMENTS WITH A INITIAL VALUE - SUCCESS")

}


void StaggeredGrid::print()
{
   bool test = false;

   if (!test)
   {
    test = true;
    for (int k = ySize_+1; k >= 0; k--)
    {
     for (int j = 0; j < xSize_+2; j++)
     {
        std::cout << flag[j+(k*(xSize_+2))] << "\t";;
     }
     std::cout << std::endl;
    }
    PROGRESS ("Print 2D ARRAY - SUCCESS")
   }

   else
        CHECK_MSG(test == true, "ERROR: CHECK THE ARRAY - HAS OUT OF BOUND VALUES.");
}

void StaggeredGrid::printobs()
{
   bool test = false;

   if (!test)
   {
    test = true;
    for (int k = ySize_+1; k >= 0; k--)
    {
     for (int j = 0; j < xSize_+2; j++)
     {
        std::cout << obstacleflag[j+(k*(xSize_+2))] << "\t";;
     }
     std::cout << std::endl;
    }
    PROGRESS ("Print 2D ARRAY - SUCCESS")
   }

   else
        CHECK_MSG(test == true, "ERROR: CHECK THE ARRAY - HAS OUT OF BOUND VALUES.");
}

void StaggeredGrid::createRectangle(real x1, real y1, real x2, real y2)
{
    for (int i = 1; i < xSize_+2; i++ )
    for (int j = 1; j < ySize_+2; j++ )
    {
        if ((y1 <= (j-1)*dy_) && (y2 >= (j-1)*dy_))
        {
            if ((x1 <= (i-1)*dx_) && (x2 >= (i-1)*dx_))
                setCellToObstacle(i,j);
        }
    }

    if (name_ == "backstep")
    {
        for (int j = 1; j < ySize_+2; j++)
            if ((y1 <= (j-1)*dy_) && (y2 >= (j-1)*dy_))
                setCellToObstacle(0,j);
        for (int i = 1; i < xSize_+2; i++)
            if ((x1 <= (i-1)*dx_) && (x2 >= (i-1)*dx_))
                setCellToObstacle(i,0);
        setCellToObstacle(0,0);
    }
}

void StaggeredGrid::imagewrite()
{
    GrayScaleImage image ( xSize_, ySize_);
     for (int j =1; j<=ySize_; ++j) {
         for (int i=1; i<=xSize_; ++i) {
             image.getElement(i-1,ySize_-j)= (isFluid(i,j))* 255;
         }
     }
     image.save("output.png");
}

void StaggeredGrid::createCircle(real x, real y, real r)
{
    for (int i = 1; i < xSize_+2; i++)
    for (int j = 1; j < ySize_+2; j++)
        if (((pow(x-(i*dx_),2)) + (pow(y-(j*dy_),2))) <= r)
        {
            setCellToObstacle(i,j);
        }

}

void StaggeredGrid::setCellToObstacle(int x, int y)
{
    flag[(y*(xSize_+2))+x] = false;
}

void StaggeredGrid::createObstacleEnumField()
{
    for (int i = 1; i < xSize_+1; i++)
    for (int j = 1; j < ySize_+1; j++)
    {
        if (isFluid(i,j))
        {
            if (!isFluid(i-1,j-1) && !isFluid(i,j-1) && !isFluid(i+1,j-1))
                {
                obstacleflag[((j-1)*(xSize_+2))+i] = NORTH;
                obstacleflag[(j*(xSize_+2))+i] = CENTER;
                }
            else if(!isFluid(i-1,j+1) && !isFluid(i,j+1) && !isFluid(i+1,j+1))
                {
                obstacleflag[((j+1)*(xSize_+2))+i] = SOUTH;
                obstacleflag[(j*(xSize_+2))+i] = CENTER;
                }
            else if(!isFluid(i-1,j+1) && !isFluid(i-1,j) && !isFluid(i-1,j-1))
                {
                obstacleflag[(j*(xSize_+2))+i-1] = EAST;
                obstacleflag[(j*(xSize_+2))+i] = CENTER;
                }
            else if(!isFluid(i+1,j+1) && !isFluid(i+1,j) && !isFluid(i+1,j-1))
                {
                obstacleflag[(j*(xSize_+2))+i] = WEST;
                obstacleflag[(j*(xSize_+2))+i] = CENTER;
                }
            else if (!isFluid(i,j-1) && isFluid(i+1,j) && isFluid(i+1,j-1))
                {
                obstacleflag[((j-1)*(xSize_+2))+i] = NE;
                obstacleflag[(j*(xSize_+2))+i] = CENTER;
                }
            else
                obstacleflag[(j*(xSize_+2))+i] = CENTER;
        }

    }
}

void StaggeredGrid::wrappedaccessu()
{
    for(int i = 1; i < xSize_; i++)
    for(int j = 1; j < ySize_+1; j++)
    {
        if (!isFluid(i,j))
        {
            switch(isObstacleField(i,j))
            {
                case NORTH: u().operator()(i-1,j) = -u().operator()(i-1,j+1);
                            u().operator()(i,j) = -u().operator()(i,j+1);
                            break;
                case SOUTH: u().operator()(i+1,j) = -u().operator()(i+1,j-1);
                            u().operator()(i,j) = -u().operator()(i,j-1);
                            break;
                case WEST:  u().operator() (i-1,j) = 0;
                            break;
                case EAST:  u().operator()(i+1,j) = 0;
                            break;
                case NE:    u().operator() (i,j) = 0;
                            u().operator() (i-1,j) = -u().operator()(i-1,j+1);
                            break;
                case CENTER:u().operator() (i,j) = u().operator() (i,j);
                            break;
            }
        }
    }
}

void StaggeredGrid::wrappedaccessv()
{
for(int i = 1; i < xSize_+1; i++)
    for(int j = 1; j < ySize_; j++)
    {
        if (!isFluid(i,j))
        {
            switch(isObstacleField(i,j))
            {
                case NORTH: v().operator()(i,j) = 0;
                            break;
                case SOUTH: v().operator()(i,j) = 0;
                            break;
                case WEST:  v().operator()(i,j-1) = -v().operator()(i-1,j-1);
                            v().operator()(i,j) = -v().operator()(i-1,j);
                            break;
                case EAST:  v().operator()(i,j+1) = -v().operator()(i+1,j+1);
                            v().operator()(i,j) = -v().operator()(i+1,j);
                            break;
                case NE:    v().operator()(i,j) = 0;
                            v().operator()(i,j-1) = -v().operator()(i+1,j-1);
                            break;
                case CENTER:v().operator()(i,j) = v().operator()(i,j);
                            break;
            }
        }
    }
}

void StaggeredGrid::wrappedaccessf()
{
for(int i = 1; i < xSize_; i++)
    for(int j = 1; j < ySize_+1; j++)
    {
        if (!isFluid(i,j))
        {
            switch(isObstacleField(i,j))
            {
                case NORTH: f().operator()(i,j) = f().operator()(i,j);
                            break;
                case SOUTH: f().operator()(i,j) = f().operator()(i,j);
                            break;
                case WEST:  f().operator() (i-1,j) = u().operator()(i-1,j);
                            break;
                case EAST:  f().operator()(i+1,j) = u().operator()(i+1,j);
                            break;
                case NE:    f().operator() (i,j) = u().operator()(i,j);
                            break;
                case CENTER:f().operator() (i,j) = f().operator() (i,j);
                            break;
            }
        }
    }
}

void StaggeredGrid::wrappedaccessg()
{
for(int i = 1; i < xSize_+1; i++)
    for(int j = 1; j < ySize_; j++)
    {
        if (!isFluid(i,j))
        {
            switch(isObstacleField(i,j))
            {
                case NORTH: g().operator()(i,j) = v().operator()(i,j);
                            break;
                case SOUTH: g().operator()(i,j) = v().operator()(i,j);
                            break;
                case WEST:  g().operator()(i,j) = g().operator()(i,j);
                            break;
                case EAST:  g().operator()(i,j) = g().operator()(i,j);
                            break;
                case NE:    g().operator()(i,j) = v().operator()(i,j);
                            break;
                case CENTER:g().operator()(i,j) = g().operator()(i,j);
                            break;
            }
        }
    }
}

void StaggeredGrid::wrappedaccessp()
{
    for (int i = 1; i < xSize_+1; i++)
    for (int j = 1; j < ySize_+1; j++)
    {
        if (!isFluid(i,j))
        {
            switch (isObstacleField(i,j))
            {
                case NORTH: p().operator()(i,j) = p().operator()(i,j+1);
                            break;
                case SOUTH: p().operator()(i,j) = p().operator()(i,j-1);
                            break;
                case WEST:  p().operator()(i,j) = p().operator()(i-1,j);
                            break;
                case EAST:  p().operator()(i,j) = p().operator()(i+1,j);
                            break;
                case NE:    p().operator()(i,j) = 0.5*(p().operator() (i,j+1)+p().operator()(i+1,j));
                            break;
                case CENTER:p().operator()(i,j) = p().operator()(i,j);
                            break;
            }
        }
    }

}

