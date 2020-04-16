#ifndef ARRAY_HH
#define ARRAY_HH


#include "Types.hh"
#include "Debug.hh"
#include <vector>
#include <iostream>

//*******************************************************************************************************************
/*!  Array class for 1,2 and 3 dimensions
*
*    - all elements should be stored in a contiguous chunk of memory ( no vector<vector> ! )
*/
//*******************************************************************************************************************
class Array
{
public:


   // Constructors for 1D,2D and 3D
   Array( int xSize );
   Array( int xSize, int ySize );
   Array( int xSize, int ySize, int zSize );

   // Depending on your implementation you might need the following:
   // ~Array();
   // Array(const Array& s);
   // Array& operator= (const Array& s);
   ~Array() {}

   // Access Operators for 1D, 2D and 3D
   inline real & operator () ( int i );
   inline real & operator () ( int i ,int j );
   inline real & operator () ( int i, int j, int k );

   // for const Arrays the following access operators are required
   // inline const real & operator () ( int i ) const;
   // inline const real & operator () ( int i ,int j ) const;
   // inline const real & operator () ( int i, int j, int k ) const;



   // initialize the whole array with a constant value
   void fill( real value );


   // return total size of the array
   int getSize() const;

   // return xSize for dimension==0, ySize for dimension==1 and zSize for dimension==2
   // other dimension values are not allowed
   int getSize(int dimension ) const;


   // Print the whole array ( for debugging purposes )
   void print();
   void printfile();

   //Multiply any elements
   void multiply(int x, int y, int multip);


    real rows() const { return rows_;}
    real column() const { return column_;}

   private:

   std::vector<real> matrix;
   int rows_;
   int column_;
   int space;
   int dimension;


};


//===================================================================================================================
//
//  Inline Access Operators and Sizes
//
//===================================================================================================================


// Operator() 1D
inline real& Array::operator ()(int i)
{
    CHECK_MSG (i <= rows_, "ERROR:::Out of Bounds: Trying to access elements Out of Bounds in 1D Array");
    CHECK_MSG (dimension == 0, "ERROR::: Dimensions Doesnot Match in 1D Array");

   return matrix[i];
}

// Operator() 2D
inline real& Array::operator ()(int i,int j)
{
    CHECK_MSG(i <= rows_ && j <= column_, "ERROR:::Out of Bounds: Trying to access elements Out of Bounds in 2D Array");
    CHECK_MSG(dimension == 1, "ERROR::: Dimensions Doesnot Match in 2D Array");

   return matrix[(j*(rows_))+i];
}

// Operator() 3D
inline real& Array::operator ()(int i, int j, int k)
{
    CHECK_MSG (i <= rows_ && j <= column_ && k <= space, "ERROR:::Out of Bounds: Trying to access elements Out of Bounds in 3D Array");
    CHECK_MSG (dimension == 2, "ERROR::: Dimensions Doesnot Match in 3D Array");

   return matrix[((i*(column_)+j)+(k*(rows_)*(column_)))];
}




#endif //ARRAY_HH

