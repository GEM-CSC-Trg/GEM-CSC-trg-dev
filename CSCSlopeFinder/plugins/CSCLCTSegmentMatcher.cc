#include <assert.h>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
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


struct SegmentLCTData
{
  void init();
  TTree* book(TTree *t);
  //============ Muon Info ================//
  int muon_charge; float muon_pt; float muon_eta;
  unsigned long long evtNum; unsigned long long lumiBlock; int runNum;

  //============ Segment Info =============//
  float Segment_GP_x; float Segment_GP_y; float Segment_GP_z;
  float Segment_LP_x; float Segment_LP_y; float Segment_LP_z;
  float Segment_slope_angle;
  int Segment_ME11_endcap; int Segment_ME11_station; 
  int Segment_ME11_ring; int Segment_ME11_chamber;

  //============ Track Info ===============//
  float Track_GP_x; float Track_GP_y; float Track_GP_z;
  float Track_LP_x; float Track_LP_y; float Track_LP_z;
  float Track_slope_angle;
  int Track_ME11_endcap; int Track_ME11_station;
  int Track_ME11_ring; int Track_ME11_chamber;

  //============ LCT Info =================//
  float LCT_slope; int LCT_quality; float LCT_strip;
  int LCT_bend;
  int LCT_ME11_endcap; int LCT_ME11_station;
  int LCT_ME11_ring; int LCT_ME11_chamber;

  //============ Segment Prop Info ========//
  float SegmentProp_GP_x; float SegmentProp_GP_y; float SegmentProp_GP_z;
  float SegmentProp_LP_x; float SegmentProp_LP_y; float SegmentProp_LP_z;
  int SegmentProp_GE11_endcap; int SegmentProp_GE11_station;
  int SegmentProp_GE11_ring; int SegmentProp_GE11_chamber;
  int SegmentProp_GE11_layer; int SegmentProp_GE11_eta;

  //============ Track Prop Info ==========//
  float TrackProp_GP_x; float TrackProp_GP_y; float TrackProp_GP_z;
  float TrackProp_LP_x; float TrackProp_LP_y; float TrackProp_LP_z;
  int TrackProp_GE11_endcap; int TrackProp_GE11_station;
  int TrackProp_GE11_ring; int TrackProp_GE11_chamber;
  int TrackProp_GE11_layer; int TrackProp_GE11_eta;

  //============ Segment Hit Info =========//
  float SegmentHit_GP_x; float SegmentHit_GP_y; float SegmentHit_GP_z;
  float SegmentHit_LP_x; float SegmentHit_LP_y; float SegmentHit_LP_z;
  float SegmentHit_residual;
  int SegmentHit_GE11_endcap; int SegmentHit_GE11_station;
  int SegmentHit_GE11_ring; int SegmentHit_GE11_chamber;
  int SegmentHit_GE11_layer; int SegmentHit_GE11_eta;
  vector<int> SegmentHit_DigiCluster_Pads; vector<int> SegmentHit_DigiCluster_BX;
  int SegmentHit_DigiCluster_BestPad; int SegmentHit_DigiCluster_BestBX;

  //============ Track Hit Info ===========//
  float TrackHit_GP_x; float TrackHit_GP_y; float TrackHit_GP_z;
  float TrackHit_LP_x; float TrackHit_LP_y; float TrackHit_LP_z;
  float TrackHit_residual;
  int TrackHit_GE11_endcap; int TrackHit_GE11_station;
  int TrackHit_GE11_ring; int TrackHit_GE11_chamber;
  int TrackHit_GE11_layer; int TrackHit_GE11_eta;

};

void SegmentLCTData::init()
{
  //=========== Muon Info ===============//
  float value = 99999;
  muon_charge = value; muon_pt = value; muon_eta = value;
  evtNum = value; lumiBlock = value; runNum = value;

  //============ Segment Info =============//
  Segment_GP_x = value; Segment_GP_y = value; Segment_GP_z = value;
  Segment_LP_x = value; Segment_LP_y = value; Segment_LP_z = value;
  Segment_slope_angle = value;
  Segment_ME11_endcap = value; Segment_ME11_station = value;
  Segment_ME11_ring = value; Segment_ME11_chamber = value;

  //============ Track Info ===============//
  Track_GP_x = value; Track_GP_y = value; Track_GP_z = value;
  Track_LP_x = value; Track_LP_y = value; Track_LP_z = value;
  Track_slope_angle = value;
  Track_ME11_endcap = value; Segment_ME11_station = value;
  Track_ME11_ring = value; Segment_ME11_chamber = value;

  //============ LCT Info =================//
  LCT_slope = value; LCT_quality = value; LCT_strip = value;
  LCT_bend = value;
  LCT_ME11_endcap = value; LCT_ME11_station = value;
  LCT_ME11_ring = value; LCT_ME11_chamber = value;

  //============ Segment Prop Info ========//
  SegmentProp_GP_x = value; SegmentProp_GP_y = value; SegmentProp_GP_z = value;
  SegmentProp_LP_x = value; SegmentProp_LP_y = value; SegmentProp_LP_z = value;
  SegmentProp_GE11_endcap = value; SegmentProp_GE11_station = value;
  SegmentProp_GE11_ring = value; SegmentProp_GE11_chamber = value;
  SegmentProp_GE11_layer = value; SegmentProp_GE11_eta = value;

  //============ Track Prop Info ==========//
  TrackProp_GP_x = value; TrackProp_GP_y = value; TrackProp_GP_z = value;
  TrackProp_LP_x = value; TrackProp_LP_y = value; TrackProp_LP_z = value;
  TrackProp_GE11_endcap = value; TrackProp_GE11_station = value;
  TrackProp_GE11_ring = value; TrackProp_GE11_chamber = value;
  TrackProp_GE11_layer = value; TrackProp_GE11_eta = value;

  //============ Segment Hit Info =========//
  SegmentHit_GP_x = value; SegmentHit_GP_y = value; SegmentHit_GP_z = value;
  SegmentHit_LP_x = value; SegmentHit_LP_y = value; SegmentHit_LP_z = value;
  SegmentHit_residual = value;
  SegmentHit_GE11_endcap = value; SegmentHit_GE11_station = value;
  SegmentHit_GE11_ring = value; SegmentHit_GE11_chamber = value;
  SegmentHit_GE11_layer = value; SegmentHit_GE11_eta = value;
  SegmentHit_DigiCluster_Pads.clear(); SegmentHit_DigiCluster_BX.clear();
  SegmentHit_DigiCluster_BestPad = value; SegmentHit_DigiCluster_BestBX = value;

  //============ Track Hit Info ===========//
  TrackHit_GP_x = value; TrackHit_GP_y = value; TrackHit_GP_z = value;
  TrackHit_LP_x = value; TrackHit_LP_y = value; TrackHit_LP_z = value;
  TrackHit_residual = value;
  TrackHit_GE11_endcap = value; TrackHit_GE11_station = value;
  TrackHit_GE11_ring = value; TrackHit_GE11_chamber = value;
  TrackHit_GE11_layer = value; TrackHit_GE11_eta = value;
}

TTree* SegmentLCTData::book(TTree *t){
  edm::Service< TFileService > fs;
  t = fs->make<TTree>("GEM_CSC_Trigger", "GEM_CSC_Trigger");

  //=========== Muon Info =============//
  t->Branch("muon_charge", &muon_charge); t->Branch("muon_pt", &muon_pt); t->Branch("muon_eta", &muon_eta);
  t->Branch("evtNum", &evtNum); t->Branch("lumiBlock", &lumiBlock); t->Branch("runNum", &runNum);

  //============ Segment Info =============//
  t->Branch("Segment_GP_x", &Segment_GP_x); t->Branch("Segment_GP_y", &Segment_GP_y); t->Branch("Segment_GP_z", &Segment_GP_z);
  t->Branch("Segment_LP_x", &Segment_LP_x); t->Branch("Segment_LP_y", &Segment_LP_y); t->Branch("Segment_LP_z", &Segment_LP_z);
  t->Branch("Segment_slope_angle", &Segment_slope_angle);
  t->Branch("Segment_ME11_endcap", &Segment_ME11_endcap); t->Branch("Segment_ME11_station", &Segment_ME11_station);
  t->Branch("Segment_ME11_ring", &Segment_ME11_ring); t->Branch("Segment_ME11_chamber", &Segment_ME11_chamber);

  //============ Track Info ===============//
  t->Branch("Track_GP_x", &Track_GP_x); t->Branch("Track_GP_y", &Track_GP_y); t->Branch("Track_GP_z", &Track_GP_z);
  t->Branch("Track_LP_x", &Track_LP_x); t->Branch("Track_LP_y", &Track_LP_y); t->Branch("Track_LP_z", &Track_LP_z);
  t->Branch("Track_slope_angle", &Track_slope_angle);
  t->Branch("Track_ME11_endcap", &Track_ME11_endcap); t->Branch("Track_ME11_station", &Track_ME11_station);
  t->Branch("Track_ME11_ring", &Track_ME11_ring); t->Branch("Track_ME11_chamber", &Track_ME11_chamber);

  //============ LCT Info =================//
  t->Branch("LCT_slope", &LCT_slope); t->Branch("LCT_quality", &LCT_quality); t->Branch("LCT_strip", &LCT_strip);
  t->Branch("LCT_bend", &LCT_bend);
  t->Branch("LCT_ME11_endcap", &LCT_ME11_endcap); t->Branch("LCT_ME11_station", &LCT_ME11_station);
  t->Branch("LCT_ME11_ring", &LCT_ME11_ring); t->Branch("LCT_ME11_chamber", &LCT_ME11_chamber);

  //============ Segment Prop Info ========//
  t->Branch("SegmentProp_GP_x", &SegmentProp_GP_x); t->Branch("SegmentProp_GP_y", &SegmentProp_GP_y); t->Branch("SegmentProp_GP_z", &SegmentProp_GP_z);
  t->Branch("SegmentProp_LP_x", &SegmentProp_LP_x); t->Branch("SegmentProp_LP_y", &SegmentProp_LP_y); t->Branch("SegmentProp_LP_z", &SegmentProp_LP_z);
  t->Branch("SegmentProp_GE11_endcap", &SegmentProp_GE11_endcap); t->Branch("SegmentProp_GE11_station", &SegmentProp_GE11_station);
  t->Branch("SegmentProp_GE11_ring", &SegmentProp_GE11_ring); t->Branch("SegmentProp_GE11_chamber", &SegmentProp_GE11_chamber);
  t->Branch("SegmentProp_GE11_layer", &SegmentProp_GE11_layer); t->Branch("SegmentProp_GE11_eta", &SegmentProp_GE11_eta);

  //============ Track Prop Info ==========//
  t->Branch("TrackProp_GP_x", &TrackProp_GP_x); t->Branch("TrackProp_GP_y", &TrackProp_GP_y); t->Branch("TrackProp_GP_z", &TrackProp_GP_z);
  t->Branch("TrackProp_LP_x", &TrackProp_LP_x); t->Branch("TrackProp_LP_y", &TrackProp_LP_y); t->Branch("TrackProp_LP_z", &TrackProp_LP_z);
  t->Branch("TrackProp_GE11_endcap", &TrackProp_GE11_endcap); t->Branch("TrackProp_GE11_station", &TrackProp_GE11_station);
  t->Branch("TrackProp_GE11_ring", &TrackProp_GE11_ring); t->Branch("TrackProp_GE11_chamber", &TrackProp_GE11_chamber);
  t->Branch("TrackProp_GE11_layer", &TrackProp_GE11_layer); t->Branch("TrackProp_GE11_eta", &TrackProp_GE11_eta);

  //============ Segment Hit Info =========//
  t->Branch("SegmentHit_GP_x", &SegmentHit_GP_x); t->Branch("SegmentHit_GP_y", &SegmentHit_GP_y); t->Branch("SegmentHit_GP_z", &SegmentHit_GP_z);
  t->Branch("SegmentHit_LP_x", &SegmentHit_LP_x); t->Branch("SegmentHit_LP_y", &SegmentHit_LP_y); t->Branch("SegmentHit_LP_z", &SegmentHit_LP_z);
  t->Branch("SegmentHit_residual", &SegmentHit_residual);
  t->Branch("SegmentHit_GE11_endcap", &SegmentHit_GE11_endcap); t->Branch("SegmentHit_GE11_station", &SegmentHit_GE11_station);
  t->Branch("SegmentHit_GE11_ring", &SegmentHit_GE11_ring); t->Branch("SegmentHit_GE11_chamber", &SegmentHit_GE11_chamber);
  t->Branch("SegmentHit_GE11_layer", &SegmentHit_GE11_layer); t->Branch("SegmentHit_GE11_eta", &SegmentHit_GE11_eta);
  t->Branch("SegmentHit_DigiCluster_Pads", &SegmentHit_DigiCluster_Pads); t->Branch("SegmentHit_DigiCluster_BX", &SegmentHit_DigiCluster_BX);
  t->Branch("SegmentHit_DigiCluster_BestPad", &SegmentHit_DigiCluster_BestPad); t->Branch("SegmentHit_DigiCluster_BestBX", &SegmentHit_DigiCluster_BestBX);

  //============ Track Hit Info ===========//
  t->Branch("TrackHit_GP_x", &TrackHit_GP_x); t->Branch("TrackHit_GP_y", &TrackHit_GP_y); t->Branch("TrackHit_GP_z", &TrackHit_GP_z);
  t->Branch("TrackHit_LP_x", &TrackHit_LP_x); t->Branch("TrackHit_LP_y", &TrackHit_LP_y); t->Branch("TrackHit_LP_z", &TrackHit_LP_z);
  t->Branch("TrackHit_residual", &TrackHit_residual);
  t->Branch("TrackHit_GE11_endcap", &TrackHit_GE11_endcap); t->Branch("TrackHit_GE11_station", &TrackHit_GE11_station);
  t->Branch("TrackHit_GE11_ring", &TrackHit_GE11_ring); t->Branch("TrackHit_GE11_chamber", &TrackHit_GE11_chamber);
  t->Branch("TrackHit_GE11_layer", &TrackHit_GE11_layer); t->Branch("TrackHit_GE11_eta", &TrackHit_GE11_eta);

  return t;
}

class CSCLCTSegmentMatcher : public edm::one::EDAnalyzer<> {
public:
  explicit CSCLCTSegmentMatcher(const edm::ParameterSet&);
  ~CSCLCTSegmentMatcher(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;



  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;
  edm::EDGetTokenT<GEMPadDigiClusterCollection> gemdigi_token;

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

  bool debug;

  SegmentLCTData data_;
  TTree* tree;

  bool isMC;

  const edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeomToken_;
  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeomToken_;
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttkToken_;
  const edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> geomToken_;

  //Maps for the CSC LCT Slope Extrapolation (CSC LCT -> GEM Layers)
  //Split by Even/Odd, by L1/L2, and by ME11a/ME11b (8 cases)
  string SlopeExtrapolationME11aEvenL1Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_even_GEMlayer1.txt";
  map<int, int> SlopeExtrapolationME11aEvenL1_Map;
  string SlopeExtrapolationME11bEvenL1Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_even_GEMlayer1.txt";
  map<int, int> SlopeExtrapolationME11bEvenL1_Map;
  string SlopeExtrapolationME11aOddL1Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_odd_GEMlayer1.txt";
  map<int, int> SlopeExtrapolationME11aOddL1_Map;
  string SlopeExtrapolationME11bOddL1Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_odd_GEMlayer1.txt";
  map<int, int> SlopeExtrapolationME11bOddL1_Map;

  string SlopeExtrapolationME11aEvenL2Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_even_GEMlayer2.txt";
  map<int, int> SlopeExtrapolationME11aEvenL2_Map;
  string SlopeExtrapolationME11bEvenL2Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_even_GEMlayer2.txt";
  map<int, int> SlopeExtrapolationME11bEvenL2_Map;
  string SlopeExtrapolationME11aOddL2Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11a_odd_GEMlayer2.txt";
  map<int, int> SlopeExtrapolationME11aOddL2_Map;
  string SlopeExtrapolationME11bOddL2Name = "../luts/GEMCSC/SlopeCorrection/FacingChambers/ExtrapolationBySlope_ME11b_odd_GEMlayer2.txt";
  map<int, int> SlopeExtrapolationME11bOddL2_Map;

  //Maps for the GEM Pad Digi Clusters to be converted into CSC eighth strip units
  //Split by Even/Odd, and by ME11a/ME11b (4 cases)
  string GEMPadDigiToCSCEightStripME11aEvenName = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1a_even.txt";
  map<int, int> GEMPadDigiToCSCEigthStripME11aEven_Map;
  string GEMPadDigiToCSCEightStripME11bEvenName = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1b_even.txt";
  map<int, int> GEMPadDigiToCSCEigthStripME11bEven_Map;
  string GEMPadDigiToCSCEightStripME11aOddName = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1a_odd.txt";
  map<int, int> GEMPadDigiToCSCEigthStripME11aOdd_Map;
  string GEMPadDigiToCSCEightStripME11bOddName = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_pad_es_ME1b_odd.txt";
  map<int, int> GEMPadDigiToCSCEigthStripME11bOdd_Map;

  //Maps for the GEM Pad Digi Clusters to be converted into CSC Min and Max WireGroups
  //Split by Even/Odd, Layer1/Layer2, and by Min/Max
  string GEMPadDigiToCSCWGMinEvenL1Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_min_wg_ME11_even.txt";
  map<int, int> GEMPadDigiToCSCWGMinEvenL1_Map;
  string GEMPadDigiToCSCWGMaxEvenL1Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_max_wg_ME11_even.txt";
  map<int, int> GEMPadDigiToCSCWGMaxEvenL1_Map;

  string GEMPadDigiToCSCWGMinOddL1Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_min_wg_ME11_odd.txt";
  map<int, int> GEMPadDigiToCSCWGMinOddL1_Map;
  string GEMPadDigiToCSCWGMaxOddL1Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l1_max_wg_ME11_odd.txt";
  map<int, int> GEMPadDigiToCSCWGMaxOddL1_Map;

  string GEMPadDigiToCSCWGMinEvenL2Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_min_wg_ME11_even.txt";
  map<int, int> GEMPadDigiToCSCWGMinEvenL2_Map;
  string GEMPadDigiToCSCWGMaxEvenL2Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_max_wg_ME11_even.txt";
  map<int, int> GEMPadDigiToCSCWGMaxEvenL2_Map;

  string GEMPadDigiToCSCWGMinOddL2Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_min_wg_ME11_odd.txt";
  map<int, int> GEMPadDigiToCSCWGMinOddL2_Map;
  string GEMPadDigiToCSCWGMaxOddL2Name = "../luts/GEMCSC/CoordinateConversion/GEMCSCLUT_roll_l2_max_wg_ME11_odd.txt";
  map<int, int> GEMPadDigiToCSCWGMaxOddL2_Map;

};


CSCLCTSegmentMatcher::CSCLCTSegmentMatcher(const edm::ParameterSet& iConfig)
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

  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree);

}


void
CSCLCTSegmentMatcher::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

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
  edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
  iEvent.getByToken(co_token, correlatedlcts);
  edm::Handle<GEMPadDigiClusterCollection> gemPadDigis;
  iEvent.getByToken(gemdigi_token, gemPadDigis);

  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;
  if (! iEvent.getByToken(muons_, muons)) return;
  if (debug) cout << "There are " << muons->size() << " muons in event" << endl;
  if (muons->size() == 0) return;

  edm::Handle<CSCSegmentCollection> cscSegments;
  if (! iEvent.getByToken(cscSegments_, cscSegments)){std::cout << "Bad segments" << std::endl;}

  if (debug) cout << "New Event" << endl;


  for (size_t i = 0; i < muons->size(); ++i){
    edm::RefToBase<reco::Muon> muRef = muons->refAt(i);
    const reco::Muon* mu = muRef.get();

    if (not mu->isGlobalMuon()) continue;

    //Lets find the ME1/1 segments from this muon
    if(!(mu->isStandAloneMuon())){continue;}
    if (debug) cout << "New Muon" << endl;
    const reco::Track* Track = mu->outerTrack().get();
    if (Track->validFraction() > 0.0) continue;
    for (size_t RecHit_iter = 0; RecHit_iter != Track->recHitsSize(); RecHit_iter++){

      //The tree fill will happen at this level (segment level), lets initialize our variables
      data_.init();
      const CSCSegment* ME11_segment;
      GlobalPoint TSOS_Segment_GP;
      LocalPoint TSOS_Segment_LP;
      LocalVector TSOS_Segment_LocalDir;
      GlobalPoint TSOS_Track_GP;
      LocalPoint TSOS_Track_LP;
      LocalVector TSOS_Track_LocalDir;
      CSCCorrelatedLCTDigi LCTDigiMatch;
      CSCDetId SegmentCSCDetId;
      CSCDetId TrackCSCDetId;
      CSCDetId LCTDetId;
      GlobalPoint TSOS_Segment_On_GEM_GP;
      LocalPoint TSOS_Segment_On_GEM_LP;
      GlobalPoint TSOS_Track_On_GEM_GP;
      LocalPoint TSOS_Track_On_GEM_LP;
      GlobalPoint GEM_Hit_Match_Segment_GP;
      LocalPoint GEM_Hit_Match_Segment_LP;
      GlobalPoint GEM_Hit_Match_Track_GP;
      LocalPoint GEM_Hit_Match_Track_LP;
      GEMDetId SegmentGEMDetId;
      GEMDetId TrackGEMDetId;
      vector<int> best_clusterPads;
      vector<int> best_clusterBXs;
      bool good_event = false;
      int LCT_Extrapolation_MatchPad = 999;
      int LCT_Extrapolation_MatchBX = 999;
      int LCT_Extrapolation_MatchPad_CSCes = 999;

      const TrackingRecHit* RecHit = (Track->recHit(RecHit_iter)).get();
      DetId RecHitId = RecHit->geographicalId();
      uint16_t RecHitDetId = RecHitId.det();
      if (RecHitDetId != DetId::Muon) continue;
      uint16_t RecHitSubDet = RecHitId.subdetId();
      if (RecHitSubDet != (uint16_t)MuonSubdetId::CSC) continue;
      SegmentCSCDetId = CSCDetId(RecHitId);
      TrackCSCDetId = SegmentCSCDetId;
      if (not(SegmentCSCDetId.station() == 1 and SegmentCSCDetId.ring() == 1 and RecHit->dimension() == 4)) continue;
      RecSegment* Rec_segment = (RecSegment*)RecHit;
      ME11_segment = (CSCSegment*)Rec_segment;

      auto SegmentCSCDetIdL4 = CSCDetId(SegmentCSCDetId.endcap(), SegmentCSCDetId.station(), SegmentCSCDetId.ring(), SegmentCSCDetId.chamber(), 4);
      const CSCLayer* ME11_layer = CSCGeometry_->layer(SegmentCSCDetIdL4);
      const CSCLayerGeometry* ME11_layer_geo = ME11_layer->geometry();

      //We have a segment, lets now try to find the track at this same position
      reco::TransientTrack TTrack = ttrackBuilder_->build(Track);
      GlobalPoint ME11_segment_GP = ME11_layer->toGlobal(ME11_segment->localPosition());
      TrajectoryStateOnSurface track_at_segment = TTrack.stateOnSurface(ME11_segment_GP);
      if (!(track_at_segment.isValid())) continue;
      LocalPoint track_at_segment_LP = ME11_layer->toLocal(track_at_segment.globalPosition());
      if (debug) cout << "Debug track strip issue? TrackLP:SegmentLP " << track_at_segment_LP << ":" << ME11_segment->localPosition() << endl;
      if (debug) cout << "Debug track strip issue? TrackGP:SegmentGP " << track_at_segment.globalPosition() << ":" << ME11_segment_GP << endl;

      //We have a CSCDet with a segment, lets now try to find a matching LCT
      for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end(); j++){
        LCTDetId = (*j).first;
        if (not(LCTDetId.station() == 1 and LCTDetId.ring() == 1)) continue;
        if (not(SegmentCSCDetId == LCTDetId)) continue;
        if (debug){
          cout << "Found a Match!!!" << endl;
          cout << "Segment DetID: " << SegmentCSCDetId << endl;
          cout << "LCT DetID    : " << LCTDetId << endl;
        }

        std::vector<CSCCorrelatedLCTDigi>::const_iterator digiItr = (*j).second.first;
        std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
        //Pick the best LCT Digi to match the segment
        for (; digiItr != last; ++digiItr){
          //float SegmentStrip = ME11_layer_geo->nearestStrip(ME11_segment->localPosition());
          float SegmentFracStrip = ME11_layer_geo->strip(ME11_segment->localPosition());
          //const float TrackStrip = ME11_layer_geo->nearestStrip(track_at_segment_LP);
          const float TrackFracStrip = ME11_layer_geo->strip(track_at_segment_LP);
          if (debug)  cout << "SegStrip:TrackStrip:LCTStrip Frac -- " << SegmentFracStrip << ":" << TrackFracStrip << ":" << digiItr->getFractionalStrip() << endl;

          //If new Itr is closer to the segment strip than the old one, replace
          if (abs(SegmentFracStrip - digiItr->getFractionalStrip()) < abs(SegmentFracStrip - LCTDigiMatch.getFractionalStrip())) LCTDigiMatch = *digiItr;
        }
        //Found the best DigiItr, now lets work on propagating to GEM
        //LCT FW Propagation
        int LCT_eighth_strip = (LCTDigiMatch.getStrip())*4 + (LCTDigiMatch.getQuartStripBit())*2 + (LCTDigiMatch.getEighthStripBit());
        if (debug) cout << "Found the LCT Eighth Strip at " << LCT_eighth_strip << endl;
        int slope_propagationL1 = 999;
        int slope_propagationL2 = 999;
        if (LCTDetId.chamber()%2 == 0){
          if (LCTDetId.isME1a()){
            slope_propagationL1 = SlopeExtrapolationME11aEvenL1_Map[LCTDigiMatch.getSlope()];
            slope_propagationL2 = SlopeExtrapolationME11aEvenL2_Map[LCTDigiMatch.getSlope()];
          }
          if (LCTDetId.isME1b()){
            slope_propagationL1 = SlopeExtrapolationME11bEvenL1_Map[LCTDigiMatch.getSlope()];
            slope_propagationL2 = SlopeExtrapolationME11bEvenL2_Map[LCTDigiMatch.getSlope()];
          }
        }
        if (LCTDetId.chamber()%2 != 0){
          if (LCTDetId.isME1a()){
            slope_propagationL1 = SlopeExtrapolationME11aOddL1_Map[LCTDigiMatch.getSlope()];
            slope_propagationL2 = SlopeExtrapolationME11aOddL2_Map[LCTDigiMatch.getSlope()];
          }
          if (LCTDetId.isME1b()){
            slope_propagationL1 = SlopeExtrapolationME11aOddL1_Map[LCTDigiMatch.getSlope()];
            slope_propagationL2 = SlopeExtrapolationME11aOddL2_Map[LCTDigiMatch.getSlope()];
          }
        }
        if (debug) cout << "Slope adjustment is L1 " << slope_propagationL1 << " and L2 " << slope_propagationL2 << endl;
        int LCT_to_GEM1_eighth_strip = LCT_eighth_strip + slope_propagationL1*((LCTDigiMatch.getBend()*2)-1);
        int LCT_to_GEM2_eighth_strip = LCT_eighth_strip + slope_propagationL2*((LCTDigiMatch.getBend()*2)-1);
        if (debug) cout << "LCT Propagations: L1 = " << LCT_to_GEM1_eighth_strip << " and L2 = " << LCT_to_GEM2_eighth_strip << endl;


        //Propagated to eighth strip, but now lets match to a GEM Pad Digi Cluster
        //Even matching window 20 CSC eighth strips
        //Odd matching window 40 CSC eighth strips
        //WG matching window 7 WG
        int even_delta_es = 50;
        int odd_delta_es = 100;
        int delta_wg = 20;
        for (GEMPadDigiClusterCollection::DigiRangeIterator j = gemPadDigis->begin(); j != gemPadDigis->end(); j++){
          GEMDetId GEMPadDigiDetID = (*j).first;
          if (!(((LCTDetId.endcap() == 2 and GEMPadDigiDetID.region() == -1) or (LCTDetId.endcap() == 1 and GEMPadDigiDetID.region() == 1)) and LCTDetId.station() == GEMPadDigiDetID.station() and LCTDetId.ring() == GEMPadDigiDetID.ring() and LCTDetId.chamber() == GEMPadDigiDetID.chamber())) continue;

          std::vector<GEMPadDigiCluster>::const_iterator digiItr = (*j).second.first;
          std::vector<GEMPadDigiCluster>::const_iterator last = (*j).second.second;
          for (; digiItr != last; ++digiItr) {
            //These are CLUSTERS, but we don't care about clusters we care about pads!!!
            cout << "Print all pads" << endl;
            for (int pad: digiItr->pads()){
              cout << pad << " ";
            }
            cout << endl;
            for (int pad: digiItr->pads()){
              int GEMPadToCSCes = 999;
              int GEMPadMaxWG = 999;
              int GEMPadMinWG = 999;
              if (LCTDetId.chamber()%2 == 0){
                if (LCTDetId.isME1a()){              
                  GEMPadToCSCes = GEMPadDigiToCSCEigthStripME11aEven_Map[pad];
                }
                if (LCTDetId.isME1b()){
                  GEMPadToCSCes = GEMPadDigiToCSCEigthStripME11bEven_Map[pad];
                }
                if (GEMPadDigiDetID.layer() == 1){
                  GEMPadMinWG = GEMPadDigiToCSCWGMinEvenL1_Map[GEMPadDigiDetID.roll()-1];
                  GEMPadMaxWG = GEMPadDigiToCSCWGMaxEvenL1_Map[GEMPadDigiDetID.roll()-1];
                }
                if (GEMPadDigiDetID.layer() == 2){
                  GEMPadMinWG = GEMPadDigiToCSCWGMinEvenL2_Map[GEMPadDigiDetID.roll()-1];
                  GEMPadMaxWG = GEMPadDigiToCSCWGMaxEvenL2_Map[GEMPadDigiDetID.roll()-1];
                }
              }
              if (LCTDetId.chamber()%2 == 1){
                if (LCTDetId.isME1a()){
                  GEMPadToCSCes = GEMPadDigiToCSCEigthStripME11aOdd_Map[pad];
                }
                if (LCTDetId.isME1b()){
                  GEMPadToCSCes = GEMPadDigiToCSCEigthStripME11bOdd_Map[pad];
                }
                if (GEMPadDigiDetID.layer() == 1){
                  GEMPadMinWG = GEMPadDigiToCSCWGMinOddL1_Map[GEMPadDigiDetID.roll()-1];
                  GEMPadMaxWG = GEMPadDigiToCSCWGMaxOddL1_Map[GEMPadDigiDetID.roll()-1];
                }
                if (GEMPadDigiDetID.layer() == 2){
                  GEMPadMinWG = GEMPadDigiToCSCWGMinOddL2_Map[GEMPadDigiDetID.roll()-1];
                  GEMPadMaxWG = GEMPadDigiToCSCWGMaxOddL2_Map[GEMPadDigiDetID.roll()-1];
                }
              }

              //Now finally, lets check if it is a match
              //Pad/Strip check -- abs(LCT_to_GEM*_eighth_strip - GEMPadToCSCes) < *_delta_es
              //WG/Eta check -- (GEMPadMinWG - delta_wg) < GEMPadDigiDetID.roll() < (GEMPadMaxWG + delta_wg)
              int tmp_delta_es = (GEMPadDigiDetID.layer() == 1) ? abs(LCT_to_GEM1_eighth_strip - GEMPadToCSCes) : abs(LCT_to_GEM2_eighth_strip - GEMPadToCSCes);
              if (((GEMPadMinWG - delta_wg) < GEMPadDigiDetID.roll()) and (GEMPadDigiDetID.roll() < (GEMPadMaxWG + delta_wg))){
                if (
                  ((LCTDetId.chamber()%2 == 0) and (tmp_delta_es < even_delta_es)) or
                  ((LCTDetId.chamber()%2 == 1) and (tmp_delta_es < odd_delta_es))
                  ){
                  if (!(tmp_delta_es < abs(LCT_Extrapolation_MatchPad_CSCes - GEMPadToCSCes))) continue;
                  cout << "good match!" << endl;
                  cout << "Pad went from " << pad << " to CSC units " << GEMPadToCSCes << " and we propagated to ";
                  if (GEMPadDigiDetID.layer() == 1) cout << LCT_to_GEM1_eighth_strip << endl;
                  if (GEMPadDigiDetID.layer() == 2) cout << LCT_to_GEM2_eighth_strip << endl;
                  cout << "Eta was " <<  GEMPadDigiDetID.roll() << " which goes to WG min/max " << GEMPadMinWG << "/" << GEMPadMaxWG << " and our LCT WG is " << LCTDigiMatch.getKeyWG() << endl;
                  //Save the matching GEMPadDigi for later comparison to the Segment Propagated Matches
                  //We don't consider alignment yet!!!!!!!!!
                  LCT_Extrapolation_MatchPad = pad;
                  LCT_Extrapolation_MatchBX = digiItr->bx();
                  LCT_Extrapolation_MatchPad_CSCes = (GEMPadDigiDetID.layer() == 1) ? LCT_to_GEM1_eighth_strip : LCT_to_GEM2_eighth_strip;
                  cout << "This is our extra pad/bx " << pad << ":" << digiItr->bx() << endl;
                }
              }
            }
          }
        }



        //Set Up Segment Propagation
        bool good_segment_prop = false;
        DetId segDetId = ME11_segment->geographicalId();
        const GeomDet* segDet = theTrackingGeometry->idToDet(segDetId());
        LocalVector momentum_at_surface = (Track->outerP()) * (ME11_segment->localDirection());
        LocalTrajectoryParameters param(ME11_segment->localPosition(), momentum_at_surface, mu->charge());
        AlgebraicSymMatrix mat(5,0);
        mat = ME11_segment->parametersError().similarityT(ME11_segment->projectionMatrix());
        LocalTrajectoryError error(asSMatrix<5>(mat));
        TrajectoryStateOnSurface TSOS_Segment(param, error, segDet->surface(), &*theService_->magneticField());
        TSOS_Segment_GP = TSOS_Segment.globalPosition();
        TSOS_Segment_LP = TSOS_Segment.localPosition();
        TSOS_Segment_LocalDir = TSOS_Segment.localDirection();

        //Set Up Track Propagation
        bool good_track_prop = false;
        TrajectoryStateOnSurface TSOS_Track = track_at_segment;
        TSOS_Track_GP = TSOS_Track.globalPosition();
        TSOS_Track_LP = TSOS_Track.localPosition();
        TSOS_Track_LocalDir = TSOS_Track.localDirection();

        if (debug) cout << "TSOS SEG LP : TSOS TRA LP" << TSOS_Segment_LP << ":" << TSOS_Track_LP << endl;
        if (debug) cout << "TSOS SEG GP : TSOS TRA GP" << TSOS_Segment_GP << ":" << TSOS_Track_GP << endl;

        auto propagator = theService_->propagator("SteppingHelixPropagatorAny");
        for (const auto& ch : GEMGeometry_->etaPartitions()){
          if (ch->id().station() != 1) continue; //Only GE1/1
          const BoundPlane& bps(ch->surface());

          TrajectoryStateOnSurface TSOS_Segment_On_GEM = propagator->propagate(TSOS_Segment, ch->surface());
          if (TSOS_Segment_On_GEM.isValid()){
            const GlobalPoint Prop_GEM_GP = TSOS_Segment_On_GEM.globalPosition();
            const LocalPoint Prop_GEM_LP = TSOS_Segment_On_GEM.localPosition();
            const LocalPoint Prop_GEM_LP_2D(Prop_GEM_LP.x(), Prop_GEM_LP.y(), 0.0);

            if (((TSOS_Segment.globalPosition().z() * Prop_GEM_GP.z()) > 0) and (bps.bounds().inside(Prop_GEM_LP_2D)) and (ch->id().station() == 1 and ch->id().ring() == 1)){
              SegmentGEMDetId = ch->id();
              TSOS_Segment_On_GEM_GP = TSOS_Segment_On_GEM.globalPosition();
              TSOS_Segment_On_GEM_LP = TSOS_Segment_On_GEM.localPosition();
              if (debug) cout << "TSOS Segment On GEM GP:LP " << TSOS_Segment_On_GEM_GP << ":" << TSOS_Segment_On_GEM_LP << endl;
              if (debug) cout << "Found a good segment prop" << endl;

              float tmp_delta_x = 999.9;
              for (auto hit = gemRecHits->begin(); hit != gemRecHits->end(); hit++){
                GEMDetId gemid((hit)->geographicalId());
                if (!(gemid.det() == DetId::Detector::Muon && gemid.subdetId() == MuonSubdetId::GEM)) continue;
                if (!(gemid.station() == ch->id().station() and gemid.chamber() == ch->id().chamber() and gemid.layer() == ch->id().layer() and abs(gemid.roll() - ch->id().roll()) <= 1 and gemid.region() == ch->id().region())) continue;
                if (!(((SegmentCSCDetId.endcap() == 2 and gemid.region() == -1) or (SegmentCSCDetId.endcap() == 1 and gemid.region() == 1)) and SegmentCSCDetId.station() == gemid.station() and SegmentCSCDetId.ring() == gemid.ring() and SegmentCSCDetId.chamber() == gemid.chamber())) continue;
                good_segment_prop = true;
                float new_delta_x = abs(hit->localPosition().x() - ch->toLocal(TSOS_Segment_On_GEM.globalPosition()).x());
                if (not (new_delta_x < tmp_delta_x)) continue;
                cout << "Best residual so far is " << tmp_delta_x << endl;
                tmp_delta_x = new_delta_x;
                const auto& etaPart = GEMGeometry_->etaPartition(gemid);
                GEM_Hit_Match_Segment_GP = etaPart->toGlobal(hit->localPosition());
                GEM_Hit_Match_Segment_LP = hit->localPosition();

                //We have a GEM Hit! Lets get its strip and start the process of finding the cluster it came from
                cout << "BunchX, firstClustStrip, clusterSize " << hit->BunchX() << " " << hit->firstClusterStrip() << " " << hit->clusterSize() << endl;
                data_.SegmentHit_DigiCluster_Pads.clear(); data_.SegmentHit_DigiCluster_BX.clear();
                for (GEMPadDigiClusterCollection::DigiRangeIterator j = gemPadDigis->begin(); j != gemPadDigis->end(); j++){
                  if ((*j).first != gemid) continue;
                  if (debug) cout << "Digi chamber! " << (*j).first << " and gemhit on " << gemid << endl;
                  std::vector<GEMPadDigiCluster>::const_iterator digiItr = (*j).second.first;
                  std::vector<GEMPadDigiCluster>::const_iterator last = (*j).second.second;
                  for (; digiItr != last; ++digiItr) {
                    for (int pad: digiItr->pads()){
                      data_.SegmentHit_DigiCluster_Pads.push_back(pad); data_.SegmentHit_DigiCluster_BX.push_back(digiItr->bx());
                      if (debug) cout << "Pad:BX  " << pad << ":" << digiItr->bx() << " ";
                      if (pad == int((hit->firstClusterStrip()/2.0))){
                        data_.SegmentHit_DigiCluster_BestPad = pad; data_.SegmentHit_DigiCluster_BestBX = digiItr->bx();
                        if (debug) cout << "    THIS IS THE MATCH" << endl;
                        cout << "We found the SegmentProp digiPad match, lets see how it compares to the LCT match" << endl;
                        cout << "Pad Segment:LCT " << pad << ":" << LCT_Extrapolation_MatchPad << endl;
                        cout << "BX  Segment:LCT " << digiItr->bx() << ":" << LCT_Extrapolation_MatchBX << endl;
                      }
                    }
                  }
                }
              }
            }
          }
          TrajectoryStateOnSurface TSOS_Track_On_GEM = propagator->propagate(TSOS_Track, ch->surface());
          if (TSOS_Track_On_GEM.isValid()){
            const GlobalPoint Prop_GEM_GP = TSOS_Track_On_GEM.globalPosition();
            const LocalPoint Prop_GEM_LP =TSOS_Track_On_GEM.localPosition();
            const LocalPoint Prop_GEM_LP_2D(Prop_GEM_LP.x(), Prop_GEM_LP.y(), 0.0);
            if (((TSOS_Track.globalPosition().z() * Prop_GEM_GP.z()) > 0) and (bps.bounds().inside(Prop_GEM_LP_2D)) and (ch->id().station() == 1 and ch->id().ring() == 1)){
              TrackGEMDetId = ch->id();
              TSOS_Track_On_GEM_GP = TSOS_Track_On_GEM.globalPosition();
              TSOS_Track_On_GEM_LP = TSOS_Track_On_GEM.localPosition();
              if (debug) cout << "Found a good track prop" << endl;

              float tmp_delta_x = 999.9;
              for (auto hit = gemRecHits->begin(); hit != gemRecHits->end(); hit++){
                GEMDetId gemid((hit)->geographicalId());
                if (!(gemid.det() == DetId::Detector::Muon && gemid.subdetId() == MuonSubdetId::GEM)) continue;
                if (!(gemid.station() == ch->id().station() and gemid.chamber() == ch->id().chamber() and gemid.layer() == ch->id().layer() and abs(gemid.roll() - ch->id().roll()) <= 1 and gemid.region() == ch->id().region())) continue;
                if (!(((SegmentCSCDetId.endcap() == 2 and gemid.region() == -1) or (SegmentCSCDetId.endcap() == 1 and gemid.region() == 1)) and SegmentCSCDetId.station() == gemid.station() and SegmentCSCDetId.ring() == gemid.ring() and SegmentCSCDetId.chamber() == gemid.chamber())) continue;
                good_track_prop = true;
                float new_delta_x = abs(hit->localPosition().x() - ch->toLocal(TSOS_Track_On_GEM.globalPosition()).x());
                if (not (new_delta_x < tmp_delta_x)) continue;
                tmp_delta_x = new_delta_x;
                const auto& etaPart = GEMGeometry_->etaPartition(gemid);
                GEM_Hit_Match_Track_GP = etaPart->toGlobal(hit->localPosition());
                GEM_Hit_Match_Track_LP = hit->localPosition();
              }
            }
          }
          if (good_segment_prop and good_track_prop) good_event = true;
        }
      }
      //This is where the fill will happen
      /*
      const CSCSegment* ME11_segment; done
      GlobalPoint TSOS_Segment_GP; done
      LocalPoint TSOS_Segment_LP; done
      LocalVector TSOS_Segment_LocalDir; done
      GlobalPoint TSOS_Track_GP; done
      LocalPoint TSOS_Track_LP; done
      LocalVector TSOS_Track_LocalDir; done
      CSCCorrelatedLCTDigi LCTDigiMatch; done
      CSCDetId SegmentCSCDetId; done
      CSCDetId TrackCSCDetId; done
      CSCDetId LCTDetId; done
      GlobalPoint TSOS_Segment_On_GEM_GP; done
      LocalPoint TSOS_Segment_On_GEM_LP; done
      GlobalPoint TSOS_Track_On_GEM_GP;
      LocalPoint TSOS_Track_On_GEM_LP;
      GlobalPoint GEM_Hit_Match_Segment_GP;
      LocalPoint GEM_Hit_Match_Segment_LP;
      GlobalPoint GEM_Hit_Match_Track_GP;
      LocalPoint GEM_Hit_Match_Track_LP;
      GEMDetId SegmentGEMDetId;
      GEMDetId TrackGEMDetId;
      */


      //============ Muon Info ================//
      data_.muon_charge = mu->charge(); data_.muon_pt = mu->pt(); data_.muon_eta = mu->eta();
      data_.evtNum = iEvent.eventAuxiliary().event(); data_.lumiBlock = iEvent.eventAuxiliary().luminosityBlock(); data_.runNum = iEvent.run();

      //============ Segment Info =============//
      data_.Segment_GP_x = TSOS_Segment_GP.x(); data_.Segment_GP_y = TSOS_Segment_GP.y(); data_.Segment_GP_z = TSOS_Segment_GP.z();
      data_.Segment_LP_x = TSOS_Segment_LP.x(); data_.Segment_LP_y = TSOS_Segment_LP.y(); data_.Segment_LP_z = TSOS_Segment_LP.z();
      data_.Segment_slope_angle = asin(TSOS_Segment_LocalDir.x() / TSOS_Segment_LocalDir.z());
      data_.Segment_ME11_endcap = SegmentCSCDetId.endcap(); data_.Segment_ME11_station = SegmentCSCDetId.station();
      data_.Segment_ME11_ring = SegmentCSCDetId.ring(); data_.Segment_ME11_chamber = SegmentCSCDetId.chamber();

      //============ Track Info ===============//
      data_.Track_GP_x = TSOS_Track_GP.x(); data_.Track_GP_y = TSOS_Track_GP.y(); data_.Track_GP_z = TSOS_Track_GP.z();
      data_.Track_LP_x = TSOS_Track_LP.x(); data_.Track_LP_y = TSOS_Track_LP.y(); data_.Track_LP_z = TSOS_Track_LP.z();
      data_.Track_slope_angle = asin(TSOS_Track_LocalDir.x() / TSOS_Track_LocalDir.y()); //For track some reason y() is actually z()
      data_.Track_ME11_endcap = TrackCSCDetId.endcap(); data_.Track_ME11_station = TrackCSCDetId.station();
      data_.Track_ME11_ring = TrackCSCDetId.ring(); data_.Track_ME11_chamber = TrackCSCDetId.chamber();

      //============ LCT Info =================//
      data_.LCT_slope = LCTDigiMatch.getSlope(); data_.LCT_quality = LCTDigiMatch.getQuality(); data_.LCT_strip = LCTDigiMatch.getFractionalStrip();
      data_.LCT_bend = LCTDigiMatch.getBend();
      data_.LCT_ME11_endcap = LCTDetId.endcap(); data_.LCT_ME11_station = LCTDetId.station();
      data_.LCT_ME11_ring = LCTDetId.ring(); data_.LCT_ME11_chamber = LCTDetId.chamber();

      //============ Segment Prop Info ========//
      data_.SegmentProp_GP_x = TSOS_Segment_On_GEM_GP.x(); data_.SegmentProp_GP_y = TSOS_Segment_On_GEM_GP.y(); data_.SegmentProp_GP_z = TSOS_Segment_On_GEM_GP.z();
      data_.SegmentProp_LP_x = TSOS_Segment_On_GEM_LP.x(); data_.SegmentProp_LP_y = TSOS_Segment_On_GEM_LP.y(); data_.SegmentProp_LP_z = TSOS_Segment_On_GEM_LP.z();
      data_.SegmentProp_GE11_endcap = SegmentGEMDetId.region(); data_.SegmentProp_GE11_station = SegmentGEMDetId.station();
      data_.SegmentProp_GE11_ring = SegmentGEMDetId.ring(); data_.SegmentProp_GE11_chamber = SegmentGEMDetId.chamber();
      data_.SegmentProp_GE11_layer = SegmentGEMDetId.layer(); data_.SegmentProp_GE11_eta = SegmentGEMDetId.roll();

      //============ Track Prop Info ==========//
      data_.TrackProp_GP_x = TSOS_Track_On_GEM_GP.x(); data_.TrackProp_GP_y = TSOS_Track_On_GEM_GP.y(); data_.TrackProp_GP_z = TSOS_Track_On_GEM_GP.z();
      data_.TrackProp_LP_x = TSOS_Track_On_GEM_LP.x(); data_.TrackProp_LP_y = TSOS_Track_On_GEM_LP.y(); data_.TrackProp_LP_z = TSOS_Track_On_GEM_LP.z();
      data_.TrackProp_GE11_endcap = TrackGEMDetId.region(); data_.TrackProp_GE11_station = TrackGEMDetId.station();
      data_.TrackProp_GE11_ring = TrackGEMDetId.ring(); data_.TrackProp_GE11_chamber = TrackGEMDetId.chamber();
      data_.TrackProp_GE11_layer = TrackGEMDetId.layer(); data_.TrackProp_GE11_eta = TrackGEMDetId.roll();

      //============ Segment Hit Info =========//
      data_.SegmentHit_GP_x = GEM_Hit_Match_Segment_GP.x(); data_.SegmentHit_GP_y = GEM_Hit_Match_Segment_GP.y(); data_.SegmentHit_GP_z = GEM_Hit_Match_Segment_GP.z();
      data_.SegmentHit_LP_x = GEM_Hit_Match_Segment_LP.x(); data_.SegmentHit_LP_y = GEM_Hit_Match_Segment_LP.y(); data_.SegmentHit_LP_z = GEM_Hit_Match_Segment_LP.z();
      //SegmentHit_residual = value;
      data_.SegmentHit_GE11_endcap = SegmentGEMDetId.region(); data_.SegmentHit_GE11_station = SegmentGEMDetId.station();
      data_.SegmentHit_GE11_ring = SegmentGEMDetId.ring(); data_.SegmentHit_GE11_chamber = SegmentGEMDetId.chamber();
      data_.SegmentHit_GE11_layer = SegmentGEMDetId.layer(); data_.SegmentHit_GE11_eta = SegmentGEMDetId.roll();

      //============ Track Hit Info ===========//
      data_.TrackHit_GP_x = GEM_Hit_Match_Track_GP.x(); data_.TrackHit_GP_y = GEM_Hit_Match_Track_GP.y(); data_.TrackHit_GP_z = GEM_Hit_Match_Track_GP.z();
      data_.TrackHit_LP_x = GEM_Hit_Match_Track_LP.x(); data_.TrackHit_LP_y = GEM_Hit_Match_Track_LP.y(); data_.TrackHit_LP_z = GEM_Hit_Match_Track_LP.z();
      //TrackHit_residual = value;
      data_.TrackHit_GE11_endcap = TrackGEMDetId.region(); data_.TrackHit_GE11_station = TrackGEMDetId.station();
      data_.TrackHit_GE11_ring = TrackGEMDetId.ring(); data_.TrackHit_GE11_chamber = TrackGEMDetId.chamber();
      data_.TrackHit_GE11_layer = TrackGEMDetId.layer(); data_.TrackHit_GE11_eta = TrackGEMDetId.roll();

      if (good_event) tree->Fill();
    }
  }
}




void CSCLCTSegmentMatcher::beginJob(){
  //Lets make the SlopeExtrapolationLUTMaps
  cout << "Begin job!" << endl;

  string delimiter = " ";
  string line;

  ifstream SlopeExtrapolationME11aEvenL1File;
  SlopeExtrapolationME11aEvenL1File.open(SlopeExtrapolationME11aEvenL1Name);
  if (SlopeExtrapolationME11aEvenL1File.is_open()){
    while(getline(SlopeExtrapolationME11aEvenL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11aEvenL1_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11bEvenL1File;
  SlopeExtrapolationME11bEvenL1File.open(SlopeExtrapolationME11bEvenL1Name);
  if (SlopeExtrapolationME11bEvenL1File.is_open()){
    while(getline(SlopeExtrapolationME11bEvenL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11bEvenL1_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11aOddL1File;
  SlopeExtrapolationME11aOddL1File.open(SlopeExtrapolationME11aOddL1Name);
  if (SlopeExtrapolationME11aOddL1File.is_open()){
    while(getline(SlopeExtrapolationME11aOddL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11aOddL1_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11bOddL1File;
  SlopeExtrapolationME11bOddL1File.open(SlopeExtrapolationME11bOddL1Name);
  if (SlopeExtrapolationME11bOddL1File.is_open()){
    while(getline(SlopeExtrapolationME11bOddL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11bOddL1_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11aEvenL2File;
  SlopeExtrapolationME11aEvenL2File.open(SlopeExtrapolationME11aEvenL2Name);
  if (SlopeExtrapolationME11aEvenL2File.is_open()){
    while(getline(SlopeExtrapolationME11aEvenL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11aEvenL2_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11bEvenL2File;
  SlopeExtrapolationME11bEvenL2File.open(SlopeExtrapolationME11bEvenL2Name);
  if (SlopeExtrapolationME11bEvenL2File.is_open()){
    while(getline(SlopeExtrapolationME11bEvenL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11bEvenL2_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11aOddL2File;
  SlopeExtrapolationME11aOddL2File.open(SlopeExtrapolationME11aOddL2Name);
  if (SlopeExtrapolationME11aOddL2File.is_open()){
    while(getline(SlopeExtrapolationME11aOddL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11aOddL2_Map[key] = value;
      }
    }
  }

  ifstream SlopeExtrapolationME11bOddL2File;
  SlopeExtrapolationME11bOddL2File.open(SlopeExtrapolationME11bOddL2Name);
  if (SlopeExtrapolationME11bOddL2File.is_open()){
    while(getline(SlopeExtrapolationME11bOddL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        SlopeExtrapolationME11bOddL2_Map[key] = value;
      }
    }
  }

  //GEMPadDigi to CSC Eighth Strip LUTs
  ifstream GEMPadDigiToCSCEightStripME11aEvenFile;
  GEMPadDigiToCSCEightStripME11aEvenFile.open(GEMPadDigiToCSCEightStripME11aEvenName);
  if (GEMPadDigiToCSCEightStripME11aEvenFile.is_open()){
    while(getline(GEMPadDigiToCSCEightStripME11aEvenFile, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCEigthStripME11aEven_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCEightStripME11bEvenFile;
  GEMPadDigiToCSCEightStripME11bEvenFile.open(GEMPadDigiToCSCEightStripME11bEvenName);
  if (GEMPadDigiToCSCEightStripME11bEvenFile.is_open()){
    while(getline(GEMPadDigiToCSCEightStripME11bEvenFile, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCEigthStripME11bEven_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCEightStripME11aOddFile;
  GEMPadDigiToCSCEightStripME11aOddFile.open(GEMPadDigiToCSCEightStripME11aOddName);
  if (GEMPadDigiToCSCEightStripME11aOddFile.is_open()){
    while(getline(GEMPadDigiToCSCEightStripME11aOddFile, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCEigthStripME11aOdd_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCEightStripME11bOddFile;
  GEMPadDigiToCSCEightStripME11bOddFile.open(GEMPadDigiToCSCEightStripME11bOddName);
  if (GEMPadDigiToCSCEightStripME11bOddFile.is_open()){
    while(getline(GEMPadDigiToCSCEightStripME11bOddFile, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCEigthStripME11bOdd_Map[key] = value;
      }
    }
  }

  //GEMPadDigi to CSC WG Min/Max LUTs
  ifstream GEMPadDigiToCSCWGMinEvenL1File;
  GEMPadDigiToCSCWGMinEvenL1File.open(GEMPadDigiToCSCWGMinEvenL1Name);
  if (GEMPadDigiToCSCWGMinEvenL1File.is_open()){
    while(getline(GEMPadDigiToCSCWGMinEvenL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMinEvenL1_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMaxEvenL1File;
  GEMPadDigiToCSCWGMaxEvenL1File.open(GEMPadDigiToCSCWGMaxEvenL1Name);
  if (GEMPadDigiToCSCWGMaxEvenL1File.is_open()){
    while(getline(GEMPadDigiToCSCWGMaxEvenL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMaxEvenL1_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMinOddL1File;
  GEMPadDigiToCSCWGMinOddL1File.open(GEMPadDigiToCSCWGMinOddL1Name);
  if (GEMPadDigiToCSCWGMinOddL1File.is_open()){
    while(getline(GEMPadDigiToCSCWGMinOddL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMinOddL1_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMaxOddL1File;
  GEMPadDigiToCSCWGMaxOddL1File.open(GEMPadDigiToCSCWGMaxOddL1Name);
  if (GEMPadDigiToCSCWGMaxOddL1File.is_open()){
    while(getline(GEMPadDigiToCSCWGMaxOddL1File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMaxOddL1_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMinEvenL2File;
  GEMPadDigiToCSCWGMinEvenL2File.open(GEMPadDigiToCSCWGMinEvenL2Name);
  if (GEMPadDigiToCSCWGMinEvenL2File.is_open()){
    while(getline(GEMPadDigiToCSCWGMinEvenL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMinEvenL2_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMaxEvenL2File;
  GEMPadDigiToCSCWGMaxEvenL2File.open(GEMPadDigiToCSCWGMaxEvenL2Name);
  if (GEMPadDigiToCSCWGMaxEvenL2File.is_open()){
    while(getline(GEMPadDigiToCSCWGMaxEvenL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMaxEvenL2_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMinOddL2File;
  GEMPadDigiToCSCWGMinOddL2File.open(GEMPadDigiToCSCWGMinOddL2Name);
  if (GEMPadDigiToCSCWGMinOddL2File.is_open()){
    while(getline(GEMPadDigiToCSCWGMinOddL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMinOddL2_Map[key] = value;
      }
    }
  }

  ifstream GEMPadDigiToCSCWGMaxOddL2File;
  GEMPadDigiToCSCWGMaxOddL2File.open(GEMPadDigiToCSCWGMaxOddL2Name);
  if (GEMPadDigiToCSCWGMaxOddL2File.is_open()){
    while(getline(GEMPadDigiToCSCWGMaxOddL2File, line)){
      int key = atoi(line.substr(0, line.find(delimiter)).c_str());
      int value = atoi(line.substr(line.find(delimiter), -1).c_str());
      if (!((key == 0) and (value == 0))){
        GEMPadDigiToCSCWGMaxOddL2_Map[key] = value;
      }
    }
  }



  cout << "Created all slope LUTs" << endl;
  cout << "Ended Begin Job, starting Event Loop" << endl;
}
void CSCLCTSegmentMatcher::endJob(){}

DEFINE_FWK_MODULE(CSCLCTSegmentMatcher);
