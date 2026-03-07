#ifndef RTW_HEADER_uvtest_h_
#define RTW_HEADER_uvtest_h_
#include <string.h>
#ifndef uvtest_COMMON_INCLUDES_
# define uvtest_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif

#include "uvtest_types.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

typedef struct {
  real_T maxl_data[398];
  real_T tmp_data[397];
  emxArray_int32_T_398_uvtest_T vwork_data;
  real32_T maxv_data[398];
  int32_T iPk_data[398];
  int32_T idx_data[398];
  int32_T c_data[398];
  int32_T sortIdx_data[398];
  real32_T y_data[398];
  int32_T vwork_data_m[398];
  int32_T iwork_data[398];
  int32_T xwork_data[398];
  int32_T c_x_data[398];
  int32_T iwork_data_c[398];
  int32_T iwork[256];
  int32_T xwork[256];
  real32_T rtb_Lowpass2_k[200];
  real32_T xmean[200];
  int32_T iFinite_data[199];
  int32_T iInfinite_data[199];
  int32_T iInflect_data[199];
  int32_T tmp_data_c[199];
  int16_T b_data[398];
  real32_T fv0[199];
  uint8_T locs_temp_data[398];
  boolean_T idelete_data[398];
  uint8_T iPk_data_b[199];
  int32_T x4[4];
  int16_T idx4[4];
  int8_T perm[4];
  real32_T y;
  real32_T numAccum;
  real32_T stageIn;
  int32_T idx;
  int32_T i;
  int32_T maxv_size;
  int32_T maxl_size;
  int32_T i0;
  int32_T loop_ub;
  int32_T iFinite_size;
  int32_T iInfinite_size;
  int32_T iInflect_size;
  int32_T c_size;
  int32_T tmp_size;
  int32_T iPk_size_idx_0;
  int32_T end;
  int32_T c_i;
  int32_T loop_ub_p;
  int32_T sortIdx_size;
  int32_T y_size;
  int32_T idelete_size_idx_0;
  int32_T dim;
  int32_T vstride;
  int32_T b;
  int32_T c_k;
  int32_T vwork_size;
  int32_T nQuartets;
  int32_T i_c;
  int32_T nLeft;
  int32_T i1;
  int32_T i2;
  int32_T i3;
} B_uvtest_T;

typedef struct {
  dspcodegen_BiquadFilter_uvtes_T gobj_0;
  dspcodegen_BiquadFilter_uvtes_T gobj_1;
  dsp_LowpassFilter_uvtest_T obj;
  boolean_T objisempty;
  boolean_T isInitialized;
} DW_uvtest_T;

typedef struct {
  real32_T input_data[200];
} ExtU_uvtest_T;

typedef struct {
  real32_T brate_data;
  real32_T len_data;
} ExtY_uvtest_T;

struct tag_RTM_uvtest_T {
  const char_T * volatile errorStatus;
};

extern B_uvtest_T uvtest_B;
extern DW_uvtest_T uvtest_DW;
extern ExtU_uvtest_T uvtest_U;
extern ExtY_uvtest_T uvtest_Y;
extern void uvtest_initialize(void);
extern void uvtest_step(void);
extern void uvtest_terminate(void);
extern RT_MODEL_uvtest_T *const uvtest_M;

#endif

