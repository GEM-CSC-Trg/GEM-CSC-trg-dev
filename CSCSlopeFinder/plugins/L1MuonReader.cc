#include <assert.h>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/BXVector.h"

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

struct L1Muon
{
  void init();
  TTree* book(TTree *t);
  //============ L1Muon Info ================//
  float pt;
  float eta;


};

void L1Muon::init()
{
  //=========== L1Muon Info ===============//
  pt = -999.0;
  eta = -999.0;
}

TTree* L1Muon::book(TTree *t){
  edm::Service< TFileService > fs;
  t = fs->make<TTree>("L1Muon", "L1Muon");

  //=========== L1Muon Info =============//
  t->Branch("pt", &pt);
  t->Branch("eta", &eta);
  cout << "Booked tree" << endl;
  return t;
}

class L1MuonReader : public edm::one::EDAnalyzer<> {
public:
  explicit L1MuonReader(const edm::ParameterSet&);
  ~L1MuonReader(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;

  edm::EDGetTokenT<MuonBxCollection> l1_muon_token;
  //MuonDigiCollection<CSCDetId,CSCCorrelatedLCTDigi>    "cscTriggerPrimitiveDigis"   ""                "L1CSCTPG"
  //BXVector<l1t::Muon>                   "gtStage2Digis"             "Muon"            "RECO"

  edm::Service<TFileService> fs;
  MuonServiceProxy* theService_;
    
  bool debug;

  L1Muon data_;
  TTree* tree;

  bool isMC;
};


L1MuonReader::L1MuonReader(const edm::ParameterSet& iConfig)
{
  cout << "Begin L1MuonReader" << endl;
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService_ = new MuonServiceProxy(serviceParameters, consumesCollector());
  l1_muon_token = consumes<MuonBxCollection>(iConfig.getParameter<edm::InputTag>("l1_muon_token"));


  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree);
}


void
L1MuonReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  cout << "Setup?" << endl;
  theService_->update(iSetup);
  auto propagator = theService_->propagator("SteppingHelixPropagatorAny");

  isMC = false;
  if (! iEvent.eventAuxiliary().isRealData()) isMC = true;
  cout << "Handle?" << endl;
  edm::Handle<MuonBxCollection> l1_muons;
  cout << "Token?" << endl;
  iEvent.getByToken(l1_muon_token, l1_muons);

  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;

  //cout << "Starting the corr LCT search" << endl;
  cout << "New Event" << endl;
  int ge11_muons = 0;
  cout << " Is valid? " << l1_muons.isValid() << endl;
  if (!(l1_muons.isValid())) return;
  cout << l1_muons->size() << endl;
  for (int ibx = l1_muons->getFirstBX(); ibx <= l1_muons->getLastBX(); ++ibx){
    for (auto it = l1_muons->begin(ibx); it != l1_muons->end(ibx); it++){
      if (it->et() > 0){
        cout << "Muon at bx " << ibx << " et: " << it->et() << " eta: " << it->eta() << " phi:  " << it->phi() << endl;
        if (abs((it->eta()) > 1.5) && (abs(it->eta()) < 2.2)){ge11_muons++;}
      }
    }
  }
  cout << "Event found " << ge11_muons << " GE11 muons" << endl;
}




void L1MuonReader::beginJob(){
  cout << "Begin job!" << endl;
  cout << "Ended Begin Job, starting Event Loop" << endl;
}
void L1MuonReader::endJob(){}

DEFINE_FWK_MODULE(L1MuonReader);
