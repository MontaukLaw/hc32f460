#include "uvtest.h"
#include "uvtest_private.h"

B_uvtest_T uvtest_B;
DW_uvtest_T uvtest_DW;
ExtU_uvtest_T uvtest_U;
ExtY_uvtest_T uvtest_Y;
RT_MODEL_uvtest_T uvtest_M_;
RT_MODEL_uvtest_T *const uvtest_M = &uvtest_M_;
static void uvtest_diff(const real32_T x[200], real32_T y[199]);
static void uvtest_getAllPeaksCodegen(const real32_T y[199], int32_T iPk_data[],
  int32_T *iPk_size, int32_T iInf_data[], int32_T *iInf_size, int32_T
  iInflect_data[], int32_T *iInflect_size);
static void uvtest_removeSmallPeaks(const real32_T y[199], const int32_T
  iFinite_data[], const int32_T *iFinite_size, int32_T iPk_data[], int32_T
  *iPk_size);
static void uvtest_do_vectors(const int32_T a_data[], const int32_T *a_size,
  const int32_T b_data[], const int32_T *b_size, int32_T c_data[], int32_T
  *c_size, int32_T ia_data[], int32_T *ia_size, int32_T ib_data[], int32_T
  *ib_size);
static void uvtest_sortIdx(const real32_T x_data[], const int32_T *x_size,
  int32_T idx_data[], int32_T *idx_size);
static int32_T uvtest_nonSingletonDim(const int32_T *x_size);
static void uvtest_merge_pow2_block(int32_T idx_data[], int32_T x_data[]);
static void uvtest_merge(int32_T idx_data[], int32_T x_data[], int32_T offset,
  int32_T np, int32_T nq, int32_T iwork_data[], int32_T xwork_data[]);
static void uvtest_merge_block(int32_T idx_data[], int32_T x_data[], int32_T
  offset, int32_T n, int32_T preSortLevel, int32_T iwork_data[], int32_T
  xwork_data[]);
static void uvtest_sortIdx_o(int32_T x_data[], int32_T *x_size, int32_T
  idx_data[], int32_T *idx_size);
static void uvtest_sort(int32_T x_data[], int32_T *x_size);
static void findPeaksSeparatedByMoreThanMin(const real32_T y[199], const int32_T
  iPk_data[], const int32_T *iPk_size, int32_T idx_data[], int32_T *idx_size);
static void uvtest_keepAtMostNpPeaks(int32_T *idx_size);
static void uvtest_findpeaks(const real32_T Yin[199], real32_T Ypk_data[],
  int32_T *Ypk_size, real_T Xpk_data[], int32_T *Xpk_size);
static void uvtest_diff_n(const real_T x_data[], const int32_T *x_size, real_T
  y_data[], int32_T *y_size);
static real_T uvtest_mean(const real_T x_data[], const int32_T *x_size);
static real32_T uvtest_mean_j(const real32_T x[200]);
static void uvtest_SystemCore_release(dspcodegen_BiquadFilter_uvtes_T *obj);
static void uvte_LPHPFilterBase_releaseImpl(dsp_LowpassFilter_uvtest_T *obj);
static void uvtes_SystemCore_releaseWrapper(dsp_LowpassFilter_uvtest_T *obj);
static void uvtest_SystemCore_release_m(dsp_LowpassFilter_uvtest_T *obj);
static void uvtest_SystemCore_delete_e(dsp_LowpassFilter_uvtest_T *obj);
static void matlabCodegenHandle_matlabCod_m(dsp_LowpassFilter_uvtest_T *obj);
static void uvtest_SystemCore_delete(dspcodegen_BiquadFilter_uvtes_T *obj);
static void matlabCodegenHandle_matlabCodeg(dspcodegen_BiquadFilter_uvtes_T *obj);
static void uvtest_diff(const real32_T x[200], real32_T y[199])
{
  real32_T work;
  int32_T ixLead;
  int32_T iyLead;
  real32_T tmp2;
  int32_T m;
  ixLead = 1;
  iyLead = 0;
  work = x[0];
  for (m = 0; m < 199; m++) {
    tmp2 = work;
    work = x[ixLead];
    y[iyLead] = x[ixLead] - tmp2;
    ixLead++;
    iyLead++;
  }
}

static void uvtest_getAllPeaksCodegen(const real32_T y[199], int32_T iPk_data[],
  int32_T *iPk_size, int32_T iInf_data[], int32_T *iInf_size, int32_T
  iInflect_data[], int32_T *iInflect_size)
{
  int32_T nPk;
  int32_T nInf;
  int32_T nInflect;
  char_T dir;
  int32_T kfirst;
  real32_T ykfirst;
  boolean_T isinfykfirst;
  real32_T yk;
  boolean_T isinfyk;
  char_T previousdir;
  int32_T k;
  nPk = 0;
  nInf = 0;
  nInflect = -1;
  dir = 'n';
  kfirst = 0;
  ykfirst = (rtInfF);
  isinfykfirst = true;
  for (k = 0; k < 199; k++) {
    yk = y[k];
    if (rtIsNaNF(y[k])) {
      yk = (rtInfF);
      isinfyk = true;
    } else if (rtIsInfF(y[k])) {
      if (y[k] > 0.0F) {
        isinfyk = true;
        nInf++;
        iInf_data[nInf - 1] = k + 1;
      } else {
        isinfyk = false;
      }
    } else {
      isinfyk = false;
    }

    if (yk != ykfirst) {
      previousdir = dir;
      if (isinfyk || isinfykfirst) {
        dir = 'n';
        if (kfirst >= 1) {
          nInflect++;
          iInflect_data[nInflect] = kfirst;
        }
      } else if (yk < ykfirst) {
        dir = 'd';
        if ('d' != previousdir) {
          nInflect++;
          iInflect_data[nInflect] = kfirst;
          if (previousdir == 'i') {
            nPk++;
            iPk_data[nPk - 1] = kfirst;
          }
        }
      } else {
        dir = 'i';
        if ('i' != previousdir) {
          nInflect++;
          iInflect_data[nInflect] = kfirst;
        }
      }

      ykfirst = yk;
      kfirst = k + 1;
      isinfykfirst = isinfyk;
    }
  }

  if ((!isinfykfirst) && ((nInflect + 1 == 0) || (iInflect_data[nInflect] < 199)))
  {
    nInflect++;
    iInflect_data[nInflect] = 199;
  }

  if (1 > nPk) {
    nPk = 0;
  }

  for (kfirst = 0; kfirst < nPk; kfirst++) {
    uvtest_B.iPk_data_b[kfirst] = (uint8_T)iPk_data[kfirst];
  }

  *iPk_size = nPk;
  for (kfirst = 0; kfirst < nPk; kfirst++) {
    iPk_data[kfirst] = uvtest_B.iPk_data_b[kfirst];
  }

  if (1 > nInf) {
    nInf = 0;
  }

  for (kfirst = 0; kfirst < nInf; kfirst++) {
    uvtest_B.iPk_data_b[kfirst] = (uint8_T)iInf_data[kfirst];
  }

  *iInf_size = nInf;
  for (kfirst = 0; kfirst < nInf; kfirst++) {
    iInf_data[kfirst] = uvtest_B.iPk_data_b[kfirst];
  }

  if (1 > nInflect + 1) {
    nPk = 0;
  } else {
    nPk = nInflect + 1;
  }

  for (kfirst = 0; kfirst < nPk; kfirst++) {
    uvtest_B.iPk_data_b[kfirst] = (uint8_T)iInflect_data[kfirst];
  }

  *iInflect_size = nPk;
  for (kfirst = 0; kfirst < nPk; kfirst++) {
    iInflect_data[kfirst] = uvtest_B.iPk_data_b[kfirst];
  }
}

static void uvtest_removeSmallPeaks(const real32_T y[199], const int32_T
  iFinite_data[], const int32_T *iFinite_size, int32_T iPk_data[], int32_T
  *iPk_size)
{
  int32_T nPk;
  int32_T k;
  real32_T u0;
  real32_T tmp;
  nPk = 0;
  for (k = 0; k < *iFinite_size; k++) {
    tmp = y[iFinite_data[k] - 1];
    if (tmp > 0.4f) {
      u0 = y[iFinite_data[k] - 2];
      if ((!(u0 > y[iFinite_data[k]])) && (!rtIsNaNF(y[iFinite_data[k]]))) {
        u0 = y[iFinite_data[k]];
      }

      if (tmp - u0 >= 0.0F) {
        nPk++;
        iPk_data[nPk - 1] = iFinite_data[k];
      }
    }
  }

  if (1 > nPk) {
    *iPk_size = 0;
  } else {
    *iPk_size = nPk;
  }
}

static void uvtest_do_vectors(const int32_T a_data[], const int32_T *a_size,
  const int32_T b_data[], const int32_T *b_size, int32_T c_data[], int32_T
  *c_size, int32_T ia_data[], int32_T *ia_size, int32_T ib_data[], int32_T
  *ib_size)
{
  int32_T na;
  int32_T nb;
  int32_T ncmax;
  int32_T nc;
  int32_T nia;
  int32_T nib;
  int32_T iafirst;
  int32_T ialast;
  int32_T ibfirst;
  int32_T iblast;
  int32_T b_ialast;
  int32_T ak;
  int32_T b_iblast;
  int32_T bk;
  na = *a_size;
  nb = *b_size;
  ncmax = *a_size + *b_size;
  *c_size = (int16_T)ncmax;
  *ia_size = *a_size;
  *ib_size = *b_size;
  nc = -1;
  nia = -1;
  nib = 0;
  iafirst = 1;
  ialast = 1;
  ibfirst = 0;
  iblast = 1;
  while ((ialast <= na) && (iblast <= nb)) {
    b_ialast = ialast;
    ak = a_data[ialast - 1];
    while ((b_ialast < *a_size) && (a_data[b_ialast] == ak)) {
      b_ialast++;
    }

    ialast = b_ialast;
    b_iblast = iblast;
    bk = b_data[iblast - 1];
    while ((b_iblast < *b_size) && (b_data[b_iblast] == bk)) {
      b_iblast++;
    }

    iblast = b_iblast;
    if (ak == bk) {
      nc++;
      c_data[nc] = ak;
      nia++;
      ia_data[nia] = iafirst;
      ialast = b_ialast + 1;
      iafirst = b_ialast + 1;
      iblast = b_iblast + 1;
      ibfirst = b_iblast;
    } else if (ak < bk) {
      nc++;
      nia++;
      c_data[nc] = ak;
      ia_data[nia] = iafirst;
      ialast = b_ialast + 1;
      iafirst = b_ialast + 1;
    } else {
      nc++;
      nib++;
      c_data[nc] = bk;
      ib_data[nib - 1] = ibfirst + 1;
      iblast = b_iblast + 1;
      ibfirst = b_iblast;
    }
  }

  while (ialast <= na) {
    iafirst = ialast;
    while ((iafirst < *a_size) && (a_data[ialast - 1] == a_data[iafirst])) {
      iafirst++;
    }

    nc++;
    nia++;
    c_data[nc] = a_data[ialast - 1];
    ia_data[nia] = ialast;
    ialast = iafirst + 1;
  }

  while (iblast <= nb) {
    na = iblast;
    while ((na < *b_size) && (b_data[iblast - 1] == b_data[na])) {
      na++;
    }

    nc++;
    nib++;
    c_data[nc] = b_data[iblast - 1];
    ib_data[nib - 1] = iblast;
    iblast = na + 1;
  }

  if (*a_size > 0) {
    if (1 > nia + 1) {
      *ia_size = 0;
    } else {
      *ia_size = nia + 1;
    }
  }

  if (*b_size > 0) {
    if (1 > nib) {
      *ib_size = 0;
    } else {
      *ib_size = nib;
    }
  }

  if (ncmax > 0) {
    if (1 > nc + 1) {
      *c_size = 0;
    } else {
      *c_size = nc + 1;
    }
  }
}

static void uvtest_sortIdx(const real32_T x_data[], const int32_T *x_size,
  int32_T idx_data[], int32_T *idx_size)
{
  int32_T n;
  int32_T k;
  int32_T i;
  int32_T i2;
  int32_T j;
  int32_T pEnd;
  int32_T p;
  int32_T q;
  int32_T qEnd;
  int32_T kEnd;
  int16_T b_idx_0;
  boolean_T x_data_0;
  real32_T tmp;
  int32_T tmp_0;
  n = *x_size + 1;
  b_idx_0 = (int16_T)*x_size;
  *idx_size = b_idx_0;
  if (0 <= b_idx_0 - 1) {
    memset(&idx_data[0], 0, b_idx_0 * sizeof(int32_T));
  }

  for (i = 1; i <= n - 2; i += 2) {
    tmp = x_data[i - 1];
    if (tmp >= x_data[i]) {
      x_data_0 = true;
    } else {
      x_data_0 = rtIsNaNF(tmp);
    }

    if (x_data_0) {
      idx_data[i - 1] = i;
      idx_data[i] = i + 1;
    } else {
      idx_data[i - 1] = i + 1;
      idx_data[i] = i;
    }
  }

  if ((*x_size & 1U) != 0U) {
    idx_data[*x_size - 1] = *x_size;
  }

  i = 2;
  while (i < n - 1) {
    i2 = i << 1;
    j = 1;
    pEnd = 1 + i;
    while (pEnd < n) {
      p = j - 1;
      q = pEnd;
      qEnd = j + i2;
      if (qEnd > n) {
        qEnd = n;
      }

      k = 0;
      kEnd = qEnd - j;
      while (k + 1 <= kEnd) {
        tmp = x_data[idx_data[p] - 1];
        tmp_0 = idx_data[q - 1];
        if (tmp >= x_data[tmp_0 - 1]) {
          x_data_0 = true;
        } else {
          x_data_0 = rtIsNaNF(tmp);
        }

        if (x_data_0) {
          uvtest_B.iwork_data_c[k] = idx_data[p];
          p++;
          if (p + 1 == pEnd) {
            while (q < qEnd) {
              k++;
              uvtest_B.iwork_data_c[k] = idx_data[q - 1];
              q++;
            }
          }
        } else {
          uvtest_B.iwork_data_c[k] = tmp_0;
          q++;
          if (q == qEnd) {
            while (p + 1 < pEnd) {
              k++;
              uvtest_B.iwork_data_c[k] = idx_data[p];
              p++;
            }
          }
        }

        k++;
      }

      for (pEnd = 0; pEnd < kEnd; pEnd++) {
        idx_data[(j + pEnd) - 1] = uvtest_B.iwork_data_c[pEnd];
      }

      j = qEnd;
      pEnd = qEnd + i;
    }

    i = i2;
  }
}

static int32_T uvtest_nonSingletonDim(const int32_T *x_size)
{
  int32_T dim;
  dim = 2;
  if (*x_size != 1) {
    dim = 1;
  }

  return dim;
}

static void uvtest_merge_pow2_block(int32_T idx_data[], int32_T x_data[])
{
  int32_T bLen;
  int32_T bLen2;
  int32_T nPairs;
  int32_T blockOffset;
  int32_T p;
  int32_T q;
  int32_T b;
  int32_T k;
  int32_T exitg1;
  for (b = 0; b < 6; b++) {
    bLen = 1 << (b + 2);
    bLen2 = bLen << 1;
    nPairs = 256 >> (b + 3);
    for (k = 0; k < nPairs; k++) {
      blockOffset = k * bLen2;
      for (p = 0; p < bLen2; p++) {
        q = blockOffset + p;
        uvtest_B.iwork[p] = idx_data[q];
        uvtest_B.xwork[p] = x_data[q];
      }

      p = 0;
      q = bLen;
      blockOffset--;
      do {
        exitg1 = 0;
        blockOffset++;
        if (uvtest_B.xwork[p] <= uvtest_B.xwork[q]) {
          idx_data[blockOffset] = uvtest_B.iwork[p];
          x_data[blockOffset] = uvtest_B.xwork[p];
          if (p + 1 < bLen) {
            p++;
          } else {
            exitg1 = 1;
          }
        } else {
          idx_data[blockOffset] = uvtest_B.iwork[q];
          x_data[blockOffset] = uvtest_B.xwork[q];
          if (q + 1 < bLen2) {
            q++;
          } else {
            blockOffset -= p;
            while (p + 1 <= bLen) {
              q = (blockOffset + p) + 1;
              idx_data[q] = uvtest_B.iwork[p];
              x_data[q] = uvtest_B.xwork[p];
              p++;
            }

            exitg1 = 1;
          }
        }
      } while (exitg1 == 0);
    }
  }
}

static void uvtest_merge(int32_T idx_data[], int32_T x_data[], int32_T offset,
  int32_T np, int32_T nq, int32_T iwork_data[], int32_T xwork_data[])
{
  int32_T p;
  int32_T q;
  int32_T iout;
  int32_T offset1;
  int32_T exitg1;
  if (nq != 0) {
    offset1 = np + nq;
    for (p = 0; p < offset1; p++) {
      q = offset + p;
      iwork_data[p] = idx_data[q];
      xwork_data[p] = x_data[q];
    }

    p = 0;
    q = np;
    iout = offset - 1;
    do {
      exitg1 = 0;
      iout++;
      if (xwork_data[p] <= xwork_data[q]) {
        idx_data[iout] = iwork_data[p];
        x_data[iout] = xwork_data[p];
        if (p + 1 < np) {
          p++;
        } else {
          exitg1 = 1;
        }
      } else {
        idx_data[iout] = iwork_data[q];
        x_data[iout] = xwork_data[q];
        if (q + 1 < offset1) {
          q++;
        } else {
          offset1 = iout - p;
          while (p + 1 <= np) {
            q = (offset1 + p) + 1;
            idx_data[q] = iwork_data[p];
            x_data[q] = xwork_data[p];
            p++;
          }

          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);
  }
}

static void uvtest_merge_block(int32_T idx_data[], int32_T x_data[], int32_T
  offset, int32_T n, int32_T preSortLevel, int32_T iwork_data[], int32_T
  xwork_data[])
{
  int32_T bLen;
  int32_T tailOffset;
  int32_T nTail;
  int32_T nPairs;
  nPairs = n >> preSortLevel;
  bLen = 1 << preSortLevel;
  while (nPairs > 1) {
    if ((nPairs & 1U) != 0U) {
      nPairs--;
      tailOffset = bLen * nPairs;
      nTail = n - tailOffset;
      if (nTail > bLen) {
        uvtest_merge(idx_data, x_data, offset + tailOffset, bLen, nTail - bLen,
                     iwork_data, xwork_data);
      }
    }

    tailOffset = bLen << 1;
    nPairs >>= 1;
    for (nTail = 0; nTail < nPairs; nTail++) {
      uvtest_merge(idx_data, x_data, offset + nTail * tailOffset, bLen, bLen,
                   iwork_data, xwork_data);
    }

    bLen = tailOffset;
  }

  if (n > bLen) {
    uvtest_merge(idx_data, x_data, offset, bLen, n - bLen, iwork_data,
                 xwork_data);
  }
}

static void uvtest_sortIdx_o(int32_T x_data[], int32_T *x_size, int32_T
  idx_data[], int32_T *idx_size)
{
  int32_T i4;
  int32_T c_x_size;
  int16_T b_x_idx_0;
  int16_T b_idx_0;
  int32_T tmp;
  int32_T tmp_0;
  b_x_idx_0 = (int16_T)*x_size;
  b_idx_0 = (int16_T)*x_size;
  *idx_size = b_x_idx_0;
  uvtest_B.nQuartets = b_x_idx_0;
  if (0 <= uvtest_B.nQuartets - 1) {
    memset(&idx_data[0], 0, uvtest_B.nQuartets * sizeof(int32_T));
  }

  if (*x_size != 0) {
    c_x_size = *x_size;
    uvtest_B.nQuartets = *x_size;
    if (0 <= uvtest_B.nQuartets - 1) {
      memcpy(&uvtest_B.c_x_data[0], &x_data[0], uvtest_B.nQuartets * sizeof
             (int32_T));
    }

    *idx_size = b_idx_0;
    uvtest_B.nQuartets = b_idx_0;
    if (0 <= uvtest_B.nQuartets - 1) {
      memset(&idx_data[0], 0, uvtest_B.nQuartets * sizeof(int32_T));
    }

    uvtest_B.x4[0] = 0;
    uvtest_B.idx4[0] = 0;
    uvtest_B.x4[1] = 0;
    uvtest_B.idx4[1] = 0;
    uvtest_B.x4[2] = 0;
    uvtest_B.idx4[2] = 0;
    uvtest_B.x4[3] = 0;
    uvtest_B.idx4[3] = 0;
    uvtest_B.nQuartets = *x_size >> 2;
    uvtest_B.nLeft = 0;
    while (uvtest_B.nLeft <= uvtest_B.nQuartets - 1) {
      uvtest_B.i_c = uvtest_B.nLeft << 2;
      uvtest_B.idx4[0] = (int16_T)(uvtest_B.i_c + 1);
      uvtest_B.idx4[1] = (int16_T)(uvtest_B.i_c + 2);
      uvtest_B.idx4[2] = (int16_T)(uvtest_B.i_c + 3);
      uvtest_B.idx4[3] = (int16_T)(uvtest_B.i_c + 4);
      uvtest_B.x4[0] = uvtest_B.c_x_data[uvtest_B.i_c];
      uvtest_B.i1 = uvtest_B.c_x_data[uvtest_B.i_c + 1];
      uvtest_B.x4[1] = uvtest_B.i1;
      uvtest_B.i3 = uvtest_B.c_x_data[uvtest_B.i_c + 2];
      uvtest_B.x4[2] = uvtest_B.i3;
      i4 = uvtest_B.c_x_data[uvtest_B.i_c + 3];
      uvtest_B.x4[3] = i4;
      if (uvtest_B.c_x_data[uvtest_B.i_c] <= uvtest_B.i1) {
        uvtest_B.i1 = 1;
        uvtest_B.i2 = 2;
      } else {
        uvtest_B.i1 = 2;
        uvtest_B.i2 = 1;
      }

      if (uvtest_B.i3 <= i4) {
        uvtest_B.i3 = 3;
        i4 = 4;
      } else {
        uvtest_B.i3 = 4;
        i4 = 3;
      }

      tmp = uvtest_B.x4[uvtest_B.i1 - 1];
      tmp_0 = uvtest_B.x4[uvtest_B.i3 - 1];
      if (tmp <= tmp_0) {
        if (uvtest_B.x4[uvtest_B.i2 - 1] <= tmp_0) {
          uvtest_B.perm[0] = (int8_T)uvtest_B.i1;
          uvtest_B.perm[1] = (int8_T)uvtest_B.i2;
          uvtest_B.perm[2] = (int8_T)uvtest_B.i3;
          uvtest_B.perm[3] = (int8_T)i4;
        } else if (uvtest_B.x4[uvtest_B.i2 - 1] <= uvtest_B.x4[i4 - 1]) {
          uvtest_B.perm[0] = (int8_T)uvtest_B.i1;
          uvtest_B.perm[1] = (int8_T)uvtest_B.i3;
          uvtest_B.perm[2] = (int8_T)uvtest_B.i2;
          uvtest_B.perm[3] = (int8_T)i4;
        } else {
          uvtest_B.perm[0] = (int8_T)uvtest_B.i1;
          uvtest_B.perm[1] = (int8_T)uvtest_B.i3;
          uvtest_B.perm[2] = (int8_T)i4;
          uvtest_B.perm[3] = (int8_T)uvtest_B.i2;
        }
      } else {
        tmp_0 = uvtest_B.x4[i4 - 1];
        if (tmp <= tmp_0) {
          if (uvtest_B.x4[uvtest_B.i2 - 1] <= tmp_0) {
            uvtest_B.perm[0] = (int8_T)uvtest_B.i3;
            uvtest_B.perm[1] = (int8_T)uvtest_B.i1;
            uvtest_B.perm[2] = (int8_T)uvtest_B.i2;
            uvtest_B.perm[3] = (int8_T)i4;
          } else {
            uvtest_B.perm[0] = (int8_T)uvtest_B.i3;
            uvtest_B.perm[1] = (int8_T)uvtest_B.i1;
            uvtest_B.perm[2] = (int8_T)i4;
            uvtest_B.perm[3] = (int8_T)uvtest_B.i2;
          }
        } else {
          uvtest_B.perm[0] = (int8_T)uvtest_B.i3;
          uvtest_B.perm[1] = (int8_T)i4;
          uvtest_B.perm[2] = (int8_T)uvtest_B.i1;
          uvtest_B.perm[3] = (int8_T)uvtest_B.i2;
        }
      }

      uvtest_B.i1 = uvtest_B.perm[0] - 1;
      idx_data[uvtest_B.i_c] = uvtest_B.idx4[uvtest_B.i1];
      uvtest_B.i2 = uvtest_B.perm[1] - 1;
      idx_data[uvtest_B.i_c + 1] = uvtest_B.idx4[uvtest_B.i2];
      uvtest_B.i3 = uvtest_B.perm[2] - 1;
      idx_data[uvtest_B.i_c + 2] = uvtest_B.idx4[uvtest_B.i3];
      i4 = uvtest_B.perm[3] - 1;
      idx_data[uvtest_B.i_c + 3] = uvtest_B.idx4[i4];
      uvtest_B.c_x_data[uvtest_B.i_c] = uvtest_B.x4[uvtest_B.i1];
      uvtest_B.c_x_data[uvtest_B.i_c + 1] = uvtest_B.x4[uvtest_B.i2];
      uvtest_B.c_x_data[uvtest_B.i_c + 2] = uvtest_B.x4[uvtest_B.i3];
      uvtest_B.c_x_data[uvtest_B.i_c + 3] = uvtest_B.x4[i4];
      uvtest_B.nLeft++;
    }

    uvtest_B.nQuartets <<= 2;
    uvtest_B.nLeft = (*x_size - uvtest_B.nQuartets) - 1;
    if (uvtest_B.nLeft + 1 > 0) {
      uvtest_B.i_c = 0;
      while (uvtest_B.i_c <= uvtest_B.nLeft) {
        uvtest_B.i1 = uvtest_B.nQuartets + uvtest_B.i_c;
        uvtest_B.idx4[uvtest_B.i_c] = (int16_T)(uvtest_B.i1 + 1);
        uvtest_B.x4[uvtest_B.i_c] = uvtest_B.c_x_data[uvtest_B.i1];
        uvtest_B.i_c++;
      }

      uvtest_B.perm[1] = 0;
      uvtest_B.perm[2] = 0;
      uvtest_B.perm[3] = 0;
      switch (uvtest_B.nLeft + 1) {
       case 1:
        uvtest_B.perm[0] = 1;
        break;

       case 2:
        if (uvtest_B.x4[0] <= uvtest_B.x4[1]) {
          uvtest_B.perm[0] = 1;
          uvtest_B.perm[1] = 2;
        } else {
          uvtest_B.perm[0] = 2;
          uvtest_B.perm[1] = 1;
        }
        break;

       default:
        if (uvtest_B.x4[0] <= uvtest_B.x4[1]) {
          if (uvtest_B.x4[1] <= uvtest_B.x4[2]) {
            uvtest_B.perm[0] = 1;
            uvtest_B.perm[1] = 2;
            uvtest_B.perm[2] = 3;
          } else if (uvtest_B.x4[0] <= uvtest_B.x4[2]) {
            uvtest_B.perm[0] = 1;
            uvtest_B.perm[1] = 3;
            uvtest_B.perm[2] = 2;
          } else {
            uvtest_B.perm[0] = 3;
            uvtest_B.perm[1] = 1;
            uvtest_B.perm[2] = 2;
          }
        } else if (uvtest_B.x4[0] <= uvtest_B.x4[2]) {
          uvtest_B.perm[0] = 2;
          uvtest_B.perm[1] = 1;
          uvtest_B.perm[2] = 3;
        } else if (uvtest_B.x4[1] <= uvtest_B.x4[2]) {
          uvtest_B.perm[0] = 2;
          uvtest_B.perm[1] = 3;
          uvtest_B.perm[2] = 1;
        } else {
          uvtest_B.perm[0] = 3;
          uvtest_B.perm[1] = 2;
          uvtest_B.perm[2] = 1;
        }
        break;
      }

      uvtest_B.i_c = 0;
      while (uvtest_B.i_c <= uvtest_B.nLeft) {
        uvtest_B.i1 = uvtest_B.perm[uvtest_B.i_c] - 1;
        uvtest_B.i2 = uvtest_B.nQuartets + uvtest_B.i_c;
        idx_data[uvtest_B.i2] = uvtest_B.idx4[uvtest_B.i1];
        uvtest_B.c_x_data[uvtest_B.i2] = uvtest_B.x4[uvtest_B.i1];
        uvtest_B.i_c++;
      }
    }

    uvtest_B.nQuartets = b_x_idx_0;
    if (0 <= uvtest_B.nQuartets - 1) {
      memset(&uvtest_B.iwork_data[0], 0, uvtest_B.nQuartets * sizeof(int32_T));
    }

    b_idx_0 = (int16_T)*x_size;
    uvtest_B.nQuartets = b_idx_0;
    if (0 <= uvtest_B.nQuartets - 1) {
      memset(&uvtest_B.xwork_data[0], 0, uvtest_B.nQuartets * sizeof(int32_T));
    }

    uvtest_B.nQuartets = 2;
    if (*x_size > 1) {
      if ((*x_size >= 256) && ((*x_size >> 8) > 0)) {
        uvtest_merge_pow2_block(idx_data, uvtest_B.c_x_data);
        if (*x_size - 256 > 0) {
          uvtest_B.nQuartets = b_x_idx_0;
          if (0 <= uvtest_B.nQuartets - 1) {
            memset(&uvtest_B.iwork_data[0], 0, uvtest_B.nQuartets * sizeof
                   (int32_T));
          }

          b_idx_0 = (int16_T)*x_size;
          uvtest_B.nQuartets = b_idx_0;
          if (0 <= uvtest_B.nQuartets - 1) {
            memset(&uvtest_B.xwork_data[0], 0, uvtest_B.nQuartets * sizeof
                   (int32_T));
          }

          uvtest_merge_block(idx_data, uvtest_B.c_x_data, 256, *x_size - 256, 2,
                             uvtest_B.iwork_data, uvtest_B.xwork_data);
        }

        uvtest_B.nQuartets = 8;
      }

      uvtest_merge_block(idx_data, uvtest_B.c_x_data, 0, *x_size,
                         uvtest_B.nQuartets, uvtest_B.iwork_data,
                         uvtest_B.xwork_data);
    }

    *x_size = c_x_size;
    uvtest_B.nQuartets = c_x_size;
    if (0 <= uvtest_B.nQuartets - 1) {
      memcpy(&x_data[0], &uvtest_B.c_x_data[0], uvtest_B.nQuartets * sizeof
             (int32_T));
    }
  }
}

static void uvtest_sort(int32_T x_data[], int32_T *x_size)
{
  uvtest_B.dim = uvtest_nonSingletonDim(x_size);
  if (uvtest_B.dim <= 1) {
    uvtest_B.b = *x_size - 1;
  } else {
    uvtest_B.b = 0;
  }

  uvtest_B.vwork_size = (int16_T)(uvtest_B.b + 1);
  uvtest_B.vstride = 1;
  uvtest_B.c_k = 0;
  while (uvtest_B.c_k <= uvtest_B.dim - 2) {
    uvtest_B.vstride *= *x_size;
    uvtest_B.c_k++;
  }

  uvtest_B.dim = 0;
  while (uvtest_B.dim <= uvtest_B.vstride - 1) {
    uvtest_B.c_k = 0;
    while (uvtest_B.c_k <= uvtest_B.b) {
      uvtest_B.vwork_data_m[uvtest_B.c_k] = x_data[uvtest_B.c_k *
        uvtest_B.vstride + uvtest_B.dim];
      uvtest_B.c_k++;
    }

    uvtest_sortIdx_o(uvtest_B.vwork_data_m, &uvtest_B.vwork_size,
                     uvtest_B.vwork_data.data, &uvtest_B.vwork_data.size);
    uvtest_B.c_k = 0;
    while (uvtest_B.c_k <= uvtest_B.b) {
      x_data[uvtest_B.dim + uvtest_B.c_k * uvtest_B.vstride] =
        uvtest_B.vwork_data_m[uvtest_B.c_k];
      uvtest_B.c_k++;
    }

    uvtest_B.dim++;
  }
}

static void findPeaksSeparatedByMoreThanMin(const real32_T y[199], const int32_T
  iPk_data[], const int32_T *iPk_size, int32_T idx_data[], int32_T *idx_size)
{
  int16_T b_locs_temp_idx_0;
  if (*iPk_size == 0) {
    *idx_size = 0;
  } else {
    uvtest_B.y_size = *iPk_size;
    uvtest_B.loop_ub_p = *iPk_size;
    for (uvtest_B.end = 0; uvtest_B.end < uvtest_B.loop_ub_p; uvtest_B.end++) {
      uvtest_B.y_data[uvtest_B.end] = y[iPk_data[uvtest_B.end] - 1];
    }

    uvtest_sortIdx(uvtest_B.y_data, &uvtest_B.y_size, uvtest_B.sortIdx_data,
                   &uvtest_B.sortIdx_size);
    uvtest_B.loop_ub_p = uvtest_B.sortIdx_size;
    for (uvtest_B.end = 0; uvtest_B.end < uvtest_B.loop_ub_p; uvtest_B.end++) {
      uvtest_B.locs_temp_data[uvtest_B.end] = (uint8_T)((uint8_T)
        (iPk_data[uvtest_B.sortIdx_data[uvtest_B.end] - 1] - 1) + 1U);
    }

    b_locs_temp_idx_0 = (int16_T)uvtest_B.sortIdx_size;
    uvtest_B.idelete_size_idx_0 = b_locs_temp_idx_0;
    uvtest_B.loop_ub_p = b_locs_temp_idx_0;
    if (0 <= uvtest_B.loop_ub_p - 1) {
      memset(&uvtest_B.idelete_data[0], 0, uvtest_B.loop_ub_p * sizeof(boolean_T));
    }

    uvtest_B.loop_ub_p = 0;
    while (uvtest_B.loop_ub_p <= uvtest_B.sortIdx_size - 1) {
      if (!uvtest_B.idelete_data[uvtest_B.loop_ub_p]) {
        uvtest_B.end = (uint8_T)((uint8_T)
          (iPk_data[uvtest_B.sortIdx_data[uvtest_B.loop_ub_p] - 1] - 1) + 1U);
        for (uvtest_B.c_i = 0; uvtest_B.c_i < uvtest_B.idelete_size_idx_0;
             uvtest_B.c_i++) {
          uvtest_B.idelete_data[uvtest_B.c_i] =
            (uvtest_B.idelete_data[uvtest_B.c_i] ||
             ((uvtest_B.locs_temp_data[uvtest_B.c_i] >= uvtest_B.end - 20) &&
              (uvtest_B.locs_temp_data[uvtest_B.c_i] <= uvtest_B.end + 20)));
        }

        uvtest_B.idelete_data[uvtest_B.loop_ub_p] = false;
      }

      uvtest_B.loop_ub_p++;
    }

    uvtest_B.end = b_locs_temp_idx_0 - 1;
    uvtest_B.idelete_size_idx_0 = 0;
    uvtest_B.loop_ub_p = 0;
    while (uvtest_B.loop_ub_p <= uvtest_B.end) {
      if (!uvtest_B.idelete_data[uvtest_B.loop_ub_p]) {
        uvtest_B.idelete_size_idx_0++;
      }

      uvtest_B.loop_ub_p++;
    }

    uvtest_B.loop_ub_p = 0;
    uvtest_B.c_i = 0;
    while (uvtest_B.c_i <= uvtest_B.end) {
      if (!uvtest_B.idelete_data[uvtest_B.c_i]) {
        uvtest_B.b_data[uvtest_B.loop_ub_p] = (int16_T)(uvtest_B.c_i + 1);
        uvtest_B.loop_ub_p++;
      }

      uvtest_B.c_i++;
    }

    *idx_size = uvtest_B.idelete_size_idx_0;
    for (uvtest_B.end = 0; uvtest_B.end < uvtest_B.idelete_size_idx_0;
         uvtest_B.end++) {
      idx_data[uvtest_B.end] =
        uvtest_B.sortIdx_data[uvtest_B.b_data[uvtest_B.end] - 1];
    }

    uvtest_sort(idx_data, idx_size);
  }
}

static void uvtest_keepAtMostNpPeaks(int32_T *idx_size)
{
  if (*idx_size > 199) {
    *idx_size = 199;
  }
}

static void uvtest_findpeaks(const real32_T Yin[199], real32_T Ypk_data[],
  int32_T *Ypk_size, real_T Xpk_data[], int32_T *Xpk_size)
{
  uvtest_getAllPeaksCodegen(Yin, uvtest_B.iFinite_data, &uvtest_B.iFinite_size,
    uvtest_B.iInfinite_data, &uvtest_B.iInfinite_size, uvtest_B.iInflect_data,
    &uvtest_B.iInflect_size);
  uvtest_removeSmallPeaks(Yin, uvtest_B.iFinite_data, &uvtest_B.iFinite_size,
    uvtest_B.tmp_data_c, &uvtest_B.tmp_size);
  uvtest_do_vectors(uvtest_B.tmp_data_c, &uvtest_B.tmp_size,
                    uvtest_B.iInfinite_data, &uvtest_B.iInfinite_size,
                    uvtest_B.c_data, &uvtest_B.c_size, uvtest_B.iInflect_data,
                    &uvtest_B.iInflect_size, uvtest_B.iFinite_data,
                    &uvtest_B.iFinite_size);
  findPeaksSeparatedByMoreThanMin(Yin, uvtest_B.c_data, &uvtest_B.c_size,
    uvtest_B.idx_data, &uvtest_B.iFinite_size);
  uvtest_keepAtMostNpPeaks(&uvtest_B.iFinite_size);
  uvtest_B.iPk_size_idx_0 = uvtest_B.iFinite_size;
  uvtest_B.loop_ub = uvtest_B.iFinite_size;
  for (uvtest_B.i0 = 0; uvtest_B.i0 < uvtest_B.loop_ub; uvtest_B.i0++) {
    uvtest_B.iPk_data[uvtest_B.i0] =
      uvtest_B.c_data[uvtest_B.idx_data[uvtest_B.i0] - 1];
  }

  *Ypk_size = uvtest_B.iFinite_size;
  for (uvtest_B.i0 = 0; uvtest_B.i0 < uvtest_B.iPk_size_idx_0; uvtest_B.i0++) {
    Ypk_data[uvtest_B.i0] = Yin[uvtest_B.iPk_data[uvtest_B.i0] - 1];
  }

  *Xpk_size = uvtest_B.iFinite_size;
  for (uvtest_B.i0 = 0; uvtest_B.i0 < uvtest_B.iPk_size_idx_0; uvtest_B.i0++) {
    Xpk_data[uvtest_B.i0] = (uint8_T)((uint8_T)(uvtest_B.iPk_data[uvtest_B.i0] -
      1) + 1U);
  }
}

static void uvtest_diff_n(const real_T x_data[], const int32_T *x_size, real_T
  y_data[], int32_T *y_size)
{
  int32_T ixLead;
  int32_T iyLead;
  real_T tmp2;
  int32_T m;
  real_T work_data_idx_0;
  int16_T ySize_idx_0;
  if (*x_size == 0) {
    *y_size = 0;
  } else {
    ixLead = *x_size - 1;
    if (ixLead >= 1) {
      ixLead = 1;
    }

    if (ixLead < 1) {
      *y_size = 0;
    } else {
      ySize_idx_0 = (int16_T)(*x_size - 1);
      *y_size = ySize_idx_0;
      if (ySize_idx_0 != 0) {
        ixLead = 1;
        iyLead = 0;
        work_data_idx_0 = x_data[0];
        for (m = 2; m <= *x_size; m++) {
          tmp2 = work_data_idx_0;
          work_data_idx_0 = x_data[ixLead];
          y_data[iyLead] = x_data[ixLead] - tmp2;
          ixLead++;
          iyLead++;
        }
      }
    }
  }
}

static real_T uvtest_mean(const real_T x_data[], const int32_T *x_size)
{
  real_T b_y;
  int32_T k;
  if (*x_size == 0) {
    b_y = 0.0;
  } else {
    b_y = x_data[0];
    for (k = 2; k <= *x_size; k++) {
      b_y += x_data[k - 1];
    }
  }

  return b_y / (real_T)*x_size;
}

static real32_T uvtest_mean_j(const real32_T x[200])
{
  real32_T b_y;
  int32_T k;
  b_y = x[0];
  for (k = 0; k < 199; k++) {
    b_y += x[k + 1];
  }

  return b_y / 200.0F;
}

static void uvtest_SystemCore_release(dspcodegen_BiquadFilter_uvtes_T *obj)
{
  if (obj->isInitialized == 1) {
    obj->isInitialized = 2;
  }
}

static void uvte_LPHPFilterBase_releaseImpl(dsp_LowpassFilter_uvtest_T *obj)
{
  uvtest_SystemCore_release(obj->FilterObj);
  obj->NumChannels = -1;
}

static void uvtes_SystemCore_releaseWrapper(dsp_LowpassFilter_uvtest_T *obj)
{
  if (obj->isSetupComplete) {
    uvte_LPHPFilterBase_releaseImpl(obj);
  }
}

static void uvtest_SystemCore_release_m(dsp_LowpassFilter_uvtest_T *obj)
{
  if (obj->isInitialized == 1) {
    uvtes_SystemCore_releaseWrapper(obj);
  }
}

static void uvtest_SystemCore_delete_e(dsp_LowpassFilter_uvtest_T *obj)
{
  uvtest_SystemCore_release_m(obj);
}

static void matlabCodegenHandle_matlabCod_m(dsp_LowpassFilter_uvtest_T *obj)
{
  if (!obj->matlabCodegenIsDeleted) {
    obj->matlabCodegenIsDeleted = true;
    uvtest_SystemCore_delete_e(obj);
  }
}

static void uvtest_SystemCore_delete(dspcodegen_BiquadFilter_uvtes_T *obj)
{
  uvtest_SystemCore_release(obj);
}

static void matlabCodegenHandle_matlabCodeg(dspcodegen_BiquadFilter_uvtes_T *obj)
{
  if (!obj->matlabCodegenIsDeleted) {
    obj->matlabCodegenIsDeleted = true;
    uvtest_SystemCore_delete(obj);
  }
}

void uvtest_FilterMain_Init(void)
{
  if (uvtest_DW.obj.FilterObj->isInitialized == 1) {
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[0] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[0] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[1] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[1] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[2] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[2] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[3] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[3] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
  }
}

void uvtest_FilterMain_Start(void)
{
  dspcodegen_BiquadFilter_uvtes_T *iobj_0;
  int32_T i;
  static const real32_T tmp[6] = { 0.0519477315F, -0.0832277462F, 0.0519477315F,
    0.0290188976F, -0.0135353142F, 0.0290188976F };

  uvtest_DW.gobj_1.matlabCodegenIsDeleted = true;
  uvtest_DW.gobj_0.matlabCodegenIsDeleted = true;
  uvtest_DW.obj.matlabCodegenIsDeleted = true;
  uvtest_DW.obj.isInitialized = 0;
  uvtest_DW.obj.NumChannels = -1;
  uvtest_DW.obj.matlabCodegenIsDeleted = false;
  uvtest_DW.objisempty = true;
  iobj_0 = &uvtest_DW.gobj_0;
  uvtest_DW.obj.isSetupComplete = false;
  uvtest_DW.obj.isInitialized = 1;
  uvtest_DW.gobj_0.isInitialized = 0;
  iobj_0->cSFunObject.P0_ICRTP = 0.0F;
  for (i = 0; i < 6; i++) {
    iobj_0->cSFunObject.P1_RTP1COEFF[i] = tmp[i];
  }

  iobj_0->cSFunObject.P2_RTP2COEFF[0] = -1.867311F;
  iobj_0->cSFunObject.P2_RTP2COEFF[1] = 0.91180259F;
  iobj_0->cSFunObject.P2_RTP2COEFF[2] = -1.76151896F;
  iobj_0->cSFunObject.P2_RTP2COEFF[3] = 0.782431126F;
  iobj_0->cSFunObject.P3_RTP3COEFF[0] = 0.0F;
  iobj_0->cSFunObject.P3_RTP3COEFF[1] = 0.0F;
  iobj_0->cSFunObject.P3_RTP3COEFF[2] = 0.0F;
  iobj_0->cSFunObject.P4_RTP_COEFF3_BOOL[0] = false;
  iobj_0->cSFunObject.P4_RTP_COEFF3_BOOL[1] = false;
  iobj_0->cSFunObject.P4_RTP_COEFF3_BOOL[2] = false;
  iobj_0->cSFunObject.P5_IC2RTP = 0.0F;
  uvtest_DW.gobj_0.matlabCodegenIsDeleted = false;
  uvtest_DW.obj.FilterObj = &uvtest_DW.gobj_0;
  uvtest_DW.obj.NumChannels = 1;
  uvtest_DW.obj.isSetupComplete = true;
}

void uvtest_FilterMain(void)
{
  dsp_BiquadFilter_0_uvtest_T *obj;
  boolean_T exitg1;
  uvtest_B.y = uvtest_U.input_data[0];
  for (uvtest_B.i = 0; uvtest_B.i < 199; uvtest_B.i++) {
    uvtest_B.y += uvtest_U.input_data[uvtest_B.i + 1];
  }

  uvtest_B.y /= 200.0F;
  for (uvtest_B.i = 0; uvtest_B.i < 200; uvtest_B.i++) {
    uvtest_B.xmean[uvtest_B.i] = uvtest_U.input_data[uvtest_B.i] - uvtest_B.y;
  }

  if (uvtest_DW.obj.FilterObj->isInitialized != 1) {
    uvtest_DW.obj.FilterObj->isSetupComplete = false;
    uvtest_DW.obj.FilterObj->isInitialized = 1;
    uvtest_DW.obj.FilterObj->isSetupComplete = true;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[0] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[0] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[1] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[1] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[2] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[2] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W0_ZERO_STATES[3] =
      uvtest_DW.obj.FilterObj->cSFunObject.P0_ICRTP;
    uvtest_DW.obj.FilterObj->cSFunObject.W1_POLE_STATES[3] =
      uvtest_DW.obj.FilterObj->cSFunObject.P5_IC2RTP;
  }

  obj = &uvtest_DW.obj.FilterObj->cSFunObject;
  uvtest_B.idx = 0;
  for (uvtest_B.i = 0; uvtest_B.i < 200; uvtest_B.i++) {
    uvtest_B.numAccum = obj->P1_RTP1COEFF[0] * uvtest_B.xmean[uvtest_B.idx];
    uvtest_B.numAccum += obj->P1_RTP1COEFF[1] * obj->W0_ZERO_STATES[0];
    uvtest_B.numAccum += obj->P1_RTP1COEFF[2] * obj->W0_ZERO_STATES[1];
    uvtest_B.numAccum -= obj->P2_RTP2COEFF[0] * obj->W1_POLE_STATES[0];
    uvtest_B.numAccum -= obj->P2_RTP2COEFF[1] * obj->W1_POLE_STATES[1];
    obj->W0_ZERO_STATES[1] = obj->W0_ZERO_STATES[0];
    obj->W0_ZERO_STATES[0] = uvtest_B.xmean[uvtest_B.idx];
    obj->W1_POLE_STATES[1] = obj->W1_POLE_STATES[0];
    obj->W1_POLE_STATES[0] = uvtest_B.numAccum;
    uvtest_B.stageIn = uvtest_B.numAccum;
    uvtest_B.numAccum *= obj->P1_RTP1COEFF[3];
    uvtest_B.numAccum += obj->P1_RTP1COEFF[4] * obj->W0_ZERO_STATES[2];
    uvtest_B.numAccum += obj->P1_RTP1COEFF[5] * obj->W0_ZERO_STATES[3];
    uvtest_B.numAccum -= obj->P2_RTP2COEFF[2] * obj->W1_POLE_STATES[2];
    uvtest_B.numAccum -= obj->P2_RTP2COEFF[3] * obj->W1_POLE_STATES[3];
    obj->W0_ZERO_STATES[3] = obj->W0_ZERO_STATES[2];
    obj->W0_ZERO_STATES[2] = uvtest_B.stageIn;
    obj->W1_POLE_STATES[3] = obj->W1_POLE_STATES[2];
    obj->W1_POLE_STATES[2] = uvtest_B.numAccum;
    uvtest_B.rtb_Lowpass2_k[uvtest_B.idx] = uvtest_B.numAccum;
    uvtest_B.idx++;
  }

  uvtest_diff(uvtest_B.rtb_Lowpass2_k, uvtest_B.fv0);
  uvtest_findpeaks(uvtest_B.fv0, uvtest_B.maxv_data, &uvtest_B.maxv_size,
                   uvtest_B.maxl_data, &uvtest_B.maxl_size);
  uvtest_Y.len_data = (real32_T)uvtest_B.maxl_size;
  uvtest_diff_n(uvtest_B.maxl_data, &uvtest_B.maxl_size, uvtest_B.tmp_data,
                &uvtest_B.maxv_size);
  uvtest_Y.brate_data = (real32_T)(60.0 / (uvtest_mean(uvtest_B.tmp_data,
    &uvtest_B.maxv_size) / 10.0));
  if (!rtIsNaNF(uvtest_B.rtb_Lowpass2_k[0])) {
    uvtest_B.idx = 1;
  } else {
    uvtest_B.idx = 0;
    uvtest_B.i = 2;
    exitg1 = false;
    while ((!exitg1) && (uvtest_B.i < 201)) {
      if (!rtIsNaNF(uvtest_B.rtb_Lowpass2_k[uvtest_B.i - 1])) {
        uvtest_B.idx = uvtest_B.i;
        exitg1 = true;
      } else {
        uvtest_B.i++;
      }
    }
  }

  if (uvtest_B.idx == 0) {
    uvtest_B.stageIn = uvtest_B.rtb_Lowpass2_k[0];
  } else {
    uvtest_B.stageIn = uvtest_B.rtb_Lowpass2_k[uvtest_B.idx - 1];
    while (uvtest_B.idx + 1 < 201) {
      if (uvtest_B.stageIn < uvtest_B.rtb_Lowpass2_k[uvtest_B.idx]) {
        uvtest_B.stageIn = uvtest_B.rtb_Lowpass2_k[uvtest_B.idx];
      }

      uvtest_B.idx++;
    }
  }

  if ((uvtest_B.stageIn - uvtest_mean_j(uvtest_B.rtb_Lowpass2_k)) / uvtest_B.y <
      0.002f) {
    uvtest_Y.brate_data = 0.0F;
  }
}

void uvtest_FilterMain_Term(void)
{
  matlabCodegenHandle_matlabCod_m(&uvtest_DW.obj);
  matlabCodegenHandle_matlabCodeg(&uvtest_DW.gobj_0);
  matlabCodegenHandle_matlabCodeg(&uvtest_DW.gobj_1);
}

void uvtest_step(void)
{
  uvtest_FilterMain();
}

void uvtest_initialize(void)
{
  rt_InitInfAndNaN(sizeof(real_T));
  uvtest_FilterMain_Start();
  uvtest_FilterMain_Init();
}

void uvtest_terminate(void)
{
  uvtest_FilterMain_Term();
}
