import FWCore.ParameterSet.Config as cms

LJFilter = cms.EDFilter('LeptonJetFilter',
    # Specify input collections
    photLabel = cms.InputTag("slimmedPhotons"),
    tauLabel = cms.InputTag("slimmedTaus"),
    muLabel = cms.InputTag("slimmedMuons"),
    elecLabel = cms.InputTag("slimmedElectrons"),
    jetLabel = cms.InputTag("slimmedJets"),
    genLabel = cms.InputTag("prunedGenParticles"),

    # Specifiy number of photons
    photMin = cms.int32(-1),
    photMax = cms.int32(-1),
    photET   = cms.double(20.),
    photEta  = cms.double(3.),
    photHoE  = cms.double(0.05),
    usePhotID    = cms.bool(False),
    photID       = cms.string("XXXXXXXXX"),

    # Specifiy number of jets
    jetsMin = cms.int32(-1),
    jetsMax = cms.int32(-1),
    jetPT   = cms.double(17.),
    jetEta  = cms.double(5.),

    # Specify number of muons
    muonsMin = cms.int32(1),
    muonsMax = cms.int32(-1),
    muPT     = cms.double(8.),
    muEta    = cms.double(2.4),
    useMuID  = cms.bool(False),
    muID     = cms.string("GlobalMuonPromptTight"),

    # Specify number of electrons
    electronsMin = cms.int32(1),
    electronsMax = cms.int32(-1),
    elecPT       = cms.double(10.),
    elecEta      = cms.double(2.5),
    useElecID    = cms.bool(False),
    elecID       = cms.string("eidRobustTight"),

    # Specify number of taus
    tausMin  = cms.int32  (  1 ) ,
    tausMax  = cms.int32  ( -1 ) , 
    tauPT    = cms.double ( 15. ) , 
    tauEta   = cms.double ( 3.) , 
    useTauID = cms.bool ( False ),
    tauID    = cms.string("leadingPionPtCut") , 

    allEventsPassFilter = cms.untracked.bool(False),
    debug   = cms.untracked.bool(False),
    # If true, filter is true if either lepton condition met
    counteitherleptontype = cms.bool(True),
    customfilterEMuTauJet2012= cms.bool(False),
    customfilterEMuTauJet2016= cms.bool(False),
    customfilterEMuTauJetLegacy  = cms.bool(False),
    HHcustomfilterEMuTauJetLegacy= cms.bool(False)

)

