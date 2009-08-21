import FWCore.ParameterSet.Config as cms

LJFilter = cms.EDFilter('LeptonJetFilter',
                        # Specify number of muons
                        muonsMin = cms.int32(1),
                        muonsMax = cms.int32(-1),
                        muPT     = cms.double(20.),

                        # Specify number of electrons
                        electronsMin = cms.int32(1),
                        electronsMax = cms.int32(-1),
                        elecPT       = cms.double(20.),

                        # Specifiy number of jets
                        jetsMin = cms.int32(-1),
                        jetsMax = cms.int32(-1),
                        jetPT   = cms.double(30.),

                        debug   = cms.untracked.bool(False),
                        # If true, filter is true if either lepton condition met
                        counteitherleptontype = cms.untracked.bool(True),

                        muLabel = cms.untracked.InputTag("cleanLayer1Muons"),
                        elecLabel = cms.untracked.InputTag("cleanLayer1Electrons"),
                        jetLabel = cms.untracked.InputTag("cleanLayer1Jets"),
)

