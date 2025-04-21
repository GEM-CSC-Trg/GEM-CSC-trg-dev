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


struct EmuData
{
  void init();
  TTree* book(TTree *t);
  //============ LCT Info ================//
  int LCT_slope;
  int LCT_quality;
  int LCT_strip;
  int LCT_wiregroup;
  int LCT_bend;

  float muon_pt;
  float muon_charge;
  float muon_LCT_distance;


};

void EmuData::init()
{
  //=========== LCT Info ===============//
  LCT_slope = -999;
  LCT_quality = -999;
  LCT_strip = -999;
  LCT_wiregroup = -999;
  LCT_bend = -999;

  muon_pt = -999.0;
  muon_charge = -999.0;
  muon_LCT_distance = -999.0;
}

TTree* EmuData::book(TTree *t){
  edm::Service< TFileService > fs;
  t = fs->make<TTree>("EmuData", "EmuData");

  //=========== LCT Info =============//
  t->Branch("LCT_slope", &LCT_slope); 
  t->Branch("LCT_quality", &LCT_quality); 
  t->Branch("LCT_strip", &LCT_strip);
  t->Branch("LCT_wiregroup", &LCT_wiregroup);
  t->Branch("LCT_bend", &LCT_bend);

  t->Branch("muon_pt", &muon_pt);
  t->Branch("muon_charge", &muon_charge);
  t->Branch("muon_LCT_distance", &muon_LCT_distance);

  return t;
}

class CSCEmulatorReader : public edm::one::EDAnalyzer<> {
public:
  explicit CSCEmulatorReader(const edm::ParameterSet&);
  ~CSCEmulatorReader(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;

  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;

  edm::EDGetTokenT<edm::View<reco::Muon> > muons_;

  edm::Service<TFileService> fs;
  MuonServiceProxy* theService_;
    
  edm::ESHandle<CSCGeometry> CSCGeometry_;

  bool debug;

  EmuData data_;
  TTree* tree;

  bool isMC;

  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeomToken_;
};


CSCEmulatorReader::CSCEmulatorReader(const edm::ParameterSet& iConfig)
  : cscGeomToken_(esConsumes())
{
  cout << "Begin CSCEmulatorReader" << endl;
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService_ = new MuonServiceProxy(serviceParameters, consumesCollector());
  muons_ = consumes<View<reco::Muon> >(iConfig.getParameter<InputTag>("muons"));
  co_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("emu_corrlctDigiTag"));


  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree);

}


void
CSCEmulatorReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  CSCGeometry_ = &iSetup.getData(cscGeomToken_);

  theService_->update(iSetup);
  auto propagator = theService_->propagator("SteppingHelixPropagatorAny");

  isMC = false;
  if (! iEvent.eventAuxiliary().isRealData()) isMC = true;
  edm::Handle<View<reco::Muon> > muons;
  edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
  iEvent.getByToken(co_token, correlatedlcts);

  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;
  if (! iEvent.getByToken(muons_, muons)) return;
  if (debug) cout << "There are " << muons->size() << " muons in event" << endl;
  //if (muons->size() == 0) return;

  //cout << "Starting the corr LCT search" << endl;
  cout << "New Event" << endl;
  for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end(); j++){
    data_.init();
    //cout << "Looping corr lcts" << endl;
    CSCDetId LCTDetId = (*j).first;
    cout << "New Chamber " << (*j).first << endl;
    if ((*j).first.station() != 1 or (*j).first.ring() != 1) continue;
    std::vector<CSCCorrelatedLCTDigi>::const_iterator digiItr = (*j).second.first;
    std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
    for (; digiItr != last; ++digiItr) {
      if (debug){
        cout << "Found a LCT" << endl;
        cout << "Strip:        " << digiItr->getStrip() << endl;
        cout << "Wiregroup:    " << digiItr->getKeyWG() << endl;
        cout << "Slope:        " << digiItr->getSlope() << endl;
        cout << "Bend          " << digiItr->getBend() << endl;
        cout << "Run2 Pattern: " << digiItr->getPattern() << endl;
        cout << "Run3 Pattern: " << digiItr->getRun3Pattern() << endl;
        cout << "Quality       " << digiItr->getQuality() << endl;
      }

      float muon_match_pt = -1.0;
      float muon_match_charge = 999.0;
      float muon_match_distance = 999.0;
      for (size_t i = 0; i < muons->size(); ++i){
        float closest_match_distance = 999;
        edm::RefToBase<reco::Muon> muRef = muons->refAt(i);
        const reco::Muon* mu = muRef.get();
        if(!(mu->isGlobalMuon())) continue;
        if(!(mu->isStandAloneMuon())) continue;
        const reco::Track* Track = mu->outerTrack().get();
        if (Track->validFraction() > 0.0) continue;
        for (size_t RecHit_iter = 0; RecHit_iter != Track->recHitsSize(); RecHit_iter++){
          const TrackingRecHit* RecHit = (Track->recHit(RecHit_iter)).get();
          DetId RecHitId = RecHit->geographicalId();
          uint16_t RecHitDetId = RecHitId.det();
          if (RecHitDetId != DetId::Muon) continue;
          uint16_t RecHitSubDet = RecHitId.subdetId();
          if (RecHitSubDet != (uint16_t)MuonSubdetId::CSC) continue;
          CSCDetId SegmentCSCDetId = CSCDetId(RecHitId);
          if (not(SegmentCSCDetId.station() == 1 and SegmentCSCDetId.ring() == 1 and RecHit->dimension() == 4)) continue;
          RecSegment* Rec_segment = (RecSegment*)RecHit;
          const CSCSegment* ME11_segment = (CSCSegment*)Rec_segment;
          auto SegmentCSCDetIdL4 = CSCDetId(SegmentCSCDetId.endcap(), SegmentCSCDetId.station(), SegmentCSCDetId.ring(), SegmentCSCDetId.chamber(), 4);
          const CSCLayer* ME11_layer = CSCGeometry_->layer(SegmentCSCDetIdL4);
          const CSCLayerGeometry* ME11_layer_geo = ME11_layer->geometry();
          if (not(SegmentCSCDetId == LCTDetId)) continue;
          if (debug) cout << "Found a muon on the same chamber as the LCTs" << endl;
          float LCT_strip = digiItr->getFractionalStrip();
          float Seg_strip = ME11_layer_geo->strip(ME11_segment->localPosition());
          if (abs(LCT_strip - Seg_strip) < closest_match_distance){
            if (debug) cout << "Found a new match! Pt " << mu->pt() << " and slope " << digiItr->getSlope() << endl;
            muon_match_pt = mu->pt();
            muon_match_charge = mu->charge();
            muon_match_distance = abs(LCT_strip - Seg_strip);
          }
        }
      }
      data_.LCT_slope = digiItr->getSlope();
      data_.LCT_quality = digiItr->getQuality();
      data_.LCT_strip = digiItr->getStrip();
      data_.LCT_wiregroup = digiItr->getKeyWG();
      data_.LCT_bend = digiItr->getBend();

      data_.muon_pt = muon_match_pt;
      data_.muon_charge = muon_match_charge;
      data_.muon_LCT_distance = muon_match_distance;
      tree->Fill();
    }
  }
}




void CSCEmulatorReader::beginJob(){
  cout << "Begin job!" << endl;
  cout << "Ended Begin Job, starting Event Loop" << endl;
}
void CSCEmulatorReader::endJob(){}

DEFINE_FWK_MODULE(CSCEmulatorReader);
