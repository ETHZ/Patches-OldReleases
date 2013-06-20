
import FWCore.ParameterSet.Config as cms

recovRecHitFilter = cms.EDFilter(
  "RecovRecHitFilter",
  EERecHitSource = cms.InputTag("reducedEcalRecHitsEE"),
  MinRecovE = cms.double(30),
  TaggingMode = cms.bool(False)
)
