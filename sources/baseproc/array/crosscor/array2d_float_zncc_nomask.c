//==============================================================================
//
//    OPENROX   : File array2d_float_zncc_nomask.c
//
//    Contents  : Implementation of array2d_float_zncc_nomask module
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#include "array2d_float_zncc_nomask.h"

#include <float.h>

#include <baseproc/maths/maths_macros.h>

#include <inout/system/errors_print.h>
#include <inout/image/pgm/pgmfile.h>

Rox_ErrorCode rox_array2d_float_zncc_nomask (
   Rox_Double * cc, 
   const Rox_Array2D_Float one, 
   const Rox_Array2D_Float two
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Float gcc;
   Rox_Uint count;
   Rox_Float sum1, sum2, sumsq1, sumsq2;
   Rox_Double mean1, mean2, nom, denom1, denom2, score;

   if (!one || !two  || !cc) 
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Sint cols = 0, rows = 0;
   error = rox_array2d_float_get_size(&rows, &cols, one);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_float_check_size(two, rows, cols); 
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Float ** dd1 = NULL;
   error = rox_array2d_float_get_data_pointer_to_pointer ( &dd1, one );
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Float ** dd2 = NULL;
   error = rox_array2d_float_get_data_pointer_to_pointer ( &dd2, two );
   ROX_ERROR_CHECK_TERMINATE ( error );

   sum1 = 0;
   sum2 = 0;
   sumsq1 = 0;
   sumsq2 = 0;
   count = 0;
   gcc = 0;

   for ( Rox_Sint i = 0; i < rows; i++)
   {
      for ( Rox_Sint j = 0; j < cols; j++)
      {
         sum1 += dd1[i][j];
         sum2 += dd2[i][j];
         sumsq1 += dd1[i][j] * dd1[i][j];
         sumsq2 += dd2[i][j] * dd2[i][j];
         gcc += dd1[i][j] * dd2[i][j];
         count++;
      }
   }

   if (count == 0) 
   { error = ROX_ERROR_ZNCC_UNDEFINED; ROX_ERROR_CHECK_TERMINATE ( error ); }

   mean1 = ((double)sum1) / ((double)count);
   mean2 = ((double)sum2) / ((double)count);

   nom = ((Rox_Double)gcc) - mean1*mean2*(Rox_Double)count;
   denom1 = (Rox_Double)sumsq1 - mean1 * mean1 * (Rox_Double)count;
   denom2 = (Rox_Double)sumsq2 - mean2 * mean2 * (Rox_Double)count;

   //   if (denom1 == 0.0 || denom2 == 0.0) 
   //   { error = ROX_ERROR_ZNCC_UNDEFINED; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if (denom1 < DBL_EPSILON || denom2 < DBL_EPSILON) 
   { error = ROX_ERROR_ZNCC_UNDEFINED; ROX_ERROR_CHECK_TERMINATE ( error ); }

   score = nom / (sqrt(denom1)*sqrt(denom2));
   *cc = score;

function_terminate:
   return error;
}
