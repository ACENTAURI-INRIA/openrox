//==============================================================================
//
//    OPENROX   : File test_mean.cpp
//
//    Contents  : Tests for mean.c
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

//=== INCLUDED HEADERS   =======================================================

#include <openrox_tests.hpp>

extern "C"
{
   #include <float.h>

	 #include <baseproc/array/mean/mean.h>
   #include <baseproc/array/error/l2_error.h>
   #include <baseproc/array/fill/fillval.h>
   #include <inout/numeric/array2d_print.h>
   #include <inout/system/print.h>
   #include <system/time/timer.h>
}

//=== INTERNAL MACROS    =======================================================

ROX_TEST_SUITE_BEGIN(mean)

//=== INTERNAL TYPESDEFS =======================================================

//=== INTERNAL DATATYPES =======================================================

//=== INTERNAL VARIABLES =======================================================

//=== INTERNAL FUNCTDEFS =======================================================

//=== INTERNAL FUNCTIONS =======================================================

//=== EXPORTED FUNCTIONS =======================================================

ROX_TEST_CASE_DECLARE ( rox::OpenROXTest, test_nreg_array2d_double_mean )
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Double l2_error = 0.0;
   Rox_Sint rows = 13;
   Rox_Sint cols = 21;

   Rox_Double one_data[23*41] = {  0.4476,  0.0438,  0.6968, -1.5721,  1.2705,  1.2464, -1.1082, -0.4893, -1.8213, -0.3831,  0.3928, -0.7733, -2.5130,  1.2432,  0.2540,  0.7158,  1.1996, -2.1243,  0.0270, -0.3318, -1.3147,
                                  -1.4356,  0.0028,  0.4466, -1.8277,  1.1934,  0.1235, -0.1845,  0.4030,  1.3008,  1.0763, -0.0420, -0.6287,  0.2763,  0.7508, -1.0491, -0.0017, -0.2220,  0.7827,  0.5746, -0.6334, -1.4102,
                                   1.1417,  0.3191,  0.5072, -0.0131, -1.9551,  0.0927,  0.5214, -3.3285,  0.4487,  0.3956,  0.0967,  0.9380, -0.1465,  1.7830,  0.8554,  1.9401, -0.2038, -0.2341,  0.2940, -0.4164,  1.9230,
                                  -0.8553, -0.4978,  0.2061, -0.9562,  0.6035, -0.1456,  1.4673,  1.1927, -1.2447,  1.8723,  0.9115, -0.3500, -0.3432,  0.6976, -0.7517, -1.3177, -0.7545, -0.3578,  0.0390, -0.8602, -0.5722,
                                   0.5089,  1.4331,  0.1160,  1.2590,  0.4601, -1.5316,  1.1371,  1.2503, -0.9839, -0.3369,  0.1424,  0.0731,  0.9940,  2.2947, -0.7595, -2.0123,  0.3352, -0.9718, -0.3503, -0.0043,  0.2947,
                                  -1.3691, -0.2904,  0.6466,  0.1373, -0.8224, -1.2330,  0.6821,  1.5786, -0.6367, -2.1851,  0.1920, -1.6214, -0.7490, -1.8882, -0.2143, -0.0911, -0.2056, -1.3105,  0.6214,  0.9010, -0.7655,
                                  -0.4509,  0.1037,  0.9938, -0.7167, -0.8137,  0.3833, -0.0428,  0.0537, -1.2522, -0.5950,  0.0431,  1.0441, -0.4199,  0.5474, -0.7516, -0.2164,  0.2636, -1.0516, -1.2603, -0.6376,  0.6155,
                                   0.7078, -1.2425, -0.5998,  0.3078, -0.2522, -0.1636,  0.9047,  0.5249, -0.1004,  0.3498, -1.1676, -1.0479, -0.9721,  0.4435, -1.8569,  0.3960, -0.9129,  0.8490, -0.1584,  0.6735,  1.3392,
                                  -0.3775,  0.1385,  0.9566,  0.4838, -1.1012, -0.0756, -0.4191, -0.7921,  0.6814, -0.5444,  2.2577, -0.3990,  0.2766,  0.9057,  0.3795,  1.0113, -0.3648, -0.3710, -0.0947,  0.0757, -0.2410,
                                  -2.8397, -1.0935, -0.9132, -0.8380, -0.7251, -0.7766, -2.2431, -1.3650,  1.4310,  0.7187, -0.3793, -0.0216,  0.8440,  0.3516, -0.6028,  0.4946,  1.3264, -0.4516, -0.2608, -2.0669,  1.5315,
                                   1.1927,  1.8129, -1.5135, -0.2591, -0.3188, -0.2654, -1.2178, -1.5780, -0.2844, -0.5603,  0.1502, -0.0856,  0.4588,  0.3597, -0.2828,  0.9414,  0.1439,  0.5935,  0.8071, -0.1075, -0.3704,
                                   1.7444, -1.8667,  1.4107,  0.1299,  2.1534,  0.3486, -0.6208,  0.3793, -0.3523, -0.5103, -0.4247,  2.0007, -0.7415, -0.5452,  1.4272,  0.9469, -1.2081, -0.3660, -0.6349, -0.8549, -0.8355,
                                   0.5461,  1.2245, -0.5802,  2.1495, -0.7087,  1.2845,  0.1372, -0.7325,  0.0013, -0.0268, -1.3235,  0.4239, -0.8799, -2.3448, -0.8744, -0.3683,  0.4001,  0.1579,  0.5945, -1.2264, -0.2310 };


   Rox_Double two_data[23*41] = { -0.2730,  1.1643,  1.2274,  0.5155,  1.3513,  1.0307,  0.8424,  0.7212, -0.5799,  0.0616,  0.0369,  1.9142, -1.0222,  0.9169, -0.8544, -0.8975, -0.4148, -0.2533, -1.0824, -0.6466,  0.3664,
                                   0.9354, -1.5019,  0.4590,  1.3195,  1.1025, -1.3020,  0.6656, -1.3693,  0.8028,  0.6662,  0.5249,  0.4064,  1.4540,  0.9454,  0.2350,  0.6120, -1.4056, -0.0582,  0.8322,  0.1823,  0.9667,
                                  -0.5718,  0.4775,  0.6206,  0.4525, -2.8316, -1.3055,  1.3200,  1.6601, -1.0828,  0.7372, -1.3393, -1.7594, -0.3716,  0.5459,  0.8282,  0.5415, -0.4719, -0.9745,  0.2557,  0.9757,  0.7384,
                                   0.2893,  0.3592, -0.0911, -1.0587,  2.5012,  1.4647,  0.8637,  0.9233, -0.7286,  0.4953, -0.6786, -0.7570,  1.2391, -0.3146, -0.6561, -0.4292, -1.0960, -0.3574, -0.2514,  1.5256,  0.5159,
                                   0.8925, -0.9848,  0.8980,  1.0295,  1.4717, -0.0815, -0.9960, -0.7669,  0.5395, -0.9696,  0.3190, -0.1241,  1.4856,  0.5047,  1.5967, -0.5027,  0.7192, -0.2015,  1.5256,  1.3703, -0.7878,
                                   0.6805, -1.1307,  0.0903,  0.3481,  0.3271, -0.1415,  0.2211,  1.5091,  0.1625,  0.0618, -1.2009,  1.5704, -0.3036, -1.2846,  0.3780,  0.8411,  0.2919,  0.4977, -0.2854,  1.6262,  2.0645,
                                  -0.4374, -0.0950,  2.0708,  0.9109, -0.4096,  0.2279, -1.4145, -0.1382,  1.6609,  0.0330, -2.6211, -1.2019, -0.2443,  0.2382, -0.6359,  0.0277, -0.1020, -0.3925,  0.0295, -1.1970,  0.0128,
                                   2.7296, -1.0899, -0.9107, -0.4597, -0.5651, -0.6590, -1.2640, -0.6023, -0.6208,  0.8170,  0.3533,  1.5796,  0.7447,  0.0962,  0.6302, -0.5058,  0.6254, -0.8141,  0.3394,  1.5877, -2.0011,
                                  -1.8353,  0.7671,  2.4753, -1.0342, -0.3821,  1.1335,  0.7480, -0.1992, -0.0479,  1.5325, -0.4451,  0.5508, -1.1813,  0.4343,  0.4698, -1.7588, -0.8856, -1.7952,  0.7423, -1.2439,  0.9399,
                                   0.7894,  0.4146, -0.2731, -0.7901, -0.2442,  0.4208, -0.0951,  1.3329, -0.9845, -1.7589, -0.2497,  0.7894,  0.8333, -1.9845,  0.0059, -0.1518, -2.4255,  0.1966, -0.2699, -0.1978, -0.3421,
                                  -0.5974,  0.4836,  0.8192,  1.2170,  1.4590, -1.2801, -0.2435,  2.0527,  2.4293, -1.6053, -1.1292, -1.0117, -0.9711,  0.4330, -0.8655, -0.8471,  1.0848,  0.2396,  0.7270,  0.5409,  1.5378,
                                   1.9624,  2.0708,  0.4771, -0.7504, -0.5481, -0.9971, -0.1282, -1.3382, -0.6420, -1.0055, -1.1038, -0.0557,  0.9857, -0.3756, -0.0784,  0.4831, -0.3290, -1.6115, -0.9193, -0.0521,  0.4692,
                                  -0.6540,  0.8103, -0.3671, -0.5084, -0.0638, -1.5189,  0.1993, -0.8621,  0.3353,  0.9383, -0.4408, -1.3529,  0.0826,  0.9050, -0.7027,  0.3648, -0.1823, -0.1926, -0.4408,  0.0533,  1.2879 };
   
   Rox_Double res_grt_data[23*41] = { 0.087300,  0.604050,  0.962100, -0.528300,  1.310900,  1.138550, -0.132900,  0.115950, -1.200600, -0.160750,  0.214850,  0.570450, -1.767600,  1.080050, -0.300200, -0.090850,  0.392400, -1.188800, -0.527700, -0.489200, -0.474150, 
                                     -0.250100, -0.749550,  0.452800, -0.254100,  1.147950, -0.589250,  0.240550, -0.483150,  1.051800,  0.871250,  0.241450, -0.111150,  0.865150,  0.848100, -0.407050,  0.305150, -0.813800,  0.362250,  0.703400, -0.225550, -0.221750, 
                                      0.284950,  0.398300,  0.563900,  0.219700, -2.393350, -0.606400,  0.920700, -0.834200, -0.317050,  0.566400, -0.621300, -0.410700, -0.259050,  1.164450,  0.841800,  1.240800, -0.337850, -0.604300,  0.274850,  0.279650,  1.330700, 
                                     -0.283000, -0.069300,  0.057500, -1.007450,  1.552350,  0.659550,  1.165500,  1.058000, -0.986650,  1.183800,  0.116450, -0.553500,  0.447950,  0.191500, -0.703900, -0.873450, -0.925250, -0.357600, -0.106200,  0.332700, -0.028150, 
                                      0.700700,  0.224150,  0.507000,  1.144250,  0.965900, -0.806550,  0.070550,  0.241700, -0.222200, -0.653250,  0.230700, -0.025500,  1.239800,  1.399700,  0.418600, -1.257500,  0.527200, -0.586650,  0.587650,  0.683000, -0.246550, 
                                     -0.344300, -0.710550,  0.368450,  0.242700, -0.247650, -0.687250,  0.451600,  1.543850, -0.237100, -1.061650, -0.504450, -0.025500, -0.526300, -1.586400,  0.081850,  0.375000,  0.043150, -0.406400,  0.168000,  1.263600,  0.649500, 
                                     -0.444150,  0.004350,  1.532300,  0.097100, -0.611650,  0.305600, -0.728650, -0.042250,  0.204350, -0.281000, -1.289000, -0.078900, -0.332100,  0.392800, -0.693750, -0.094350,  0.080800, -0.722050, -0.615400, -0.917300,  0.314150, 
                                      1.718700, -1.166200, -0.755250, -0.075950, -0.408650, -0.411300, -0.179650, -0.038700, -0.360600,  0.583400, -0.407150,  0.265850, -0.113700,  0.269850, -0.613350, -0.054900, -0.143750,  0.017450,  0.090500,  1.130600, -0.330950, 
                                     -1.106400,  0.452800,  1.715950, -0.275200, -0.741650,  0.528950,  0.164450, -0.495650,  0.316750,  0.494050,  0.906300,  0.075900, -0.452350,  0.670000,  0.424650, -0.373750, -0.625200, -1.083100,  0.323800, -0.584100,  0.349450, 
                                     -1.025150, -0.339450, -0.593150, -0.814050, -0.484650, -0.177900, -1.169100, -0.016050,  0.223250, -0.520100, -0.314500,  0.383900,  0.838650, -0.816450, -0.298450,  0.171400, -0.549550, -0.127500, -0.265350, -1.132350,  0.594700, 
                                      0.297650,  1.148250, -0.347150,  0.478950,  0.570100, -0.772750, -0.730650,  0.237350,  1.072450, -1.082800, -0.489500, -0.548650, -0.256150,  0.396350, -0.574150,  0.047150,  0.614350,  0.416550,  0.767050,  0.216700,  0.583700, 
                                      1.853400,  0.102050,  0.943900, -0.310250,  0.802650, -0.324250, -0.374500, -0.479450, -0.497150, -0.757900, -0.764250,  0.972500,  0.122100, -0.460400,  0.674400,  0.715000, -0.768550, -0.988750, -0.777100, -0.453500, -0.183150, 
                                     -0.053950,  1.017400, -0.473650,  0.820550, -0.386250, -0.117200,  0.168250, -0.797300,  0.168300,  0.455750, -0.882150, -0.464500, -0.398650, -0.719900, -0.788550, -0.001750,  0.108900, -0.017350,  0.076850, -0.586550,  0.528450 };

   Rox_Array2D_Double res = NULL;
   Rox_Array2D_Double one = NULL;
   Rox_Array2D_Double two = NULL;
   Rox_Array2D_Double res_grt = NULL;

   error = rox_array2d_double_new ( &res_grt, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_set_buffer_no_stride ( res_grt, res_grt_data );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_new ( &res, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_new ( &one, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
   
   error = rox_array2d_double_set_buffer_no_stride ( one, one_data );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_new ( &two, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_set_buffer_no_stride ( two, two_data );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_mean ( res, one, two );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_difference_l2_norm ( &l2_error, res_grt, res );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   ROX_TEST_CHECK_CLOSE (l2_error, 0.0, 10*DBL_EPSILON);

   rox_log("l2_error mean = %0.12f \n", l2_error);

   error = rox_array2d_double_del ( &res );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_del ( &one );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
   
   error = rox_array2d_double_del ( &two );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );  
}

ROX_TEST_CASE_DECLARE ( rox::OpenROXTest, test_perf_array2d_double_mean )
{
   Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Sint rows = 3000;
   Rox_Sint cols = 4000;
   Rox_Array2D_Double res = NULL;
   Rox_Array2D_Double one = NULL;
   Rox_Array2D_Double two = NULL;
   Rox_Double time_ms = 0.0, total_time = 0.0;

#ifdef DEBUG
   // Small number of tests for slow debug (e.g. with valgrind)
   Rox_Sint nb_tests = 1;
#else 
   // High number of tests for measuring average performance in release
   Rox_Sint nb_tests = 1000;
#endif
   // Define timer to measure performances
   Rox_Timer timer = NULL;

   // Init new timer
   error = rox_timer_new(&timer);
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_new ( &res, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_new ( &one, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_fillval(one, 1.0);
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_new ( &two, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_fillval(two, 2.0);
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   for ( Rox_Sint i = 0; i < nb_tests; ++i )
   {  
      rox_timer_start(timer);

      error = rox_array2d_double_mean ( res, one, two );
      ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

      // Display elapsed time
      rox_timer_stop(timer);
      rox_timer_get_elapsed_ms(&time_ms, timer);
      total_time += time_ms;
   }

   rox_log("AVERAGE time for computine the mean of a (%d x %d) float images = %f (ms)\n", cols, rows, total_time/nb_tests);

   error = rox_array2d_double_del ( &res );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_double_del ( &one );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
   
   error = rox_array2d_double_del ( &two );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_timer_del ( &timer );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
}

ROX_TEST_CASE_DECLARE ( rox::OpenROXTest, test_nreg_array2d_float_mean )
{
	Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Float l2_error = 0.0;
   Rox_Sint rows = 13;
   Rox_Sint cols = 21;

   Rox_Float one_data[23*41] = {  0.4476,  0.0438,  0.6968, -1.5721,  1.2705,  1.2464, -1.1082, -0.4893, -1.8213, -0.3831,  0.3928, -0.7733, -2.5130,  1.2432,   0.2540,   0.7158,   1.1996, -2.1243,   0.0270,  -0.3318,   -1.3147,
                                 -1.4356,  0.0028,  0.4466, -1.8277,  1.1934,  0.1235, -0.1845,  0.4030,  1.3008,  1.0763, -0.0420, -0.6287,  0.2763,  0.7508,  -1.0491,  -0.0017,  -0.2220,  0.7827,   0.5746,  -0.6334,   -1.4102,
                                  1.1417,  0.3191,  0.5072, -0.0131, -1.9551,  0.0927,  0.5214, -3.3285,  0.4487,  0.3956,  0.0967,  0.9380, -0.1465,  1.7830,   0.8554,   1.9401,  -0.2038, -0.2341,   0.2940,  -0.4164,    1.9230,
                                 -0.8553, -0.4978,  0.2061, -0.9562,  0.6035, -0.1456,  1.4673,  1.1927, -1.2447,  1.8723,  0.9115, -0.3500, -0.3432,  0.6976,  -0.7517,  -1.3177,  -0.7545, -0.3578,   0.0390,  -0.8602,   -0.5722,
                                  0.5089,  1.4331,  0.1160,  1.2590,  0.4601, -1.5316,  1.1371,  1.2503, -0.9839, -0.3369,  0.1424,  0.0731,  0.9940,  2.2947,  -0.7595,  -2.0123,   0.3352, -0.9718,  -0.3503,  -0.0043,    0.2947,
                                 -1.3691, -0.2904,  0.6466,  0.1373, -0.8224, -1.2330,  0.6821,  1.5786, -0.6367, -2.1851,  0.1920, -1.6214, -0.7490, -1.8882,  -0.2143,  -0.0911,  -0.2056, -1.3105,   0.6214,   0.9010,   -0.7655,
                                 -0.4509,  0.1037,  0.9938, -0.7167, -0.8137,  0.3833, -0.0428,  0.0537, -1.2522, -0.5950,  0.0431,  1.0441, -0.4199,  0.5474,  -0.7516,  -0.2164,   0.2636, -1.0516,  -1.2603,  -0.6376,    0.6155,
                                  0.7078, -1.2425, -0.5998,  0.3078, -0.2522, -0.1636,  0.9047,  0.5249, -0.1004,  0.3498, -1.1676, -1.0479, -0.9721,  0.4435,  -1.8569,   0.3960,  -0.9129,  0.8490,  -0.1584,   0.6735,    1.3392,
                                 -0.3775,  0.1385,  0.9566,  0.4838, -1.1012, -0.0756, -0.4191, -0.7921,  0.6814, -0.5444,  2.2577, -0.3990,  0.2766,  0.9057,   0.3795,   1.0113,  -0.3648, -0.3710,  -0.0947,   0.0757,   -0.2410,
                                 -2.8397, -1.0935, -0.9132, -0.8380, -0.7251, -0.7766, -2.2431, -1.3650,  1.4310,  0.7187, -0.3793, -0.0216,  0.8440,  0.3516,  -0.6028,   0.4946,   1.3264, -0.4516,  -0.2608,  -2.0669,    1.5315,
                                  1.1927,  1.8129, -1.5135, -0.2591, -0.3188, -0.2654, -1.2178, -1.5780, -0.2844, -0.5603,  0.1502, -0.0856,  0.4588,  0.3597,  -0.2828,   0.9414,   0.1439,  0.5935,   0.8071,  -0.1075,   -0.3704,
                                  1.7444, -1.8667,  1.4107,  0.1299,  2.1534,  0.3486, -0.6208,  0.3793, -0.3523, -0.5103, -0.4247,  2.0007, -0.7415, -0.5452,   1.4272,   0.9469,  -1.2081, -0.3660,  -0.6349,  -0.8549,   -0.8355,
                                  0.5461,  1.2245, -0.5802,  2.1495, -0.7087,  1.2845,  0.1372, -0.7325,  0.0013, -0.0268, -1.3235,  0.4239, -0.8799, -2.3448,  -0.8744,  -0.3683,   0.4001,  0.1579,   0.5945,  -1.2264,   -0.2310 };


   Rox_Float two_data[23*41] = { -0.2730,  1.1643,  1.2274,  0.5155,  1.3513,  1.0307,  0.8424,  0.7212, -0.5799,  0.0616,  0.0369,  1.9142, -1.0222,  0.9169, -0.8544, -0.8975, -0.4148, -0.2533, -1.0824, -0.6466,  0.3664,
                                  0.9354, -1.5019,  0.4590,  1.3195,  1.1025, -1.3020,  0.6656, -1.3693,  0.8028,  0.6662,  0.5249,  0.4064,  1.4540,  0.9454,  0.2350,  0.6120, -1.4056, -0.0582,  0.8322,  0.1823,  0.9667,
                                 -0.5718,  0.4775,  0.6206,  0.4525, -2.8316, -1.3055,  1.3200,  1.6601, -1.0828,  0.7372, -1.3393, -1.7594, -0.3716,  0.5459,  0.8282,  0.5415, -0.4719, -0.9745,  0.2557,  0.9757,  0.7384,
                                  0.2893,  0.3592, -0.0911, -1.0587,  2.5012,  1.4647,  0.8637,  0.9233, -0.7286,  0.4953, -0.6786, -0.7570,  1.2391, -0.3146, -0.6561, -0.4292, -1.0960, -0.3574, -0.2514,  1.5256,  0.5159,
                                  0.8925, -0.9848,  0.8980,  1.0295,  1.4717, -0.0815, -0.9960, -0.7669,  0.5395, -0.9696,  0.3190, -0.1241,  1.4856,  0.5047,  1.5967, -0.5027,  0.7192, -0.2015,  1.5256,  1.3703, -0.7878,
                                  0.6805, -1.1307,  0.0903,  0.3481,  0.3271, -0.1415,  0.2211,  1.5091,  0.1625,  0.0618, -1.2009,  1.5704, -0.3036, -1.2846,  0.3780,  0.8411,  0.2919,  0.4977, -0.2854,  1.6262,  2.0645,
                                 -0.4374, -0.0950,  2.0708,  0.9109, -0.4096,  0.2279, -1.4145, -0.1382,  1.6609,  0.0330, -2.6211, -1.2019, -0.2443,  0.2382, -0.6359,  0.0277, -0.1020, -0.3925,  0.0295, -1.1970,  0.0128,
                                  2.7296, -1.0899, -0.9107, -0.4597, -0.5651, -0.6590, -1.2640, -0.6023, -0.6208,  0.8170,  0.3533,  1.5796,  0.7447,  0.0962,  0.6302, -0.5058,  0.6254, -0.8141,  0.3394,  1.5877, -2.0011,
                                 -1.8353,  0.7671,  2.4753, -1.0342, -0.3821,  1.1335,  0.7480, -0.1992, -0.0479,  1.5325, -0.4451,  0.5508, -1.1813,  0.4343,  0.4698, -1.7588, -0.8856, -1.7952,  0.7423, -1.2439,  0.9399,
                                  0.7894,  0.4146, -0.2731, -0.7901, -0.2442,  0.4208, -0.0951,  1.3329, -0.9845, -1.7589, -0.2497,  0.7894,  0.8333, -1.9845,  0.0059, -0.1518, -2.4255,  0.1966, -0.2699, -0.1978, -0.3421,
                                 -0.5974,  0.4836,  0.8192,  1.2170,  1.4590, -1.2801, -0.2435,  2.0527,  2.4293, -1.6053, -1.1292, -1.0117, -0.9711,  0.4330, -0.8655, -0.8471,  1.0848,  0.2396,  0.7270,  0.5409,  1.5378,
                                  1.9624,  2.0708,  0.4771, -0.7504, -0.5481, -0.9971, -0.1282, -1.3382, -0.6420, -1.0055, -1.1038, -0.0557,  0.9857, -0.3756, -0.0784,  0.4831, -0.3290, -1.6115, -0.9193, -0.0521,  0.4692,
                                 -0.6540,  0.8103, -0.3671, -0.5084, -0.0638, -1.5189,  0.1993, -0.8621,  0.3353,  0.9383, -0.4408, -1.3529,  0.0826,  0.9050, -0.7027,  0.3648, -0.1823, -0.1926, -0.4408,  0.0533,  1.2879 };
   
   Rox_Float res_grt_data[23*41] = { 0.087300,  0.604050,  0.962100, -0.528300, 1.310900, 1.138550, -0.132900, 0.115950, -1.200600, -0.160750, 0.214850, 0.570450, -1.767600, 1.080050, -0.300200, -0.090850, 0.392400, -1.188800, -0.527700, -0.489200, -0.474150, 
                                    -0.250100, -0.749550,  0.452800, -0.254100, 1.147950, -0.589250, 0.240550, -0.483150, 1.051800, 0.871250, 0.241450, -0.111150, 0.865150, 0.848100, -0.407050, 0.305150, -0.813800, 0.362250, 0.703400, -0.225550, -0.221750, 
                                     0.284950,  0.398300,  0.563900, 0.219700, -2.393350, -0.606400, 0.920700, -0.834200, -0.317050, 0.566400, -0.621300, -0.410700, -0.259050, 1.164450, 0.841800, 1.240800, -0.337850, -0.604300, 0.274850, 0.279650, 1.330700, 
                                    -0.283000, -0.069300,  0.057500, -1.007450, 1.552350, 0.659550, 1.165500, 1.058000, -0.986650, 1.183800, 0.116450, -0.553500, 0.447950, 0.191500, -0.703900, -0.873450, -0.925250, -0.357600, -0.106200, 0.332700, -0.028150, 
                                     0.700700,  0.224150,  0.507000, 1.144250, 0.965900, -0.806550, 0.070550, 0.241700, -0.222200, -0.653250, 0.230700, -0.025500, 1.239800, 1.399700, 0.418600, -1.257500, 0.527200, -0.586650, 0.587650, 0.683000, -0.246550, 
                                    -0.344300, -0.710550,  0.368450, 0.242700, -0.247650, -0.687250, 0.451600, 1.543850, -0.237100, -1.061650, -0.504450, -0.025500, -0.526300, -1.586400, 0.081850, 0.375000, 0.043150, -0.406400, 0.168000, 1.263600, 0.649500, 
                                    -0.444150,  0.004350,  1.532300, 0.097100, -0.611650, 0.305600, -0.728650, -0.042250, 0.204350, -0.281000, -1.289000, -0.078900, -0.332100, 0.392800, -0.693750, -0.094350, 0.080800, -0.722050, -0.615400, -0.917300, 0.314150, 
                                     1.718700, -1.166200, -0.755250, -0.075950, -0.408650, -0.411300, -0.179650, -0.038700, -0.360600, 0.583400, -0.407150, 0.265850, -0.113700, 0.269850, -0.613350, -0.054900, -0.143750, 0.017450, 0.090500, 1.130600, -0.330950, 
                                    -1.106400,  0.452800,  1.715950, -0.275200, -0.741650, 0.528950, 0.164450, -0.495650, 0.316750, 0.494050, 0.906300, 0.075900, -0.452350, 0.670000, 0.424650, -0.373750, -0.625200, -1.083100, 0.323800, -0.584100, 0.349450, 
                                    -1.025150, -0.339450, -0.593150, -0.814050, -0.484650, -0.177900, -1.169100, -0.016050, 0.223250, -0.520100, -0.314500, 0.383900, 0.838650, -0.816450, -0.298450, 0.171400, -0.549550, -0.127500, -0.265350, -1.132350, 0.594700, 
                                     0.297650,  1.148250, -0.347150, 0.478950, 0.570100, -0.772750, -0.730650, 0.237350, 1.072450, -1.082800, -0.489500, -0.548650, -0.256150, 0.396350, -0.574150, 0.047150, 0.614350, 0.416550, 0.767050, 0.216700, 0.583700, 
                                     1.853400,  0.102050,  0.943900, -0.310250, 0.802650, -0.324250, -0.374500, -0.479450, -0.497150, -0.757900, -0.764250, 0.972500, 0.122100, -0.460400, 0.674400, 0.715000, -0.768550, -0.988750, -0.777100, -0.453500, -0.183150, 
                                    -0.053950,  1.017400, -0.473650, 0.820550, -0.386250, -0.117200, 0.168250, -0.797300, 0.168300, 0.455750, -0.882150, -0.464500, -0.398650, -0.719900, -0.788550, -0.001750, 0.108900, -0.017350, 0.076850, -0.586550, 0.528450 };

   Rox_Array2D_Float res = NULL;
   Rox_Array2D_Float one = NULL;
   Rox_Array2D_Float two = NULL;
   Rox_Array2D_Float res_grt = NULL;

   error = rox_array2d_float_new ( &res_grt, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_set_buffer_no_stride ( res_grt, res_grt_data );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_new ( &res, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_new ( &one, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
   
   error = rox_array2d_float_set_buffer_no_stride ( one, one_data );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_new ( &two, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_set_buffer_no_stride ( two, two_data );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_mean ( res, one, two );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_difference_l2_norm ( &l2_error, res_grt, res );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   // rox_array2d_float_print(one);
   // rox_array2d_float_print(two);
   // rox_array2d_float_print(res);

   ROX_TEST_CHECK_CLOSE (l2_error, 0.0, 10*FLT_EPSILON);

   rox_log("l2_error mean = %0.12f \n", l2_error);

   error = rox_array2d_float_del ( &res );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_del ( &one );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
   
   error = rox_array2d_float_del ( &two );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );  
}

ROX_TEST_CASE_DECLARE ( rox::OpenROXTest, test_perf_array2d_float_mean )
{
	Rox_ErrorCode error = ROX_ERROR_NONE;
   Rox_Sint rows = 3000;
   Rox_Sint cols = 4000;
   Rox_Array2D_Float res = NULL;
   Rox_Array2D_Float one = NULL;
   Rox_Array2D_Float two = NULL;
   Rox_Double time_ms = 0.0, total_time = 0.0;

#ifdef DEBUG
   // Small number of tests for slow debug (e.g. with valgrind)
   Rox_Sint nb_tests = 1;
#else 
   // High number of tests for measuring average performance in release
   Rox_Sint nb_tests = 1000;
#endif
   // Define timer to measure performances
   Rox_Timer timer = NULL;

   // Init new timer
   error = rox_timer_new(&timer);
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_new ( &res, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_new ( &one, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_fillval(one, 1.0);
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_new ( &two, rows, cols );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_fillval(two, 2.0);
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   for ( Rox_Sint i = 0; i < nb_tests; ++i )
   {  
      rox_timer_start(timer);

      error = rox_array2d_float_mean ( res, one, two );
      ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

      // Display elapsed time
      rox_timer_stop(timer);
      rox_timer_get_elapsed_ms(&time_ms, timer);
      total_time += time_ms;
   }

   rox_log("AVERAGE time for computine the mean of a (%d x %d) float images = %f (ms)\n", cols, rows, total_time/nb_tests);

   error = rox_array2d_float_del ( &res );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_array2d_float_del ( &one );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
   
   error = rox_array2d_float_del ( &two );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );

   error = rox_timer_del ( &timer );
   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
}

ROX_TEST_SUITE_END()
