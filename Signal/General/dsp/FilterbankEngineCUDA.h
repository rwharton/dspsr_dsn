//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// dspsr/Signal/General/dsp/FilterbankCUDA.h

#ifndef __FilterbankEngineCUDA_h
#define __FilterbankEngineCUDA_h

#include "dsp/FilterbankEngine.h"
#include "dsp/LaunchConfig.h"

#include <cufft.h>

namespace CUDA
{
  class elapsed
  {
  public:
    elapsed ();
    void wrt (cudaEvent_t before);

    double total;
    cudaEvent_t after;
  };

  //! Discrete convolution filterbank step implemented using CUDA streams
  class FilterbankEngine : public dsp::Filterbank::Engine
  {
    unsigned nstream;

  public:

    //! Default Constructor
    FilterbankEngine (cudaStream_t stream);

    ~FilterbankEngine ();

    void setup (dsp::Filterbank*);
    void set_scratch (float *);

    void perform (const dsp::TimeSeries* in, dsp::TimeSeries* out,
                  uint64_t npart, uint64_t in_step, uint64_t out_step);

    void perform (const dsp::TimeSeries* in, dsp::TimeSeries* out, dsp::TimeSeries* zero_DM_out,
                  uint64_t npart, uint64_t in_step, uint64_t out_step);

    void finish ();

    FTransform::Plan* get_forward ();

    FTransform::Plan* get_backward ();

  protected:

    //! forward fft plan
    cufftHandle plan_fwd;

    //! backward fft plan
    cufftHandle plan_bwd;

    //! Complex-valued data
    bool real_to_complex;

    //! inplace FFT in CUDA memory
    float2* d_fft;

    //! zero DM response in CUDA memory
    float2* d_zero_DM_response;

    //! convolution kernel in CUDA memory
    float2* d_kernel;

    //! device scratch sapce
    float* scratch;

    unsigned npt_fwd;
    unsigned nchan_subband;
    unsigned freq_res;
    unsigned nfilt_pos;
    unsigned nkeep;

    LaunchConfig1D multiply;

    cudaStream_t stream;

    bool verbose;

  };

}

#endif
