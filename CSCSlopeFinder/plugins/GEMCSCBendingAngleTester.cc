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
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
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
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "DataFormats/L1TMuon/interface/EMTFTrack.h"

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
using namespace l1t;

struct LCTL1MuonMatcher
{
  void init();
  TTree* book(TTree *t, const char *name);
  //============ Event Info ===============//
  unsigned long long evtNum; unsigned long long lumiBlock; int runNum;

  //============ LCT Info =================//
  bool has_LCT;
  int LCT_CSC_endcap;    int LCT_CSC_station;
  int LCT_CSC_ring;      int LCT_CSC_chamber;
  bool LCT_CSC_ME1a;     bool LCT_CSC_ME1b;
  int LCT_eighthstrip;   int LCT_slope;
  int LCT_wiregroup;     int LCT_quality;
  int LCT_bend;          int LCT_BX;
  int LCT_trknum;
  int CLCT_quality;

  //============ LCT Prop =================//
  bool has_LCT_prop1;    bool has_LCT_prop2;
  int LCT_GE1_region;    int LCT_GE2_region;
  int LCT_GE1_station;   int LCT_GE2_station;
  int LCT_GE1_ring;      int LCT_GE2_ring;
  int LCT_GE1_chamber;   int LCT_GE2_chamber;
  int LCT_GE1_layer;     int LCT_GE2_layer;
  int LCT_GE1_strip;     int LCT_GE2_strip;
  int LCT_GE1_wiregroup; int LCT_GE2_wiregroup;

  //============ LCT Match ================//
  bool has_LCT_match1; bool has_LCT_match2;
  float LCT_match_GE1_residual; float LCT_match_GE2_residual;
  int LCT_match_GE1_pad; int LCT_match_GE2_pad;
  int LCT_match_GE1_BX; int LCT_match_GE2_BX;
  int LCT_match_GE1_padES; int LCT_match_GE2_padES;
  int LCT_match_GE1_padES_aligned; int LCT_match_GE2_padES_aligned;
  int LCT_match_GE1_WGMin; int LCT_match_GE2_WGMin;
  int LCT_match_GE1_WGMax; int LCT_match_GE2_WGMax;
  int LCT_match_GE1_roll; int LCT_match_GE2_roll;

  vector<int> LCT_all_GE1_pads_ES; vector<int> LCT_all_GE2_pads_ES;
  vector<int> LCT_all_GE1_pads_ES_aligned; vector<int> LCT_all_GE2_pads_ES_aligned;
  vector<int> LCT_all_GE1_bxs; vector<int> LCT_all_GE2_bxs;
  vector<int> LCT_all_GE1_WGMin; vector<int> LCT_all_GE2_WGMin;
  vector<int> LCT_all_GE1_WGMax; vector<int> LCT_all_GE2_WGMax;
  int LCT_slope_with_GE1; int LCT_slope_with_GE2;
  int LCT_BendingAngle_GE1; int LCT_BendingAngle_GE2;
  int BendingAngleRealistic;

  //============ L1Muon Match =============//
  float l1muon_pt;
  float l1muon_eta;
  float l1muon_phi;
  int l1muon_charge;
  float LCT_eta_approx;
  float LCT_phi_approx;

  //============ EMTFTrack Match =============//
  bool has_emtf_track_match;
  float emtftrack_pt;
  float emtftrack_eta;
  float emtftrack_phi;
  float emtftrack_charge;
  float emtftrack_endcap;

  float emtftrack_mode;
  float emtftrack_numhits;

  bool has_emtftrack_l1muon_match;
  float l1muon_match_pt;
  float l1muon_match_eta;
  float l1muon_match_phi;
  float l1muon_match_charge;

  bool has_reco_l1_match;
  float reco_l1_match_dR;
  float reco_l1_match_pt;
  float reco_l1_match_charge;

  bool z_cand;
  float z_mass;
  bool z_opposite_charge;
  int lct_to_zmu_idx;
};

void LCTL1MuonMatcher::init()
{
  //=========== Event Info ==============//
  float value = 99999;
  evtNum = value; lumiBlock = value; runNum = value;

  //============ LCT Info =================//
  has_LCT = false;
  LCT_CSC_endcap = value; LCT_CSC_station = value;
  LCT_CSC_ring = value; LCT_CSC_chamber = value;
  LCT_CSC_ME1a = false; LCT_CSC_ME1b = false;
  LCT_eighthstrip = value; LCT_slope = value;
  LCT_wiregroup = value; LCT_quality = value;
  LCT_bend = value; LCT_BX = value;
  LCT_trknum = value;
  CLCT_quality = value;

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
  LCT_match_GE1_padES_aligned = value; LCT_match_GE2_padES_aligned = value;
  LCT_match_GE1_WGMin = value; LCT_match_GE2_WGMin = value;
  LCT_match_GE1_WGMax = value; LCT_match_GE2_WGMax = value;
  LCT_match_GE1_roll = value; LCT_match_GE2_roll = value;
  LCT_all_GE1_pads_ES.clear(); LCT_all_GE2_pads_ES.clear();
  LCT_all_GE1_pads_ES_aligned.clear(); LCT_all_GE2_pads_ES_aligned.clear();
  LCT_all_GE1_bxs.clear(); LCT_all_GE2_bxs.clear();
  LCT_all_GE1_WGMin.clear(); LCT_all_GE2_WGMin.clear();
  LCT_all_GE1_WGMax.clear(); LCT_all_GE2_WGMax.clear();
  LCT_slope_with_GE1 = value; LCT_slope_with_GE2 = value;
  LCT_BendingAngle_GE1 = value; LCT_BendingAngle_GE2 = value;
  BendingAngleRealistic = value;

  //============ L1Muon Match =============//
  l1muon_pt = value;
  l1muon_eta = value;
  l1muon_phi = value;
  l1muon_charge = value;
  LCT_eta_approx = value;
  LCT_phi_approx = value;


  //============ EMTFTrack Match =============//
  has_emtf_track_match = 0;
  emtftrack_pt = value;
  emtftrack_eta = value;
  emtftrack_phi = value;
  emtftrack_charge = value;
  emtftrack_endcap = value;

  emtftrack_mode = value;
  emtftrack_numhits = value;

  has_emtftrack_l1muon_match = 0;
  l1muon_match_pt = value;
  l1muon_match_eta = value;
  l1muon_match_phi = value;
  l1muon_match_charge = value;

  has_reco_l1_match = false;
  reco_l1_match_dR = value;
  reco_l1_match_pt = value;
  reco_l1_match_charge = value;

  z_cand = false;
  z_mass = value;
  z_opposite_charge = false;
  lct_to_zmu_idx = value;
}

TTree* LCTL1MuonMatcher::book(TTree *t, const char *name){
  edm::Service< TFileService > fs;
  //t = fs->make<TTree>("LCTL1MuonMatcher", "LCTL1MuonMatcher");
  t = fs->make<TTree>(Form(name), Form(name));

  //=========== Event Info ============//
  t->Branch("evtNum", &evtNum); t->Branch("lumiBlock", &lumiBlock); t->Branch("runNum", &runNum);

  //============ LCT Info =================//
  t->Branch("has_LCT", &has_LCT);
  t->Branch("LCT_CSC_endcap", &LCT_CSC_endcap); t->Branch("LCT_CSC_station", &LCT_CSC_station);
  t->Branch("LCT_CSC_ring", &LCT_CSC_ring); t->Branch("LCT_CSC_chamber", &LCT_CSC_chamber);
  t->Branch("LCT_CSC_ME1a", &LCT_CSC_ME1a); t->Branch("LCT_CSC_ME1b", &LCT_CSC_ME1b);
  t->Branch("LCT_eighthstrip", &LCT_eighthstrip); t->Branch("LCT_slope", &LCT_slope);
  t->Branch("LCT_wiregroup", &LCT_wiregroup); t->Branch("LCT_quality", &LCT_quality);
  t->Branch("LCT_bend", &LCT_bend); t->Branch("LCT_BX", &LCT_BX);
  t->Branch("LCT_trknum", &LCT_trknum);
  t->Branch("CLCT_quality", &CLCT_quality);

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
  t->Branch("LCT_match_GE1_padES_aligned", &LCT_match_GE1_padES_aligned); t->Branch("LCT_match_GE2_padES_aligned", &LCT_match_GE2_padES_aligned);
  t->Branch("LCT_all_GE1_pads_ES_align_corr", &LCT_all_GE1_pads_ES_aligned); t->Branch("LCT_all_GE2_pads_ES_align_corr", &LCT_all_GE2_pads_ES_aligned);
  t->Branch("LCT_match_GE1_WGMin", &LCT_match_GE1_WGMin); t->Branch("LCT_match_GE2_WGMin", &LCT_match_GE2_WGMin);
  t->Branch("LCT_match_GE1_WGMax", &LCT_match_GE1_WGMax); t->Branch("LCT_match_GE2_WGMax", &LCT_match_GE2_WGMax);
  t->Branch("LCT_match_GE1_roll", &LCT_match_GE2_roll); t->Branch("LCT_match_GE2_roll", &LCT_match_GE2_roll);
  t->Branch("LCT_all_GE1_pads_ES", &LCT_all_GE1_pads_ES); t->Branch("LCT_all_GE2_pads_ES", &LCT_all_GE2_pads_ES);
  t->Branch("LCT_all_GE1_bxs", &LCT_all_GE1_bxs); t->Branch("LCT_all_GE2_bxs", &LCT_all_GE2_bxs);
  t->Branch("LCT_all_GE1_WGMin", &LCT_all_GE1_WGMin); t->Branch("LCT_all_GE2_WGMin", &LCT_all_GE2_WGMin);
  t->Branch("LCT_all_GE1_WGMax", &LCT_all_GE1_WGMax); t->Branch("LCT_all_GE2_WGMax", &LCT_all_GE2_WGMax);
  t->Branch("LCT_slope_with_GE1", &LCT_slope_with_GE1); t->Branch("LCT_slope_with_GE2", &LCT_slope_with_GE2);
  t->Branch("LCT_BendingAngle_GE1", &LCT_BendingAngle_GE1); t->Branch("LCT_BendingAngle_GE2", &LCT_BendingAngle_GE2);
  t->Branch("BendingAngleRealistic", &BendingAngleRealistic);

  //============ L1Muon Match =============//
  t->Branch("l1muon_pt", &l1muon_pt);
  t->Branch("l1muon_eta", &l1muon_eta);
  t->Branch("l1muon_phi", &l1muon_phi);
  t->Branch("l1muon_charge", &l1muon_charge);
  t->Branch("LCT_eta_approx", &LCT_eta_approx);
  t->Branch("LCT_phi_approx", &LCT_phi_approx);

  //============ L1Muon Match =============//
    t->Branch("has_emtf_track_match", &has_emtf_track_match);
    t->Branch("emtftrack_pt", &emtftrack_pt);
    t->Branch("emtftrack_eta", &emtftrack_eta);
    t->Branch("emtftrack_phi", &emtftrack_phi);
    t->Branch("emtftrack_charge", &emtftrack_charge);
    t->Branch("emtftrack_endcap", &emtftrack_endcap);

    t->Branch("emtftrack_mode", &emtftrack_mode);
    t->Branch("emtftrack_numhits", &emtftrack_numhits);

    t->Branch("has_emtftrack_l1muon_match", &has_emtftrack_l1muon_match);
    t->Branch("l1muon_match_pt", &l1muon_match_pt);
    t->Branch("l1muon_match_eta", &l1muon_match_eta);
    t->Branch("l1muon_match_phi", &l1muon_match_phi);
    t->Branch("l1muon_match_charge", &l1muon_match_charge);

    t->Branch("has_reco_l1_match", &has_reco_l1_match);
    t->Branch("reco_l1_match_dR", &reco_l1_match_dR);
    t->Branch("reco_l1_match_pt", &reco_l1_match_pt);
    t->Branch("reco_l1_match_charge", &reco_l1_match_charge);

    t->Branch("z_cand", &z_cand);
    t->Branch("z_mass", &z_mass);
    t->Branch("z_opposite_charge", &z_opposite_charge);
    t->Branch("lct_to_zmu_idx", &lct_to_zmu_idx);

  return t;
}

class GEMCSCBendingAngleTester : public edm::one::EDAnalyzer<> {
public:
  explicit GEMCSCBendingAngleTester(const edm::ParameterSet&);
  ~GEMCSCBendingAngleTester(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;

  void fillMap(map<int, int>& thisMap, string filename);

  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;
  edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
  edm::EDGetTokenT<GEMPadDigiClusterCollection> gemdigi_token;
  edm::Handle<GEMPadDigiClusterCollection> gemPadDigis;
  edm::EDGetTokenT<MuonBxCollection> l1_muon_token;
  edm::Handle<MuonBxCollection> l1_muons;
  edm::EDGetTokenT<RegionalMuonCandBxCollection> emtf_muon_token;
  edm::Handle<RegionalMuonCandBxCollection> emtf_muons;
  edm::EDGetTokenT<EMTFTrackCollection> emtf_track_token;
  edm::Handle<EMTFTrackCollection> emtf_tracks;
  edm::EDGetTokenT<edm::View<reco::Muon> > muon_token;
  edm::Handle<View<reco::Muon> > muons;
  edm::EDGetTokenT<reco::VertexCollection> vertexCollection_token;
  edm::Handle<reco::VertexCollection> vertexCollection;



  edm::Service<TFileService> fs;
  MuonServiceProxy* theService_;

  bool use_alignment;
  bool debug;

  LCTL1MuonMatcher data_;
  TTree* tree;

  LCTL1MuonMatcher dataHighestEMTF_;
  TTree* treeHighestEMTF;

  LCTL1MuonMatcher dataOnlyRecos_;
  TTree* treeOnlyRecos;

  edm::ESHandle<GEMGeometry> GEMGeometry_;
  edm::ESHandle<CSCGeometry> CSCGeometry_;

  const edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeomToken_;
  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeomToken_;

  bool isMC;

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

  string GEMAlignmentExtraCorrPositiveL1Name;
  map<int, int> GEMAlignmentExtraCorrPositiveL1_Map;
  string GEMAlignmentExtraCorrPositiveL2Name;
  map<int, int> GEMAlignmentExtraCorrPositiveL2_Map;

  string GEMAlignmentExtraCorrNegativeL1Name;
  map<int, int> GEMAlignmentExtraCorrNegativeL1_Map;
  string GEMAlignmentExtraCorrNegativeL2Name;
  map<int, int> GEMAlignmentExtraCorrNegativeL2_Map;


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
  int delta_wg = 7;

  int me1a_counter = 0;

};


GEMCSCBendingAngleTester::GEMCSCBendingAngleTester(const edm::ParameterSet& iConfig)
  : gemGeomToken_(esConsumes()),
    cscGeomToken_(esConsumes())
{
  cout << "Begin CSC_LCT_Segment_Matcher" << endl;
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService_ = new MuonServiceProxy(serviceParameters, consumesCollector());
  co_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("corrlctDigiTag"));
  gemdigi_token = consumes<GEMPadDigiClusterCollection>(iConfig.getParameter<edm::InputTag>("gemPadDigiCluster"));
  l1_muon_token = consumes<MuonBxCollection>(iConfig.getParameter<edm::InputTag>("l1_muon_token"));
  emtf_muon_token = consumes<RegionalMuonCandBxCollection>(iConfig.getParameter<edm::InputTag>("emtf_muon_token"));
  emtf_track_token = consumes<EMTFTrackCollection>(iConfig.getParameter<edm::InputTag>("emtf_track_token"));
  muon_token = consumes<View<reco::Muon> >(iConfig.getParameter<InputTag>("muon_token"));
  vertexCollection_token = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection_token"));

  luts_folder = iConfig.getParameter<string>("luts_folder");

  use_alignment = iConfig.getParameter<bool>("alignment");
  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree, "AllLCTs");
  treeHighestEMTF = dataHighestEMTF_.book(treeHighestEMTF, "HighestEMTFpT");
  treeOnlyRecos = dataOnlyRecos_.book(treeOnlyRecos, "OnlyRecos");


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

  //Maps for additional GEM Alignment Corrects (After Offline)
  //Split by +/- endcap
  GEMAlignmentExtraCorrPositiveL1Name = luts_folder + "/GEMCSC/AlignmentCorrection/additional_corr_positive_endcap_L1.txt";
  GEMAlignmentExtraCorrPositiveL2Name = luts_folder + "/GEMCSC/AlignmentCorrection/additional_corr_positive_endcap_L2.txt";
  GEMAlignmentExtraCorrNegativeL1Name = luts_folder + "/GEMCSC/AlignmentCorrection/additional_corr_negative_endcap_L1.txt";
  GEMAlignmentExtraCorrNegativeL2Name = luts_folder + "/GEMCSC/AlignmentCorrection/additional_corr_negative_endcap_L2.txt";

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
GEMCSCBendingAngleTester::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  GEMGeometry_ = &iSetup.getData(gemGeomToken_);
  CSCGeometry_ = &iSetup.getData(cscGeomToken_);
  theService_->update(iSetup);

  isMC = false;
  // loading collections (from RAW/Reco)
  if (! iEvent.eventAuxiliary().isRealData()) isMC = true;
  iEvent.getByToken(co_token, correlatedlcts); // CSC trg obj
  iEvent.getByToken(gemdigi_token, gemPadDigis); // GEM trg obj
  iEvent.getByToken(l1_muon_token, l1_muons);
  iEvent.getByToken(emtf_muon_token, emtf_muons);
  iEvent.getByToken(emtf_track_token, emtf_tracks);
  iEvent.getByToken(muon_token, muons); // reco muon obj
  iEvent.getByToken(vertexCollection_token, vertexCollection); // vtx info for tight ID



  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;

  if (debug) cout << "New Event" << endl;

  std::cout << "There are " << l1_muons->size() << " L1 Muons" << std::endl;
  std::cout << "There are " << emtf_muons->size() << " EMTF Muons" << std::endl;
  std::cout << "There are " << emtf_tracks->size() << " EMTF Tracks" << std::endl;
  std::cout << "There are " << muons->size() << " RECO Muons" << std::endl;

  //Init highest EMTF data struct
  dataHighestEMTF_.init();
  //Set the EMTFTrack pT to -1 since this will be our discriminator
  dataHighestEMTF_.emtftrack_pt = -1;

  int zmu_cand_idx1 = 99;
  int zmu_cand_idx2 = 99;
  float zmu_cand_mass = 1000.0;
  float zmass = 91.1876;
  bool z_opposite_charge = false;
  TLorentzVector vec1;
  TLorentzVector vec2;
  TLorentzVector comb;

  // label Z candidate, and for signal we want tight ID muons that actually come from Z candidate
  //

  if (vertexCollection->size() > 0){
    auto primary_vertex = vertexCollection->at(0);
    if (muons->size() != 0){
      for (size_t i = 0; i < muons->size(); ++i){
        edm::RefToBase<reco::Muon> muRef = muons->refAt(i);
        const reco::Muon* mu = muRef.get();
        if (not (mu->isGlobalMuon())) continue;
        if (not (muon::isTightMuon(*mu, primary_vertex))) continue;
        for (size_t j = i+1; j < muons->size(); ++j){
          edm::RefToBase<reco::Muon> muRef2 = muons->refAt(j);
          const reco::Muon* mu2 = muRef2.get();
          if (not (mu2->isGlobalMuon())) continue;
          std::cout << "Global pair " << i << " " << j << std::endl;
          vec1.SetPtEtaPhiE(mu->pt(), mu->eta(), mu->phi(), mu->energy());
          vec2.SetPtEtaPhiE(mu2->pt(), mu2->eta(), mu2->phi(), mu2->energy());
          comb = vec1 + vec2;
          std::cout << "Combined mass is " << comb.M() << " from m1 " << vec1.M() << " and m2 " << vec2.M() << std::endl;
          std::cout << "Their charges are " << mu->charge() << " " << mu2->charge() << std::endl;

          if (abs(comb.M() - zmass) < abs(zmu_cand_mass - zmass)){
            zmu_cand_idx1 = i;
            zmu_cand_idx2 = j;
            zmu_cand_mass = comb.M();
            if (mu->charge() != mu2->charge()){
              z_opposite_charge = true;
            }
            if (z_opposite_charge) std::cout << "Nice" << std::endl;
          }
        }
      }
    }
  }

  for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end(); j++){
    CSCDetId LCTDetId = (*j).first;
    if (!(LCTDetId.station() == 1 and ((LCTDetId.ring() == 1) or (LCTDetId.ring() == 4)))) continue;
    //data_.LCT_CSC_ME1a = LCTDetId.isME1a(); //These functions are broken! We must make our own
    //data_.LCT_CSC_ME1b = LCTDetId.isME1b(); //Will do it at the LCT loop
    if (debug) cout << "Found a LCTDet to SegmentDet Match on Det " << LCTDetId << endl;
    //Loop over LCTs
    // save LCT objects
    std::vector<CSCCorrelatedLCTDigi>::const_iterator CSCCorrLCT = (*j).second.first;
    std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
    for (; CSCCorrLCT != last; ++CSCCorrLCT){
      data_.init();
      dataOnlyRecos_.init();
      data_.evtNum = iEvent.eventAuxiliary().event();
      data_.lumiBlock = iEvent.eventAuxiliary().luminosityBlock();
      data_.runNum = iEvent.run();

      data_.LCT_CSC_endcap = LCTDetId.endcap();
      data_.LCT_CSC_station = LCTDetId.station();
      data_.LCT_CSC_ring = LCTDetId.ring();
      data_.LCT_CSC_chamber = LCTDetId.chamber();
      data_.has_LCT = true;
      data_.LCT_eighthstrip = (CSCCorrLCT->getStrip())*4 + (CSCCorrLCT->getQuartStripBit())*2 + (CSCCorrLCT->getEighthStripBit());
      data_.LCT_slope = CSCCorrLCT->getSlope();
      data_.LCT_wiregroup = CSCCorrLCT->getKeyWG();
      data_.LCT_quality = CSCCorrLCT->getQuality();
      data_.LCT_bend = CSCCorrLCT->getBend();
      data_.LCT_BX = CSCCorrLCT->getBX();
      data_.LCT_CSC_ME1a = (data_.LCT_eighthstrip > 512) ? 1 : 0;
      data_.LCT_CSC_ME1b = (data_.LCT_eighthstrip < 512) ? 1 : 0;
      data_.LCT_trknum = CSCCorrLCT->getTrknmb();
      data_.CLCT_quality = (CSCCorrLCT->getCLCT()).getQuality();

      if (debug){
        std::cout << "Dump the ME1/1 LCT" << std::endl;
        std::cout << "Quality " << CSCCorrLCT->getQuality() << std::endl;
        std::cout << "Pattern " << CSCCorrLCT->getPattern() << std::endl;
        std::cout << "Bend " << CSCCorrLCT->getBend() << std::endl;
        std::cout << "Slope " << CSCCorrLCT->getSlope() << std::endl;
      }

      //Find LCT Propagation onto GEM
      // LUT -->
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
      int slope_propagationL1 = CSCLCTPropL1Map[data_.LCT_slope];
      int slope_propagationL2 = CSCLCTPropL2Map[data_.LCT_slope];
      if (debug) cout << "LCT Eighth Strip Prop Adjust L1/L2 = " << slope_propagationL1 << "/" << slope_propagationL2 << endl;
      // in eight strip units position on GEM
      int LCTToGEML1EighthStrip = data_.LCT_eighthstrip - slope_propagationL1*((data_.LCT_bend*2)-1); //For some reason, the slope is 0-Positive, 1-Negative
      int LCTToGEML2EighthStrip = data_.LCT_eighthstrip - slope_propagationL2*((data_.LCT_bend*2)-1); //This is stupid ):
      if (debug) cout << "LCT Prop from " << data_.LCT_eighthstrip << " to L1 " << LCTToGEML1EighthStrip << " and L2 " << LCTToGEML2EighthStrip << endl;
      // remember that the window is different for even and odds chamber
      int window_es = (data_.LCT_CSC_chamber%2 == 0) ? even_delta_es : odd_delta_es;
      int strip_start = 999;
      int strip_end = 999;
      // A and B have different strip number
      if (data_.LCT_CSC_ME1a){
        strip_start = 512;
        strip_end = 895;
      }
      if (data_.LCT_CSC_ME1b){
        strip_start = 0;
        strip_end = 511;
      }
      //
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

      //Match LCT Propagated 8th strips to GEM Pad Digi Clusters
      //Even matching window is 20 CSC 8th strips
      //Odd matching window is 40 CSC 8th strips --> the es is the phi coordinate (local x)
      //WG matching window is 7 WG --> wire group is eta on CSC (local y)
      for (GEMPadDigiClusterCollection::DigiRangeIterator j = gemPadDigis->begin(); j != gemPadDigis->end(); j++){
        GEMDetId GEMPadDigiDetID = (*j).first;
        if ((data_.LCT_GE1_region ==  GEMPadDigiDetID.region()) and (data_.LCT_GE1_station == GEMPadDigiDetID.station()) and (data_.LCT_GE1_ring == GEMPadDigiDetID.ring()) and (data_.LCT_GE1_chamber == GEMPadDigiDetID.chamber()) and (data_.LCT_GE1_layer == GEMPadDigiDetID.layer()) and data_.has_LCT_prop1){
          if (debug) cout << "Found a GEM Digi on correct chamber L1 " << GEMPadDigiDetID << endl;
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
          map<int, int> ExtraCorrMap = (GEMPadDigiDetID.region() == 1) ? GEMAlignmentExtraCorrPositiveL1_Map : GEMAlignmentExtraCorrNegativeL1_Map;
          int AlignCorrKey = GEMPadDigiDetID.chamber() * 10 + GEMPadDigiDetID.roll(); // roll is eta in GEM


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
          vector<int> tmp_all_L1_pads_aligned;
          vector<int> tmp_all_L1_bxs;
          vector<int> tmp_all_L1_WGMin;
          vector<int> tmp_all_L1_WGMax;

          //8Oct2024 New Idea from Giovanni to fix weird +/- Muon Bending Distributions
          //Instead of matching to invididual pads, try to match to center of clusters
          for (; digiItr != last; ++digiItr) {
            int digiBX = digiItr->bx();
            int digiALCTMatch = digiItr->alctMatchTime();
            int delayGEMinOTMB = 1;
            int tmbL1aWindowSize = 7;
            int CSCConstants_LCT_CENTRAL_BX = 8;
            int adjustedBX = digiBX + CSCConstants_LCT_CENTRAL_BX - int(tmbL1aWindowSize/2.0) - digiALCTMatch + delayGEMinOTMB;

            int centralClusterPad = int((digiItr->pads().front())+(digiItr->pads().size()/2.0));

            //int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] : AlignCorrMap[AlignCorrKey] * (-1.0); //After P5 tests in 2024, we noticed the negative endcap LUT sign should be flipped, this line is no longer needed
            int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] : AlignCorrMap[AlignCorrKey];
            int extra_corr_tmp = (GEMPadDigiDetID.region() == 1) ? ExtraCorrMap[AlignCorrKey] : ExtraCorrMap[AlignCorrKey];
            if (use_alignment == false) align_corr_tmp = 0;
            int tmp_delta_es = data_.LCT_GE1_strip - (DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp); // residual
            int WG_Max = WGMaxMap[GEMPadDigiDetID.roll()-1];
            int WG_Min = WGMinMap[GEMPadDigiDetID.roll()-1];
            tmp_all_L1_pads.push_back(DigiToESMap[centralClusterPad]);
            tmp_all_L1_pads_aligned.push_back(DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp);
            tmp_all_L1_bxs.push_back(adjustedBX);
            tmp_all_L1_WGMin.push_back(WG_Min);
            tmp_all_L1_WGMax.push_back(WG_Max);
            if ((abs(tmp_delta_es) < abs(data_.LCT_match_GE1_residual)) and ((WG_Min - delta_wg) < data_.LCT_wiregroup) and (data_.LCT_wiregroup < (WG_Max + delta_wg))){
              data_.has_LCT_match1 = true;
              data_.LCT_match_GE1_residual = tmp_delta_es;
              if (debug) cout << "Filled residual with " << tmp_delta_es << endl;
              data_.LCT_match_GE1_pad = centralClusterPad;
              data_.LCT_match_GE1_BX = adjustedBX;
              data_.LCT_match_GE1_padES = DigiToESMap[centralClusterPad];
              data_.LCT_match_GE1_padES_aligned = DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp;
              data_.LCT_match_GE1_WGMin = WG_Min;
              data_.LCT_match_GE1_WGMax = WG_Max;
              data_.LCT_match_GE1_roll = GEMPadDigiDetID.roll();

              data_.LCT_slope_with_GE1 = SlopeAmendMap[abs((DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp) - data_.LCT_eighthstrip)]*pow(-1.0, ((DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp) - data_.LCT_eighthstrip) < 0);
              data_.LCT_BendingAngle_GE1 = data_.LCT_eighthstrip - (DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp);
            }
          }
          data_.LCT_all_GE1_pads_ES = tmp_all_L1_pads;
          data_.LCT_all_GE1_pads_ES_aligned = tmp_all_L1_pads_aligned;
          data_.LCT_all_GE1_bxs = tmp_all_L1_bxs;
          data_.LCT_all_GE1_WGMin = tmp_all_L1_WGMin;
          data_.LCT_all_GE1_WGMax = tmp_all_L1_WGMax;
        }

        /*
          //Old method of individual pads, now dead ):
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
              tmp_all_L1_pads_aligned.push_back(DigiToESMap[pad] + align_corr_tmp);
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
                data_.LCT_match_GE1_padES_aligned = DigiToESMap[pad] + align_corr_tmp;
                data_.LCT_match_GE1_WGMin = WG_Min;
                data_.LCT_match_GE1_WGMax = WG_Max;

                data_.LCT_slope_with_GE1 = SlopeAmendMap[abs((DigiToESMap[pad] + align_corr_tmp) - data_.LCT_eighthstrip)]*pow(-1.0, ((DigiToESMap[pad] + align_corr_tmp) - data_.LCT_eighthstrip) < 0);
                data_.LCT_BendingAngle_GE1 = data_.LCT_eighthstrip - (DigiToESMap[pad] + align_corr_tmp);

              }
            }
          }
          data_.LCT_all_GE1_pads_ES = tmp_all_L1_pads;
          data_.LCT_all_GE1_pads_ES_aligned = tmp_all_L1_pads_aligned;
          data_.LCT_all_GE1_bxs = tmp_all_L1_bxs;
          data_.LCT_all_GE1_WGMin = tmp_all_L1_WGMin;
          data_.LCT_all_GE1_WGMax = tmp_all_L1_WGMax;
        }
        */










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
          map<int, int> ExtraCorrMap = (GEMPadDigiDetID.region() == 1) ? GEMAlignmentExtraCorrPositiveL2_Map : GEMAlignmentExtraCorrNegativeL2_Map;
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
          vector<int> tmp_all_L2_aligned;
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

            int centralClusterPad = int((digiItr->pads().front())+(digiItr->pads().size()/2.0));


            //int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] : AlignCorrMap[AlignCorrKey] * (-1.0); //After P5 tests in 2024, we noticed the negative endcap LUT sign should be flipped, this line is no longer needed
            int align_corr_tmp = (GEMPadDigiDetID.region() == 1) ? AlignCorrMap[AlignCorrKey] : AlignCorrMap[AlignCorrKey];
            int extra_corr_tmp = (GEMPadDigiDetID.region() == 1) ? ExtraCorrMap[AlignCorrKey] : ExtraCorrMap[AlignCorrKey];
            if (use_alignment == false) align_corr_tmp = 0;
            int tmp_delta_es = data_.LCT_GE2_strip - (DigiToESMap[centralClusterPad] + align_corr_tmp);
            int WG_Max = WGMaxMap[GEMPadDigiDetID.roll()-1];
            int WG_Min = WGMinMap[GEMPadDigiDetID.roll()-1];
            tmp_all_L2_pads.push_back(DigiToESMap[centralClusterPad]);
            tmp_all_L2_aligned.push_back(DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp);
            tmp_all_L2_bxs.push_back(adjustedBX);
            tmp_all_L2_WGMin.push_back(WG_Min);
            tmp_all_L2_WGMax.push_back(WG_Max);
            if ((abs(tmp_delta_es) < abs(data_.LCT_match_GE2_residual)) and ((WG_Min - delta_wg) < data_.LCT_wiregroup) and (data_.LCT_wiregroup < (WG_Max + delta_wg))){
              data_.has_LCT_match2 = true;
              data_.LCT_match_GE2_residual = tmp_delta_es;
              if (debug) cout << "Filled residual with " << tmp_delta_es << endl;
              data_.LCT_match_GE2_pad = centralClusterPad;
              data_.LCT_match_GE2_BX = adjustedBX;
              data_.LCT_match_GE2_padES = DigiToESMap[centralClusterPad];
              data_.LCT_match_GE2_padES_aligned = DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp;
              data_.LCT_match_GE2_WGMin = WG_Min;
              data_.LCT_match_GE2_WGMax = WG_Max;
              data_.LCT_match_GE2_roll = GEMPadDigiDetID.roll();

              data_.LCT_slope_with_GE2 = SlopeAmendMap[abs((DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp) - data_.LCT_eighthstrip)]*pow(-1.0, ((DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp) - data_.LCT_eighthstrip) < 0);
              data_.LCT_BendingAngle_GE2 = data_.LCT_eighthstrip - (DigiToESMap[centralClusterPad] + align_corr_tmp + extra_corr_tmp);

            }
          }
          data_.LCT_all_GE2_pads_ES = tmp_all_L2_pads;
          data_.LCT_all_GE2_pads_ES_aligned = tmp_all_L2_aligned;
          data_.LCT_all_GE2_bxs = tmp_all_L2_bxs;
          data_.LCT_all_GE2_WGMin = tmp_all_L2_WGMin;
          data_.LCT_all_GE2_WGMax = tmp_all_L2_WGMax;
        }
      }
      //Real FW will take the OR of bending angle with priority to L1, lets mimic that

      data_.BendingAngleRealistic = data_.has_LCT_match1 ? data_.LCT_BendingAngle_GE1 : data_.LCT_BendingAngle_GE2;

      auto CSCDetIdL4 = CSCDetId(LCTDetId.endcap(), LCTDetId.station(), LCTDetId.ring(), LCTDetId.chamber(), 4); // layer 4 is the most important to get the position
      const CSCLayer* ME11_layer = CSCGeometry_->layer(CSCDetIdL4);
      const CSCLayerGeometry* ME11_layer_geo = ME11_layer->geometry();
      LocalPoint CSCCorrLCT_LP = ME11_layer_geo->stripWireGroupIntersection(CSCCorrLCT->getStrip(), CSCCorrLCT->getKeyWG());
      GlobalPoint CSCCorrLCT_GP = ME11_layer->toGlobal(CSCCorrLCT_LP);
      int ge11_muons = 0;
      if ((l1_muons.isValid())){
        for (int ibx = l1_muons->getFirstBX(); ibx <= l1_muons->getLastBX(); ++ibx){
          for (auto it = l1_muons->begin(ibx); it != l1_muons->end(ibx); it++){
            if (it->et() > 0){
              if (abs((it->eta()) > 1.5) && (abs(it->eta()) < 2.2)){ge11_muons++;}
              if ((abs(it->eta() - CSCCorrLCT_GP.eta()) < 0.1) && ((abs(it->phi() - CSCCorrLCT_GP.phi())) < 0.1)){ // *
                data_.l1muon_pt = it->pt();
                data_.l1muon_eta = it->eta();
                data_.l1muon_phi = it->phi();
                data_.l1muon_charge = it->charge();

                data_.LCT_eta_approx = CSCCorrLCT_GP.eta();
                data_.LCT_phi_approx = CSCCorrLCT_GP.phi();
              }
            }
          }
        }
      }
      if (debug) std::cout << "Loop over all EMTF Tracks" << std::endl;
      if ((emtf_tracks.isValid())){
        std::cout << "Event has " << emtf_tracks->size() << " emtf tracks" << std::endl;
        for (unsigned int track_id = 0; track_id < emtf_tracks->size(); ++track_id) {
          const auto& track = emtf_tracks->at(track_id);
          if (debug){
            std::cout << "Looping emtfs tracks!" << std::endl;
            std::cout << "Track Endcap " << track.Endcap() << std::endl;
            std::cout << "Track Sector " << track.Sector() << std::endl;
            std::cout << "Track Sector_idx " << track.Sector_idx() << std::endl;
            std::cout << "Track Mode " << track.Mode() << std::endl;
            std::cout << "Track Mode_CSC " << track.Mode_CSC() << std::endl;
            std::cout << "Track Rank " << track.Rank() << std::endl;
            std::cout << "Track Winner " << track.Winner() << std::endl;
            std::cout << "Track Charge " << track.Charge() << std::endl;
            std::cout << "Track BX " << track.BX() << std::endl;
            std::cout << "Track First_BX " << track.First_BX() << std::endl;
            std::cout << "Track Second_BX " << track.Second_BX() << std::endl;
            std::cout << "Track Pt " << track.Pt() << std::endl;
            std::cout << "Track Eta " << track.Eta() << std::endl;
            std::cout << "Track Phi_glob " << track.Phi_glob() << std::endl;
            std::cout << "Track Track_num " << track.Track_num() << std::endl;
          }
          if (debug) std::cout << "Loop over track.hits" << std::endl;
          EMTFHitCollection emtf_hits = track.Hits();
          // mode --> related to what station is, a 4 bit number
          if ((track.Mode_CSC() & 8) == 8){
            bool has_me11 = false;
            int n_me11_lcts = 0;
            for (unsigned int hit_id = 0; hit_id < emtf_hits.size(); ++hit_id) {
              const auto& hit = emtf_hits.at(hit_id);
              if (hit.Is_CSC() != 1) continue;
              if (hit.Station() != 1) continue;
              if (!(hit.Ring() == 1 || hit.Ring() == 4)) continue;
              has_me11 = true;
              n_me11_lcts++;
              if ((debug) or (true)){
                std::cout << "CSC Hit : Matching LCT Target" << std::endl;
                std::cout << "Subsystem " << hit.Subsystem() << std::endl;
                std::cout << "Endcap " << hit.Endcap() << std::endl;
                std::cout << "Station " << hit.Station() << std::endl;
                std::cout << "Ring " << hit.Ring() << std::endl;
                std::cout << "Quality " << hit.Quality() << ":" << CSCCorrLCT->getQuality() << std::endl;
                std::cout << "Pattern " << hit.Pattern() << ":" << CSCCorrLCT->getPattern() << std::endl;
                std::cout << "Bend " << hit.Bend() << ":" << CSCCorrLCT->getBend() << std::endl;
                std::cout << "Slope " << hit.Slope() << ":" << CSCCorrLCT->getSlope() << std::endl;
                std::cout << "Strip " << hit.Strip() << ":" << CSCCorrLCT->getStrip() << std::endl;
                std::cout << "WG " << hit.Wire() << ":" << CSCCorrLCT->getKeyWG() << std::endl;
                std::cout << "Chamber " << hit.Chamber() << ":" << LCTDetId.chamber() << std::endl;
              }
              if ((hit.Chamber() == LCTDetId.chamber()) && (hit.Endcap() == LCTDetId.zendcap())){
                ///if (hit.Strip() == CSCCorrLCT->getStrip()){
                if ((hit.Strip() == CSCCorrLCT->getStrip()) and (hit.Wire() == CSCCorrLCT->getKeyWG())){
                  data_.has_emtf_track_match = 1;
                  data_.emtftrack_pt = track.Pt();
                  data_.emtftrack_eta = track.Eta();
                  data_.emtftrack_phi = track.Phi_glob();
                  data_.emtftrack_charge = track.Charge();
                  data_.emtftrack_endcap = track.Endcap();

                  data_.emtftrack_mode = track.Mode();
                  data_.emtftrack_numhits = track.NumHits();


                  if ((l1_muons.isValid())){
                    for (int ibx = l1_muons->getFirstBX(); ibx <= l1_muons->getLastBX(); ++ibx){
                      for (auto it = l1_muons->begin(ibx); it != l1_muons->end(ibx); it++){
                        if (it->et() > 0){
                          if ((it->pt() == track.Pt()) && (it->eta() == track.Eta()) && (abs(it->phi() - track.Phi_glob()*3.14159265/180) < 0.1)){
                            data_.has_emtftrack_l1muon_match = 1;
                            data_.l1muon_match_pt = it->pt();
                            data_.l1muon_match_eta = it->eta();
                            data_.l1muon_match_phi = it->phi();
                            data_.l1muon_match_charge = it->charge();
                          }
                        }
                      }
                    }
                  }
                  if (data_.has_emtftrack_l1muon_match == 0){
                    if (debug) std::cout << "This emtf track could not find an L1 Match!!!" << std::endl;
                  }
                }
              }
            }
            if (has_me11 == false){
              if (debug) std::cout << "A track with mode inc ME11 doesn't have a 11 hit?????" << std::endl;
            }
            std::cout << "This track in evt " << data_.evtNum << " had " << n_me11_lcts << " ME11 LCTS" << std::endl;
          }
        }
      }
      if (data_.has_emtf_track_match == 1){
        //Match emtf track to a reco muon track
        float tmp_dR = 999.0;
        if (muons->size() != 0){
          for (size_t i = 0; i < muons->size(); ++i){
            edm::RefToBase<reco::Muon> muRef = muons->refAt(i);
            const reco::Muon* mu = muRef.get();
            if (mu->isGlobalMuon()){
              if (debug | true){
                std::cout << "Muon pt/eta/phi = " << mu->pt() << "/" << mu->eta() << "/" << mu->phi() << std::endl;
                std::cout << "L1 pt/eta/phi   = " << data_.l1muon_match_pt << "/" << data_.l1muon_match_eta << "/" << data_.l1muon_match_phi << std::endl;
              }
              float new_dR = abs(pow(pow(mu->eta() - data_.l1muon_match_eta, 2) + pow(mu->phi() - data_.l1muon_match_phi, 2), 0.5));
              if (new_dR < tmp_dR){
                data_.has_reco_l1_match = true;
                data_.reco_l1_match_dR = new_dR;
                data_.reco_l1_match_pt = mu->pt();
                data_.reco_l1_match_charge = mu->charge();
                tmp_dR = new_dR;
                data_.z_cand = ((int(i) == zmu_cand_idx1) or (int(i) == zmu_cand_idx2));
                data_.z_mass = data_.z_cand ? zmu_cand_mass : 99999;
                data_.z_opposite_charge = z_opposite_charge;
                data_.lct_to_zmu_idx = data_.z_cand ? int(i) : 99999;
              }
            }
          }
          std::cout << "Best muon/l1 match dR was " << data_.reco_l1_match_dR << " and went from l1 pt " << data_.l1muon_match_pt << " to reco pt " << data_.reco_l1_match_pt << std::endl;
        }





      }
      tree->Fill();
      if (data_.has_reco_l1_match){
        dataOnlyRecos_ = data_;
        treeOnlyRecos->Fill();
      }
      //At the end of the GEM loop (and LCT loops inherently)
      //Check if this has a higher emtftrack_pt than the current highest
      //Have to add an extra check for 99999 due to initalized value
      if ((data_.emtftrack_pt > dataHighestEMTF_.emtftrack_pt) and (data_.emtftrack_pt < 99999)){
        dataHighestEMTF_ = data_;
      }
    }
  }
  //This is the end of the event, save the highest tree now
  //But if the tree was never filled (no LCTs in event, no GEM Match) then we don't care
  if (dataHighestEMTF_.emtftrack_pt >= 0){
    treeHighestEMTF->Fill();
  }
}





void GEMCSCBendingAngleTester::fillMap(map<int, int>& thisMap, string filename){
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

void GEMCSCBendingAngleTester::beginJob(){
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

  fillMap(GEMAlignmentExtraCorrPositiveL1_Map, GEMAlignmentExtraCorrPositiveL1Name);
  fillMap(GEMAlignmentExtraCorrPositiveL2_Map, GEMAlignmentExtraCorrPositiveL2Name);
  fillMap(GEMAlignmentExtraCorrNegativeL1_Map, GEMAlignmentExtraCorrNegativeL1Name);
  fillMap(GEMAlignmentExtraCorrNegativeL2_Map, GEMAlignmentExtraCorrNegativeL2Name);

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
void GEMCSCBendingAngleTester::endJob(){
  cout << "Found " << me1a_counter << " ME1a" << endl;
}

DEFINE_FWK_MODULE(GEMCSCBendingAngleTester);
