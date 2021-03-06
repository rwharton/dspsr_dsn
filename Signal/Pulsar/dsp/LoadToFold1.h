//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// dspsr/Signal/Pulsar/dsp/LoadToFold1.h

#ifndef __dspsr_LoadToFold_h
#define __dspsr_LoadToFold_h

#include "dsp/SingleThread.h"
#include "dsp/Filterbank.h"
#include "dsp/InverseFilterbank.h"

namespace dsp {

  class TimeSeries;

  class Dedispersion;
  class Convolution;
  class Detection;
  class Fold;
  class Archiver;
  class FoldManager;

  class Response;
  class RFIFilter;
  class ResponseProduct;

  class OperationThread;
  class SKFilterbank;
  class SpectralKurtosis;
  class Resize;
  class SampleDelay;
  class PhaseLockedFilterbank;

  class PhaseSeriesUnloader;
  class SignalPath;

  class LoadToFoldN;

  //! A single LoadToFold thread
  class LoadToFold : public SingleThread
  {

  public:

    //! Configuration parameters
    class Config;

    //! Set the configuration to be used in prepare and run
    void set_configuration (Config*);

    //! Constructor
    LoadToFold (Config* config = 0);

    //! Destructor
    ~LoadToFold ();

    //! Create the pipeline
    void construct ();

    //! Finish preparing
    void prepare ();

    //! Run through the data
    void run ();

    //! Finish everything
    void finish ();

  protected:

    friend class LoadToFoldN;

    //! Share any necessary resources with the specified thread
    void share (SingleThread*);

    //! Wrap up tasks at end of data
    void end_of_data ();

    //! Return true if the output will be divided into sub-integrations
    bool output_subints () const;

    //! The dedispersion kernel
    Reference::To<Dedispersion> kernel;

    //! A folding algorithm for each pulsar to be folded
    std::vector< Reference::To<Fold> > fold;

    //! Manages the execution of multiple Fold algorithms
    Reference::To<FoldManager> fold_manager;

    //! Wrap each folder in a separate thread of execution
    std::vector< Reference::To<OperationThread> > asynch_fold;

    //! An unloader for each pulsar to be folded
    std::vector< Reference::To<PhaseSeriesUnloader> > unloader;

    //! An unique signal path for each pulsar to be folded
    std::vector< Reference::To<SignalPath> > path;

    //! Manage the archivers
    bool manage_archiver;

    //! The RFI filter
    Reference::To<RFIFilter> rfi_filter;

  private:

    //! Configuration parameters
    Reference::To<Config> config;

    //! Integrates the passband
    Reference::To<Response> passband;

    //! Optional SK filterbank
    // Reference::To<SKFilterbank> skfilterbank;

    //! Optional Spectral Kurtosis (for convolution)
    Reference::To<SpectralKurtosis> skestimator;

    //! Optional zero DM TimeSeries for Spectral Kurtosis
    Reference::To<TimeSeries> zero_DM_time_series;

    //! Optional SK Resizer
    Reference::To<Resize> skresize;

    //! Creates the filterbank
    Reference::To<Filterbank> filterbank;

    //! Optional filterbank engine
    Reference::To<Filterbank::Engine> filterbank_engine;

    //! reference to inverse filterbank
    Reference::To<InverseFilterbank> inverse_filterbank;

    //! Optional inverse filterbank engine
    Reference::To<InverseFilterbank::Engine> inverse_filterbank_engine;

    //! Performs coherent dedispersion
    Reference::To<Convolution> convolution;

    //! The product of the RFIFilter and Dedispersion kernel
    Reference::To<ResponseProduct> response_product;

    //! Removes inter-channel dispersion delays
    Reference::To<SampleDelay> sample_delay;

    //! Creates a filterbank in phase with the pulsar signal
    /*! Useful when trying to squeeze frequency resolution out of a short
      period pulsar for the purposes of scintillation measurments */
    Reference::To<PhaseLockedFilterbank> phased_filterbank;

    //! Detects the phase-coherent signal
    Reference::To<Detection> detect;

    //! Prepare to remove interchannel dispersion delays
    void prepare_interchan (TimeSeries*, bool run_on_gpu=false);

    //! Build to fold the given TimeSeries
    void build_fold (TimeSeries*);
    void build_fold (Reference::To<Fold>&, PhaseSeriesUnloader*);
    void configure_fold (unsigned ifold, TimeSeries* to_fold);
    void configure_detection (Detection*, unsigned);

    PhaseSeriesUnloader* get_unloader (unsigned ifold);

    //! Prepare all fold instances
    void prepare_fold ();
    bool fold_prepared;

    //! Prepare the given Archiver
    void prepare_archiver (Archiver*);

    //! Parse the epoch string into a reference epoch
    MJD parse_epoch (const std::string&);
  };

}

#endif // !defined(__LoadToFold_h)
