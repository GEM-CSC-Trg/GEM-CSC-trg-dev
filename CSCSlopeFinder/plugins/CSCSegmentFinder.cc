#include <assert.h>
#include <memory>
#include <cmath>
#include <iostream>
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


struct MuonData
{
  void init();
  TTree* book(TTree *t);
  //============ Muon Info ================//
  int muon_charge; float muon_pt; float muon_eta;
  unsigned long long evtNum; unsigned long long lumiBlock; int runNum;

  //============ CSC Info =================//
  float CSC_Segment_GP[3]; float CSC_Segment_LP[3];
  float CSC_Segment_GlobalDirection[3]; float CSC_Segment_LocalDirection[3];
  float CSC_Segment_GEM1_Prop_GP[3]; float CSC_Segment_GEM1_Prop_LP[3];
  float CSC_Segment_GEM2_Prop_GP[3]; float CSC_Segment_GEM2_Prop_LP[3];

  //============ Track Info ===============//
  float Track_At_CSC_GP[3]; float Track_At_CSC_LP[3];
  float Track_At_CSC_GlobalDirection[3]; float Track_At_CSC_LocalDirection[3];
  float Track_Chi2; float Track_ndof;
  float Track_At_CSC_GEM1_Prop_GP[3]; float Track_At_CSC_GEM1_Prop_LP[3];
  float Track_At_CSC_GEM2_Prop_GP[3]; float Track_At_CSC_GEM2_Prop_LP[3];

  //============ GEM Info ================//
  float GEM1_SegmentMatch_GP[3]; float GEM1_SegmentMatch_LP[3];
  float GEM2_SegmentMatch_GP[3]; float GEM2_SegmentMatch_LP[3];
  float GEM1_TrackMatch_GP[3]; float GEM1_TrackMatch_LP[3];
  float GEM2_TrackMatch_GP[3]; float GEM2_TrackMatch_LP[3];
  bool Has_GEM1_SegmentMatch; bool Has_GEM2_SegmentMatch;
  bool Has_GEM1_TrackMatch; bool Has_GEM2_TrackMatch;
};

void MuonData::init()
{
  //=========== Muon Info ===============//
  float value = 99999;
  muon_charge = value; muon_pt = value; muon_eta = value;
  evtNum = value; lumiBlock = value; runNum = value;

  //=========== CSC Info ================//
  for(int i=0; i<3; ++i){
    CSC_Segment_GP[i] = value; CSC_Segment_LP[i] = value;
    CSC_Segment_GlobalDirection[i] = value; CSC_Segment_LocalDirection[i] = value;
    CSC_Segment_GEM1_Prop_GP[i] = value; CSC_Segment_GEM1_Prop_LP[i] = value;
    CSC_Segment_GEM2_Prop_GP[i] = value; CSC_Segment_GEM2_Prop_LP[i] = value;
  }

  //============ Track Info ===============//
  for(int i=0; i<3; ++i){
    Track_At_CSC_GP[i] = value; Track_At_CSC_LP[i] = value;
    Track_At_CSC_GlobalDirection[i] = value; Track_At_CSC_LocalDirection[i] = value;
    Track_At_CSC_GEM1_Prop_GP[i] = value; Track_At_CSC_GEM1_Prop_LP[i] = value;
    Track_At_CSC_GEM2_Prop_GP[i] = value; Track_At_CSC_GEM2_Prop_LP[i] = value;
  }
  Track_Chi2 = value; Track_ndof = value;

  //============ GEM Info ================//
  for(int i=0; i<3; ++i){
    GEM1_SegmentMatch_GP[i] = value; GEM1_SegmentMatch_LP[i] = value;
    GEM2_SegmentMatch_GP[i] = value; GEM2_SegmentMatch_LP[i] = value;
    GEM1_TrackMatch_GP[i] = value; GEM1_TrackMatch_LP[i] = value;
    GEM2_TrackMatch_GP[i] = value; GEM2_TrackMatch_LP[i] = value;
  }
  Has_GEM1_SegmentMatch = false; Has_GEM2_SegmentMatch = false;
  Has_GEM1_TrackMatch = false; Has_GEM2_TrackMatch = false;
}

TTree* MuonData::book(TTree *t){
  edm::Service< TFileService > fs;
  t = fs->make<TTree>("GEM_CSC_Trigger", "GEM_CSC_Trigger");

  //=========== Muon Info =============//
  t->Branch("muon_charge", &muon_charge); t->Branch("muon_pt", &muon_pt); t->Branch("muon_eta", &muon_eta);
  t->Branch("evtNum", &evtNum); t->Branch("lumiBlock", &lumiBlock); t->Branch("runNum", &runNum);

  //=========== CSC Info ================//
  t->Branch("CSC_Segment_GP", &CSC_Segment_GP, "CSC_Segment_GP[3] (x,y,z)/F");
  t->Branch("CSC_Segment_LP", &CSC_Segment_LP, "CSC_Segment_LP[3] (x,y,z)/F");
  t->Branch("CSC_Segment_GlobalDirection", &CSC_Segment_GlobalDirection, "CSC_Segment_GlobalDirection[3] (x,y,z)/F");
  t->Branch("CSC_Segment_LocalDirection", &CSC_Segment_LocalDirection, "CSC_Segment_LocalDirection[3] (x,y,z)/F");
  t->Branch("CSC_Segment_GEM1_Prop_GP", &CSC_Segment_GEM1_Prop_GP, "CSC_Segment_GEM1_Prop_GP[3] (x,y,z)/F");
  t->Branch("CSC_Segment_GEM2_Prop_GP", &CSC_Segment_GEM2_Prop_GP, "CSC_Segment_GEM2_Prop_GP[3] (x,y,z)/F");
  t->Branch("CSC_Segment_GEM1_Prop_LP", &CSC_Segment_GEM1_Prop_LP, "CSC_Segment_GEM1_Prop_LP[3] (x,y,z)/F");
  t->Branch("CSC_Segment_GEM2_Prop_LP", &CSC_Segment_GEM2_Prop_LP, "CSC_Segment_GEM2_Prop_LP[3] (x,y,z)/F");

  //============ Track Info ===============//
  t->Branch("Track_At_CSC_GP", &Track_At_CSC_GP, "Track_At_CSC_GP[3] (x,y,z)/F");
  t->Branch("Track_At_CSC_LP", &Track_At_CSC_LP, "Track_At_CSC_LP[3] (x,y,z)/F");
  t->Branch("Track_At_CSC_GlobalDirection", &Track_At_CSC_GlobalDirection, "Track_At_CSC_GlobalDirection[3] (x,y,z)/F");
  t->Branch("Track_At_CSC_LocalDirection", &Track_At_CSC_LocalDirection, "Track_At_CSC_LocalDirection[3] (x,y,z)/F");
  t->Branch("Track_Chi2", &Track_Chi2);
  t->Branch("Track_ndof", &Track_ndof);
  t->Branch("Track_At_CSC_GEM1_Prop_GP", &Track_At_CSC_GEM1_Prop_GP, "Track_At_CSC_GEM1_Prop_GP[3] (x,y,z)/F");
  t->Branch("Track_At_CSC_GEM2_Prop_GP", &Track_At_CSC_GEM2_Prop_GP, "Track_At_CSC_GEM2_Prop_GP[3] (x,y,z)/F");
  t->Branch("Track_At_CSC_GEM1_Prop_LP", &Track_At_CSC_GEM1_Prop_LP, "Track_At_CSC_GEM1_Prop_LP[3] (x,y,z)/F");
  t->Branch("Track_At_CSC_GEM2_Prop_LP", &Track_At_CSC_GEM2_Prop_LP, "Track_At_CSC_GEM2_Prop_LP[3] (x,y,z)/F");

  //============ GEM Info ================//
  t->Branch("GEM1_SegmentMatch_GP", &GEM1_SegmentMatch_GP, "GEM1_SegmentMatch_GP[3] (x,y,z)/F");
  t->Branch("GEM1_SegmentMatch_LP", &GEM1_SegmentMatch_LP, "GEM1_SegmentMatch_LP[3] (x,y,z)/F");
  t->Branch("GEM2_SegmentMatch_GP", &GEM2_SegmentMatch_GP, "GEM2_SegmentMatch_GP[3] (x,y,z)/F");
  t->Branch("GEM2_SegmentMatch_LP", &GEM2_SegmentMatch_LP, "GEM2_SegmentMatch_LP[3] (x,y,z)/F");
  t->Branch("GEM1_TrackMatch_GP", &GEM1_TrackMatch_GP, "GEM1_TrackMatch_GP[3] (x,y,z)/F");
  t->Branch("GEM1_TrackMatch_LP", &GEM1_TrackMatch_LP, "GEM1_TrackMatch_LP[3] (x,y,z)/F");
  t->Branch("GEM2_TrackMatch_GP", &GEM2_TrackMatch_GP, "GEM2_TrackMatch_GP[3] (x,y,z)/F");
  t->Branch("GEM2_TrackMatch_LP", &GEM2_TrackMatch_LP, "GEM2_TrackMatch_LP[3] (x,y,z)/F");
  t->Branch("Has_GEM1_SegmentMatch", &Has_GEM1_SegmentMatch);
  t->Branch("Has_GEM2_SegmentMatch", &Has_GEM2_SegmentMatch);
  t->Branch("Has_GEM1_TrackMatch", &Has_GEM1_TrackMatch);
  t->Branch("Has_GEM2_TrackMatch", &Has_GEM2_TrackMatch);
  return t;
}

class CSCSegmentFinder : public edm::one::EDAnalyzer<> {
public:
  explicit CSCSegmentFinder(const edm::ParameterSet&);
  ~CSCSegmentFinder(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;

  void FindCSCSegment(const reco::Muon* mu, MuonData& data_);
  void FindTrackAtCSC(const reco::Muon* mu, MuonData& data_);
  void PropagateToGEM(const reco::Muon* mu, MuonData& data_);

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

  MuonData data_;
  TTree* tree;

  bool isMC;
  const CSCSegment *ME11_Segment;

  const edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeomToken_;
  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeomToken_;
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttkToken_;
  const edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> geomToken_;
};


CSCSegmentFinder::CSCSegmentFinder(const edm::ParameterSet& iConfig)
  : gemGeomToken_(esConsumes()),
    cscGeomToken_(esConsumes()),
    ttkToken_(esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))),
    geomToken_(esConsumes())
{
  cout << "Begin CSCSegmentFinder" << endl;
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService_ = new MuonServiceProxy(serviceParameters, consumesCollector());
  muons_ = consumes<View<reco::Muon> >(iConfig.getParameter<InputTag>("muons"));
  vertexCollection_ = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"));
  gemRecHits_ = consumes<GEMRecHitCollection>(iConfig.getParameter<edm::InputTag>("gemRecHits"));
  cscSegments_ = consumes<CSCSegmentCollection>(edm::InputTag("cscSegments"));

  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree);

}


void
CSCSegmentFinder::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

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

  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;
  if (! iEvent.getByToken(muons_, muons)) return;
  if (debug) cout << "There are " << muons->size() << " muons in event" << endl;
  if (muons->size() == 0) return;

  edm::Handle<CSCSegmentCollection> cscSegments;
  if (! iEvent.getByToken(cscSegments_, cscSegments)){std::cout << "Bad segments" << std::endl;}

  for (size_t i = 0; i < muons->size(); ++i){
    edm::RefToBase<reco::Muon> muRef = muons->refAt(i);
    const reco::Muon* mu = muRef.get();
    if (debug) cout << "new muon, i = " << i << " is it global? " << mu->isGlobalMuon() << endl;

    if (not mu->isGlobalMuon()) continue;
    
    data_.init();
    data_.muon_pt = mu->pt();
    FindCSCSegment(mu, data_);
    if (data_.CSC_Segment_GP[0] > 1000) continue;
    FindTrackAtCSC(mu, data_);
    if (data_.Track_At_CSC_GP[0] > 1000) continue;
    PropagateToGEM(mu, data_);
    tree->Fill();
  }
}

void CSCSegmentFinder::FindCSCSegment(const reco::Muon* mu, MuonData& data_){
  if(!(mu->isStandAloneMuon())){return;}
  const reco::Track* Track = mu->outerTrack().get();
  if (Track->validFraction() > 0.0) return;
  for (size_t RecHit_iter = 0; RecHit_iter != Track->recHitsSize(); RecHit_iter++){
    const TrackingRecHit* RecHit = (Track->recHit(RecHit_iter)).get();
    DetId RecHitId = RecHit->geographicalId();
    uint16_t RecHitDetId = RecHitId.det();
    if (RecHitDetId == DetId::Muon){
      uint16_t RecHitSubDet = RecHitId.subdetId();
      if (RecHitSubDet == (uint16_t)MuonSubdetId::CSC){
        if (CSCDetId(RecHitId).station() == 1 and CSCDetId(RecHitId).ring() == 1 and RecHit->dimension() == 4){
          RecSegment* Rec_Segment = (RecSegment*)RecHit;
          //CSCSegment* ME11_Segment = (CSCSegment*)Rec_Segment;
          ME11_Segment = (CSCSegment*)Rec_Segment;
          if (debug) cout << "Found a segment!" << endl;


          auto cscDetID_FAKE = CSCDetId(CSCDetId(RecHitId).endcap(), CSCDetId(RecHitId).station(), CSCDetId(RecHitId).ring(), CSCDetId(RecHitId).chamber(), 3);
          const CSCLayer* tmp_ME11_layer = CSCGeometry_->layer(cscDetID_FAKE);
          const CSCLayerGeometry* tmp_ME11_layer_geo = tmp_ME11_layer->geometry();

          LocalPoint ME11_Segment_LP = ME11_Segment->localPosition();
          GlobalPoint ME11_Segment_GP = tmp_ME11_layer->toGlobal(ME11_Segment_LP);
          LocalVector ME11_Segment_LocalDir = ME11_Segment->localDirection();
          GlobalVector ME11_Segment_GlobalDir = tmp_ME11_layer->toGlobal(ME11_Segment_LocalDir);

          data_.CSC_Segment_GP[0] = ME11_Segment_GP.x(); data_.CSC_Segment_GP[1] = ME11_Segment_GP.y(); data_.CSC_Segment_GP[2] = ME11_Segment_GP.z();
          data_.CSC_Segment_LP[0] = ME11_Segment_LP.x(); data_.CSC_Segment_LP[1] = ME11_Segment_LP.y(); data_.CSC_Segment_LP[2] = ME11_Segment_LP.z();
          data_.CSC_Segment_GlobalDirection[0] = ME11_Segment_GlobalDir.x(); data_.CSC_Segment_GlobalDirection[1] = ME11_Segment_GlobalDir.y(); data_.CSC_Segment_GlobalDirection[2] = ME11_Segment_GlobalDir.z();
          data_.CSC_Segment_LocalDirection[0] = ME11_Segment_LocalDir.x(); data_.CSC_Segment_LocalDirection[1] = ME11_Segment_LocalDir.y(); data_.CSC_Segment_LocalDirection[2] = ME11_Segment_LocalDir.z();
        }
      }
    }
  }
}


void CSCSegmentFinder::FindTrackAtCSC(const reco::Muon* mu, MuonData& data_){
  if (debug) cout << "Starting Track Finder" << endl;
  if (!(mu->isStandAloneMuon())) return;
  const reco::Track* Track = mu->outerTrack().get();
  if (Track->validFraction() > 0.0) return;
  reco::TransientTrack TTrack = ttrackBuilder_->build(Track);

  GlobalPoint SegmentPosition = GlobalPoint(data_.CSC_Segment_GP[0], data_.CSC_Segment_GP[1], data_.CSC_Segment_GP[2]);

  TrajectoryStateOnSurface track_at_segment = TTrack.stateOnSurface(SegmentPosition);
  if (!(track_at_segment.isValid())) return;

  GlobalPoint TrackAtCSCGP = track_at_segment.globalPosition();
  LocalPoint TrackAtCSCLP = track_at_segment.localPosition();
  GlobalVector TrackAtCSCGlobalDirection = track_at_segment.globalDirection();
  LocalVector TrackAtCSCLocalDirection = track_at_segment.localDirection();


  data_.Track_At_CSC_GP[0] = TrackAtCSCGP.x(); data_.Track_At_CSC_GP[1] = TrackAtCSCGP.y(); data_.Track_At_CSC_GP[2] = TrackAtCSCGP.z();
  data_.Track_At_CSC_LP[0] = TrackAtCSCLP.x(); data_.Track_At_CSC_LP[1] = TrackAtCSCLP.y(); data_.Track_At_CSC_LP[2] = TrackAtCSCLP.z();
  data_.Track_At_CSC_GlobalDirection[0] = TrackAtCSCGlobalDirection.x(); data_.Track_At_CSC_GlobalDirection[1] = TrackAtCSCGlobalDirection.y(); data_.Track_At_CSC_GlobalDirection[2] = TrackAtCSCGlobalDirection.z();
  data_.Track_At_CSC_LocalDirection[0] = TrackAtCSCLocalDirection.x(); data_.Track_At_CSC_LocalDirection[1] = TrackAtCSCLocalDirection.y(); data_.Track_At_CSC_LocalDirection[2] = TrackAtCSCLocalDirection.z();
  data_.Track_Chi2 = Track->chi2(); data_.Track_ndof = Track->ndof();

}

void CSCSegmentFinder::PropagateToGEM(const reco::Muon* mu, MuonData& data_){
  const reco::Track* Track = mu->outerTrack().get();

  //Set Up Segment Propagation
  DetId segDetId = ME11_Segment->geographicalId();
  const GeomDet* segDet = theTrackingGeometry->idToDet(segDetId);
  LocalVector momentum_at_surface = (Track->outerP()) * (ME11_Segment->localDirection());
  LocalTrajectoryParameters param(ME11_Segment->localPosition(), momentum_at_surface, mu->charge());
  AlgebraicSymMatrix mat(5,0);
  mat = ME11_Segment->parametersError().similarityT(ME11_Segment->projectionMatrix());
  LocalTrajectoryError error(asSMatrix<5>(mat));
  TrajectoryStateOnSurface TSOS_Segment(param, error, segDet->surface(), &*theService_->magneticField());

  //Set Up Track Propagation
  reco::TransientTrack TTrack = ttrackBuilder_->build(Track);
  GlobalPoint SegmentPosition = GlobalPoint(data_.CSC_Segment_GP[0], data_.CSC_Segment_GP[1], data_.CSC_Segment_GP[2]);
  TrajectoryStateOnSurface TSOS_Track = TTrack.stateOnSurface(SegmentPosition); 

  auto propagator = theService_->propagator("SteppingHelixPropagatorAny");

  if (debug) cout << "New muon to propagate!" << endl;
  for(const auto& ch : GEMGeometry_->etaPartitions()){
    if (ch->id().station() != 1) continue; // Only GE1/1 for now
    const BoundPlane& bps(ch->surface());
    const auto& etaPart_ch = GEMGeometry_->etaPartition(ch->id());

    TrajectoryStateOnSurface TSOS_GEM_From_Segment = propagator->propagate(TSOS_Segment, ch->surface());
    if (TSOS_GEM_From_Segment.isValid()){
      const GlobalPoint Prop_GEM_GP = TSOS_GEM_From_Segment.globalPosition();
      const LocalPoint Prop_GEM_LP = ch->toLocal(Prop_GEM_GP);
      const LocalPoint Prop_GEM_LP_2D(Prop_GEM_LP.x(), Prop_GEM_LP.y(), 0.0);
      if (((TSOS_Segment.globalPosition().z() * Prop_GEM_GP.z()) > 0) and (bps.bounds().inside(Prop_GEM_LP_2D)) and (ch->id().station() == 1 and ch->id().ring() == 1)){
        if (debug) cout << "Found a propagation! " << TSOS_GEM_From_Segment.globalPosition() << endl;
        if (debug) cout << ch->id().layer() << endl;
        if (ch->id().layer() == 1){
          data_.CSC_Segment_GEM1_Prop_GP[0] = Prop_GEM_GP.x(); data_.CSC_Segment_GEM1_Prop_GP[1] = Prop_GEM_GP.y(); data_.CSC_Segment_GEM1_Prop_GP[2] = Prop_GEM_GP.z();
          data_.CSC_Segment_GEM1_Prop_LP[0] = Prop_GEM_LP.x(); data_.CSC_Segment_GEM1_Prop_LP[1] = Prop_GEM_LP.y(); data_.CSC_Segment_GEM1_Prop_LP[2] = Prop_GEM_LP.z();
        }
        if (ch->id().layer() == 2){
          data_.CSC_Segment_GEM2_Prop_GP[0] = Prop_GEM_GP.x(); data_.CSC_Segment_GEM2_Prop_GP[1] = Prop_GEM_GP.y(); data_.CSC_Segment_GEM2_Prop_GP[2] = Prop_GEM_GP.z();
          data_.CSC_Segment_GEM2_Prop_LP[0] = Prop_GEM_LP.x(); data_.CSC_Segment_GEM2_Prop_LP[1] = Prop_GEM_LP.y(); data_.CSC_Segment_GEM2_Prop_LP[2] = Prop_GEM_LP.z();
        }
      }
    }

    TrajectoryStateOnSurface TSOS_GEM_From_Track = propagator->propagate(TSOS_Track, ch->surface());
    if (TSOS_GEM_From_Track.isValid()){
      const GlobalPoint Prop_GEM_GP = TSOS_GEM_From_Track.globalPosition();
      const LocalPoint Prop_GEM_LP = ch->toLocal(Prop_GEM_GP);
      const LocalPoint Prop_GEM_LP_2D(Prop_GEM_LP.x(), Prop_GEM_LP.y(), 0.0);
      if (((TSOS_Track.globalPosition().z() * Prop_GEM_GP.z()) > 0) and (bps.bounds().inside(Prop_GEM_LP_2D)) and (ch->id().station() == 1 and ch->id().ring() == 1)){
        if (debug) cout << "Found a propagation! " << TSOS_GEM_From_Track.globalPosition() << endl;
        if (debug) cout << ch->id().layer() << endl;
        if (ch->id().layer() == 1){
          data_.Track_At_CSC_GEM1_Prop_GP[0] = Prop_GEM_GP.x(); data_.Track_At_CSC_GEM1_Prop_GP[1] = Prop_GEM_GP.y(); data_.Track_At_CSC_GEM1_Prop_GP[2] = Prop_GEM_GP.z();
          data_.Track_At_CSC_GEM1_Prop_LP[0] = Prop_GEM_LP.x(); data_.Track_At_CSC_GEM1_Prop_LP[1] = Prop_GEM_LP.y(); data_.Track_At_CSC_GEM1_Prop_LP[2] = Prop_GEM_LP.z();
        }
        if (ch->id().layer() == 2){
          data_.Track_At_CSC_GEM2_Prop_GP[0] = Prop_GEM_GP.x(); data_.Track_At_CSC_GEM2_Prop_GP[1] = Prop_GEM_GP.y(); data_.Track_At_CSC_GEM2_Prop_GP[2] = Prop_GEM_GP.z();
          data_.Track_At_CSC_GEM2_Prop_LP[0] = Prop_GEM_LP.x(); data_.Track_At_CSC_GEM2_Prop_LP[1] = Prop_GEM_LP.y(); data_.Track_At_CSC_GEM2_Prop_LP[2] = Prop_GEM_LP.z();
        }
      }
    }
    //Here we will loop over all rechits for each SUCCESSFUL PROP
    float delta_x_track = 999.9;
    float delta_x_segment = 999.9;
    for(auto hit = gemRecHits->begin(); hit != gemRecHits->end(); hit++){
      GEMDetId gemid((hit)->geographicalId());
      if(!(gemid.det() == DetId::Detector::Muon && gemid.subdetId() == MuonSubdetId::GEM)) continue;
      if(!(gemid.station() == ch->id().station() and gemid.chamber() == ch->id().chamber() and gemid.layer() == ch->id().layer() and abs(gemid.roll() - ch->id().roll()) <= 1 and gemid.region() == ch->id().region())) continue;
      const auto& etaPart = GEMGeometry_->etaPartition(gemid);
      LocalPoint hit_LP = hit->localPosition();
      GlobalPoint hit_GP = etaPart->toGlobal(hit_LP);

      if (ch->id().layer() == 1){
        if (abs(hit_LP.x() - data_.Track_At_CSC_GEM1_Prop_LP[0]) < delta_x_track){
          data_.Has_GEM1_TrackMatch = true;
          delta_x_track = hit_LP.x() - data_.Track_At_CSC_GEM1_Prop_LP[0];
          data_.GEM1_TrackMatch_GP[0] = hit_GP.x(); data_.GEM1_TrackMatch_GP[1] = hit_GP.y(); data_.GEM1_TrackMatch_GP[2] = hit_GP.z();
          data_.GEM1_TrackMatch_LP[0] = hit_LP.x(); data_.GEM1_TrackMatch_LP[1] = hit_LP.y(); data_.GEM1_TrackMatch_LP[2] = hit_LP.z();
        }
        if (abs(hit_LP.x() - data_.CSC_Segment_GEM1_Prop_LP[0]) < delta_x_segment){
          data_.Has_GEM1_SegmentMatch = true;
          delta_x_segment = hit_LP.x() - data_.CSC_Segment_GEM1_Prop_LP[0];
          data_.GEM1_SegmentMatch_GP[0] = hit_GP.x(); data_.GEM1_SegmentMatch_GP[1] = hit_GP.y(); data_.GEM1_SegmentMatch_GP[2] = hit_GP.z();
          data_.GEM1_SegmentMatch_LP[0] = hit_LP.x(); data_.GEM1_SegmentMatch_LP[1] = hit_LP.y(); data_.GEM1_SegmentMatch_LP[2] = hit_LP.z();
        }
      }
      if (ch->id().layer() == 2){
        if (abs(hit_LP.x() - data_.Track_At_CSC_GEM2_Prop_LP[0]) < delta_x_track){
          data_.Has_GEM2_TrackMatch = true;
          delta_x_track = hit_LP.x() - data_.Track_At_CSC_GEM2_Prop_LP[0];
          data_.GEM2_TrackMatch_GP[0] = hit_GP.x(); data_.GEM2_TrackMatch_GP[1] = hit_GP.y(); data_.GEM2_TrackMatch_GP[2] = hit_GP.z();
          data_.GEM2_TrackMatch_LP[0] = hit_LP.x(); data_.GEM2_TrackMatch_LP[1] = hit_LP.y(); data_.GEM2_TrackMatch_LP[2] = hit_LP.z();
        }
        if (abs(hit_LP.x() - data_.CSC_Segment_GEM2_Prop_LP[0]) < delta_x_segment){
          data_.Has_GEM2_SegmentMatch = true;
          delta_x_segment = hit_LP.x() - data_.CSC_Segment_GEM2_Prop_LP[0];
          data_.GEM2_SegmentMatch_GP[0] = hit_GP.x(); data_.GEM2_SegmentMatch_GP[1] = hit_GP.y(); data_.GEM2_SegmentMatch_GP[2] = hit_GP.z();
          data_.GEM2_SegmentMatch_LP[0] = hit_LP.x(); data_.GEM2_SegmentMatch_LP[1] = hit_LP.y(); data_.GEM2_SegmentMatch_LP[2] = hit_LP.z();
        }
      }
    }
  }
}




void CSCSegmentFinder::beginJob(){}
void CSCSegmentFinder::endJob(){}

DEFINE_FWK_MODULE(CSCSegmentFinder);
