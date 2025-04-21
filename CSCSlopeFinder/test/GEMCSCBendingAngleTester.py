import FWCore.ParameterSet.Config as cms
#from Configuration.Eras.Era_Phase2C9_cff import Phase2C9
#from Configuration.Eras.Era_Run3_cff import Run3
from Configuration.Eras.Era_Phase2_cff import Phase2

#process = cms.Process('analyzer',Phase2C9)
#process = cms.Process('analyzer',Run3)
process = cms.Process('analyzer', Phase2)

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('RecoMuon.TrackingTools.MuonServiceProxy_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('TrackingTools.TransientTrack.TransientTrackBuilder_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

from Configuration.AlCa.GlobalTag import GlobalTag



process.GlobalTag = GlobalTag(process.GlobalTag, '140X_dataRun3_HLT_v3', '')


process.MessageLogger.cerr.FwkReport.reportEvery = 5000

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.register ('nEvents',
			-1, #Max number of events 
			VarParsing.multiplicity.singleton, 
			VarParsing.varType.int, 
			"Number of events")
options.parseArguments()

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(options.nEvents)
)
process.maxEvents.input = cms.untracked.int32(-1)


process.source = cms.Source("PoolSource", 
				fileNames = cms.untracked.vstring(options.inputFiles), 
				inputCommands = cms.untracked.vstring(
			"keep *", 
			"drop TotemTimingDigiedmDetSetVector_totemTimingRawToDigi_TotemTiming_reRECO", 
			"drop TotemTimingRecHitedmDetSetVector_totemTimingRecHits__reRECO"
			)
				)

#outfile = "out_LCT_test.root"
#process.source.fileNames.append("file:lcts2_withGEM.root")


process.options = cms.untracked.PSet(
      TryToContinue = cms.untracked.vstring('ProductNotFound')
)

#process.TFileService = cms.Service("TFileService", fileName = cms.string(outfile)) #variable name set above
process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputFile))


process.GEMCSCBendingAngleTester = cms.EDAnalyzer('GEMCSCBendingAngleTester', 
	process.MuonServiceProxy,
        l1_muon_token = cms.InputTag("gmtStage2Digis", "Muon"),
        emtf_muon_token = cms.InputTag("gmtStage2Digis", "EMTF"),
        emtf_track_token = cms.InputTag("emtfStage2Digis", "", "L1CSCTPG"),
        corrlctDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCCorrelatedLCTDigi"),
        gemPadDigiCluster = cms.InputTag("muonCSCDigis", "MuonGEMPadDigiCluster"),
        muon_token = cms.InputTag("muons"),
		vertexCollection_token = cms.InputTag("offlinePrimaryVertices"),
        luts_folder = cms.string("../luts"),
        alignment = cms.bool(True),
        debug = cms.bool(False),
)

process.p = cms.Path(process.GEMCSCBendingAngleTester)
