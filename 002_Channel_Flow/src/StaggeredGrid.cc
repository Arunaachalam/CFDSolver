#include "StaggeredGrid.hh"
#include "Array.hh"
#include "FileReader.hh"

#include <iostream>
//
///////////////////////////////////////////////////////////////////////////////////

//Constructors

///////////////////////////////////////////////////////////////////////////////////

StaggeredGrid::StaggeredGrid(int xSize, int ySize, real dx, real dy)
:p_(xSize+2,ySize+2),rhs_(xSize+2,ySize+2),u_(xSize+1,ySize+2),v_(xSize+2,ySize+1),f_(xSize+1,ySize+2),g_(xSize+2,ySize+1)
{

	dx_ = dx/xSize;
	dy_ = dy/ySize;
    PROGRESS ("STAGGERED GRIDS CREATED")
}

StaggeredGrid::StaggeredGrid ( const FileReader & configuration )
:p_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+2), rhs_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+2),u_(configuration.getIntParameter("imax")+1,configuration.getIntParameter("jmax")+2),v_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+1),f_(configuration.getIntParameter("imax")+1,configuration.getIntParameter("jmax")+2),g_(configuration.getIntParameter("imax")+2,configuration.getIntParameter("jmax")+1)
{

    dx_ = configuration.getRealParameter("xlength")/configuration.getIntParameter("imax");
    dy_ = configuration.getRealParameter("ylength")/configuration.getIntParameter("jmax");
    xSize_ = configuration.getRealParameter("imax");
    ySize_ = configuration.getRealParameter("jmax");
    PROGRESS ("STAGGERED GRIDS CREATED")

}
