//==============================================================================
//
//    OPENROX   : File objset_edge_segment_tools.c
//
//    Contents  : Implementation of module odometry segments
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#include "objset_edge_segment_tools.h"

#include <float.h>
#include <generated/dynvec_edge_segment_site_struct.h>

#include <system/time/timer.h>

#include <baseproc/maths/maths_macros.h>
#include <baseproc/array/fill/fillunit.h>
#include <baseproc/array/fill/fillval.h>
#include <baseproc/array/add/add.h>
#include <baseproc/array/multiply/mulmatmat.h>
#include <baseproc/array/scale/scale.h>
#include <baseproc/array/inverse/svdinverse.h>
#include <baseproc/array/norm/norm2sq.h>
#include <baseproc/array/robust/tukey.h>
#include <baseproc/array/robust/huber.h>
#include <baseproc/maths/linalg/matse3.h>
#include <baseproc/maths/maths_macros.h>
#include <baseproc/geometry/line/line2d.h>
#include <baseproc/geometry/line/line3d.h>
#include <baseproc/geometry/point/point2d_tools.h>

#include <inout/numeric/array2d_print.h>
#include <inout/system/print.h>
#include <inout/system/errors_print.h>
#include <baseproc/calculus/jacobians/interaction_row_point_to_line_matse3.h>

Rox_ErrorCode rox_objset_edge_segment_add_segment3d (
   Rox_ObjSet_Edge_Segment objset_edge_segment,
   Rox_Segment3D segment3d,
   const Rox_Double sampling_step
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Edge_Segment toadd = NULL;

   if ( !objset_edge_segment )
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_edge_segment_new ( &toadd, (Rox_Sint) sampling_step );
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_edge_segment_set_segment3d ( toadd, segment3d );
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_objset_edge_segment_append ( objset_edge_segment, toadd );
   ROX_ERROR_CHECK_TERMINATE ( error );

function_terminate:
   if (error) rox_edge_segment_del ( &toadd );
   return error;
}

Rox_ErrorCode rox_edge_segment_build_error (
   Rox_Double ** res_error,
   const Rox_MatUT3 K,
   const Rox_Edge_Segment edge_segment
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!res_error)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Double * ptrRes = *res_error;
   if (!ptrRes)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   for (Rox_Uint idsite = 0; idsite < edge_segment->sites->used; idsite++)
   {
      Rox_Edge_Segment_Site_Struct * site = &edge_segment->sites->data[idsite];
      if (site->state) continue;

      // Normalized coordinates of the point
      Rox_Point2D_Double_Struct point2d_meters;

      error = rox_point2d_convert_pixel_double_to_meter_double ( &point2d_meters, &(site->coords), K );
      ROX_ERROR_CHECK_TERMINATE ( error );

      // Signed distance in normalized coordinates
      error = rox_line2d_normal_signed_distance ( ptrRes, &edge_segment->line_image_meters, &point2d_meters );
      ROX_ERROR_CHECK_TERMINATE ( error );

      if(0)
      {
         Rox_Double error_pixels = 0.0;

         // Signed distance in normalized coordinates
         error = rox_line2d_normal_signed_distance ( &error_pixels, &edge_segment->line_image_pixels, &(site->coords) );
         ROX_ERROR_CHECK_TERMINATE ( error );
      }

      ptrRes++;
   }

   *res_error = ptrRes;

function_terminate:
   return error;
}

#define SAVE_DATA

Rox_ErrorCode rox_edge_segment_build_interaction_matrix (
   Rox_Matrix L,
   Rox_MatUT3 K,
   Rox_Edge_Segment edge_segment
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

#ifdef SAVE_DATA
   FILE * file_lines3d  = fopen("result_lines3d_c_mod_par.txt",  "a");
   FILE * file_lines2d  = fopen("result_lines2d_c_mod_met.txt",  "a");
   FILE * file_points2d = fopen("result_points2d_c_mes_met.txt", "a");
#endif

   if (!L || !K || !edge_segment)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Sint valid_segment_sites = edge_segment->sites->used;

   // error = rox_array2d_double_check_size(L, valid_segment_sites, 6);
   // ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Double ** L_data = NULL;
   error = rox_array2d_double_get_data_pointer_to_pointer( &L_data, L);

   // Get the 2D line parameters in meters
   Rox_Double rho   = edge_segment->line_image_meters.rho;
   Rox_Double theta = edge_segment->line_image_meters.theta;

   Rox_Double sinth = sin(theta);
   Rox_Double costh = cos(theta);

   Rox_Double lt = 0.0;   
   Rox_Double lr = 0.0;

   error = rox_line3d_get_interaction_matrix_point_to_line_parameters ( &lr, &lt, &edge_segment->line );
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Double Lr[6];
   Rox_Double Lt[6];

   error = rox_ansi_interaction_rho_theta_matse3 ( Lr, Lt, lr, lt, rho, costh, sinth );
   ROX_ERROR_CHECK_TERMINATE ( error );

   for (Rox_Sint idsite = 0; idsite < valid_segment_sites; idsite++)
   {
      Rox_Edge_Segment_Site site = &edge_segment->sites->data[idsite];
      if (site->state) continue;

      // Normalized coordinates of the point
      Rox_Point2D_Double_Struct point2d_meters;

      error = rox_point2d_convert_pixel_double_to_meter_double ( &point2d_meters, &(site->coords), K );
      ROX_ERROR_CHECK_TERMINATE ( error );

      Rox_Double x = point2d_meters.u;
      Rox_Double y = point2d_meters.v;
      Rox_Double la = x * sinth - y * costh;

      for (Rox_Sint col = 0; col < 6; col++)
      {
         L_data[idsite][col] = Lr[col] + la * Lt[col];
      }

      #ifdef SAVE_DATA
         fprintf(file_lines3d,  "%3.12f %3.12f \n", lr, lt);
         fprintf(file_lines2d,  "%3.12f %3.12f \n", rho, theta);
         fprintf(file_points2d, "%3.12f %3.12f \n", x, y);
      #endif

   }

function_terminate:
   #ifdef SAVE_DATA
      fclose(file_lines3d);
      fclose(file_lines2d);
      fclose(file_points2d);
   #endif

   return error;
}

Rox_ErrorCode rox_edge_segment_build_linear_system (
   Rox_Matrix LtL,
   Rox_Matrix Lte,
   const Rox_MatUT3 K,
   const Rox_Edge_Segment edge_segment,
   const Rox_Double * ptrError,
   const Rox_Double * ptrWeight
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Double Lr[6];
   Rox_Double Lt[6];
   Rox_Double L[6];

   Rox_Double w = 0.0, e = 0.0;

   if ( !LtL || !Lte )
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if ( !K || !edge_segment || !ptrError || !ptrWeight )
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_array2d_double_check_size ( LtL, 6, 6 );
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_double_fillval ( LtL, 0 );
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_double_check_size ( Lte, 6, 1 );
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_double_fillval ( Lte, 0 );
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Double ** LtL_data = NULL;
   error = rox_array2d_double_get_data_pointer_to_pointer ( &LtL_data, LtL );
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Double * Lte_data = NULL;
   error = rox_array2d_double_get_data_pointer ( &Lte_data, Lte );
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Double ** K_data = NULL;
   error = rox_array2d_double_get_data_pointer_to_pointer ( &K_data, K );
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Double fu = K_data[0][0];
   Rox_Double fv = K_data[1][1];
   Rox_Double cu = K_data[0][2];
   Rox_Double cv = K_data[1][2];

   Rox_Double ifu = 1.0 / fu;
   Rox_Double ifv = 1.0 / fv;
   Rox_Double icu = - cu / fu;
   Rox_Double icv = - cv / fv;

   // Get the 2D line parameters in meters
   Rox_Double rho   = edge_segment->line_image_meters.rho;
   Rox_Double theta = edge_segment->line_image_meters.theta;

   Rox_Double st = sin ( theta );
   Rox_Double ct = cos ( theta );

   Rox_Double lt = 0.0;   
   Rox_Double lr = 0.0;

   error = rox_line3d_get_interaction_matrix_point_to_line_parameters ( &lr, &lt, &edge_segment->line );
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_ansi_interaction_rho_theta_matse3 ( Lr, Lt, lr, lt, rho, ct, st );
   ROX_ERROR_CHECK_TERMINATE ( error );

   for (Rox_Uint idsite = 0; idsite < edge_segment->sites->used; idsite++)
   {
      Rox_Edge_Segment_Site_Struct * site = &edge_segment->sites->data[idsite];
      if (site->state) continue;

      // x and y are the coordinates of the measured point in meters
      Rox_Double x = ifu * site->coords.u + icu;
      Rox_Double y = ifv * site->coords.v + icv;

      Rox_Double la = x * st - y * ct;

      for ( Rox_Uint col = 0; col < 6; col++ )
      {
         L[col] = w * ( Lr[col] + la * Lt[col] );
      }

      // e = x * cos(theta) + y * sin (theta) - rho
      e = *ptrError;

      w = *ptrWeight;

      e = e * w;

      for ( Rox_Uint i = 0; i < 6; i++ )
      {
         for ( Rox_Uint j = 0; j < 6; j++ )
         {
            LtL_data[i][j] += L[i] * L[j];
         }

         Lte_data[i] += L[i] * e;
      }

      ptrError++;
      ptrWeight++;
   }

function_terminate:
   return error;
}


Rox_ErrorCode rox_objset_edge_segment_get_valid_measures (
   Rox_Sint * valid_measures,
   Rox_ObjSet_Edge_Segment objset_edge_segment
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!objset_edge_segment || !valid_measures)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   *valid_measures = 0;
   for (Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      Rox_Sint valid_measures_segment = 0;
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];

      error = rox_edge_segment_get_valid_measures ( &valid_measures_segment, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

      *valid_measures += valid_measures_segment;
   }

function_terminate:
   return error;
}

Rox_ErrorCode rox_objset_edge_segment_transform_project (

   Rox_ObjSet_Edge_Segment objset_edge_segment,
   const Rox_MatUT3 K,
   const Rox_MatSE3 cTo
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   
   if (!objset_edge_segment || !K ||! cTo)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   for (Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];

      error = rox_edge_segment_transform_project ( edge_segment, cTo, K );
      ROX_ERROR_CHECK_TERMINATE ( error );
   }

function_terminate:
   return error;
}

Rox_ErrorCode rox_objset_edge_segment_build_error (
   Rox_Double ** res_error,
   const Rox_MatUT3 K,
   const Rox_ObjSet_Edge_Segment objset_edge_segment
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if ( !objset_edge_segment || !K || ! res_error )
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   for ( Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];
      
      error = rox_edge_segment_build_error ( res_error, K, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );
   }

function_terminate:
   return error;
}


Rox_ErrorCode rox_objset_edge_segment_build_interaction_matrix (
   Rox_Matrix L,
   const Rox_MatUT3 K,
   const Rox_ObjSet_Edge_Segment objset_edge_segment
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Sint initial_row = 0, initial_col = 0, rows = 0, cols = 6;
   Rox_Array2D_Double L_sub = NULL;


   if (!objset_edge_segment || !K ||! L)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   for (Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      // Compute contribution of segment id to linear system
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];

      error = rox_edge_segment_get_valid_measures(&rows, edge_segment);
      ROX_ERROR_CHECK_TERMINATE ( error );

      if (rows == 0) continue;

      // Create submatrix of proper size
      error = rox_array2d_double_new_subarray2d(&L_sub, L, initial_row, initial_col, rows, cols);
      ROX_ERROR_CHECK_TERMINATE ( error );

      // Compute contribution of segment id to interaction matrix
      error = rox_edge_segment_build_interaction_matrix ( L_sub, K, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

      // Add actual rows for next segment
      initial_row += rows;

      error = rox_array2d_double_del(&L_sub);
      ROX_ERROR_CHECK_TERMINATE ( error );
   }

function_terminate:
   if(error) rox_array2d_double_del(&L_sub);
   return error;
}

Rox_ErrorCode rox_objset_edge_segment_build_linear_system (
   Rox_Matrix LtL,
   Rox_Matrix Lte,
   const Rox_MatUT3 K,
   const Rox_ObjSet_Edge_Segment objset_edge_segment,
   Rox_Double ** dverr,
   Rox_Double ** dvw
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Double * ptrErr = NULL, * ptrWeight = NULL;
   Rox_Array2D_Double lLtL = NULL, lLte = NULL;

   error = rox_array2d_double_new(&lLtL, 6, 6);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_double_new(&lLte, 6, 1);
   ROX_ERROR_CHECK_TERMINATE ( error );

   ptrErr = dverr[0];
   ptrWeight = dvw[0];

   for (Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      // Compute contribution of segment id to linear system
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];
      error = rox_edge_segment_build_linear_system ( lLtL, lLte, K, edge_segment, ptrErr, ptrWeight );
      ROX_ERROR_CHECK_TERMINATE ( error );

      // Add contribution of segment id to linear system
      error = rox_array2d_double_add ( LtL, LtL, lLtL );
      ROX_ERROR_CHECK_TERMINATE ( error );

      error = rox_array2d_double_add ( Lte, Lte, lLte );
      ROX_ERROR_CHECK_TERMINATE ( error );

      ptrErr += edge_segment->sites->used;
      ptrWeight += edge_segment->sites->used;
   }

function_terminate:
   rox_array2d_double_del(&lLtL);
   rox_array2d_double_del(&lLte);
   return error;
}

Rox_ErrorCode rox_objset_edge_segment_get_measures (
   Rox_ObjSet_Edge_Segment objset_edge_segment,
   const Rox_Tracking_Segment tracker,
   const Rox_MatSE3 pose,
   const Rox_MatUT3 calibration,
   const Rox_Image image
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   const Rox_Bool log_segments_before = 1;
   static int iter = 0;

   // // Define timer to measure performances
   // Rox_Timer timer = NULL;
   // Rox_Double time = 0.0;
   // Rox_Double time0 = 0.0;
   // Rox_Double time1 = 0.0;
   // Rox_Double time2 = 0.0;
   // Rox_Double time3 = 0.0;
   // Rox_Double time4 = 0.0;

   // // Init new timer
   // error = rox_timer_new(&timer);
   // ROX_ERROR_CHECK_TERMINATE ( error );

   // For each segment
   for ( Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];

   // rox_timer_start(timer);

      error = rox_edge_segment_transform_project ( edge_segment, pose, calibration );
      ROX_ERROR_CHECK_TERMINATE ( error );
   
   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time0 += time;

   // rox_timer_start(timer);

      error = rox_edge_segment_sample ( edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time1 += time;

   // rox_timer_start(timer);

      error = rox_tracking_segment_initialize ( tracker, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time2 += time;


      if ( log_segments_before )
      {
         // rox_edge_segment_log_segments(edge_segment, id, iter);
      }

   // rox_timer_start(timer);

      // Tracking results will be written in edge_segment
      error = rox_tracking_segment_make ( tracker, image, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time3 += time;


   // rox_timer_start(timer);

      // Do not clean sites otherwise we loose count of bad states ???
      error = rox_edge_segment_clean ( edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time4 += time;

   }

   // rox_log("time to transform_project the 3D segments = %f (ms)\n", time0);
   // rox_log("time to sample the 2D segments = %f (ms)\n", time1);
   // rox_logrox_log("time to initialize tracking = %f (ms)\n", time2);
   // rox_log("time to make tracking = %f (ms)\n", time3);
   // rox_log("time to clean segments = %f (ms)\n", time4);

   iter++;

function_terminate:
   return error;
}


Rox_ErrorCode rox_objset_edge_segment_get_measures_gradient (
   Rox_ObjSet_Edge_Segment objset_edge_segment,
   const Rox_Tracking_Segment tracker,
   const Rox_MatSE3 pose,
   const Rox_MatUT3 calibration,
   const Rox_Array2D_Uint gradient_scale,
   const Rox_Array2D_Float gradient_angle
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   const Rox_Bool log_segments_before = 1;
   static int iter = 0;

   // // Define timer to measure performances
   // Rox_Timer timer = NULL;
   // Rox_Double time = 0.0;
   // Rox_Double time0 = 0.0;
   // Rox_Double time1 = 0.0;
   // Rox_Double time2 = 0.0;
   // Rox_Double time3 = 0.0;
   // Rox_Double time4 = 0.0;

   // // Init new timer
   // error = rox_timer_new(&timer);
   // ROX_ERROR_CHECK_TERMINATE ( error );

   // For each segment
   for ( Rox_Uint id = 0; id < objset_edge_segment->used; id++)
   {
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];

   // rox_timer_start(timer);

      error = rox_edge_segment_transform_project ( edge_segment, pose, calibration );
      ROX_ERROR_CHECK_TERMINATE ( error );
   
   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time0 += time;

   // rox_timer_start(timer);

      error = rox_edge_segment_sample ( edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time1 += time;

   // rox_timer_start(timer);

      error = rox_tracking_segment_initialize ( tracker, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time2 += time;


      if ( log_segments_before )
      {
         // rox_edge_segment_log_segments(edge_segment, id, iter);
      }

   // rox_timer_start(timer);

      // Tracking results will be written in edge_segment
      error = rox_tracking_segment_make_gradient ( tracker, gradient_scale, gradient_angle, edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time3 += time;


   // rox_timer_start(timer);

      // Do not clean sites otherwise we loose count of bad states ???
      error = rox_edge_segment_clean ( edge_segment );
      ROX_ERROR_CHECK_TERMINATE ( error );

   // // Display elapsed time
   // rox_timer_stop(timer);
   // rox_timer_get_elapsed_ms(&time, timer);

   // time4 += time;

   }

   // rox_log("time to transform_project the 3D segments = %f (ms)\n", time0);
   // rox_log("time to sample the 2D segments = %f (ms)\n", time1);
   // rox_log("time to initialize tracking = %f (ms)\n", time2);
   // rox_log("time to make tracking = %f (ms)\n", time3);
   // rox_log("time to clean segments = %f (ms)\n", time4);

   iter++;

function_terminate:
   return error;
}


Rox_ErrorCode rox_objset_edge_segment_get_score (
   Rox_Double * score,
   const Rox_ObjSet_Edge_Segment objset_edge_segment,
   const Rox_Double score_max
)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if ( !score || !objset_edge_segment )

   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Sint nbs = objset_edge_segment->used;

   Rox_Double local_score = 0;

   for ( Rox_Sint id = 0; id < nbs; id++)
   {
      Rox_Edge_Segment edge_segment = objset_edge_segment->data[id];

      Rox_Sint inliers = 0;
      Rox_Sint outliers = 0;

      Rox_Double score_segment = 0.0;
      Rox_Sint nbp = edge_segment->sites->used;

      // For each segment lopp over sites
      for ( Rox_Sint idsite = 0; idsite < nbp; idsite++)
      {
         Rox_Edge_Segment_Site_Struct * site = &edge_segment->sites->data[idsite];
         if (site->state)
         {
            outliers++;
            continue;
         }
         else
         {
            inliers++;
         }

         Rox_Double distance = 0.0;
         // Unsigned distance in pixels coordinates
         error = rox_line2d_normal_unsigned_distance ( &distance, &edge_segment->line_image_pixels, &site->coords );

         score_segment += distance;
      }

      // If more than half of the points are inliers then compute score, otherwise set score max
      if ((inliers > 0) && (inliers > nbp/2+1))
      {
         local_score += score_segment/inliers;
      }
      else
      {
         local_score += score_max;
      }
   }

   if (nbs > 0)
   {
      local_score = local_score /nbs;
   }
   else
   {
      local_score = score_max;
   }

   *score = local_score;

function_terminate:
   return error;
}
