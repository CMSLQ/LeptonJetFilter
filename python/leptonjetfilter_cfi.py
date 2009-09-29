import FWCore.ParameterSet.Config as cms

LJFilter = cms.EDFilter('LeptonJetFilter',
    # Specify input collections
    muLabel = cms.untracked.InputTag("cleanLayer1Muons"),
    elecLabel = cms.untracked.InputTag("cleanLayer1Electrons"),
    jetLabel = cms.untracked.InputTag("cleanLayer1Jets"),

    # Specifiy number of jets
    jetsMin = cms.int32(-1),
    jetsMax = cms.int32(-1),
    jetPT   = cms.double(30.),
    jetEta  = cms.double(100.),

    # Specify number of muons
    muonsMin = cms.int32(1),
    muonsMax = cms.int32(-1),
    muPT     = cms.double(20.),
    muEta    = cms.double(100.),
    useMuID  = cms.bool(False),
    muID     = cms.string("GlobalMuonPromptTight"),

    # Specify number of electrons
    electronsMin = cms.int32(1),
    electronsMax = cms.int32(-1),
    elecPT       = cms.double(20.),
    elecEta      = cms.double(100.),
    useElecID    = cms.bool(False),
    elecID       = cms.string("eidTight"),

    debug   = cms.untracked.bool(False),
    # If true, filter is true if either lepton condition met
    counteitherleptontype = cms.bool(True)
)

