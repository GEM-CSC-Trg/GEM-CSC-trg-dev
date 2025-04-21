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


struct ZMuData
{
  void init();
  TTree* book(TTree *t);
  //============ Muon Info ================//
  int muon1_charge; float muon1_pt; float muon1_eta;
  int muon2_charge; float muon2_pt; float muon2_eta;
  float dimuon_mass;
  unsigned long long evtNum; unsigned long long lumiBlock; int runNum;

};

void ZMuData::init()
{
  //=========== Muon Info ===============//
  float value = 99999;
  muon1_charge = value; muon1_pt = value; muon1_eta = value;
  muon2_charge = value; muon2_pt = value; muon2_eta = value;
  dimuon_mass = value;
  evtNum = value; lumiBlock = value; runNum = value;

}

TTree* ZMuData::book(TTree *t){
  edm::Service< TFileService > fs;
  t = fs->make<TTree>("ZMuonTree", "ZMuonTree");

  //=========== Muon Info =============//
  t->Branch("muon1_charge", &muon1_charge); t->Branch("muon1_pt", &muon1_pt); t->Branch("muon1_eta", &muon1_eta);
  t->Branch("muon2_charge", &muon2_charge); t->Branch("muon2_pt", &muon2_pt); t->Branch("muon2_eta", &muon2_eta);
  t->Branch("dimuon_mass", &dimuon_mass);
  t->Branch("evtNum", &evtNum); t->Branch("lumiBlock", &lumiBlock); t->Branch("runNum", &runNum);

  return t;
}

class ZMuonFinder : public edm::one::EDAnalyzer<> {
public:
  explicit ZMuonFinder(const edm::ParameterSet&);
  ~ZMuonFinder(){};

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() ;
  virtual void endJob() ;



  edm::EDGetTokenT<edm::View<reco::Muon> > muons_;
  edm::EDGetTokenT<reco::VertexCollection> vertexCollection_;

  edm::Service<TFileService> fs;
  MuonServiceProxy* theService_;
  
  edm::ESHandle<Propagator> propagator_;
  edm::ESHandle<TransientTrackBuilder> ttrackBuilder_;

  ESHandle<GlobalTrackingGeometry> theTrackingGeometry;

  bool debug;

  ZMuData data_;
  TTree* tree;

  bool isMC;

  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttkToken_;
};


ZMuonFinder::ZMuonFinder(const edm::ParameterSet& iConfig)
{
  cout << "Begin ZMuonFinder" << endl;
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService_ = new MuonServiceProxy(serviceParameters, consumesCollector());
  muons_ = consumes<View<reco::Muon> >(iConfig.getParameter<InputTag>("muons"));

  debug = iConfig.getParameter<bool>("debug");
  std::cout << "debug " << debug << std::endl;

  tree = data_.book(tree);
}


void ZMuonFinder::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  theService_->update(iSetup);

  isMC = false;
  if (! iEvent.eventAuxiliary().isRealData()) isMC = true;
  edm::Handle<View<reco::Muon> > muons;

  if (debug) cout << "New! EventNumber = " << iEvent.eventAuxiliary().event() << " LumiBlock = " << iEvent.eventAuxiliary().luminosityBlock() << " RunNumber = " << iEvent.run() << endl;
  if (! iEvent.getByToken(muons_, muons)) return;
  if (muons->size() == 0) return;

  //cout << "Starting the corr LCT search" << endl;
  cout << "New Event" << endl;
  cout << "There are " << muons->size() << " muons in event" << endl;

  int bad_pair_counter = 0;
  int good_pair_counter = 0;
  for (size_t i = 0; i < muons->size(); ++i){
    //First loop to get a 'muon1'
    for (size_t j = 0; j < muons->size(); ++j){
      //Second loop to get a 'muon2'
      if ((i == j) or (j < i)) continue; //Skip duplicates and double counting
      edm::RefToBase<reco::Muon> muRef1 = muons->refAt(i);
      const reco::Muon* mu1 = muRef1.get();
      edm::RefToBase<reco::Muon> muRef2 = muons->refAt(j);
      const reco::Muon* mu2 = muRef2.get();

      //std::cout << "Found a pair of muons, lets find its invar mass" << std::endl;
      //std::cout << mu1->pt() << " " << mu1->eta() << " " << mu1->phi() << " " << mu1->mass() << std::endl;
      TLorentzVector mu1_lorentz = TLorentzVector();
      mu1_lorentz.SetPtEtaPhiM(mu1->pt(), mu1->eta(), mu1->phi(), mu1->mass());
      //std::cout << mu2->pt() << " " << mu2->eta() << " " << mu2->phi() << " " << mu2->mass() << std::endl;
      TLorentzVector mu2_lorentz = TLorentzVector();
      mu2_lorentz.SetPtEtaPhiM(mu2->pt(), mu2->eta(), mu2->phi(), mu2->mass());
      TLorentzVector dimuon = mu1_lorentz + mu2_lorentz;
      //std::cout << "DiMu mass " << dimuon.M() << std::endl;
      if (abs(dimuon.M() - 91) < 20){
        std::cout << "Good Z Mass!!! " << dimuon.M() << std::endl;
        std::cout << "Mu1 index " << i << " and Mu2 index " << j << std::endl;
        good_pair_counter++;
        data_.muon1_charge = mu1->charge();
        data_.muon1_pt = mu1->pt();
        data_.muon1_eta = mu1->eta();
        data_.muon2_charge = mu2->charge();
        data_.muon2_pt = mu2->pt();
        data_.muon2_eta = mu2->eta();
        data_.dimuon_mass = dimuon.M();
        data_.evtNum = iEvent.eventAuxiliary().event();
        data_.lumiBlock = iEvent.eventAuxiliary().luminosityBlock();
        data_.runNum = iEvent.run();
        tree->Fill();        
      }
      else{
        bad_pair_counter++;
      }
    }
  }
  std::cout << "End of event, number of good pairs = " << good_pair_counter << std::endl;
  std::cout << "End of event, number of bad pairs = " << bad_pair_counter << std::endl;
}




void ZMuonFinder::beginJob(){}
void ZMuonFinder::endJob(){}

DEFINE_FWK_MODULE(ZMuonFinder);
