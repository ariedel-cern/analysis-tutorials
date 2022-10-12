// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \brief This task contains the individual steps that are to be taken
///        in the second part of the tutorial. These are 5 steps, and at the end,
///        the participant is expected to have a two-particle correlation spectrum.
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

//This is an example of a conveient declaration of "using"
using MyCompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;

//STEP 3: Partition and Filter simultaneously (and check results)
//This is a logical combination of filtering and partitioning.
//In practice, the partitions act on top of the already filtered data.
//This example also provides specific histograms to inspect the outcome.
struct partandfiltexample {
  // all defined filters are applied
  Filter etaFilter = nabs(aod::track::eta) < 0.5f;
  Filter trackDCA = nabs(aod::track::dcaXY) < 0.2f;
  using MyFilteredTracks = soa::Filtered<MyCompleteTracks>;

  Partition<MyFilteredTracks> leftTracks = aod::track::eta < 0.0f;
  Partition<MyFilteredTracks> rightTracks = aod::track::eta >= 0.0f;
  
  //Configurable for number of bins
  Configurable<int> nBins{"nBins", 100, "N bins in all histos"};

  // histogram defined with HistogramRegistry
  HistogramRegistry registry{
    "registry",
    {
      {"hVertexZ", "hVertexZ", {HistType::kTH1F, {{nBins, -15., 15.}}}},
      {"etaHistogramleft", "etaHistogramleft", {HistType::kTH1F, {{nBins, -1., +1}}}},
      {"ptHistogramleft", "ptHistogramleft", {HistType::kTH1F, {{nBins, 0., 10.0}}}},
      {"etaHistogramright", "etaHistogramright", {HistType::kTH1F, {{nBins, -1., +1}}}},
      {"ptHistogramright", "ptHistogramright", {HistType::kTH1F, {{nBins, 0., 10.0}}}}
      
    }
  };

  void process(aod::Collision const& collision, MyFilteredTracks const& tracks)
  {
    //Fill the event counter
    //check getter here: https://aliceo2group.github.io/analysis-framework/docs/datamodel/ao2dTables.html
    registry.get<TH1>(HIST("hVertexZ"))->Fill(collision.posZ());
    //This will take place once per event!
    for (auto& track : leftTracks) { //<- only for a subset
      if(track.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
      registry.get<TH1>(HIST("etaHistogramleft"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramleft"))->Fill(track.pt());
    }
    for (auto& track : rightTracks) { //<- only for a subset
      if(track.tpcNClsCrossedRows() < 70 ) continue; //can't filter on dynamic
      registry.get<TH1>(HIST("etaHistogramright"))->Fill(track.eta()); //<- this should show the selection
      registry.get<TH1>(HIST("ptHistogramright"))->Fill(track.pt());
    }
  }
};


WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<partandfiltexample>(cfgc)
  };
}
