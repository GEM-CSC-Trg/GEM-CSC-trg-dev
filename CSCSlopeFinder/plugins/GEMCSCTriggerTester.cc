#include <assert.h>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

//user include files below
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "MagneticField/Engine/interface/MagneticField.h"

#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2D.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiClusterCollection.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiCluster.h"
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TGraphAsymmErrors.h"
#include "TLorentzVector.h"

using namespace std;
using namespace edm;


struct GEMCSCTriggerData
{
  void init();
  TTree* book(TTree *t);
  //============ Muon Info ================//
  int muon_charge; float muon_pt; float muon_eta; float muon_phi; float muon_energy;
  unsigned long long evtNum; unsigned long long lumiBlock; int runNum;

  //============ Segment Info =============//
  bool has_segment; bool has_segment_ME31;
  int segment_CSC_endcap; int segment_CSC_station; 
  int segment_CSC_ring;   int segment_CSC_chamber;
  bool segment_CSC_ME1a; bool segment_CSC_ME1b;
  float segment_CSC_LP_x; float segment_CSC_LP_y; float segment_CSC_LP_z;
  float segment_CSC_GP_x; float segment_CSC_GP_y; float segment_CSC_GP_z;
  float segment_CSC_Ldir_x; float segment_CSC_Ldir_y; float segment_CSC_Ldir_z;
  float segment_CSC_slope_angle;
  float segment_CSC_nRecHits; float segment_CSC_chi2;
  int segment_CSC_strip; int segment_CSC_WG;

  //============ Segment Prop =============//
  bool has_segment_prop1;  bool has_segment_prop2;
  int segment_GE1_region;  int segment_GE2_region;
  int segment_GE1_station; int segment_GE2_station;
  int segment_GE1_ring;    int segment_GE2_ring;
  int segment_GE1_chamber; int segment_GE2_chamber;
  int segment_GE1_layer;   int segment_GE2_layer;
  int segment_GE1_roll;    int segment_GE2_roll;
  float segment_GE1_LP_x; float segment_GE1_LP_y; float segment_GE1_LP_z;
  float segment_GE2_LP_x; float segment_GE2_LP_y; float segment_GE2_LP_z;
  float segment_GE1_GP_x; float segment_GE1_GP_y; float segment_GE1_GP_z;
  float segment_GE2_GP_x; float segment_GE2_GP_y; float segment_GE2_GP_z;

  //============ Segment Match ============//
  bool has_segment_match1; bool has_segment_match2;
  float segment_match_GE1_residual; float segment_match_GE2_residual;
  float segment_match_GE1_LP_x; float segment_match_GE1_LP_y; float segment_match_GE1_LP_z;
  float segment_match_GE2_LP_x; float segment_match_GE2_LP_y; float segment_match_GE2_LP_z;
  float segment_match_GE1_GP_x; float segment_match_GE1_GP_y; float segment_match_GE1_GP_z;
  float segment_match_GE2_GP_x; float segment_match_GE2_GP_y; float segment_match_GE2_GP_z;

  //============ Track Info ===============//
  bool has_track;
  float track_chi2;
  int track_CSC_endcap; int track_CSC_station;
  int track_CSC_ring;   int track_CSC_chamber;
  float track_CSC_LP_x; float track_CSC_LP_y; float track_CSC_LP_z;
  float track_CSC_GP_x; float track_CSC_GP_y; float track_CSC_GP_z;
  float track_CSC_Ldir_x; float track_CSC_Ldir_y; float track_CSC_Ldir_z;

  //============ Track Prop ===============//
  bool has_track_prop1;  bool has_track_prop2;
  int track_GE1_region;  int track_GE2_region;
  int track_GE1_station; int track_GE2_station;
  int track_GE1_ring;    int track_GE2_ring;
  int track_GE1_chamber; int track_GE2_chamber;
  int track_GE1_layer;   int track_GE2_layer;
  int track_GE1_roll;    int track_GE2_roll;
  float track_GE1_LP_x; float track_GE1_LP_y; float track_GE1_LP_z;
  float track_GE2_LP_x; float track_GE2_LP_y; float track_GE2_LP_z;
  float track_GE1_GP_x; float track_GE1_GP_y; float track_GE1_GP_z;
  float track_GE2_GP_x; float track_GE2_GP_y; float track_GE2_GP_z;

  //============ Track Match ==============//
  bool has_track_match1; bool has_track_match2;
  float track_match_GE1_residual; float track_match_GE2_residual;
  float track_match_GE1_LP_x; float track_match_GE1_LP_y; float track_match_GE1_LP_z;
  float track_match_GE2_LP_x; float track_match_GE2_LP_y; float track_match_GE2_LP_z;
  float track_match_GE1_GP_x; float track_match_GE1_GP_y; float track_match_GE1_GP_z;
  float track_match_GE2_GP_x; float track_match_GE2_GP_y; float track_match_GE2_GP_z;

  //============ LCT Info =================//
  bool has_LCT;
  int LCT_CSC_endcap;    int LCT_CSC_station;
  int LCT_CSC_ring;      int LCT_CSC_chamber;
  bool LCT_CSC_ME1a;     bool LCT_CSC_ME1b;
  float LCT_segment_residual;
  int LCT_eighthstrip;   int LCT_slope;
  int LCT_wiregroup;     int LCT_quality;
  int LCT_bend;          int LCT_BX;
  int LCT_trknum;

  //============ LCT Prop =================//
  bool has_LCT_prop1;    bool has_LCT_prop2;
  int LCT_GE1_region;    int LCT_GE2_region;
  int LCT_GE1_station;   int LCT_GE2_station;
  int LCT_GE1_ring;      int LCT_GE2_ring;
  int LCT_GE1_chamber;   int LCT_GE2_chamber;
  int LCT_GE1_layer;     int LCT_GE2_layer;
  int LCT_GE1_strip;     int LCT_GE2_strip;
  int LCT_GE1_wiregroup; int LCT_GE2_wiregroup;

  //============ LCT Prop =================//
  bool has_LCT_match1; bool has_LCT_match2;
  float LCT_match_GE1_residual; float LCT_match_GE2_residual;
  int LCT_match_GE1_pad; int LCT_match_GE2_pad;
  int LCT_match_GE1_BX; int LCT_match_GE2_BX;
  int LCT_match_GE1_padES; int LCT_match_GE2_padES;
  int LCT_match_GE1_WGMin; int LCT_match_GE2_WGMin;
  int LCT_match_GE1_WGMax; int LCT_match_GE2_WGMax;
  vector<int> LCT_all_GE1_pads_ES; vector<int> LCT_all_GE2_pads_ES;
  vector<int> LCT_all_GE1_pads_ES_align_corr; vector<int> LCT_all_GE2_pads_ES_align_corr;
  vector<int> LCT_all_GE1_bxs; vector<int> LCT_all_GE2_bxs;
  vector<int> LCT_all_GE1_WGMin; vector<int> LCT_all_GE2_WGMin;
  vector<int> LCT_all_GE1_WGMax; vector<int> LCT_all_GE2_WGMax;
  int LCT_slope_with_GE1; int LCT_slope_with_GE2;
};

void GEMCSCTriggerData::init()
{
  //=========== Muon Info ===============//
  float value = 99999;
  muon_charge = value; muon_pt = value; muon_eta = value; muon_phi = value; muon_energy = value;
  evtNum = value; lumiBlock = value; runNum = value;

  //============ Segment Info =============//
  has_segment = false; has_segment_ME31 = false;
  segment_CSC_endcap = value; segment_CSC_station = value;
  segment_CSC_ring = value; segment_CSC_chamber = value;
  segment_CSC_ME1a = false; segment_CSC_ME1b = false;
  segment_CSC_LP_x = value; segment_CSC_LP_y = value; segment_CSC_LP_z = value;
  segment_CSC_GP_x = value; segment_CSC_GP_y = value; segment_CSC_GP_z = value;
  segment_CSC_Ldir_x = value; segment_CSC_Ldir_y = value; segment_CSC_Ldir_z = value;
  segment_CSC_slope_angle = value;
  segment_CSC_nRecHits = value; segment_CSC_chi2 = value;
  segment_CSC_strip = value; segment_CSC_WG = value;

  //============ Segment Prop =============//
  has_segment_prop1 = false;   has_segment_prop2 = false;
  segment_GE1_region = value;  segment_GE2_region = value;
  segment_GE1_station = value; segment_GE2_station = value;
  segment_GE1_ring = value;    segment_GE2_ring = value;
  segment_GE1_chamber = value; segment_GE2_chamber = value;
  segment_GE1_layer = value;   segment_GE2_layer = value;
  segment_GE1_roll = value;    segment_GE2_roll = value;
  segment_GE1_LP_x = value; segment_GE1_LP_y = value; segment_GE1_LP_z = value;
  segment_GE2_LP_x = value; segment_GE2_LP_y = value; segment_GE2_LP_z = value;
  segment_GE1_GP_x = value; segment_GE1_GP_y = value; segment_GE1_GP_z = value;
  segment_GE2_GP_x = value; segment_GE2_GP_y = value; segment_GE2_GP_z = value;

  //============ Segment Match ============//
  has_segment_match1 = false; has_segment_match2 = false;
  segment_match_GE1_residual = value; segment_match_GE2_residual = value;
  segment_match_GE1_LP_x = value; segment_match_GE1_LP_y = value; segment_match_GE1_LP_z = value;
  segment_match_GE2_LP_x = value; segment_match_GE2_LP_y = value; segment_match_GE2_LP_z = value;
  segment_match_GE1_GP_x = value; segment_match_GE1_GP_y = value; segment_match_GE1_GP_z = value;
  segment_match_GE2_GP_x = value; segment_match_GE2_GP_y = value; segment_match_GE2_GP_z = value;

  //============ Track Info ===============//
  has_track = false;
  track_chi2 = value;
  track_CSC_endcap = value; track_CSC_station = value;
  track_CSC_ring = value; track_CSC_chamber = value;
  track_CSC_LP_x = value; track_CSC_LP_y = value; track_CSC_LP_z = value;
  track_CSC_GP_x = value; track_CSC_GP_y = value; track_CSC_GP_z = value;
  track_CSC_Ldir_x = value; track_CSC_Ldir_y = value; track_CSC_Ldir_z = value;

  //============ Track Prop ===============//
  has_track_prop1 = false; has_track_prop2 = false;
  track_GE1_region = value;  track_GE2_region = value;
  track_GE1_station = value; track_GE2_station = value;
  track_GE1_ring = value;    track_GE2_ring = value;
  track_GE1_chamber = value; track_GE2_chamber = value;
  track_GE1_layer = value;   track_GE2_layer = value;
  track_GE1_roll = value;    track_GE2_roll = value;
  track_GE1_LP_x = value; track_GE1_LP_y = value; track_GE1_LP_z = value;
  track_GE2_LP_x = value; track_GE2_LP_y = value; track_GE2_LP_z = value;
  track_GE1_GP_x = value; track_GE1_GP_y = value; track_GE1_GP_z = value;
  track_GE2_GP_x = value; track_GE2_GP_y = value; track_GE2_GP_z = value;

  //============ Track Match ==============//
  has_track_match1 = false; has_track_match2 = false;
  track_match_GE1_residual = value; track_match_GE2_residual = value;
  track_match_GE1_LP_x = value; track_match_GE1_LP_y = value; track_match_GE1_LP_z = value;
  track_match_GE2_LP_x = value; track_match_GE2_LP_y = value; track_match_GE2_LP_z = value;
  track_match_GE1_GP_x = value; track_match_GE1_GP_y = value; track_match_GE1_GP_z = value;
  track_match_GE2_GP_x = value; track_match_GE2_GP_y = value; track_match_GE2_GP_z = value;

  //============ LCT Info =================//
  has_LCT = false;
  LCT_CSC_endcap = value; LCT_CSC_station = value;
  LCT_CSC_ring = value; LCT_CSC_chamber = value;
  LCT_CSC_ME1a = false; LCT_CSC_ME1b = false;
  LCT_segment_residual = value;
  LCT_eighthstrip = value; LCT_slope = value;
  LCT_wiregroup = value; LCT_quality = value;
  LCT_bend = value; LCT_BX = value;
  LCT_trknum = value;

  //============ LCT Prop =================//
  has_LCT_prop1 = false; has_LCT_prop2 = false;
  LCT_GE1_region = value;  LCT_GE2_region = value;
  LCT_GE1_station = value; LCT_GE2_station = value;
  LCT_GE1_ring = value;    LCT_GE2_ring = value;
  LCT_GE1_chamber = value; LCT_GE2_chamber = value;
  LCT_GE1_layer = value;   LCT_GE2_layer = value;
  LCT_GE1_strip = value;   LCT_GE2_strip = value;

  //============ LCT Match ================//
  has_LCT_match1 = false; has_LCT_match2 = false;
  LCT_match_GE1_residual = value; LCT_match_GE2_residual = value;
  LCT_match_GE1_pad = value; LCT_match_GE2_pad = value;
  LCT_match_GE1_BX = value; LCT_match_GE2_BX = value;
  LCT_match_GE1_padES = value; LCT_match_GE2_padES = value;
  LCT_match_GE1_WGMin = value; LCT_match_GE2_WGMin = value;
  LCT_match_GE1_WGMax = value; LCT_match_GE2_WGMax = value;
  LCT_all_GE1_pads_ES.clear(); LCT_all_GE2_pads_ES.clear();
  LCT_all_GE1_pads_ES_align_corr.clear(); LCT_all_GE2_pads_ES_align_corr.clear();
  LCT_all_GE1_bxs.clear(); LCT_all_GE2_bxs.clear();
  LCT_all_GE1_WGMin.clear(); LCT_all_GE2_WGMin.clear();
  LCT_all_GE1_WGMax.clear(); LCT_all_GE2_WGMax.clear();
  LCT_slope_with_GE1 = value; LCT_slope_with_GE2 = value;
}

TTree* GEMCSCTriggerData::book(TTree *t){
  edm::Service< TFileService > fs;
  t = fs->make<TTree>("GEM_CSC_Trigger", "GEM_CSC_Trigger");

  //=========== Muon Info =============//
  t->Branch("muon_charge", &muon_charge); t->Branch("muon_pt", &muon_pt); t->Branch("muon_eta", &muon_eta); t->Branch("muon_phi", &muon_phi); t->Branch("muon_energy", &muon_energy);
  t->Branch("evtNum", &evtNum); t->Branch("lumiBlock", &lumiBlock); t->Branch("runNum", &runNum);

  //============ Segment Info =============//
  t->Branch("has_segment", &has_segment); t->Branch("has_segment_ME31", &has_segment_ME31);
  t->Branch("segment_CSC_endcap", &segment_CSC_endcap); t->Branch("segment_CSC_station", &segment_CSC_station);
  t->Branch("segment_CSC_ring", &segment_CSC_ring); t->Branch("segment_CSC_chamber", &segment_CSC_chamber);
  t->Branch("segment_CSC_ME1a", &segment_CSC_ME1a); t->Branch("segment_CSC_ME1b", &segment_CSC_ME1b);
  t->Branch("segment_CSC_LP_x", &segment_CSC_LP_x); t->Branch("segment_CSC_LP_y", &segment_CSC_LP_y); t->Branch("segment_CSC_LP_z", &segment_CSC_LP_z);
  t->Branch("segment_CSC_GP_x", &segment_CSC_GP_x); t->Branch("segment_CSC_GP_y", &segment_CSC_GP_y); t->Branch("segment_CSC_GP_z", &segment_CSC_GP_z);
  t->Branch("segment_CSC_LDir_x", &segment_CSC_Ldir_x); t->Branch("segment_CSC_LDir_y", &segment_CSC_Ldir_y); t->Branch("segment_CSC_LDir_z", &segment_CSC_Ldir_z);
  t->Branch("segment_CSC_slope_angle", &segment_CSC_slope_angle);
  t->Branch("segment_CSC_nRecHits", &segment_CSC_nRecHits); t->Branch("segment_CSC_chi2", &segment_CSC_chi2);
  t->Branch("segment_CSC_strip", &segment_CSC_strip); t->Branch("segment_CSC_WG", &segment_CSC_WG);

  //============ Segment Prop =============//
  t->Branch("has_segment_prop1", &has_segment_prop1); t->Branch("has_segment_prop2", &has_segment_prop2);
  t->Branch("segment_GE1_region", &segment_GE1_region); t->Branch("segment_GE2_region", &segment_GE2_region);
  t->Branch("segment_GE1_station", &segment_GE1_station); t->Branch("segment_GE2_station", &segment_GE2_station);
  t->Branch("segment_GE1_ring", &segment_GE1_ring); t->Branch("segment_GE2_ring", &segment_GE1_ring);
  t->Branch("segment_GE1_chamber", &segment_GE1_chamber); t->Branch("segment_GE2_chamber", &segment_GE2_chamber);
  t->Branch("egment_GE1_layer", &segment_GE1_layer); t->Branch("segment_GE2_layer", &segment_GE2_layer);
  t->Branch("segment_GE1_roll", &segment_GE1_roll); t->Branch("segment_GE2_roll", &segment_GE2_roll);
  t->Branch("segment_GE1_LP_x", &segment_GE1_LP_x); t->Branch("segment_GE1_LP_y", &segment_GE1_LP_y); t->Branch("segment_GE1_LP_z", &segment_GE1_LP_z);
  t->Branch("segment_GE2_LP_x", &segment_GE2_LP_x); t->Branch("segment_GE2_LP_y", &segment_GE2_LP_y); t->Branch("segment_GE2_LP_z", &segment_GE2_LP_z);
  t->Branch("segment_GE1_GP_x", &segment_GE1_GP_x); t->Branch("segment_GE1_GP_y", &segment_GE1_GP_y); t->Branch("segment_GE1_GP_z", &segment_GE1_GP_z);
  t->Branch("segment_GE2_GP_x", &segment_GE2_GP_x); t->Branch("segment_GE2_GP_y", &segment_GE2_GP_y); t->Branch("segment_GE2_GP_z", &segment_GE2_GP_z);


  //============ Segment Match ============//
  t->Branch("has_segment_match1", &has_segment_match1); t->Branch("has_segment_match2", &has_segment_match2);
  t->Branch("segment_match_GE1_residual", &segment_match_GE1_residual); t->Branch("segment_match_GE2_residual", &segment_match_GE2_residual);
  t->Branch("segment_match_GE1_LP_x", &segment_match_GE1_LP_x); t->Branch("segment_match_GE1_LP_y", &segment_match_GE1_LP_y); t->Branch("segment_match_GE1_LP_z", &segment_match_GE1_LP_z);
  t->Branch("segment_match_GE2_LP_x", &segment_match_GE2_LP_x); t->Branch("segment_match_GE2_LP_y", &segment_match_GE2_LP_y); t->Branch("segment_match_GE2_LP_z", &segment_match_GE2_LP_z);
  t->Branch("segment_match_GE1_GP_x", &segment_match_GE1_GP_x); t->Branch("segment_match_GE1_GP_y", &segment_match_GE1_GP_y); t->Branch("segment_match_GE1_GP_z", &segment_match_GE1_GP_z);
  t->Branch("segment_match_GE2_GP_x", &segment_match_GE2_GP_x); t->Branch("segment_match_GE2_GP_y", &segment_match_GE2_GP_y); t->Branch("segment_match_GE2_GP_z", &segment_match_GE2_GP_z);


  //============ Track Info ===============//
  t->Branch("has_track", &has_track);
  t->Branch("track_chi2", &track_chi2);
  t->Branch("track_CSC_endcap", &track_CSC_endcap); t->Branch("track_CSC_station", &track_CSC_station);
  t->Branch("track_CSC_ring", &track_CSC_ring); t->Branch("track_CSC_chamber", &track_CSC_chamber);
  t->Branch("track_CSC_LP_x", &track_CSC_LP_x); t->Branch("track_CSC_LP_y", &track_CSC_LP_y); t->Branch("track_CSC_LP_z", &track_CSC_LP_z);
  t->Branch("track_CSC_GP_x", &track_CSC_GP_x); t->Branch("track_CSC_GP_y", &track_CSC_GP_y); t->Branch("track_CSC_GP_z", &track_CSC_GP_z);
  t->Branch("track_CSC_LDir_x", &track_CSC_Ldir_x); t->Branch("track_CSC_LDir_y", &track_CSC_Ldir_y); t->Branch("track_CSC_LDir_z", &track_CSC_Ldir_z);


  //============ LCT Info =================//
  t->Branch("has_LCT", &has_LCT);
  t->Branch("LCT_CSC_endcap", &LCT_CSC_endcap); t->Branch("LCT_CSC_station", &LCT_CSC_station);
  t->Branch("LCT_CSC_ring", &LCT_CSC_ring); t->Branch("LCT_CSC_chamber", &LCT_CSC_chamber);
  t->Branch("LCT_CSC_ME1a", &LCT_CSC_ME1a); t->Branch("LCT_CSC_ME1b", &LCT_CSC_ME1b);
  t->Branch("LCT_segment_residual", &LCT_segment_residual);
  t->Branch("LCT_eighthstrip", &LCT_eighthstrip); t->Branch("LCT_slope", &LCT_slope);
  t->Branch("LCT_wiregroup", &LCT_wiregroup); t->Branch("LCT_quality", &LCT_quality);
  t->Branch("LCT_bend", &LCT_bend); t->Branch("LCT_BX", &LCT_BX);
  t->Branch("LCT_trknum", &LCT_trknum);

  //============ LCT Prop =================//
  t->Branch("has_LCT_prop1", &has_LCT_prop1); t->Branch("has_LCT_prop2", &has_LCT_prop2);
  t->Branch("LCT_GE1_region", &LCT_GE1_region); t->Branch("LCT_GE2_region", &LCT_GE2_region);
  t->Branch("LCT_GE1_station", &LCT_GE1_station); t->Branch("LCT_GE2_station", &LCT_GE2_station);
  t->Branch("LCT_GE1_ring", &LCT_GE1_ring); t->Branch("LCT_GE2_ring", &LCT_GE2_ring);
  t->Branch("LCT_GE1_chamber", &LCT_GE1_chamber); t->Branch("LCT_GE2_chamber", &LCT_GE2_chamber);
  t->Branch("LCT_GE1_layer", &LCT_GE1_layer); t->Branch("LCT_GE2_layer", &LCT_GE2_layer);
  t->Branch("LCT_GE1_strip", &LCT_GE1_strip); t->Branch("LCT_GE2_strip", &LCT_GE2_strip);

  //============ LCT Match ================//
  t->Branch("has_LCT_match1", &has_LCT_match1); t->Branch("has_LCT_match2", &has_LCT_match2);
  t->Branch("LCT_match_GE1_residual", &LCT_match_GE1_residual); t->Branch("LCT_match_GE2_residual", &LCT_match_GE2_residual);
  t->Branch("LCT_match_GE1_pad", &LCT_match_GE1_pad); t->Branch("LCT_match_GE2_pad", &LCT_match_GE2_pad);
  t->Branch("LCT_match_GE1_BX", &LCT_match_GE1_BX); t->Branch("LCT_match_GE2_BX", &LCT_match_GE2_BX);
  t->Branch("LCT_match_GE1_padES", &LCT_match_GE1_padES); t->Branch("LCT_match_GE2_padES", &LCT_match_GE2_padES);
  t->Branch("LCT_all_GE1_pads_ES_align_corr", &LCT_all_GE1_pads_ES_align_corr); t->Branch("LCT_all_GE2_pads_ES_align_corr", &LCT_all_GE2_pads_ES_align_corr);
  t->Branch("LCT_match_GE1_WGMin", &LCT_match_GE1_WGMin); t->Branch("LCT_match_GE2_WGMin", &LCT_match_GE2_WGMin);
  t->Branch("LCT_match_GE1_WGMax", &LCT_match_GE1_WGMax); t->Branch("LCT_match_GE2_WGMax", &LCT_match_GE2_WGMax);
  t->Branch("LCT_all_GE1_pads_ES", &LCT_all_GE1_pads_ES); t->Branch("LCT_all_GE2_pads_ES", &LCT_all_GE2_pads_ES);
  t->Branch("LCT_all_GE1_bxs", &LCT_all_GE1_bxs); t->Branch("LCT_all_GE2_bxs", &LCT_all_GE2_bxs);
  t->Branch("LCT_all_GE1_WGMin", &LCT_all_GE1_WGMin); t->Branch("LCT_all_GE2_WGMin", &LCT_all_GE2_WGMin);
  t->Branch("LCT_all_GE1_WGMax", &LCT_all_GE1_WGMax); t->Branch("LCT_all_GE2_WGMax", &LCT_all_GE2_WGMax);
  t->Branch("LCT_slope_with_GE1", &LCT_slope_with_GE1); t->Branch("LCT_slope_with_GE2", &LCT_slope_with_GE2);
  return t;
}

class GEMCSCTriggerTester : public edm::one::EDAnalyzer<> {
public:
  explicit GEMCSCTriggerTester(const edm::ParameterSet&);
  ~GEMCSCTriggerTester(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;

  void startMuonAnalysis(const reco::Muon* mu);
  void findSegmentTrackLCT(const TrackingRecHit* RecHit, const reco::Track* Track, CSCSegment*& ME11_Segment, TrajectoryStateOnSurface& Track_At_Segment);
  void propagateSegmentTrackLCT(const reco::Track* Track, CSCSegment* ME11_Segment, TrajectoryStateOnSurface Track_At_Segment);
  void matchSegmentTrackLCT();

  void fillMap(map<int, int>& thisMap, string filename);

  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;
  edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
  edm::EDGetTokenT<GEMPadDigiClusterCollection> gemdigi_token;
  edm::Handle<GEMPadDigiClusterCollection> gemPadDigis;

  edm::EDGetTokenT<GEMRecHitCollection> gemRecHits_;
  edm::Handle<GEMRecHitCollection> gemRecHits;
  edm::EDGetTokenT<edm::View<reco::Muon> > muons_;
  edm::EDGetTokenT<reco::VertexCollection> vertexCollection_;
  edm::EDGetTokenT<CSCSegmentCollection> cscSegments_;

  edm::Service<TFileService> fs;
  MuonServiceProxy* theService_;
  
  edm::ESHandle<Propagator> propagator_;
  edm::ESHandle<TransientTrackBuilder> ttrackBuilder_;

  ESHandle<GlobalTrackingGeometry> theTrackingGeometry;
  
  edm::ESHandle<GEMGeometry> GEMGeometry_;
  edm::ESHandle<CSCGeometry> CSCGeometry_;

  bool use_alignment;
  bool debug;

  GEMCSCTriggerData data_;
  TTree* tree;

  bool isMC;

  const edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeomToken_;
  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeomToken_;
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttkToken_;
  const edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> geomToken_;

  //Maps for the CSC LCT Slope Extrapolation (CSC LCT -> GEM Layers)
  //Start with a LUTS folder
  string luts_folder;
  //Split by Even/Odd, by L1/L2, and by ME11a/ME11b (8 cases)
  string SlopeExtrapolationME11aEvenL1Name;
  map<int, int> SlopeExtrapolationME11aEvenL1_Map;
  string SlopeExtrapolationME11bEvenL1Name;
  map<int, int> SlopeExtrapolationME11bEvenL1_Map;
  string SlopeExtrapolationME11aOddL1Name;
  map<int, int> SlopeExtrapolationME11aOddL1_Map;
  string SlopeExtrapolationME11bOddL1Name;
  map<int, int> SlopeExtrapolationME11bOddL1_Map;

  string SlopeExtrapolationME11aEvenL2Name;
  map<int, int> SlopeExtrapolationME11aEvenL2_Map;
  string SlopeExtrapolationME11bEvenL2Name;
  map<int, int> SlopeExtrapolationME11bEvenL2_Map;
  string SlopeExtrapolationME11aOddL2Name;
  map<int, int> SlopeExtrapolationME11aOddL2_Map;
  string SlopeExtrapolationME11bOddL2Name;
  map<int, int> SlopeExtrapolationME11bOddL2_Map;

  //Maps for the GEM Pad Digi Clusters to be converted into CSC eighth strip units
  //Split by Even/Odd, and by ME11a/ME11b (4 cases)
  string GEMPadDigiToCSCEightStripME11aEvenName;
  map<int, int> GEMPadDigiToCSCEigthStripME11aEven_Map;
  string GEMPadDigiToCSCEightStripME11bEvenName;
  map<int, int> GEMPadDigiToCSCEigthStripME11bEven_Map;
  string GEMPadDigiToCSCEightStripME11aOddName;
  map<int, int> GEMPadDigiToCSCEigthStripME11aOdd_Map;
  string GEMPadDigiToCSCEightStripME11bOddName;
  map<int, int> GEMPadDigiToCSCEigthStripME11bOdd_Map;

  //Maps for the GEM Pad Digi Clusters to be converted into CSC Min and Max WireGroups
  //Split by Even/Odd, Layer1/Layer2, and by Min/Max
  string GEMPadDigiToCSCWGMinEvenL1Name;
  map<int, int> GEMPadDigiToCSCWGMinEvenL1_Map;
  string GEMPadDigiToCSCWGMaxEvenL1Name;
  map<int, int> GEMPadDigiToCSCWGMaxEvenL1_Map;

  string GEMPadDigiToCSCWGMinOddL1Name;
  map<int, int> GEMPadDigiToCSCWGMinOddL1_Map;
  string GEMPadDigiToCSCWGMaxOddL1Name;
  map<int, int> GEMPadDigiToCSCWGMaxOddL1_Map;

  string GEMPadDigiToCSCWGMinEvenL2Name;
  map<int, int> GEMPadDigiToCSCWGMinEvenL2_Map;
  string GEMPadDigiToCSCWGMaxEvenL2Name;
  map<int, int> GEMPadDigiToCSCWGMaxEvenL2_Map;

  string GEMPadDigiToCSCWGMinOddL2Name;
  map<int, int> GEMPadDigiToCSCWGMinOddL2_Map;
  string GEMPadDigiToCSCWGMaxOddL2Name;
  map<int, int> GEMPadDigiToCSCWGMaxOddL2_Map;

  string GEMAlignmentCorrectionsPositiveName;
  map<int, int> GEMAlignmentCorrectionsPositive_Map;
  string GEMAlignmentCorrectionsNegativeName;
  map<int, int> GEMAlignmentCorrectionsNegative_Map;


  //Maps for the CSC LCT Slope Adjustment (CSC LCT Slope -> Slope with GEM)
  //Split by Even/Odd, by L1/L2, and by ME11a/ME11b (8 cases)
  string SlopeAmendmentME11aEvenL1Name;
  map<int, int> SlopeAmendmentME11aEvenL1_Map;
  string SlopeAmendmentME11bEvenL1Name;
  map<int, int> SlopeAmendmentME11bEvenL1_Map;
  string SlopeAmendmentME11aOddL1Name;
  map<int, int> SlopeAmendmentME11aOddL1_Map;
  string SlopeAmendmentME11bOddL1Name;
  map<int, int> SlopeAmendmentME11bOddL1_Map;

  string SlopeAmendmentME11aEvenL2Name;
  map<int, int> SlopeAmendmentME11aEvenL2_Map;
  string SlopeAmendmentME11bEvenL2Name;
  map<int, int> SlopeAmendmentME11bEvenL2_Map;
  string SlopeAmendmentME11aOddL2Name;
  map<int, int> SlopeAmendmentME11aOddL2_Map;
  string SlopeAmendmentME11bOddL2Name;
  map<int, int> SlopeAmendmentME11bOddL2_Map;



  //Even matching window is 20 CSC 8th strips
  //Odd matching window is 40 CSC 8th strips
  //WG matching window is 7 WG
  int even_delta_es = 20;
  int odd_delta_es = 40;
  int delta_wg = 20;

  int me1a_counter = 0;

};


GEMCSCTriggerTester::GEMCSCTriggerTester(const edm::ParameterSet& iConfig)
  : gemGeomToken_(esConsumes()),
    cscGeomToken_(esConsumes()),
    ttkToken_(esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))),
    geomToken_(esConsumes())
{
  cout << "Begin CSC_LCT_Segment_Matcher" << endl;
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService_ = new MuonServiceProxy(serviceParameters, consumesCollector());
  muons_ = consumes<View<reco::Muon> >(iConfig.getParameter<InputTag>("muons"));
  vertexCollection_ = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"));
  gemRecHits_ = consumes<GEMRecHitCollection>(iConfig.getParameter<edm::InputTag>("gemRecHits"));
  cscSegments_ = consumes<CSCSegmentCollection>(edm::InputTag("cscSegments"));
  co_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("corrlctDigiTag"));
  gemdigi_token = consumes<GEMPadDigiClusterCollection>(iConfig.getParameter<edm::InputTag>("gemPadDigiCluster"));

  luts_folder = iConfig.getParameter<string>("luts_folder");

  use_alignment = iConfig.getParameter<bool>("alignment");
  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree);



  //Maps for the CSC LCT Slope Extrapolation (CSC LCT -> GEM Layers)
  //Split by Even/Odd, by L1/L2, and by ME11a/ME11b (8 cases)
  SlopeExtrapolationME11aEvenL1Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_even_GEMlayer1.txt";
  SlopeExtrapolationME11bEvenL1Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_even_GEMlayer1.txt";
  SlopeExtrapolationME11aOddL1Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_odd_GEMlayer1.txt";
  SlopeExtrapolationME11bOddL1Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_odd_GEMlayer1.txt";

  SlopeExtrapolationME11aEvenL2Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_even_GEMlayer2.txt";
  SlopeExtrapolationME11bEvenL2Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_even_GEMlayer2.txt";
  SlopeExtrapolationME11aOddL2Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_odd_GEMlayer2.txt";
  SlopeExtrapolationME11bOddL2Name = luts_folder + "/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_odd_GEMlayer2.txt";


  //Maps for the GEM Pad Digi Clusters to be converted into CSC eighth strip units
  //Split by Even/Odd, and by ME11a/ME11b (4 cases)
  GEMPadDigiToCSCEightStripME11aEvenName = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1a_even.txt";
  GEMPadDigiToCSCEightStripME11bEvenName = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1b_even.txt";
  GEMPadDigiToCSCEightStripME11aOddName = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1a_odd.txt";
  GEMPadDigiToCSCEightStripME11bOddName = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1b_odd.txt";

  //Maps for the GEM Pad Digi Clusters to be converted into CSC Min and Max WireGroups
  //Split by Even/Odd, Layer1/Layer2, and by Min/Max
  GEMPadDigiToCSCWGMinEvenL1Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_min_wg_ME11_even.txt";
  GEMPadDigiToCSCWGMaxEvenL1Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_max_wg_ME11_even.txt";

  GEMPadDigiToCSCWGMinOddL1Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_min_wg_ME11_odd.txt";
  GEMPadDigiToCSCWGMaxOddL1Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_max_wg_ME11_odd.txt";

  GEMPadDigiToCSCWGMinEvenL2Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_min_wg_ME11_even.txt";
  GEMPadDigiToCSCWGMaxEvenL2Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_max_wg_ME11_even.txt";

  GEMPadDigiToCSCWGMinOddL2Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_min_wg_ME11_odd.txt";
  GEMPadDigiToCSCWGMaxOddL2Name = luts_folder + "/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_max_wg_ME11_odd.txt";

  //Maps for GEM Alignment Corrections in CSC eighth strip units
  //Split by +/- endcap
  GEMAlignmentCorrectionsPositiveName = luts_folder + "/GEMCSC/AlignmentCorrection/GEMCSCLUT_align_corr_es_ME11_positive_endcap.txt";
  GEMAlignmentCorrectionsNegativeName = luts_folder + "/GEMCSC/AlignmentCorrection/GEMCSCLUT_align_corr_es_ME11_negative_endcap.txt";

  //Maps for the CSC LCT Slope Adjustment (CSC LCT Slope -> Slope with GEM)
  //Split by Even/Odd, by L1/L2, and by ME11a/ME11b (8 cases)
  SlopeAmendmentME11aEvenL1Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11a_even_GEMlayer1.txt";
  SlopeAmendmentME11bEvenL1Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11b_even_GEMlayer1.txt";
  SlopeAmendmentME11aOddL1Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11a_odd_GEMlayer1.txt";
  SlopeAmendmentME11bOddL1Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11b_odd_GEMlayer1.txt";

  SlopeAmendmentME11aEvenL2Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11a_even_GEMlayer2.txt";
  SlopeAmendmentME11bEvenL2Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11b_even_GEMlayer2.txt";
  SlopeAmendmentME11aOddL2Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11a_odd_GEMlayer2.txt";
  SlopeAmendmentME11bOddL2Name = luts_folder + "/GEMCSC/BendingAngle/SlopeAmendment_ME11b_odd_GEMlayer2.txt";




}


void
GEMCSCTriggerTester::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  GEMGeometry_ = &iSetup.getData(gemGeomToken_);
  CSCGeometry_ = &iSetup.getData(cscGeomToken_);
  ttrackBuilder_ = &iSetup.getData(ttkToken_);
  theTrackingGeometry = &iSetup.getData(geomToken_);

  theService_->update(iSetup);
  auto propagator = theService_->propagator("SteppingHelixPropagatorAny");

  isMC = false;
  if (! iEvent.eventAuxiliary().isRealData()) isMC = true;
  iEvent.getByToken(gemRecHits_, gemRecHits);
  edm::Handle<View<reco::Muon> > muons;
  iEvent.getByToken(co_token, correlatedlcts);
  iEvent.getByToken(gemdigi_token, gemPadDigis);

  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;
  if (! iEvent.getByToken(muons_, muons)) return;
  if (debug) cout << "There are " << muons->size() << " muons in event" << endl;
  if (muons->size() == 0) return;

  edm::Handle<CSCSegmentCollection> cscSegments;
  if (! iEvent.getByToken(cscSegments_, cscSegments)){std::cout << "Bad segments" << std::endl;}

  if (debug) cout << "New Event" << endl;

  std::cout << "Nmuons = " << muons->size() << std::endl;

  for (size_t i = 0; i < muons->size(); ++i){
    edm::RefToBase<reco::Muon> muRef = muons->refAt(i);
    const reco::Muon* mu = muRef.get();

    if (not mu->isGlobalMuon()) continue;

    //Lets find the ME1/1 segments from this muon
    if (!(mu->isStandAloneMuon())) continue;
    if (debug) cout << "New Muon" << endl;
    startMuonAnalysis(mu);
  }
}


void GEMCSCTriggerTester::startMuonAnalysis(const reco::Muon* mu){
  const reco::Track* Track = mu->outerTrack().get();
  if (Track->validFraction() > 0.0) return;

  //Find ME11 Segment from Track
  for (size_t RecHit_iter = 0; RecHit_iter != Track->recHitsSize(); RecHit_iter++){
    data_.init();
    data_.muon_charge = mu->charge();
    data_.muon_pt = mu->pt();
    data_.muon_eta = mu->eta();
    data_.muon_phi = mu->phi();
    data_.muon_energy = mu->energy();
    const TrackingRecHit* RecHit = (Track->recHit(RecHit_iter)).get();

    CSCSegment* ME11_Segment;
    TrajectoryStateOnSurface Track_At_Segment = TrajectoryStateOnSurface();

    findSegmentTrackLCT(RecHit, Track, ME11_Segment, Track_At_Segment);
    if (not ((data_.has_segment) or (data_.has_track) or (data_.has_LCT))) continue;
    if (debug) cout << "Results of findSegmentTrackLCT S:T:L " << data_.has_segment << ":" << data_.has_track << ":" << data_.has_LCT << endl;


    propagateSegmentTrackLCT(Track, ME11_Segment, Track_At_Segment);
    //if (!(data_.has_segment_prop1 or data_.has_segment_prop2)) continue;
    if (debug) cout << "Results from propagateSegmentTrackLCT S:T:L " << data_.has_segment_prop1 << data_.has_segment_prop2 << ":" << data_.has_track_prop1 << data_.has_track_prop2 << ":" << data_.has_LCT_prop1 << data_.has_LCT_prop2 << endl;

    matchSegmentTrackLCT();
    if (debug) cout << "Results from matchSegmentTrackLCT S:T:L " << data_.has_segment_match1 << data_.has_segment_match2 << ":" << data_.has_track_match1 << data_.has_track_match2 << ":" << data_.has_LCT_match1 << data_.has_LCT_match2 << endl;

    tree->Fill();
  } 
}


void GEMCSCTriggerTester::findSegmentTrackLCT(const TrackingRecHit* RecHit, const reco::Track* Track, CSCSegment*& ME11_Segment, TrajectoryStateOnSurface& Track_At_Segment){
  //if (debug) cout << "Starting findSegmentTrackLCT" << endl; //Even for debug this print out is obnoxious
  DetId RecHitId = RecHit->geographicalId();
  uint16_t RecHitDetId = RecHitId.det();
  if (RecHitDetId != DetId::Muon) return;
  uint16_t RecHitSubDet = RecHitId.subdetId();
  if (RecHitSubDet != (uint16_t)MuonSubdetId::CSC) return;
  CSCDetId SegmentCSCDetId = CSCDetId(RecHitId);
  if (!(SegmentCSCDetId.station() == 1 and ((SegmentCSCDetId.ring() == 1) or (SegmentCSCDetId.ring() == 4)) and RecHit->dimension() == 4)) return;
  //Found a rechit on Muon Detector, CSC, Station/Ring 1/1, and Segment (dimensions 4)
  RecSegment* Rec_Segment = (RecSegment*)RecHit;
  ME11_Segment = (CSCSegment*)Rec_Segment;
  CSCDetId SegmentCSCDetIdL4 = CSCDetId(SegmentCSCDetId.endcap(), SegmentCSCDetId.station(), SegmentCSCDetId.ring(), SegmentCSCDetId.chamber(), 4);
  //Build a new CSCDetId at layer 4 (key layer)
  const CSCLayer* ME11_Layer = CSCGeometry_->layer(SegmentCSCDetIdL4);
  const CSCLayerGeometry* ME11_Layer_Geo = ME11_Layer->geometry();
  reco::TransientTrack TTrack = ttrackBuilder_->build(Track);
  LocalPoint ME11_Segment_LP = ME11_Segment->localPosition();
  GlobalPoint ME11_Segment_GP = ME11_Layer->toGlobal(ME11_Segment_LP);

  data_.has_segment = true;
  data_.segment_CSC_endcap = SegmentCSCDetId.endcap();
  data_.segment_CSC_station = SegmentCSCDetId.station();
  data_.segment_CSC_ring = SegmentCSCDetId.ring();
  data_.segment_CSC_chamber = SegmentCSCDetId.chamber();
  data_.segment_CSC_ME1a = SegmentCSCDetId.ring() == 4;
  data_.segment_CSC_ME1b = SegmentCSCDetId.ring() == 1;
  data_.segment_CSC_LP_x = ME11_Segment_LP.x();
  data_.segment_CSC_LP_y = ME11_Segment_LP.y();
  data_.segment_CSC_LP_z = ME11_Segment_LP.z();
  data_.segment_CSC_GP_x = ME11_Segment_GP.x();
  data_.segment_CSC_GP_y = ME11_Segment_GP.y();
  data_.segment_CSC_GP_z = ME11_Segment_GP.z();
  LocalVector ME11_Segment_LDir = ME11_Segment->localDirection();
  data_.segment_CSC_Ldir_x = ME11_Segment_LDir.x();
  data_.segment_CSC_Ldir_y = ME11_Segment_LDir.y();
  data_.segment_CSC_Ldir_z = ME11_Segment_LDir.z();
  data_.segment_CSC_slope_angle = atan(data_.segment_CSC_Ldir_x/data_.segment_CSC_Ldir_z);
  data_.segment_CSC_nRecHits = ME11_Segment->nRecHits();
  data_.segment_CSC_chi2 = ME11_Segment->chi2();
  data_.segment_CSC_strip = ME11_Layer_Geo->nearestStrip(ME11_Segment->localPosition());
  data_.segment_CSC_WG = ME11_Layer_Geo->nearestWire(ME11_Segment->localPosition());
  if (debug) cout << "Found an ME11 Segment" << endl;

  //Check if the track also had a ME3/1 segment! Filters our soft scattered muons
  bool has_ME31 = false;
  for (size_t RecHit_iter = 0; RecHit_iter != Track->recHitsSize(); RecHit_iter++){
    const TrackingRecHit* RecHit_ME31 = (Track->recHit(RecHit_iter)).get();
    DetId RecHitId_ME31 = RecHit_ME31->geographicalId();
    uint16_t RecHitDetId_ME31 = RecHitId_ME31.det();
    if (RecHitDetId_ME31 != DetId::Muon) continue;
    uint16_t RecHitSubDet_ME31 = RecHitId_ME31.subdetId();
    if (RecHitSubDet_ME31 != (uint16_t)MuonSubdetId::CSC) continue;
    CSCDetId SegmentCSCDetId_ME31 = CSCDetId(RecHitId_ME31);
    if (SegmentCSCDetId_ME31.station() == 3 and SegmentCSCDetId_ME31.ring() == 1 and RecHit_ME31->dimension() == 4) has_ME31 = true;
    //Found a rechit on Muon Detector, CSC, Station/Ring 3/1, and Segment (dimensions 4) 
  }
  data_.has_segment_ME31 = has_ME31;


  //Find the Track at this Segment Position
  Track_At_Segment = TTrack.stateOnSurface(ME11_Segment_GP);
  if ((Track_At_Segment.isValid())){
    GlobalPoint Track_At_Segment_GP = Track_At_Segment.globalPosition();
    LocalPoint Track_At_Segment_LP = ME11_Layer->toLocal(Track_At_Segment_GP);
    data_.has_track = true;
    data_.track_chi2 = Track->chi2();
    data_.track_CSC_endcap = SegmentCSCDetId.endcap();
    data_.track_CSC_station = SegmentCSCDetId.station();
    data_.track_CSC_ring = SegmentCSCDetId.ring();
    data_.track_CSC_chamber = SegmentCSCDetId.chamber();
    data_.track_CSC_LP_x = Track_At_Segment_GP.x();
    data_.track_CSC_LP_y = Track_At_Segment_GP.y();
    data_.track_CSC_LP_z = Track_At_Segment_GP.z();
    data_.track_CSC_GP_x = Track_At_Segment_GP.x();
    data_.track_CSC_GP_y = Track_At_Segment_GP.y();
    data_.track_CSC_GP_z = Track_At_Segment_GP.z();
    LocalVector Track_At_Segment_LDir = Track_At_Segment.localDirection();
    data_.track_CSC_Ldir_x = Track_At_Segment_LDir.x();
    data_.track_CSC_Ldir_y = Track_At_Segment_LDir.y();
    data_.track_CSC_Ldir_z = Track_At_Segment_LDir.z();
    if (debug) cout << "Found a matching Track" << endl;
    if (debug) cout << "Segment LP : GP " << ME11_Segment_LP << " : " << ME11_Segment_GP << endl;
    if (debug) cout << "Track   LP : GP " << Track_At_Segment_LP << " : " << Track_At_Segment_LP << endl;
  }

  //Prepare FracStrips for LCT Matching
  float SegmentFracStripME1a = ME11_Layer_Geo->strip(ME11_Segment->localPosition()) + 64;
  float SegmentFracStripME1b = ME11_Layer_Geo->strip(ME11_Segment->localPosition());
  for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end(); j++){
    CSCDetId LCTDetId = (*j).first;
    if (!(LCTDetId.station() == 1 and ((LCTDetId.ring() == 1) or (LCTDetId.ring() == 4)))) continue;
    if (!(LCTDetId.chamber() == SegmentCSCDetId.chamber())) continue;  //Segment is still using Ring == 4 for ME1a, but the LCT is not! Cannot set DetID equal to eachother
    data_.LCT_CSC_endcap = LCTDetId.endcap();
    data_.LCT_CSC_station = LCTDetId.station();
    data_.LCT_CSC_ring = LCTDetId.ring();
    data_.LCT_CSC_chamber = LCTDetId.chamber();
    //data_.LCT_CSC_ME1a = LCTDetId.isME1a(); //These functions are broken! We must make our own
    //data_.LCT_CSC_ME1b = LCTDetId.isME1b(); //Will do it at the LCT loop
    if (debug) cout << "Found a LCTDet to SegmentDet Match on Det " << LCTDetId << endl;
    //Loop over LCT Clusters on the Chamber and find best match
    std::vector<CSCCorrelatedLCTDigi>::const_iterator CSCCorrLCT = (*j).second.first;
    std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
    CSCCorrelatedLCTDigi LCTDigiMatch = CSCCorrelatedLCTDigi();
    for (; CSCCorrLCT != last; ++CSCCorrLCT){
      me1a_counter = me1a_counter + 1;
      float LCTFracStrip = CSCCorrLCT->getFractionalStrip();
      //Match near the ME1a/b border
      float SegmentFracStrip = (abs(SegmentFracStripME1a - LCTFracStrip) < abs(SegmentFracStripME1b - LCTFracStrip) ) ? SegmentFracStripME1a : SegmentFracStripME1b; 

      if (abs(SegmentFracStrip - LCTFracStrip) < abs(SegmentFracStrip - LCTDigiMatch.getFractionalStrip())){
        //This is the best LCT Match so far
        LCTDigiMatch = *CSCCorrLCT;
        data_.has_LCT = true;
        if (debug) cout << "Found a matching LCT (checking for better one), residual = " << abs(SegmentFracStrip - LCTFracStrip) << endl;
        data_.LCT_eighthstrip = (CSCCorrLCT->getStrip())*4 + (CSCCorrLCT->getQuartStripBit())*2 + (CSCCorrLCT->getEighthStripBit());
        data_.LCT_segment_residual = SegmentFracStrip - LCTFracStrip;
        data_.LCT_slope = CSCCorrLCT->getSlope();
        data_.LCT_wiregroup = CSCCorrLCT->getKeyWG();
        data_.LCT_quality = CSCCorrLCT->getQuality();
        data_.LCT_bend = CSCCorrLCT->getBend();
        data_.LCT_BX = CSCCorrLCT->getBX();
        data_.LCT_CSC_ME1a = (data_.LCT_eighthstrip > 512) ? 1 : 0;
        data_.LCT_CSC_ME1b = (data_.LCT_eighthstrip < 512) ? 1 : 0;
        data_.LCT_trknum = CSCCorrLCT->getTrknmb();
      }
    }
  }
}


void GEMCSCTriggerTester::propagateSegmentTrackLCT(const reco::Track* Track, CSCSegment* ME11_Segment, TrajectoryStateOnSurface Track_At_Segment){
  if (debug) cout << "Starting propagateSegmentTrackLCT" << endl;
  //Set up Segment propagation
  DetId segDetId = ME11_Segment->geographicalId();
  if (debug) cout << "ME11 Seg at " << CSCDetId(segDetId) << endl;
  const GeomDet* segDet = theTrackingGeometry->idToDet(segDetId());
  LocalVector momentum_at_surface = (Track->outerP()) * (ME11_Segment->localDirection());
  LocalTrajectoryParameters param(ME11_Segment->localPosition(), momentum_at_surface, data_.muon_charge);
  AlgebraicSymMatrix mat(5,0);
  mat = ME11_Segment->parametersError().similarityT(ME11_Segment->projectionMatrix());
  LocalTrajectoryError error(asSMatrix<5>(mat));
  TrajectoryStateOnSurface TSOS_Segment(param, error, segDet->surface(), &*theService_->magneticField());

  //Set up Track propagation
  TrajectoryStateOnSurface TSOS_Track = Track_At_Segment;
  if (debug) cout << "Track TSOS? " << TSOS_Track.isValid() << endl;


  if (debug) cout << "Found Starting Trajectories ***NOTE Track Local Coords are not the same!!!***" << endl;
  if (debug and data_.has_segment) cout << "Seg TSOS   LP : GP : LocalDir " << TSOS_Segment.localPosition() << " : " << TSOS_Segment.globalPosition() << " : " << TSOS_Segment.localDirection() << endl;
  if (debug and data_.has_track) cout << "Track TSOS LP : GP : LocalDir " << TSOS_Track.localPosition() << " : " << TSOS_Track.globalPosition() << " : " << TSOS_Track.localDirection() << endl;

  auto propagator = theService_->propagator("SteppingHelixPropagatorAny");
  for (const auto& ch : GEMGeometry_->etaPartitions()){
    if (!(ch->id().station() == 1 and ch->id().ring() == 1)) continue; //Only look at GE1/1
    CSCDetId SegmentDetId = CSCDetId(segDetId);
    //if (!(((SegmentDetId.endcap() == 2 and ch->id().region() == -1) or (SegmentDetId.endcap() == 1 and ch->id().region() == 1)) and (SegmentDetId.station() == ch->id().station()) and (SegmentDetId.ring() == ch->id().ring()) and (SegmentDetId.chamber() == ch->id().chamber()))) continue; //Only look at same detector as segment
    if (!(((SegmentDetId.endcap() == 2 and ch->id().region() == -1) or (SegmentDetId.endcap() == 1 and ch->id().region() == 1)) and (SegmentDetId.station() == ch->id().station()) and (SegmentDetId.chamber() == ch->id().chamber()))) continue; //Only look at same detector as segment, cannot use ring for ME1a/b cut differences
    if (debug) cout << "Found same chamber as segment " << ch->id() << " and " << SegmentDetId << endl;
    bool good_segment_prop = false;
    const BoundPlane& bps(ch->surface());
    TrajectoryStateOnSurface TSOS_Segment_On_GEM = TrajectoryStateOnSurface();
    if (data_.has_segment){
      const BoundPlane& bps(ch->surface());
      TSOS_Segment_On_GEM = propagator->propagate(TSOS_Segment, ch->surface());
      if (TSOS_Segment_On_GEM.isValid()){
        const GlobalPoint Prop_GEM_GP = TSOS_Segment_On_GEM.globalPosition();
        const LocalPoint Prop_GEM_LP = TSOS_Segment_On_GEM.localPosition();
        const LocalPoint Prop_GEM_LP_2D(Prop_GEM_LP.x(), Prop_GEM_LP.y(), 0.0);
        if (((TSOS_Segment.globalPosition().z() * Prop_GEM_GP.z()) > 0.0) and (bps.bounds().inside(Prop_GEM_LP_2D))){
          good_segment_prop = true;
        }
      }
    }
    bool good_track_prop = false;
    TrajectoryStateOnSurface TSOS_Track_On_GEM = TrajectoryStateOnSurface();
    if (data_.has_track){
      TSOS_Track_On_GEM = propagator->propagate(TSOS_Track, ch->surface());
      if (TSOS_Track_On_GEM.isValid()){
        const GlobalPoint Prop_GEM_GP = TSOS_Track_On_GEM.globalPosition();
        const LocalPoint Prop_GEM_LP = TSOS_Track_On_GEM.localPosition();
        const LocalPoint Prop_GEM_LP_2D(Prop_GEM_LP.x(), Prop_GEM_LP.y(), 0.0);
        if (((TSOS_Track.globalPosition().z() * Prop_GEM_GP.z()) > 0.0) and (bps.bounds().inside(Prop_GEM_LP_2D))){
          good_track_prop = true;
        }
      }
    }
    if (debug) cout << "Finished Seg/Track props, got Seg:Tra " << good_segment_prop << ":" << good_track_prop << endl;
    //End of chamber loop, lets check if props were successful and fill correct references
    int ch_region = ch->id().region();
    int ch_station = ch->id().station();
    int ch_ring = ch->id().ring();
    int ch_chamber = ch->id().chamber();
    int ch_layer = ch->id().layer();
    int ch_roll = ch->id().roll();

    if (good_segment_prop){
      LocalPoint Segment_On_GEM_LP = TSOS_Segment_On_GEM.localPosition();
      GlobalPoint Segment_On_GEM_GP = TSOS_Segment_On_GEM.globalPosition();
      if (ch_layer == 1){
        data_.has_segment_prop1 = true;
        TrajectoryStateOnSurface TSOS_Segment_On_GE1 = TSOS_Segment_On_GEM;
        if (debug) cout << "Found segprop at L1 LP/GP " << TSOS_Segment_On_GEM.localPosition() << "/" << TSOS_Segment_On_GEM.globalPosition() << endl;
        data_.segment_GE1_region = ch_region;
        data_.segment_GE1_station = ch_station;
        data_.segment_GE1_ring = ch_ring;
        data_.segment_GE1_chamber = ch_chamber;
        data_.segment_GE1_layer = ch_layer;
        data_.segment_GE1_roll = ch_roll;
        data_.segment_GE1_LP_x = Segment_On_GEM_LP.x();
        data_.segment_GE1_LP_y = Segment_On_GEM_LP.y();
        data_.segment_GE1_LP_z = Segment_On_GEM_LP.z();
        data_.segment_GE1_GP_x = Segment_On_GEM_GP.x();
        data_.segment_GE1_GP_y = Segment_On_GEM_GP.y();
        data_.segment_GE1_GP_z = Segment_On_GEM_GP.z();
      }
      if (ch_layer == 2){
        data_.has_segment_prop2 = true;
        TrajectoryStateOnSurface TSOS_Segment_On_GE2 = TSOS_Segment_On_GEM;
        if (debug) cout << "Found segprop at L2 LP/GP " << TSOS_Segment_On_GEM.localPosition() << "/" << TSOS_Segment_On_GEM.globalPosition() << endl;
        data_.segment_GE2_region = ch_region;
        data_.segment_GE2_station = ch_station;
        data_.segment_GE2_ring = ch_ring;
        data_.segment_GE2_chamber = ch_chamber;
        data_.segment_GE2_layer = ch_layer;
        data_.segment_GE2_roll = ch_roll;
        data_.segment_GE2_LP_x = Segment_On_GEM_LP.x();
        data_.segment_GE2_LP_y = Segment_On_GEM_LP.y();
        data_.segment_GE2_LP_z = Segment_On_GEM_LP.z();
        data_.segment_GE2_GP_x = Segment_On_GEM_GP.x();
        data_.segment_GE2_GP_y = Segment_On_GEM_GP.y();
        data_.segment_GE2_GP_z = Segment_On_GEM_GP.z();
      }
    }
    if (good_track_prop){
      LocalPoint Track_On_GEM_LP = TSOS_Track_On_GEM.localPosition();
      GlobalPoint Track_On_GEM_GP = TSOS_Track_On_GEM.globalPosition();
      if (ch_layer == 1){
        data_.has_track_prop1 = true;
        TrajectoryStateOnSurface TSOS_Track_On_GE1 = TSOS_Track_On_GEM;
        if (debug) cout << "Found traprop at L1 LP/GP " << TSOS_Track_On_GEM.localPosition() << "/" << TSOS_Track_On_GEM.globalPosition() << endl;
        data_.track_GE1_region = ch_region;
        data_.track_GE1_station = ch_station;
        data_.track_GE1_ring = ch_ring;
        data_.track_GE1_chamber = ch_chamber;
        data_.track_GE1_layer = ch_layer;
        data_.track_GE1_roll = ch_roll;
        data_.track_GE1_LP_x = Track_On_GEM_LP.x();
        data_.track_GE1_LP_y = Track_On_GEM_LP.y();
        data_.track_GE1_LP_z = Track_On_GEM_LP.z();
        data_.track_GE1_GP_x = Track_On_GEM_GP.x();
        data_.track_GE1_GP_y = Track_On_GEM_GP.y();
        data_.track_GE1_GP_z = Track_On_GEM_GP.z();
      }
      if (ch_layer == 2){
        data_.has_track_prop2 = true;
        TrajectoryStateOnSurface TSOS_Track_On_GE2 = TSOS_Track_On_GEM;
        if (debug) cout << "Found traprop at L2 LP/GP " << TSOS_Track_On_GEM.localPosition() << "/" << TSOS_Track_On_GEM.globalPosition() << endl;
        data_.track_GE2_region = ch_region;
        data_.track_GE2_station = ch_station;
        data_.track_GE2_ring = ch_ring;
        data_.track_GE2_chamber = ch_chamber;
        data_.track_GE2_layer = ch_layer;
        data_.track_GE2_roll = ch_roll;
        data_.track_GE2_LP_x = Track_On_GEM_LP.x();
        data_.track_GE2_LP_y = Track_On_GEM_LP.y();
        data_.track_GE2_LP_z = Track_On_GEM_LP.z();
        data_.track_GE2_GP_x = Track_On_GEM_GP.x();
        data_.track_GE2_GP_y = Track_On_GEM_GP.y();
        data_.track_GE2_GP_z = Track_On_GEM_GP.z();
      }
    }
  }
  //Find LCT Propagation
  int LCT_eighth_strip = data_.LCT_eighthstrip;
  int LCT_slope = data_.LCT_slope;
  int LCT_bend = data_.LCT_bend;
  if (debug) cout << "Found the LCT Eighth Strip at " << LCT_eighth_strip << endl;
  if (debug) cout << "And slope at " << LCT_slope << endl;
  map<int, int> CSCLCTPropL1Map;
  map<int, int> CSCLCTPropL2Map;
  bool LCTChamber_Even = (data_.LCT_CSC_chamber % 2 == 0);
  if (data_.LCT_CSC_ME1a){
    CSCLCTPropL1Map = (LCTChamber_Even) ? SlopeExtrapolationME11aEvenL1_Map : SlopeExtrapolationME11aOddL1_Map;
    CSCLCTPropL2Map = (LCTChamber_Even) ? SlopeExtrapolationME11aEvenL2_Map : SlopeExtrapolationME11aOddL2_Map;
  }
  if (data_.LCT_CSC_ME1b){
    CSCLCTPropL1Map = (LCTChamber_Even) ? SlopeExtrapolationME11bEvenL1_Map : SlopeExtrapolationME11bOddL1_Map;
    CSCLCTPropL2Map = (LCTChamber_Even) ? SlopeExtrapolationME11bEvenL2_Map : SlopeExtrapolationME11bOddL2_Map;
  }
  int slope_propagationL1 = CSCLCTPropL1Map[LCT_slope];
  int slope_propagationL2 = CSCLCTPropL2Map[LCT_slope];
  if (debug) cout << "LCT Eighth Strip Prop Adjust L1/L2 = " << slope_propagationL1 << "/" << slope_propagationL2 << endl;
  int LCTToGEML1EighthStrip = LCT_eighth_strip - slope_propagationL1*((LCT_bend*2)-1); //For some reason, the slope is 0-Positive, 1-Negative
  int LCTToGEML2EighthStrip = LCT_eighth_strip - slope_propagationL2*((LCT_bend*2)-1); //This is stupid ):
  if (debug) cout << "LCT Prop from " << LCT_eighth_strip << " to L1 " << LCTToGEML1EighthStrip << " and L2 " << LCTToGEML2EighthStrip << endl;

  int window_es = (data_.LCT_CSC_chamber%2 == 0) ? even_delta_es : odd_delta_es;
  int strip_start = 999;
  int strip_end = 999;
  if (data_.LCT_CSC_ME1a){
    strip_start = 512;
    strip_end = 895;
  }
  if (data_.LCT_CSC_ME1b){
    strip_start = 0;
    strip_end = 511;
  }
  if (((strip_start - window_es) < LCTToGEML1EighthStrip) and (LCTToGEML1EighthStrip < (strip_end + window_es))){
    data_.has_LCT_prop1 = true;
    data_.LCT_GE1_region = int(2.0*(1.5-data_.LCT_CSC_endcap));
    data_.LCT_GE1_station = data_.LCT_CSC_station;
    data_.LCT_GE1_ring = data_.LCT_CSC_ring;
    data_.LCT_GE1_chamber = data_.LCT_CSC_chamber;
    data_.LCT_GE1_layer = 1;
    data_.LCT_GE1_strip = LCTToGEML1EighthStrip;
  }
  if (((strip_start - window_es) < LCTToGEML2EighthStrip) and (LCTToGEML2EighthStrip < (strip_end + window_es))){
    data_.has_LCT_prop2 = true;
    data_.LCT_GE2_region = int(2.0*(1.5-data_.LCT_CSC_endcap));
    data_.LCT_GE2_station = data_.LCT_CSC_station;
    data_.LCT_GE2_ring = data_.LCT_CSC_ring;
    data_.LCT_GE2_chamber = data_.LCT_CSC_chamber;
    data_.LCT_GE2_layer = 2;
    data_.LCT_GE2_strip = LCTToGEML2EighthStrip;
  }
}


void GEMCSCTriggerTester::matchSegmentTrackLCT(){
  if (debug) cout << "Starting matchSegmentTrackLCT" << endl;
  float segment_residual1 = 999.9;
  float segment_residual2 = 999.9;
  float track_residual1 = 999.9;
  float track_residual2 = 999.9;
  for (auto hit = gemRecHits->begin(); hit != gemRecHits->end(); hit++){
    GEMDetId gemid((hit)->geographicalId());
    if (!(gemid.det() == DetId::Detector::Muon && gemid.subdetId() == MuonSubdetId::GEM)) continue;
    const auto& etaPart = GEMGeometry_->etaPartition(gemid);
    LocalPoint hit_LP = hit->localPosition();
    GlobalPoint hit_GP = etaPart->toGlobal(hit_LP);
    //Seg Layer 1 Match
    if (data_.has_segment_prop1){
      GEMDetId seg_ge1_id = GEMDetId(data_.segment_GE1_region, data_.segment_GE1_ring, data_.segment_GE1_station, data_.segment_GE1_layer, data_.segment_GE1_chamber, data_.segment_GE1_roll);
      if ((gemid.station() == seg_ge1_id.station() and gemid.chamber() == seg_ge1_id.chamber() and gemid.layer() == seg_ge1_id.layer() and abs(gemid.roll() - seg_ge1_id.roll()) <= 1 and gemid.region() == seg_ge1_id.region())){
        data_.has_segment_match1 = true;
        float new_residual = hit_LP.x() - data_.segment_GE1_LP_x;
        if (abs(new_residual) < abs(segment_residual1)){
          data_.segment_match_GE1_residual = new_residual;
          data_.segment_match_GE1_LP_x = hit_LP.x();
          data_.segment_match_GE1_LP_y = hit_LP.y();
          data_.segment_match_GE1_LP_z = hit_LP.z();
          data_.segment_match_GE1_GP_x = hit_GP.x();
          data_.segment_match_GE1_GP_y = hit_GP.y();
          data_.segment_match_GE1_GP_z = hit_GP.z();
        }
      }
    }
    //Seg Layer 2 Match
    if (data_.has_segment_prop2){
      GEMDetId seg_ge2_id = GEMDetId(data_.segment_GE2_region, data_.segment_GE2_ring, data_.segment_GE2_station, data_.segment_GE2_layer, data_.segment_GE2_chamber, data_.segment_GE2_roll);
      if ((gemid.station() == seg_ge2_id.station() and gemid.chamber() == seg_ge2_id.chamber() and gemid.layer() == seg_ge2_id.layer() and abs(gemid.roll() - seg_ge2_id.roll()) <= 1 and gemid.region() == seg_ge2_id.region())){
        data_.has_segment_match2 = true;
        float new_residual = hit_LP.x() - data_.segment_GE2_LP_x;
        if (abs(new_residual) < abs(segment_residual2)){
          data_.segment_match_GE2_residual = new_residual;
          data_.segment_match_GE2_LP_x = hit_LP.x();
          data_.segment_match_GE2_LP_y = hit_LP.y();
          data_.segment_match_GE2_LP_z = hit_LP.z();
          data_.segment_match_GE2_GP_x = hit_GP.x();
          data_.segment_match_GE2_GP_y = hit_GP.y();
          data_.segment_match_GE2_GP_z = hit_GP.z();
        }
      }
    }
    //Tra Layer 1 Match
    if (data_.has_track_prop1){
      GEMDetId tra_ge1_id = GEMDetId(data_.track_GE1_region, data_.track_GE1_ring, data_.track_GE1_station, data_.track_GE1_layer, data_.track_GE1_chamber, data_.track_GE1_roll);
      if ((gemid.station() == tra_ge1_id.station() and gemid.chamber() == tra_ge1_id.chamber() and gemid.layer() == tra_ge1_id.layer() and abs(gemid.roll() - tra_ge1_id.roll()) <= 1 and gemid.region() == tra_ge1_id.region())){
        data_.has_track_match1 = true;
        float new_residual = hit_LP.x() - data_.track_GE1_LP_x;
        if (abs(new_residual) < abs(track_residual1)){
          data_.track_match_GE1_residual = new_residual;
          data_.track_match_GE1_LP_x = hit_LP.x();
          data_.track_match_GE1_LP_y = hit_LP.y();
          data_.track_match_GE1_LP_z = hit_LP.z();
          data_.track_match_GE1_GP_x = hit_GP.x();
          data_.track_match_GE1_GP_y = hit_GP.y();
          data_.track_match_GE1_GP_z = hit_GP.z();
        }
      }
    }
    //Tra Layer 2 Match
    if (data_.has_track_prop2){
      GEMDetId tra_ge2_id = GEMDetId(data_.track_GE2_region, data_.track_GE2_ring, data_.track_GE2_station, data_.track_GE2_layer, data_.track_GE2_chamber, data_.track_GE2_roll);
      if ((gemid.station() == tra_ge2_id.station() and gemid.chamber() == tra_ge2_id.chamber() and gemid.layer() == tra_ge2_id.layer() and abs(gemid.roll() - tra_ge2_id.roll()) <= 1 and gemid.region() == tra_ge2_id.region())){
        data_.has_track_match2 = true;
        float new_residual = hit_LP.x() - data_.track_GE2_LP_x;
        if (abs(new_residual) < abs(track_residual2)){
          data_.track_match_GE2_residual = new_residual;
          data_.track_match_GE2_LP_x = hit_LP.x();
          data_.track_match_GE2_LP_y = hit_LP.y();
          data_.track_match_GE2_LP_z = hit_LP.z();
          data_.track_match_GE2_GP_x = hit_GP.x();
          data_.track_match_GE2_GP_y = hit_GP.y();
          data_.track_match_GE2_GP_z = hit_GP.z();
        }
      }
    }
  }
  //Match LCT Propagated 8th strips to GEM Pad Digi Clusters
  //Even matching window is 20 CSC 8th strips
  //Odd matching window is 40 CSC 8th strips
  //WG matching window is 7 WG
  if (debug and (data_.LCT_quality == 6)) cout << "LCT QUALITY 6, CHECKING GEM CHAMBER " << data_.LCT_GE1_region << data_.LCT_GE1_chamber << " HAD STRIP PROP ON " << data_.LCT_GE1_strip << " AND " << data_.LCT_GE2_strip << endl;
  for (GEMPadDigiClusterCollection::DigiRangeIterator j = gemPadDigis->begin(); j != gemPadDigis->end(); j++){
    GEMDetId GEMPadDigiDetID = (*j).first;
    if ((data_.LCT_GE1_region ==  GEMPadDigiDetID.region()) and (data_.LCT_GE1_station == GEMPadDigiDetID.station()) and (data_.LCT_GE1_ring == GEMPadDigiDetID.ring()) and (data_.LCT_GE1_chamber == GEMPadDigiDetID.chamber()) and (data_.LCT_GE1_layer == GEMPadDigiDetID.layer()) and data_.has_LCT_prop1){
      if (debug) cout << "Found a GEM Digi on correct chamber L1 " << GEMPadDigiDetID << endl;
      if (debug and (data_.LCT_quality == 6)) cout << "Dumping all pads for chamber " << GEMPadDigiDetID << " " << endl;
      //Set up maps
      //int GEMPadToCSCes = 999;
      //int GEMPadMaxWG = 999;
      //int GEMPadMinWG = 999;
      map<int, int> DigiToESMap;
      map<int, int> WGMaxMap;
      map<int, int> WGMinMap;
      map<int, int> SlopeAmendMap;
      //Lets include Alignment Corrections (GEMAlignmentCorrectionsPositive_Map)
      //Measured with GEMHit - PropHit, so we can add alignment corr to the prop
      //To use AlignCorr map, we have AlignCorrMap[chEta]
      map<int, int> AlignCorrMap = (GEMPadDigiDetID.region() == 1) ? GEMAlignmentCorrectionsPositive_Map : GEMAlignmentCorrectionsNegative_Map;
      int AlignCorrKey = GEMPadDigiDetID.chamber() * 10 + GEMPadDigiDetID.roll();

      if (data_.LCT_CSC_ME1a){
        DigiToESMap = (data_.LCT_GE1_chamber%2 == 0) ? GEMPadDigiToCSCEigthStripME11aEven_Map : GEMPadDigiToCSCEigthStripME11aOdd_Map;
        SlopeAmendMap = (data_.LCT_GE1_chamber%2 == 0) ? SlopeAmendmentME11aEvenL1_Map : SlopeAmendmentME11aOddL1_Map;
      }
      if (data_.LCT_CSC_ME1b){
        DigiToESMap = (data_.LCT_GE1_chamber%2 == 0) ? GEMPadDigiToCSCEigthStripME11bEven_Map : GEMPadDigiToCSCEigthStripME11bOdd_Map;
        SlopeAmendMap = (data_.LCT_GE1_chamber%2 == 0) ? SlopeAmendmentME11bEvenL1_Map : SlopeAmendmentME11bOddL1_Map;
      }
      WGMinMap = (data_.LCT_GE1_chamber%2 == 0) ? GEMPadDigiToCSCWGMinEvenL1_Map : GEMPadDigiToCSCWGMinOddL1_Map;
      WGMaxMap = (data_.LCT_GE1_chamber%2 == 0) ? GEMPadDigiToCSCWGMaxEvenL1_Map : GEMPadDigiToCSCWGMaxOddL1_Map;

      std::vector<GEMPadDigiCluster>::const_iterator digiItr = (*j).second.first;
      std::vector<GEMPadDigiCluster>::const_iterator last = (*j).second.second;
      //Same L1 chamber, prep the vector of all pads_ES and bxs
      vector<int> tmp_all_L1_pads;
      vector<int> tmp_all_L1_aligncorr;
      vector<int> tmp_all_L1_bxs;
      vector<int> tmp_all_L1_WGMin;
      vector<int> tmp_all_L1_WGMax;
      for (; digiItr != last; ++digiItr) {
        int digiBX = digiItr->bx();
        int digiALCTMatch = digiItr->alctMatchTime();
        int delayGEMinOTMB = 1;
        int tmbL1aWindowSize = 7;
        int CSCConstants_LCT_CENTRAL_BX = 8;
        int adjustedBX = digiBX + CSCConstants_LCT_CENTRAL_BX - int(tmbL1aWindowSize/2.0) - digiALCTMatch + delayGEMinOTMB;
        //These are CLUSTERS, but we don't care about clusters we care about pads!!! 
        for (int pad: digiItr->pads()){   
          //Now finally, lets check if it is a match
          //Pad/Strip check -- abs(LCT_to_GEM*_eighth_strip - GEMPadToCSCes) < *_delta_es
          //WG/Eta check -- (GEMPadMinWG - delta_wg) < GEMPadDigiDetID.roll() < (GEMPadMaxWG + delta_wg)
          if (debug and data_.LCT_quality == 6) cout << " pad:ES " << pad << ":" << DigiToESMap[pad] << endl;

          //int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] * (-1.0) : AlignCorrMap[AlignCorrKey];
          //int align_corr_tmp = AlignCorrMap[AlignCorrKey]; //For now we show alignment needs the flip
          int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] : AlignCorrMap[AlignCorrKey] * (-1.0);
          if (use_alignment == false) align_corr_tmp = 0;
          int tmp_delta_es = data_.LCT_GE1_strip - (DigiToESMap[pad] + align_corr_tmp);
          int WG_Max = WGMaxMap[GEMPadDigiDetID.roll()-1];
          int WG_Min = WGMinMap[GEMPadDigiDetID.roll()-1];
          tmp_all_L1_pads.push_back(DigiToESMap[pad]);
          tmp_all_L1_aligncorr.push_back(AlignCorrMap[AlignCorrKey]);
          tmp_all_L1_bxs.push_back(adjustedBX);
          tmp_all_L1_WGMin.push_back(WG_Min);
          tmp_all_L1_WGMax.push_back(WG_Max);
          if ((abs(tmp_delta_es) < abs(data_.LCT_match_GE1_residual)) and ((WG_Min - delta_wg) < data_.LCT_wiregroup) and (data_.LCT_wiregroup < (WG_Max + delta_wg))){
            data_.has_LCT_match1 = true;
            data_.LCT_match_GE1_residual = tmp_delta_es;
            if (debug) cout << "Filled residual with " << tmp_delta_es << endl;
            data_.LCT_match_GE1_pad = pad;
            data_.LCT_match_GE1_BX = adjustedBX;
            data_.LCT_match_GE1_padES = DigiToESMap[pad];
            data_.LCT_match_GE1_WGMin = WG_Min;
            data_.LCT_match_GE1_WGMax = WG_Max;

            data_.LCT_slope_with_GE1 = SlopeAmendMap[abs((DigiToESMap[pad] + align_corr_tmp) - data_.LCT_eighthstrip)]*pow(-1.0, ((DigiToESMap[pad] + align_corr_tmp) - data_.LCT_eighthstrip) < 0);

          }
        }
      }
      data_.LCT_all_GE1_pads_ES = tmp_all_L1_pads;
      data_.LCT_all_GE1_pads_ES_align_corr = tmp_all_L1_aligncorr;
      data_.LCT_all_GE1_bxs = tmp_all_L1_bxs;
      data_.LCT_all_GE1_WGMin = tmp_all_L1_WGMin;
      data_.LCT_all_GE1_WGMax = tmp_all_L1_WGMax;
    }


    if ((data_.LCT_GE2_region ==  GEMPadDigiDetID.region()) and (data_.LCT_GE2_station == GEMPadDigiDetID.station()) and (data_.LCT_GE2_ring == GEMPadDigiDetID.ring()) and (data_.LCT_GE2_chamber == GEMPadDigiDetID.chamber()) and (data_.LCT_GE2_layer == GEMPadDigiDetID.layer()) and data_.has_LCT_prop2){
      if (debug) cout << "Found a GEM Digi on correct chamber L2 " << GEMPadDigiDetID << endl;
      if (debug and (data_.LCT_quality == 6)) cout << "Dumping all pads for chamber " << GEMPadDigiDetID << " " << endl;
      map<int, int> DigiToESMap;
      map<int, int> WGMaxMap;
      map<int, int> WGMinMap;
      map<int, int> SlopeAmendMap;
      //Lets include Alignment Corrections (GEMAlignmentCorrectionsPositive_Map)
      //Measured with GEMHit - PropHit, so we can add alignment corr to the prop
      //To use AlignCorr map, we have AlignCorrMap[chEta]
      map<int, int> AlignCorrMap = (GEMPadDigiDetID.region() == 1) ? GEMAlignmentCorrectionsPositive_Map : GEMAlignmentCorrectionsNegative_Map;
      int AlignCorrKey = GEMPadDigiDetID.chamber() * 10 + GEMPadDigiDetID.roll();
      if (data_.LCT_CSC_ME1a){
        DigiToESMap = (data_.LCT_GE2_chamber%2 == 0) ? GEMPadDigiToCSCEigthStripME11aEven_Map : GEMPadDigiToCSCEigthStripME11aOdd_Map;
        SlopeAmendMap = (data_.LCT_GE2_chamber%2 == 0) ? SlopeAmendmentME11aEvenL2_Map : SlopeAmendmentME11aOddL2_Map;
      }
      if (data_.LCT_CSC_ME1b){
        DigiToESMap = (data_.LCT_GE2_chamber%2 == 0) ? GEMPadDigiToCSCEigthStripME11bEven_Map : GEMPadDigiToCSCEigthStripME11bOdd_Map;
        SlopeAmendMap = (data_.LCT_GE2_chamber%2 == 0) ? SlopeAmendmentME11bEvenL2_Map : SlopeAmendmentME11bOddL2_Map;
      }
      WGMinMap = (data_.LCT_GE1_chamber%2 == 0) ? GEMPadDigiToCSCWGMinEvenL2_Map : GEMPadDigiToCSCWGMinOddL2_Map;
      WGMaxMap = (data_.LCT_GE1_chamber%2 == 0) ? GEMPadDigiToCSCWGMaxEvenL2_Map : GEMPadDigiToCSCWGMaxOddL2_Map;

      std::vector<GEMPadDigiCluster>::const_iterator digiItr = (*j).second.first;
      std::vector<GEMPadDigiCluster>::const_iterator last = (*j).second.second;
      //Same L2 chamber, prep the vector of all pads_ES and bxs
      vector<int> tmp_all_L2_pads;
      vector<int> tmp_all_L2_aligncorr;
      vector<int> tmp_all_L2_bxs;
      vector<int> tmp_all_L2_WGMin;
      vector<int> tmp_all_L2_WGMax;

      for (; digiItr != last; ++digiItr) {
        int digiBX = digiItr->bx();
        int digiALCTMatch = digiItr->alctMatchTime();
        int delayGEMinOTMB = 1;
        int tmbL1aWindowSize = 7;
        int CSCConstants_LCT_CENTRAL_BX = 8;
        int adjustedBX = digiBX + CSCConstants_LCT_CENTRAL_BX - int(tmbL1aWindowSize/2.0) - digiALCTMatch + delayGEMinOTMB;
        for (int pad: digiItr->pads()){
          if (debug and (data_.LCT_quality == 6)) cout << " pad:ES " << pad << ":" << DigiToESMap[pad] << endl;
          //int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] * (-1.0) : AlignCorrMap[AlignCorrKey];
          //int align_corr_tmp = AlignCorrMap[AlignCorrKey]; //For now we show alignment needs the flip
          int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] : AlignCorrMap[AlignCorrKey] * (-1.0);
          if (use_alignment == false) align_corr_tmp = 0;
          int tmp_delta_es = data_.LCT_GE2_strip - (DigiToESMap[pad] + align_corr_tmp);
          int WG_Max = WGMaxMap[GEMPadDigiDetID.roll()-1];
          int WG_Min = WGMinMap[GEMPadDigiDetID.roll()-1];
          tmp_all_L2_pads.push_back(DigiToESMap[pad]);
          tmp_all_L2_aligncorr.push_back(AlignCorrMap[AlignCorrKey]);
          tmp_all_L2_bxs.push_back(adjustedBX);
          tmp_all_L2_WGMin.push_back(WG_Min);
          tmp_all_L2_WGMax.push_back(WG_Max);
          if ((abs(tmp_delta_es) < abs(data_.LCT_match_GE2_residual)) and ((WG_Min - delta_wg) < data_.LCT_wiregroup) and (data_.LCT_wiregroup < (WG_Max + delta_wg))){
            data_.has_LCT_match2 = true;
            data_.LCT_match_GE2_residual = tmp_delta_es;
            if (debug) cout << "Filled residual with " << tmp_delta_es << endl;
            data_.LCT_match_GE2_pad = pad;
            data_.LCT_match_GE2_BX = adjustedBX;
            data_.LCT_match_GE2_padES = DigiToESMap[pad];
            data_.LCT_match_GE2_WGMin = WG_Min;
            data_.LCT_match_GE2_WGMax = WG_Max;

            data_.LCT_slope_with_GE2 = SlopeAmendMap[abs((DigiToESMap[pad] + align_corr_tmp) - data_.LCT_eighthstrip)]*pow(-1.0, ((DigiToESMap[pad] + align_corr_tmp) - data_.LCT_eighthstrip) < 0);

          }
        }
      }
      data_.LCT_all_GE2_pads_ES = tmp_all_L2_pads;
      data_.LCT_all_GE2_pads_ES_align_corr = tmp_all_L2_aligncorr;
      data_.LCT_all_GE2_bxs = tmp_all_L2_bxs;
      data_.LCT_all_GE2_WGMin = tmp_all_L2_WGMin;
      data_.LCT_all_GE2_WGMax = tmp_all_L2_WGMax;
    }
  }
}

void GEMCSCTriggerTester::fillMap(map<int, int>& thisMap, string filename){
  string delimiter = " ";
  string line;
  ifstream thisFile;
  thisFile.open(filename);
  if (thisFile.is_open()){
    while(getline(thisFile, line)){
      if (line.at(0) == '#') continue;
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      //if (!((key == 0) and (value == 0))){
      //  thisMap[key] = value;
      //}
      thisMap[key] = value;
    }
  }
}

void GEMCSCTriggerTester::beginJob(){
  //Lets make the SlopeExtrapolationLUTMaps
  cout << "Begin job!" << endl;


  fillMap(SlopeExtrapolationME11aEvenL1_Map, SlopeExtrapolationME11aEvenL1Name);
  fillMap(SlopeExtrapolationME11aEvenL2_Map, SlopeExtrapolationME11aEvenL2Name);

  fillMap(SlopeExtrapolationME11bEvenL1_Map, SlopeExtrapolationME11bEvenL1Name);
  fillMap(SlopeExtrapolationME11bEvenL2_Map, SlopeExtrapolationME11bEvenL2Name);

  fillMap(SlopeExtrapolationME11aOddL1_Map, SlopeExtrapolationME11aOddL1Name);
  fillMap(SlopeExtrapolationME11aOddL2_Map, SlopeExtrapolationME11aOddL2Name);

  fillMap(SlopeExtrapolationME11bOddL1_Map, SlopeExtrapolationME11bOddL1Name);
  fillMap(SlopeExtrapolationME11bOddL2_Map, SlopeExtrapolationME11bOddL2Name);

  fillMap(GEMPadDigiToCSCEigthStripME11aEven_Map, GEMPadDigiToCSCEightStripME11aEvenName);
  fillMap(GEMPadDigiToCSCEigthStripME11bEven_Map, GEMPadDigiToCSCEightStripME11bEvenName);

  fillMap(GEMPadDigiToCSCEigthStripME11aOdd_Map, GEMPadDigiToCSCEightStripME11aOddName);
  fillMap(GEMPadDigiToCSCEigthStripME11bOdd_Map, GEMPadDigiToCSCEightStripME11bOddName);

  fillMap(GEMPadDigiToCSCWGMinEvenL1_Map, GEMPadDigiToCSCWGMinEvenL1Name);
  fillMap(GEMPadDigiToCSCWGMinEvenL2_Map, GEMPadDigiToCSCWGMinEvenL2Name);

  fillMap(GEMPadDigiToCSCWGMinOddL1_Map, GEMPadDigiToCSCWGMinOddL1Name);
  fillMap(GEMPadDigiToCSCWGMinOddL2_Map, GEMPadDigiToCSCWGMinOddL2Name);

  fillMap(GEMPadDigiToCSCWGMaxEvenL1_Map, GEMPadDigiToCSCWGMaxEvenL1Name);
  fillMap(GEMPadDigiToCSCWGMaxEvenL2_Map, GEMPadDigiToCSCWGMaxEvenL2Name);

  fillMap(GEMPadDigiToCSCWGMaxOddL1_Map, GEMPadDigiToCSCWGMaxOddL1Name);
  fillMap(GEMPadDigiToCSCWGMaxOddL2_Map, GEMPadDigiToCSCWGMaxOddL2Name);

  fillMap(GEMAlignmentCorrectionsPositive_Map, GEMAlignmentCorrectionsPositiveName);
  fillMap(GEMAlignmentCorrectionsNegative_Map, GEMAlignmentCorrectionsNegativeName);

  fillMap(SlopeAmendmentME11aEvenL1_Map, SlopeAmendmentME11aEvenL1Name);
  fillMap(SlopeAmendmentME11aEvenL2_Map, SlopeAmendmentME11aEvenL2Name);

  fillMap(SlopeAmendmentME11bEvenL1_Map, SlopeAmendmentME11bEvenL1Name);
  fillMap(SlopeAmendmentME11bEvenL2_Map, SlopeAmendmentME11bEvenL2Name);

  fillMap(SlopeAmendmentME11aOddL1_Map, SlopeAmendmentME11aOddL1Name);
  fillMap(SlopeAmendmentME11aOddL2_Map, SlopeAmendmentME11aOddL2Name);

  fillMap(SlopeAmendmentME11bOddL1_Map, SlopeAmendmentME11bOddL1Name);
  fillMap(SlopeAmendmentME11bOddL2_Map, SlopeAmendmentME11bOddL2Name);

  cout << "Created all LUTs" << endl;
  cout << "Ended Begin Job, starting Event Loop" << endl;
  cout << "Counting all ME1a, starting at " << me1a_counter << endl;
}
void GEMCSCTriggerTester::endJob(){
  cout << "Found " << me1a_counter << " ME1a" << endl;
}

DEFINE_FWK_MODULE(GEMCSCTriggerTester);
