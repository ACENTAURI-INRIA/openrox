//==============================================================================
//
//    OPENROX   : File ansi_linsys_se3_light_affine_premul.c
//
//    Contents  : Implementation of linsys_se3_light_affine_premul module
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#include "ansi_linsys_se3_light_affine_premul.h"
#include <stdio.h>
#include <system/errors/errors.h>

int rox_ansi_linsys_se3_light_affine_premul (
   double ** LtL_data, 
   double *  Lte_data,
   double ** K_data,
   double ** T_data,
   float ** Iu_data, 
   float ** Iv_data, 
   float ** Id_data, 
   float ** Ia_data, 
   unsigned int ** Im_data,
   int rows,
   int cols
)
{
   int error = ROX_ERROR_NONE;
      
   float fu = (float) K_data[0][0];
   float fv = (float) K_data[1][1];
   float cu = (float) K_data[0][2];
   float cv = (float) K_data[1][2];

   float r11 = (float) T_data[0][0]; float r12 = (float) T_data[0][1]; float r13 = (float) T_data[0][2]; float tx = (float) T_data[0][3];
   float r21 = (float) T_data[1][0]; float r22 = (float) T_data[1][1]; float r23 = (float) T_data[1][2]; float ty = (float) T_data[1][3];
   float r31 = (float) T_data[2][0]; float r32 = (float) T_data[2][1]; float r33 = (float) T_data[2][2]; float tz = (float) T_data[2][3];

   float taux = -(r11 * tx + ty * r21 + tz * r31);
   float tauy = -(r12 * tx + ty * r22 + tz * r32);
   float tauz = -(r13 * tx + ty * r23 + tz * r33);

   for ( int i = 0; i < rows; i++)
   {
      float L[6] = {0.0};

      float vr = (float)(i);
      float y = (vr - cv)/fv;

      for ( int j = 0; j < cols; j++)
      {
         if (!Im_data[i][j]) continue;

         float ur = (float)(j);

         // Compute normalized coordinates
         float x = (ur - cu)/fu;

         // Retrieve per pixel params
         float Iu_val = Iu_data[i][j];
         float Iv_val = Iv_data[i][j];
         float d = Id_data[i][j];
         float a = Ia_data[i][j];

         float gv = Iv_val * fv;
         float gu = Iu_val * fu;
         float gw = (gu * (x - taux)  + gv * (y - tauy))/tauz;

         // Jacobian row
         // Translation
         L[0] = gu;
         L[1] = gv;
         L[2] = gw;
         // Rotation
         L[3] =   gw * y;
         L[4] = - gw * x;
         L[5] = x * gv - y * gu;

         // Update system
         for (int k = 0; k < 6; k++)
         {
            for (int l = 0; l <= k; l++)
            {
               LtL_data[k][l] += L[k] * L[l];
            }

            Lte_data[k] += L[k] * d;
         }

         LtL_data[6][0] += L[0] * a;
         LtL_data[6][1] += L[1] * a;
         LtL_data[6][2] += L[2] * a;
         LtL_data[6][3] += L[3] * a;
         LtL_data[6][4] += L[4] * a;
         LtL_data[6][5] += L[5] * a;
         LtL_data[6][6] += a * a;

         LtL_data[7][0] += L[0];
         LtL_data[7][1] += L[1];
         LtL_data[7][2] += L[2];
         LtL_data[7][3] += L[3];
         LtL_data[7][4] += L[4];
         LtL_data[7][5] += L[5];
         LtL_data[7][6] += a;
         LtL_data[7][7] += 1;

         Lte_data[6] += a * d;
         Lte_data[7] += d;
      }
   }

   // Symmetrise
   for (int k = 0; k < 8; k++)
   {
      for (int l = k; l < 8; l++)
      {
         LtL_data[k][l] = LtL_data[l][k];
      }
   }

// function_terminate:
   return error;
}
