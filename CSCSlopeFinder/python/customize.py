
# import FWCore.ParameterSet.Config as cms
# from PhysicsTools.NanoAOD.common_cff import Var, CandVars
# from RecoTauTag.RecoTau.tauIdWPsDefs import WORKING_POINTS_v2p5
# from PhysicsTools.NanoAOD.nano_eras_cff import *


# era_dependent_settings = cms.PSet(
#   cmsE = cms.double(13600.0), #center of mass energy in GeV (Run3 default)
# )
# (~run3_common).toModify(era_dependent_settings, cmsE = 13000.0)




# def customize(process):
#   #customize printout frequency
#   maxEvts = process.maxEvents.input.value()
#   if maxEvts > 10000 or maxEvts < 0:
#     process.MessageLogger.cerr.FwkReport.reportEvery = 1000
#   elif maxEvts > 10:
#     process.MessageLogger.cerr.FwkReport.reportEvery = maxEvts//10
#   #customize stored objects
#   process = customizeGenParticles(process)
#   process = customizeTaus(process)
#   process = customizeBoostedTaus(process)

#   from PhysicsTools.NanoAOD.leptonTimeLifeInfo_common_cff import addTrackVarsToTimeLifeInfo
#   process = addTrackVarsToTimeLifeInfo(process)
#   process = addIPCovToLeptons(process)

#   process = customizePV(process)

#   process = addSpinnerWeights(process)

#   return process
