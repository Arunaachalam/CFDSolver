#include "Array.hh"
#include "Debug.hh"


#include <iostream>
#include <array>
#include <fstream>
#include <iomanip>

//===================================================================================================================
//
//  Constructors
//
//===================================================================================================================

// Construct 1D array
Array::Array( int xSize )
{
   rows_ = xSize;
   dimension = 0;
   matrix.resize(xSize);
   PROGRESS ("1D Array Constructor RUN")
}

//Consruct 2D array
Array::Array( int xSize, int ySize )
{
   rows_ = xSize;
   column_ = ySize;
   dimension = 1;
   matrix.resize(xSize*ySize);
   PROGRESS ("2D Array Constructor RUN")
}

//Construct 3D array
Array::Array( int xSize, int ySize, int zSize )
{
   rows_ = xSize;
   column_ = ySize;
   space = zSize;
   dimension = 2;
   matrix.resize(xSize*ySize*zSize);
   PROGRESS ("3D Array Constructor RUN")
}




//===================================================================================================================
//
//  Convenience Functions
//
//===================================================================================================================

//Multiply a specific element
void Array::multiply(int x, int y, int multip)
{
   matrix[x+(y*rows_)] *= multip;
   PROGRESS ("MULTIPLY ONE ELEMENT WITH A NUMBER - SUCCESS")

}

//initialize the whole array with a constant value
void Array::fill( real value )
{

   int i = getSize();
   matrix.assign(i,value);
   PROGRESS ("FILL ALL ELEMENTS WITH A INITIAL VALUE - SUCCESS")

}


// Print the whole array (for debugging purposes)
void Array::print()
{
   bool test = false;
   if (dimension == 0)
   {
    test = true;
    for (int i = 0; i < rows_; i++)
        std::cout << matrix[i] << "\t";
    PROGRESS ("Print 1D ARRAY - SUCCESS")
   }

   else if (dimension == 1)
   {
    test = true;
    for (int k = column_-1; k >= 0; k--)
    {
     for (int j = 0; j < rows_; j++)
     {
        std::cout << matrix[j+(k*(rows_))] << "\t";;
     }
     std::cout << std::endl;
    }
    PROGRESS ("Print 2D ARRAY - SUCCESS")
   }

   else if (dimension == 2)
   {
    test = true;
    for(int k = 0; k < space; k++)
    {
        for (int j = column_-1; j >= 0; j--)
        {
         for (int i = 0; i < rows_; i++)
         {
            std::cout << matrix[(i+(j*rows_))+(k*rows_*column_)];
         }
        }
    }
    PROGRESS ("Print 3D ARRAY - SUCCESS")
   }
   else
        CHECK_MSG(test == true, "ERROR: CHECK THE ARRAY - HAS OUT OF BOUND VALUES.");
}

//Print file in a text file
void Array::printfile()
{
    std::ofstream myfile;
    myfile.open("datas.txt");
    for (int k = column_-1; k >= 0; k--)
    {
     for (int j = 0; j < rows_; j++)
     {
        myfile << j << '\t' << k << '\t' << std::setprecision(15) << matrix[j+(k*(rows_))] << std::endl;
     }
     myfile << std::endl;
    }
    myfile.close();
    PROGRESS ("DATA file Created - datas.txt")
}

//Return dimensions
int Array::getSize( int dimension ) const
{
   if (dimension == 2)
	{
        return space;
	}
   else if (dimension == 1)
   {
        return column_;
   }
   else if (dimension == 0)
        return rows_;
    else
    {
        CHECK_MSG (0,"ERROR In DIMENSION : OUT OF BOUNDS");
        return 0;
    }

}

//return total size of the array
int Array::getSize() const
{
    if(dimension == 0)
        return rows_;
    else if (dimension == 1)
        return rows_*column_;
    else if (dimension == 2)
        return rows_*column_*space;
    else
    {
        CHECK_MSG(0,"ERROR In DIMENSION : OUT OF BOUNDS");
        return 0;
    }
}
