//==============================================================================
//
//    OPENROX   : File motion_detection.c
//
//    Contents  : Implementation motion detection module
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#include "motion_detection.h"
#include "cluster_struct.h"

#include <string.h>
#include <generated/array2d_uint.h>
#include <generated/dynvec_rect_sint_struct.h>
#include <generated/dynvec_point2d_float_struct.h>

#include <system/memory/memory.h>

#include <baseproc/maths/maths_macros.h>
#include <baseproc/maths/kernels/gaussian2d.h>
#include <baseproc/image/convolve/array2d_float_symmetric_separable_convolve.h>
#include <baseproc/array/fill/fillval.h>
#include <baseproc/array/substract/substract.h>
#include <baseproc/array/conversion/array2d_uchar_from_float.h>

#include <inout/system/errors_print.h>

//! \ingroup Detection_Motion
//! \brief Motion detection structure
struct Rox_Detection_Motion_Params_Struct
{
  //! *****************
  //! *** IMPORTANT ***
  //! *****************
  //! If you aim at changing the data structure, you need to
  //! modify the save and load methods of the track structure too!

   //! to be commented
   Rox_Sint  bandwidth[2];

   //! to be commented
   Rox_Uchar sensitivity;
};

//! \ingroup Detection_Motion
//! \brief Motion detection structure
struct Rox_Detection_Motion_Struct
{
   //! *****************
   //! *** IMPORTANT ***
   //! *****************
   //! If you aim at changing the data structure, you need to
   //! modify the save and load methods of the track structure too!

   // Params struct
   Rox_Detection_Motion_Params params;

   // Data struct

   //! model image
   Rox_Image     model;
   //! To be commented
   Rox_Array2D_Float     Id;
   //! Current image
   Rox_Image     Ic;
   //! To be commented
   Rox_Imask      M;
   //! To be commented
   Rox_Cluster      cluster;
   //! To be commented
   Rox_DynVec_Rect_Sint  windows;
};

Rox_ErrorCode
rox_points2d_get_center_rect(Rox_DynVec_Point2D_Float center_list, Rox_DynVec_Rect_Sint list);

Rox_ErrorCode
rox_rect_list_set_cluster(Rox_DynVec_Rect_Sint list, Rox_Cluster cluster);

Rox_ErrorCode
rox_array2d_float_erode(Rox_Array2D_Float output, Rox_Array2D_Float input, Rox_Float threshold);

Rox_ErrorCode
rox_array2d_uchar_apply_mask(Rox_Image out, Rox_Imask mask);

Rox_ErrorCode
rox_array2d_uint_set_ones_rectangle(Rox_Imask mask, Rox_Sint posu, Rox_Sint posv, Rox_Sint sizu, Rox_Sint sizv);

Rox_ErrorCode rox_points2d_get_center_rect(Rox_DynVec_Point2D_Float center_list, Rox_DynVec_Rect_Sint list)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Rect_Sint rect = NULL;
   Rox_Point2D_Float_Struct point ;

   rox_dynvec_point2d_float_reset(center_list);

   for ( Rox_Uint k = 0; k < list->used; k++)
   {
      rect = &(list->data[k]);

      point.u = (Rox_Float) (rect->x + rect->width/2.0);
      point.v = (Rox_Float) (rect->y + rect->height/2.0);

      error = rox_dynvec_point2d_float_append(center_list, &point);
   }

   return error;
}

Rox_ErrorCode rox_array2d_float_erode(Rox_Array2D_Float output, Rox_Array2D_Float input, Rox_Float threshold)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!output || !input)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if (threshold < 0 || threshold > 1)
   { error = ROX_ERROR_INVALID_VALUE; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_array2d_float_match_size(output, input);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Sint rows = 0, cols = 0;
   error = rox_array2d_float_get_size(&rows, &cols, input);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Float ** inp_data = NULL;
   error = rox_array2d_float_get_data_pointer_to_pointer( &inp_data, input);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Float ** out_data = NULL;
   error = rox_array2d_float_get_data_pointer_to_pointer( &out_data, output);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Float s = threshold * 255.0f;
   for ( Rox_Sint r = 1; r < rows-1; r++)
   {
      for ( Rox_Sint c = 1; c < cols-1; c++)
      {
         if(   (fabs(inp_data[r-1][c-1])>s) && (fabs(inp_data[r-1][c])>s) && (fabs(inp_data[r-1][c+1])>s)
            && (fabs(inp_data[r][c-1])>s)   && (fabs(inp_data[r][c])>s)   && (fabs(inp_data[r][c+1])>s)
            && (fabs(inp_data[r+1][c-1])>s) && (fabs(inp_data[r+1][c])>s) && (fabs(inp_data[r+1][c+1])>s))
         {
            out_data[r][c] = 255.0;
         }
         else
         {
            out_data[r][c] = 0.0;
         }
      }
   }

   // Fill first and last rows with 0
   memset(out_data[0], 0, cols * sizeof(Rox_Float));
   memset(out_data[rows-1], 0, cols * sizeof(Rox_Float));

   // Fill first and last cols with 0
   for ( Rox_Sint r = 0; r < rows; r++)
   {
      out_data[r][0] = 0.0;
      out_data[r][cols -1] = 0.0;
   }

function_terminate:
   return error;
}

Rox_ErrorCode rox_array2d_uchar_apply_mask(Rox_Image out, Rox_Imask mask)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if(!out || !mask)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_array2d_match_size((Rox_Array2D)out, (Rox_Array2D)mask);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Sint cols = 0, rows = 0;

   error = rox_array2d_uchar_get_size(&rows, &cols, out);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Uchar ** data_out = NULL;
   error = rox_array2d_uchar_get_data_pointer_to_pointer ( &data_out, out);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Uint ** data_mask = NULL;
   error = rox_array2d_uint_get_data_pointer_to_pointer ( &data_mask, mask);
   ROX_ERROR_CHECK_TERMINATE ( error );

   if(!data_out || !data_mask)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   for ( Rox_Sint i = 0; i < rows; i++)
   {
      for ( Rox_Sint j = 0; j < cols; j++)
      {
         if(data_mask[i][j] == 0) data_out[i][j] = 0;
      }
   }

function_terminate:
   return error;
}

Rox_ErrorCode rox_array2d_uint_set_ones_rectangle(Rox_Imask mask, Rox_Sint posu, Rox_Sint posv, Rox_Sint sizu, Rox_Sint sizv)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (mask == NULL)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Sint cols = 0, rows = 0;
   error = rox_array2d_uint_get_size(&rows, &cols, mask);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Uint ** mask_data = NULL;
   error = rox_array2d_uint_get_data_pointer_to_pointer(&mask_data, mask);
   ROX_ERROR_CHECK_TERMINATE ( error );

   if (posu > cols || posv > rows)
   { error = ROX_ERROR_INVALID_VALUE; ROX_ERROR_CHECK_TERMINATE ( error ); }

   // reset mask
   error = rox_array2d_uint_fillval(mask, 0);
   ROX_ERROR_CHECK_TERMINATE(error)

   // Fill mask
   Rox_Sint end_cols = posu + sizu;
   Rox_Sint end_rows = posv + sizv;

   if(end_cols > cols) end_cols = cols;
   if(end_rows > rows) end_rows = rows;

   for ( Rox_Sint r = posv; r < end_rows; r++)
   {
      for ( Rox_Sint c = posu; c < end_cols; c++)
      {
         mask_data[r][c] = ~0;
      }
   }

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_params_new(Rox_Detection_Motion_Params * params)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Detection_Motion_Params ret = NULL;

   if (!params)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE(error)}

   ret = (Rox_Detection_Motion_Params) rox_memory_allocate(sizeof(*ret), 1);
   if (!ret)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE(error)}

   ret->bandwidth[0] = 20;
   ret->bandwidth[1] = 30;
   ret->sensitivity = 1;

   *params = ret;

function_terminate:
   if(error != ROX_ERROR_NONE) rox_detection_motion_params_del(&ret);
   return error;
}

Rox_Void rox_detection_motion_params_del(Rox_Detection_Motion_Params * params)
{
   Rox_Detection_Motion_Params todel = NULL;

   if(params != 0)
   {
      todel = *params;
      *params = 0;

      if(todel != 0)
      rox_memory_delete(todel);
   }
}

Rox_ErrorCode rox_detection_motion_params_set_bandwidth(Rox_Detection_Motion_Params params, const Rox_Sint sizu, const Rox_Sint sizv)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!params)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if (sizu < 1 || sizu > 255)
   { error = ROX_ERROR_INVALID_VALUE; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if (sizv < 1 || sizv > 255)
   { error = ROX_ERROR_INVALID_VALUE; ROX_ERROR_CHECK_TERMINATE ( error ); }

   params->bandwidth[0] = sizu;
   params->bandwidth[1] = sizv;

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_params_set_sensitivity(Rox_Detection_Motion_Params params, Rox_Float sensitivity)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (params == NULL)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if (sensitivity < 0.0f || sensitivity > 1.0f)
   { error = ROX_ERROR_INVALID_VALUE; ROX_ERROR_CHECK_TERMINATE ( error ); }

   params->sensitivity = (Rox_Uchar)sensitivity;

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_new(Rox_Detection_Motion * motion, Rox_Image model)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Detection_Motion ret = NULL;

   if (!motion || !model)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   ret = (Rox_Detection_Motion)rox_memory_allocate(sizeof(*ret), 1);
   if(!ret)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Sint cols = 0, rows = 0;
   error = rox_array2d_uchar_get_size(&rows, &cols, model);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_uchar_new(&ret->model, rows, cols);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_uchar_copy(ret->model, model);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_float_new(&ret->Id, rows, cols);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_uchar_new(&ret->Ic, rows, cols);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_uint_new(&ret->M, rows, cols);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_uint_fillval(ret->M, 0);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_detection_motion_params_new(&ret->params);
   ROX_ERROR_CHECK_TERMINATE ( error );

   *motion = ret;

function_terminate:
   if(error != ROX_ERROR_NONE) rox_detection_motion_del(&ret);
   return error;
}

Rox_ErrorCode rox_detection_motion_new_init_params(Rox_Detection_Motion * motion, Rox_Image model, Rox_Detection_Motion_Params params)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Detection_Motion ret = 0;

   if(!motion || !params || !model)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_detection_motion_new(&ret, model);
   ROX_ERROR_CHECK_TERMINATE ( error );

   ret->params->bandwidth[0] = params->bandwidth[0];
   ret->params->bandwidth[1] = params->bandwidth[1];
   ret->params->sensitivity = params->sensitivity;

   *motion = ret;

function_terminate:

   if(error != ROX_ERROR_NONE) rox_detection_motion_del(&ret);
   return error;
}

Rox_ErrorCode rox_detection_motion_set_window_list (
   Rox_DynVec_Rect_Sint list, Rox_Detection_Motion motion, Rox_Image image)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Cluster cluster = NULL;
   Rox_Float sensitivity = 0.0;
   Rox_Float sigma = 1.5;

   Rox_Array2D_Float hfilter = NULL;
   Rox_Array2D_Float vfilter = NULL;
   Rox_Array2D_Float If = NULL;

   if(!list || !motion || !image)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   sensitivity = (50.0f - 30.0f * motion->params->sensitivity) / 255.0f;

   error = rox_cluster_new(&cluster);
   ROX_ERROR_CHECK_TERMINATE ( error );

   // Difference between the current image and the model
   error = rox_array2d_float_substract_uchar(motion->Id, motion->model, image);

   // Filtering to remove noise
   Rox_Sint cols = 0, rows = 0;
   error = rox_array2d_float_get_size(&rows, &cols, motion->Id);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_float_new(&If, rows, cols);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_kernelgen_gaussian2d_separable_float_new(&hfilter, &vfilter, sigma, 3 * sigma);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_array2d_float_symmetric_seperable_convolve(If, motion->Id, hfilter);
   ROX_ERROR_CHECK_TERMINATE ( error );

   // Erode filtered image
   error = rox_array2d_float_erode(motion->Id, If, sensitivity);
   ROX_ERROR_CHECK_TERMINATE ( error );

   // Conversion float to uchar
   error = rox_array2d_uchar_from_float(motion->Ic, motion->Id);
   ROX_ERROR_CHECK_TERMINATE ( error );

   // Apply mask
   error = rox_array2d_uchar_apply_mask(motion->Ic, motion->M);
   ROX_ERROR_CHECK_TERMINATE ( error );

   error = rox_cluster_binary(cluster, motion->Ic, motion->params->bandwidth);
   ROX_ERROR_CHECK_TERMINATE ( error );

   // Convert cluster to window
   error = rox_rect_list_set_cluster(list, cluster);
   ROX_ERROR_CHECK_TERMINATE ( error );

function_terminate:
   rox_array2d_float_del(&If);
   rox_cluster_del(&cluster);
   rox_array2d_float_del(&vfilter);
   rox_array2d_float_del(&hfilter);
   return error;
}

Rox_ErrorCode rox_array2d_float_convert_binarize_uchar(Rox_Array2D_Float image_float, Rox_Image image_uchar, Rox_Uchar theshold)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if(!image_uchar || !image_float)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   Rox_Float ** data_float = NULL;
   error = rox_array2d_float_get_data_pointer_to_pointer(&data_float, image_float);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Uchar ** data_uchar = NULL;
   error = rox_array2d_uchar_get_data_pointer_to_pointer(&data_uchar, image_uchar);
   ROX_ERROR_CHECK_TERMINATE ( error );

   Rox_Sint cols = 0, rows = 0;
   error = rox_array2d_float_get_size(&rows, &cols, image_float);
   ROX_ERROR_CHECK_TERMINATE ( error );

   for ( Rox_Sint r=0;r<rows;r++)
   {
      for ( Rox_Sint c=0;c<cols;c++)
      {
         if(data_uchar[r][c] > theshold)
         {
            data_float[r][c] = 255.0;
         }
         else
         {
            data_float[r][c] =   0.0;
         }
      }
   }

function_terminate:
   return error;
}

Rox_Void rox_detection_motion_del(Rox_Detection_Motion * motion)
{
   Rox_Detection_Motion todel = NULL;

   if(motion != 0)
   {
      todel = *motion;
      *motion = 0;

      if(todel != 0)
      {
         rox_detection_motion_params_del(&todel->params);
         rox_array2d_uchar_del(&todel->model);
         rox_array2d_float_del(&todel->Id);
         rox_array2d_uint_del(&todel->M);
         rox_array2d_uchar_del(&todel->Ic);
         rox_memory_delete(todel);
      }
   }
}

Rox_ErrorCode rox_detection_motion_set_imask_window_list(Rox_Detection_Motion motion, Rox_DynVec_Rect_Sint list)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!motion || !list)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   for ( Rox_Uint i = 0; i < list->used; i++)
   {
      error = rox_detection_motion_set_imask_window(motion, &list->data[i]);
      ROX_ERROR_CHECK_TERMINATE ( error );
   }

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_set_model(Rox_Detection_Motion motion, Rox_Image model)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!motion || !model)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_array2d_uchar_copy(motion->model, model);
   ROX_ERROR_CHECK_TERMINATE ( error );

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_set_imask(Rox_Detection_Motion motion, Rox_Imask mask)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!motion || !mask)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_array2d_uint_copy(motion->M, mask);
   ROX_ERROR_CHECK_TERMINATE ( error );

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_set_imask_window(Rox_Detection_Motion motion, Rox_Rect_Sint window)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!motion)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   error = rox_array2d_uint_set_ones_rectangle(motion->M, window->x, window->y, window->width, window->height);
   ROX_ERROR_CHECK_TERMINATE ( error );

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_set_bandwidth(Rox_Detection_Motion motion, const Rox_Sint sizu, const Rox_Sint sizv)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if(!motion)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   motion->params->bandwidth[0] = sizu;
   motion->params->bandwidth[1] = sizv;

function_terminate:
   return error;
}

Rox_ErrorCode rox_detection_motion_set_sensitivity(Rox_Detection_Motion motion, Rox_Float sensitivity)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   if (!motion)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   if (sensitivity < 0.0 || sensitivity > 1.0)
   {error = ROX_ERROR_INVALID_VALUE; ROX_ERROR_CHECK_TERMINATE ( error ); }

   motion->params->sensitivity = (Rox_Uchar)sensitivity;

function_terminate:
   return error;
}

Rox_ErrorCode rox_rect_list_set_cluster(Rox_DynVec_Rect_Sint window_list, Rox_Cluster cluster)
{
   Rox_ErrorCode error = ROX_ERROR_NONE;

   // Rox_Sint count = 0;

   if (!window_list || !cluster)
   { error = ROX_ERROR_NULL_POINTER; ROX_ERROR_CHECK_TERMINATE ( error ); }

   // Reset the number of used cells
   rox_dynvec_rect_sint_reset(window_list);

   for ( Rox_Sint i = 0; i < cluster->count; i++)
   {
      Rox_Sint posu = cluster->bounds[4*i];
      Rox_Sint posv = cluster->bounds[4*i+1];
      Rox_Sint sizu = cluster->bounds[4*i+2] - cluster->bounds[4*i];
      Rox_Sint sizv = cluster->bounds[4*i+3] - cluster->bounds[4*i+1];

      // keep only clusters larger than 2 x 2
      if((sizu > 3) && (sizv > 3))
      {
         Rox_Rect_Sint_Struct window;

         window.x = posu;
         window.y = posv;
         window.width = sizu;
         window.height = sizv;

         error = rox_dynvec_rect_sint_append(window_list, &window);
         ROX_ERROR_CHECK_TERMINATE ( error );

         // count++;
      }
   }

function_terminate:
   return error;
}

