#ifndef RTW_HEADER_uvtest_types_h_
#define RTW_HEADER_uvtest_types_h_
#include "rtwtypes.h"
#ifndef struct_md715e9ba570c4c116997cfce93715cb9a
#define struct_md715e9ba570c4c116997cfce93715cb9a

struct md715e9ba570c4c116997cfce93715cb9a
{
  int32_T S0_isInitialized;
  real32_T W0_ZERO_STATES[4];
  real32_T W1_POLE_STATES[4];
  int32_T W2_PreviousNumChannels;
  real32_T P0_ICRTP;
  real32_T P1_RTP1COEFF[6];
  real32_T P2_RTP2COEFF[4];
  real32_T P3_RTP3COEFF[3];
  boolean_T P4_RTP_COEFF3_BOOL[3];
  real32_T P5_IC2RTP;
};

#endif

#ifndef typedef_dsp_BiquadFilter_0_uvtest_T
#define typedef_dsp_BiquadFilter_0_uvtest_T

typedef struct md715e9ba570c4c116997cfce93715cb9a dsp_BiquadFilter_0_uvtest_T;

#endif

#ifndef struct_mdgWn4XqgWVFeoFDZoJGebz
#define struct_mdgWn4XqgWVFeoFDZoJGebz

struct mdgWn4XqgWVFeoFDZoJGebz
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  dsp_BiquadFilter_0_uvtest_T cSFunObject;
};

#endif

#ifndef typedef_dspcodegen_BiquadFilter_uvtes_T
#define typedef_dspcodegen_BiquadFilter_uvtes_T

typedef struct mdgWn4XqgWVFeoFDZoJGebz dspcodegen_BiquadFilter_uvtes_T;

#endif

#ifndef struct_tag_sibHABGMpBjS4v57vRH5HpC
#define struct_tag_sibHABGMpBjS4v57vRH5HpC

struct tag_sibHABGMpBjS4v57vRH5HpC
{
  uint32_T MinPeakHeight;
  uint32_T MinPeakProminence;
  uint32_T MinPeakWidth;
  uint32_T MaxPeakWidth;
  uint32_T MinPeakDistance;
  uint32_T Threshold;
  uint32_T NPeaks;
  uint32_T SortStr;
  uint32_T Annotate;
  uint32_T WidthReference;
};

#endif

#ifndef typedef_sibHABGMpBjS4v57vRH5HpC_uvtes_T
#define typedef_sibHABGMpBjS4v57vRH5HpC_uvtes_T

typedef struct tag_sibHABGMpBjS4v57vRH5HpC sibHABGMpBjS4v57vRH5HpC_uvtes_T;

#endif

#ifndef struct_tag_spGKsvEVm7uA89hv31XX4LH
#define struct_tag_spGKsvEVm7uA89hv31XX4LH

struct tag_spGKsvEVm7uA89hv31XX4LH
{
  uint32_T MissingPlacement;
  uint32_T ComparisonMethod;
};

#endif

#ifndef typedef_spGKsvEVm7uA89hv31XX4LH_uvtes_T
#define typedef_spGKsvEVm7uA89hv31XX4LH_uvtes_T

typedef struct tag_spGKsvEVm7uA89hv31XX4LH spGKsvEVm7uA89hv31XX4LH_uvtes_T;

#endif

#ifndef struct_tag_skA4KFEZ4HPkJJBOYCrevdH
#define struct_tag_skA4KFEZ4HPkJJBOYCrevdH

struct tag_skA4KFEZ4HPkJJBOYCrevdH
{
  uint32_T SafeEq;
  uint32_T Absolute;
  uint32_T NaNBias;
  uint32_T NaNWithFinite;
  uint32_T FiniteWithNaN;
  uint32_T NaNWithNaN;
};

#endif

#ifndef typedef_skA4KFEZ4HPkJJBOYCrevdH_uvtes_T
#define typedef_skA4KFEZ4HPkJJBOYCrevdH_uvtes_T

typedef struct tag_skA4KFEZ4HPkJJBOYCrevdH skA4KFEZ4HPkJJBOYCrevdH_uvtes_T;

#endif

#ifndef typedef_cell_wrap_uvtest_T
#define typedef_cell_wrap_uvtest_T

typedef struct {
  uint32_T f1[8];
} cell_wrap_uvtest_T;

#endif

#ifndef struct_tag_sJCxfmxS8gBOONUZjbjUd9E
#define struct_tag_sJCxfmxS8gBOONUZjbjUd9E

struct tag_sJCxfmxS8gBOONUZjbjUd9E
{
  boolean_T CaseSensitivity;
  boolean_T StructExpand;
  char_T PartialMatching[6];
  boolean_T IgnoreNulls;
};

#endif

#ifndef typedef_sJCxfmxS8gBOONUZjbjUd9E_uvtes_T
#define typedef_sJCxfmxS8gBOONUZjbjUd9E_uvtes_T

typedef struct tag_sJCxfmxS8gBOONUZjbjUd9E sJCxfmxS8gBOONUZjbjUd9E_uvtes_T;

#endif

#ifndef struct_mdzqJ27I7nxvfpRecJ1CKthC
#define struct_mdzqJ27I7nxvfpRecJ1CKthC

struct mdzqJ27I7nxvfpRecJ1CKthC
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  cell_wrap_uvtest_T inputVarSize;
  int32_T NumChannels;
  dspcodegen_BiquadFilter_uvtes_T *FilterObj;
};

#endif

#ifndef typedef_dsp_LowpassFilter_uvtest_T
#define typedef_dsp_LowpassFilter_uvtest_T

typedef struct mdzqJ27I7nxvfpRecJ1CKthC dsp_LowpassFilter_uvtest_T;

#endif

#ifndef struct_emxArray_int32_T_398
#define struct_emxArray_int32_T_398

struct emxArray_int32_T_398
{
  int32_T data[398];
  int32_T size;
};

#endif

#ifndef typedef_emxArray_int32_T_398_uvtest_T
#define typedef_emxArray_int32_T_398_uvtest_T

typedef struct emxArray_int32_T_398 emxArray_int32_T_398_uvtest_T;

#endif

typedef struct tag_RTM_uvtest_T RT_MODEL_uvtest_T;

#endif

