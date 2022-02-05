//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __baseband_dsp_CyclicFold_h
#define __baseband_dsp_CyclicFold_h

#include "dsp/Transformation.h"
#include "dsp/TimeSeries.h"
#include "dsp/PhaseSeries.h"
#include "dsp/Fold.h"
#include "FTransformAgent.h"
#include "dsp/Apodization.h"

namespace dsp {

  //! Fold TimeSeries data into cyclic spectra
  /*! 
    Folds input voltage data into periodic correlations to compute cyclic
    spectra, using the 'time domain' lag folding algorithm described by
    Demorest (2011).  Output format is periodic spectra to fit into
    the usual pulse profile data structures.

    This Operation does not modify the TimeSeries.  Rather, it accumulates
    periodic correaltion data within its data structures, then Fourier
    transforms the lag dimension into spectra for the final output.

    Since basically the entire algorithm for standard folding applies
    here, this class inherits Fold.  The major differences are 
    that voltage input is required, and the output contains a
    different number of channels than the input.  The work is
    performed by CyclicFoldEngine.
  */
  class CyclicFold : public Fold {

    /* TODO:
     * Need to figure out how to overlap input by nlag points...
     */

  public:
    
    //! Constructor
    CyclicFold ();
    
    //! Destructor
    ~CyclicFold ();

    //! Prepare for folding
    virtual void prepare ();

    void set_engine(Engine*);
 
    //! Set the number of lags to fold
    void set_nlag(unsigned _nlag) { nlag = _nlag; }
    //! Get the number of lags to fold
    unsigned get_nlag() const { return nlag; }

    //! Set the amount of oversampling to improve channel isolation
    void set_mover(unsigned _mover) { mover = _mover; }
    //! Get the amount of oversampling
    unsigned get_mover() const { return mover; }

    //! Set number of channels to make
    void set_nchan(unsigned nchan) { set_nlag(mover*nchan/2 + 1); }

    //! Set the number of polarizations to compute
    void set_npol(unsigned _npol) { npol = _npol; }
    //! Get the number of lags to fold
    unsigned get_npol() const { return npol; }

  protected:

    //! Check that the input state is appropriate
    virtual void check_input();

    //! Prepare the output PhaseSeries
    virtual void prepare_output();

    //! Transfer current settings to engine
    virtual void setup_engine();

    //! Number of lags to compute when folding
    unsigned nlag;

    //! Oversampling factor
    unsigned mover;

    //! Number of output polns to compute
    unsigned npol;

  };

  //! Engine class that actually performs the computation
  /*! Engine class that performs the 'lag/fold' computation.  Could
   *  be supplemented with a GPU version, etc
   */
  class CyclicFoldEngine : public Fold::Engine
  {
  public:

    CyclicFoldEngine();
    ~CyclicFoldEngine();

    //! Set the number of lags to fold
    virtual void set_nlag (unsigned _nlag);

    //! Set the amount of oversampling
    virtual void set_mover (unsigned _mover);

    //! Set the number of phase bins and initialize any other data structures
    virtual void set_nbin (unsigned _nbin) { nbin = _nbin; }

    //! Set the number of polarizations to compute
    virtual void set_npol (unsigned _npol) { npol_out = _npol; }

    //! Set the phase bin into which the idat'th sample will be integrated
    virtual void set_bin (uint64_t idat, double ibin, double bins_per_samp);

    uint64_t set_bins (double phi, double phase_per_sample, uint64_t ndat, uint64_t idat_start);

    uint64_t get_bin_hits (int ibin);

    //! Return the PhaseSeries into which data will be folded
    virtual PhaseSeries* get_profiles ();

    void set_profiles (PhaseSeries* _out) { out = _out; }

    //! Perform the fold operation
    virtual void fold ();

    //! Synchronize the folded profile
    virtual void synch (PhaseSeries*);

    //! Zero internal data
    virtual void zero ();

    //! Enable engine to prepare any internal memory required for the plan
    virtual void set_ndat (uint64_t ndat, uint64_t idat_start);

    /* WvS - this is called only when data have been masked by weights */
    uint64_t get_ndat_folded () const { return 0; }

  protected:

    // Copy of output
    PhaseSeries* out;

    unsigned nlag;
    unsigned mover;
    unsigned nbin;
    unsigned npol_out;

    // Array of bins to fold into
    unsigned* binplan[2];
    uint64_t binplan_size;

    // Temp array to accumulate lag-domain results
    float* lagdata;
    uint64_t lagdata_size;
    float* get_lagdata_ptr(unsigned ichan, unsigned ipol, unsigned ibin);

    // FFT plan for going from lags to channels
    FTransform::Plan* lag2chan;
  }; 

}

#endif // !defined(__CyclicFold_h)
