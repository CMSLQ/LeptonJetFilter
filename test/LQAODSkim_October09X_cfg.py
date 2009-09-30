import FWCore.ParameterSet.Config as cms

process = cms.Process("USER")

# initialize MessageLogger and output report
############## IMPORTANT ########################################
# if you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.cerr.default.limit = 100
#################################################################
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# source
process.source = cms.Source("PoolSource", 
     fileNames = cms.untracked.vstring('file:input_file.root')
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('skim_histo.root'),
    closeFileFast = cms.untracked.bool(True)
)

# Skim definition
process.load("Leptoquarks.LeptonJetFilter.leptonjetfilter_cfi")
process.LJFilter.muLabel = 'muons'
process.LJFilter.elecLabel = 'gsfElectrons'
process.LJFilter.jetLabel = 'iterativeCone5CaloJets'
process.LJFilter.counteitherleptontype = False
process.LJFilter.muonsMin = -1
process.LJFilter.elecPT = 30.

# Path definition
process.p = cms.Path( process.LJFilter )

# Output module configuration
process.load("Configuration.EventContent.EventContent_cff")
process.out = cms.OutputModule("PoolOutputModule",
   process.AODSIMEventContent,
   fileName = cms.untracked.string('LQAODSkim_output.root'),
   SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
   dataset = cms.untracked.PSet(
       filterName = cms.untracked.string('AODSIM'),
       dataTier = cms.untracked.string('AODSIM')
   )
)
process.outpath = cms.EndPath(process.out)
