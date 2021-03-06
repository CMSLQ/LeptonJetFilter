// -*- C++ -*-
//
// Package:    LeptonJetFilter
// Class:      LeptonJetFilter
//
/**\class LeptonJetFilter LeptonJetFilter.cc AnalysisCode/LeptonJetFilter/src/LeptonJetFilter.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Jeff Temple
//         Created:  Mon Aug  3 13:02:30 CEST 2009
// $Id: LeptonJetFilter.cc,v 1.14 2013/02/07 03:46:09 hsaka Exp $
//         Modified January 10, 2018; David M. Morse
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "RecoEgamma/EgammaTools/interface/HoECalculator.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1.h"

using namespace edm;
using namespace std;
using namespace reco;

//
// class declaration
//

class LeptonJetFilter : public edm::EDFilter {
   public:
      explicit LeptonJetFilter(const edm::ParameterSet&);
      ~LeptonJetFilter();

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      int photMin_, jetsMin_, muonsMin_, electronsMin_, tausMin_;
      int photMax_, jetsMax_, muonsMax_, electronsMax_, tausMax_;
      double  photET_, photEta_, photHoE_, jetPT_, jetEta_, muPT_, muEta_, elecPT_, elecEta_, tauPT_, tauEta_;
      bool useMuID_, useElecID_, usePhotID_, useTauID_;
      string muID_, elecID_, photID_, tauID_;
      bool debug_, allEventsPassFilter_;
      bool counteitherleptontype_;
      bool customfilterEMuTauJet2012_;
      bool customfilterEMuTauJet2016_;
      bool customfilterEMuTauJetLegacy_;
      bool HHcustomfilterEMuTauJetLegacy_;

      edm::EDGetTokenT<pat::PhotonCollection>   photonCollectionToken_;
      edm::EDGetTokenT<pat::JetCollection>      jetCollectionToken_;
      edm::EDGetTokenT<pat::TauCollection>      tauCollectionToken_;
      edm::EDGetTokenT<pat::MuonCollection>     muonCollectionToken_;
      edm::EDGetTokenT<pat::ElectronCollection> electronCollectionToken_;
      edm::EDGetTokenT<reco::GenParticleCollection> genPartInputToken_;

      edm::EDGetTokenT<LHEEventProduct>    lheEventProductToken_;

      float TotalCount;
      float PassedCount;
      float SumOfAMCAtNLOWeights;
      float SumOfTopPtReweights;

      edm::Service<TFileService> histServ;
      TH1F* hCount;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
LeptonJetFilter::LeptonJetFilter(const edm::ParameterSet& iConfig) :
  photonCollectionToken_  (consumes<pat::PhotonCollection >(iConfig.getParameter<edm::InputTag>("photLabel"))),
  jetCollectionToken_  (consumes<pat::JetCollection >(iConfig.getParameter<edm::InputTag>("jetLabel"))),
  tauCollectionToken_  (consumes<pat::TauCollection >(iConfig.getParameter<edm::InputTag>("tauLabel"))),
  muonCollectionToken_  (consumes<pat::MuonCollection >(iConfig.getParameter<edm::InputTag>("muLabel"))),
  electronCollectionToken_  (consumes<pat::ElectronCollection >(iConfig.getParameter<edm::InputTag>("elecLabel"))),
  genPartInputToken_ (consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genLabel")))
{
   //now do what ever initialization is needed


  photMin_   = iConfig.getParameter<int>("photMin");
  photMax_   = iConfig.getParameter<int>("photMax");
  photET_    = iConfig.getParameter<double>("photET");
  photEta_   = iConfig.getParameter<double>("photEta");
  photHoE_   = iConfig.getParameter<double>("photHoE");
  usePhotID_ = iConfig.getParameter<bool>("usePhotID");
  photID_    = iConfig.getParameter<string>("photID");

  jetsMin_ = iConfig.getParameter<int>("jetsMin");
  jetsMax_ = iConfig.getParameter<int>("jetsMax");
  jetPT_   = iConfig.getParameter<double>("jetPT");
  jetEta_  = iConfig.getParameter<double>("jetEta");

  muonsMin_ = iConfig.getParameter<int>("muonsMin");
  muonsMax_ = iConfig.getParameter<int>("muonsMax");
  muPT_     = iConfig.getParameter<double>("muPT");
  muEta_    = iConfig.getParameter<double>("muEta");
  useMuID_  = iConfig.getParameter<bool>("useMuID");
  muID_     = iConfig.getParameter<string>("muID");

  electronsMin_ = iConfig.getParameter<int>("electronsMin");
  electronsMax_ = iConfig.getParameter<int>("electronsMax");
  elecPT_       = iConfig.getParameter<double>("elecPT");
  elecEta_      = iConfig.getParameter<double>("elecEta");
  useElecID_    = iConfig.getParameter<bool>("useElecID");
  elecID_       = iConfig.getParameter<string>("elecID");

  tausMin_  = iConfig.getParameter<int>("tausMin");
  tausMax_  = iConfig.getParameter<int>("tausMax");
  tauPT_    = iConfig.getParameter<double>("tauPT" );
  tauEta_   = iConfig.getParameter<double>("tauEta") ;
  useTauID_ = iConfig.getParameter<bool> ("useTauID");
  tauID_    = iConfig.getParameter<string>("tauID") ;

  allEventsPassFilter_ = iConfig.getUntrackedParameter<bool>("allEventsPassFilter");
  debug_   = iConfig.getUntrackedParameter<bool>("debug");
  counteitherleptontype_ = iConfig.getParameter<bool>("counteitherleptontype");
  customfilterEMuTauJet2012_ = iConfig.getParameter<bool>("customfilterEMuTauJet2012");
  customfilterEMuTauJet2016_ = iConfig.getParameter<bool>("customfilterEMuTauJet2016");
  customfilterEMuTauJetLegacy_   = iConfig.getParameter<bool>("customfilterEMuTauJetLegacy");
  HHcustomfilterEMuTauJetLegacy_ = iConfig.getParameter<bool>("HHcustomfilterEMuTauJetLegacy");

  lheEventProductToken_ = consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));

  TotalCount=0;
  PassedCount=0;
  SumOfAMCAtNLOWeights=0;
  SumOfTopPtReweights=0;
  TFileDirectory COUNT=histServ->mkdir("EventCount");
  hCount=COUNT.make<TH1F>("EventCounter","Event Counter",4,-0.5,3.5);
  hCount->GetXaxis()->SetBinLabel(1,"all events");
  hCount->GetXaxis()->SetBinLabel(2,"passed");
  hCount->GetXaxis()->SetBinLabel(3,"sumOfAmcAtNLOWeights");
  hCount->GetXaxis()->SetBinLabel(4,"sumOfTopPtReweights");
}


LeptonJetFilter::~LeptonJetFilter()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
LeptonJetFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  ++TotalCount;

  if( allEventsPassFilter_ )
    {
      ++PassedCount;
      if (debug_) cout <<"PASSED!"<<endl;
      return true;
    }

  // keep all amc@NLO weights from LHE
  edm::Handle<LHEEventProduct> EvtHandle;
  iEvent.getByToken( lheEventProductToken_ , EvtHandle );

  //Non-madgraph samples may not have this information, if so, skip
  if (EvtHandle.isValid())
  {
    //Powheg samples have valid EvtHandle but seg fault when trying to access weights, so skip if the weights vector is empty
    if (EvtHandle->weights().size()>0)
      EvtHandle->weights()[0].wgt < 0 ? SumOfAMCAtNLOWeights-=1. : SumOfAMCAtNLOWeights+=1.;
  }

  // get TopPt weights

  if( !iEvent.isRealData() ) {
    edm::Handle<reco::GenParticleCollection> genParticles;
    iEvent.getByToken(genPartInputToken_, genParticles);
    
    if( genParticles.isValid() ) {
      edm::LogInfo("LeptonJetFilterInfo") << "Total # GenParticles: " << genParticles->size();
      
      int nTops = 0;
      float topPt1 = -1;
      float topPt2 = -1;
      int i=0;
      for( reco::GenParticleCollection::const_iterator it = genParticles->begin(); it != genParticles->end(); ++it ) {
        // exit from loop when you reach the required number of GenParticles
	i++;
	//if(i >= 50)
        //  break;
	
        //skip LHC protons - pdgId=2212, extremely high eta, phi=0, energy ~= 6500
        if(it->pdgId()==2212 && it->energy()>=6000) continue;
        // actually let's skip any particle with huge eta
        if(fabs(it->eta()) > 10) continue;
        // skip pythia8 "partons in preparation of hadronization process"
        // details here: http://home.thep.lu.se/~torbjorn/pythia81html/ParticleProperties.html
        if(it->status() >= 71 && it->status()<=79) continue;
	
        // keep for topptreweight below
        // see: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
        if(TMath::Abs(it->pdgId())==6){
	  if( it->status()==62) {
	    nTops++;
	    if(topPt1<0)
	      topPt1 = it->pt();
	    else
	      topPt2 = it->pt();
	  }
	}
      }
      // for topptreweight
      if(nTops==2){
        SumOfTopPtReweights += sqrt(exp(0.0615-0.0005*topPt1)*exp(0.0615-0.0005*topPt2));
      }
      else SumOfTopPtReweights += 1.;
    } else {
      edm::LogError("LeptonJetFilter") << "Error! Can't get the genPartInputToken_";
    }
  }
  else SumOfTopPtReweights += 1.;
  

  // get photons
  Handle<pat::PhotonCollection> photons;
  iEvent.getByToken(photonCollectionToken_,photons);

  // get jets
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jetCollectionToken_, jets);

  // get leptons
  edm::Handle<pat::ElectronCollection> electrons;
  iEvent.getByToken(electronCollectionToken_, electrons);

  edm::Handle<pat::MuonCollection > muons;
  iEvent.getByToken(muonCollectionToken_, muons);

  edm::Handle<pat::TauCollection > taus;
  iEvent.getByToken(tauCollectionToken_, taus);

  // Step 0: count photons
  int nphotons = 0;

  for(const pat::Photon &pho : *photons)
  {
    if (debug_) cout << "Photons:" << endl;
    if (debug_) cout << "pT: " << pho.pt() << " eta: " <<  pho.eta() << " phi: " <<  pho.phi() << " HoE: " 
      << pho.hadronicOverEm() <<endl;
    bool passID = true;

    if(usePhotID_)
    {
      if (!(pho.photonID(photID_)>0.))
        passID = false;
    }

    if(pho.pt()>photET_ && fabs(pho.eta())<photEta_ && passID && pho.hadronicOverEm()<photHoE_)
    {
      ++nphotons;
    }
  }

  if (debug_) cout <<"# Photons = "<<nphotons<<endl;
  // If not enough photons found (or too many found), return false
  if (photMin_>-1 && nphotons<photMin_) return false;
  if (photMax_>-1 && nphotons>photMax_) return false;


  // Step 1:  Count jets
  int njets=0;
  int njets_IDed=0;
  float ptleadjet=0.0;
  float highestJetbTagMVA  = -999.;
  float highestJetbTagCISV = -999.;

  for(const pat::Jet &jet : *jets)
  {
    if(debug_) cout << "Jet:" << endl;
    if(debug_) cout << "pT: " << jet.pt() << " eta: " <<  jet.eta() << " phi: " <<  jet.phi() << endl;
    if(jet.pt()>jetPT_ && fabs(jet.eta())<jetEta_)
    {
      ++njets;
      if(jet.pt() > ptleadjet)
        ptleadjet=jet.pt();
      if(jet.bDiscriminator("pfCombinedMVAV2BJetTags") > highestJetbTagMVA)
	highestJetbTagMVA = jet.bDiscriminator("pfCombinedMVAV2BJetTags");
      if(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > highestJetbTagCISV)
	highestJetbTagCISV = jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
      bool looseJetID = false;
      float eta = jet.eta();
      float NHF = jet.neutralHadronEnergyFraction();
      float NEMF = jet.neutralEmEnergyFraction();
      float CEMF = jet.chargedEmEnergyFraction();
      float CHF = jet.chargedHadronEnergyFraction();
      //float MUF = jet.muonEnergyFraction();
      float NumConst = jet.chargedMultiplicity()+jet.neutralMultiplicity();
      float NumNeutralParticle = jet.neutralMultiplicity();
      float CHM = jet.chargedMultiplicity();
      if (abs(eta)<=2.7)
	      looseJetID = ((NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=2.7);
      else if (abs(eta)<=3.0)
	looseJetID = (NHF< 0.98 && NEMF>0.01 && NumNeutralParticle>2 && abs(eta)>2.7 && abs(eta)<=3.0 );
      else
	looseJetID = (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 && abs(eta)<5.0 );
      if (looseJetID) ++njets_IDed;
    }
  }


  if (debug_) cout <<"# Jets = "<<njets<<endl;
  // If not enough jets found (or too many found), return false
  if (jetsMin_>-1 && njets<jetsMin_) return false;
  if (jetsMax_>-1 && njets>jetsMax_) return false;

  // Step 2:  Count leptons
  int nmuons=0;
  int nelectrons=0;
  int ntaus=0;
  float ptleadmu=0.0;
  float ptlead2mu=0.0;
  float ptleade=0.0;
  float ptlead2e=0.0;
  float ptleadtau=0.0;
  
  int sumMuonCharge=0;//for same-sign muon pair requirement.
  bool isSameSignMuon_=false;

  // count muons
  for(const pat::Muon &mu : *muons)
  {
    if (debug_) cout << "Muon pT: " << mu.pt() << " eta: " <<  mu.eta() << " phi: " <<  mu.phi() << endl;
    bool passID = true;

    if (useMuID_)
    {
      if(!(mu.muonID(muID_)))
        passID = false;
    }

    if (mu.pt()>muPT_ && fabs(mu.eta())<muEta_ && passID)
    {
      ++nmuons;
      // Object Collections are always stored in descending Pt order //
      if( nmuons == 1 ) ptleadmu=mu.pt(); 
      if( nmuons == 2 ) ptlead2mu=mu.pt();
      sumMuonCharge+=mu.charge();
    }
    if( nmuons>2 || ( nmuons==2 && fabs(sumMuonCharge)==2 ) )
      isSameSignMuon_=true;
  }
  if (debug_) cout <<"LeadMuonPt = "<<ptleadmu<<endl;
  if (debug_) cout <<"Lead2MuonPt = "<<ptlead2mu<<endl;
  if (debug_) cout <<"# Muons = "<<nmuons<<endl;

  // count electrons
  for(const pat::Electron &elec : *electrons)
  {
    if (debug_) cout << "Electron:" << endl;
    if (debug_) cout << "pT: " << elec.pt() << " SC Eta: " <<  (*elec.superCluster()).eta() << " eta: "<< elec.eta() <<" phi: " <<  elec.phi() << endl;
    bool passID = true;

    if (useElecID_)
    {
      if(!(elec.electronID(elecID_)>0.))
        passID = false;
    }

    if (elec.pt()>elecPT_ && fabs((*elec.superCluster()).eta())<elecEta_ && passID)
    {
      ++nelectrons;
      // Object Collections are always stored in descending Pt order //
      if( nelectrons == 1 ) ptleade=elec.pt(); 
      if( nelectrons == 2 ) ptlead2e=elec.pt();
      //      if (elec.pt() > ptleade)
      //        ptleade=elec.pt();
    }
  }
  if (debug_) cout <<"# Electrons = "<<nelectrons<<endl;

  // count taus
  for(const pat::Tau &tau : *taus)
  {
    if (debug_) cout << "Tau:" << endl;
    if (debug_) cout << "pT: " << tau.pt() << " eta: " <<  tau.eta() << " phi: " <<  tau.phi() << endl;
    bool passID = true;

    if ( useTauID_ )
    { 
      if (tau.tauID(tauID_) < 0.5 )
        passID = false;
    }

    if (tau.pt()>tauPT_ && fabs(tau.eta())<tauEta_ && passID)
    {
      ++ntaus;
      if (tau.pt() > ptleadtau)
        ptleadtau=tau.pt();
    }   

  }
  if (debug_) cout <<"# Taus = "<<ntaus<<endl;

  if (customfilterEMuTauJet2012_==false && customfilterEMuTauJet2016_==false && customfilterEMuTauJetLegacy_==false && HHcustomfilterEMuTauJetLegacy_==false)
  {
    // If we require both electron and muon condition to be met, check electron here
    if (counteitherleptontype_==false)
    {
      if (electronsMin_>-1 && nelectrons<electronsMin_) return false;
      if (electronsMax_>-1 && nelectrons>electronsMax_) return false;
    }

    // If we require both electron and muon condition to be met, check muon here
    if (counteitherleptontype_==false)
    {
      if (muonsMin_>-1 && nmuons<muonsMin_) return false;
      if (muonsMax_>-1 && nmuons>muonsMax_) return false;
    }

    if (counteitherleptontype_==false)
    {
      if (tausMin_>-1 && ntaus<tausMin_) return false;
      if (tausMax_>-1 && ntaus>tausMax_) return false; 
    }

    // Otherwise, only fail cut if neither electron nor muon meet expectations
    if (counteitherleptontype_==true)
    {
      if ((muonsMin_>-1 && nmuons<muonsMin_) && (electronsMin_>-1 && nelectrons<electronsMin_) && (tausMin_>-1 && ntaus<tausMin_)) return false;
      if ((muonsMax_>-1 && nmuons>muonsMax_) && (electronsMax_>-1 && nelectrons>electronsMax_) && (tausMax_>-1 && ntaus>tausMax_)) return false;
    }
  }  

  if (customfilterEMuTauJet2012_==true)
  {
    bool keepevent = false;
    if ( ptleadmu > 25 && ptleadtau > 20 ) keepevent=true;
    if ( ptleadmu > 25 && ptlead2mu > 25 ) keepevent=true;
    if ( ptleadmu > 20 && ptlead2mu > 10 && isSameSignMuon_ ) keepevent=true; // 1 same-sign muon pair is required.
    if ( ptleadmu > 40 || ptleade > 40 ) keepevent=true;
    if (debug_) std::cout<<ptleade<<" : "<<ptleadmu<<" : "<<ptleadtau<<" : "<<ptleadjet<<std::endl;
    if (keepevent==false) return false;
  }
 
  if (customfilterEMuTauJet2016_==true)
  {
    bool keepevent = false;
    if ( (ptleadmu > 45 || ptleade > 45) && ptleadjet>45) keepevent=true;
    if (debug_) std::cout<<ptleade<<" : "<<ptleadmu<<" : "<<ptleadtau<<" : "<<ptleadjet<<std::endl;
    //std::cout<<std::endl<<ptleade<<" : "<<ptlead2e<<std::endl<<ptleadmu<<" : "<<ptlead2mu<<std::endl<<njets<<" : "<<ptleadjet<<std::endl<<keepevent<<std::endl;
   if (keepevent==false) return false;
  }
  

  if (customfilterEMuTauJetLegacy_==true)
  {
    bool keepevent = false;
    if ( (ptleadmu > 45 || ptleade > 45) && ptleadjet>45) keepevent=true;
    if (debug_) std::cout<<ptleade<<" : "<<ptleadmu<<" : "<<ptleadtau<<" : "<<ptleadjet<<std::endl;
    //std::cout<<std::endl<<ptleade<<" : "<<ptlead2e<<std::endl<<ptleadmu<<" : "<<ptlead2mu<<std::endl<<njets<<" : "<<ptleadjet<<std::endl<<keepevent<<std::endl;
   if (keepevent==false) return false;
  }
  


  if (HHcustomfilterEMuTauJetLegacy_==true)
  {
    bool keepevent = false;
    //if ( highestJetbTagMVA >= -0.5884 || highestJetbTagCISV >= 0.5426){
    if ( ptleadmu > 16 && ptlead2mu > 8  && njets_IDed>3) keepevent=true;
    if ( ptleade  > 16 && ptlead2e  > 12 && njets_IDed>3) keepevent=true;
    // keepevent=true;
    //}
    if (debug_) std::cout<<ptleade<<" : "<<ptleadmu<<" : "<<ptleadtau<<" : "<<ptleadjet<<std::endl;
    //std::cout<<std::endl<<ptleade<<" : "<<ptlead2e<<std::endl<<ptleadmu<<" : "<<ptlead2mu<<std::endl<<njets<<" : "<<ptleadjet<<std::endl<<keepevent<<std::endl;
   if (keepevent==false) return false;
  }
  

  ++PassedCount;
  if (debug_) cout <<"PASSED!"<<endl;
  // std::cout<<"KEPT"<<std::endl;
  
  return true;
}

// ------------ method called once each job just before starting event loop  ------------
void
LeptonJetFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
LeptonJetFilter::endJob() {
  hCount->SetBinContent(1,TotalCount);
  hCount->SetBinContent(2,PassedCount);
  hCount->SetBinContent(3,SumOfAMCAtNLOWeights);
  hCount->SetBinContent(4,SumOfTopPtReweights);
  cout <<"Total events = "<<TotalCount<<"  Events Passed = "<<PassedCount<<"  Sum of amcatnlo weights = "<<SumOfAMCAtNLOWeights<<"  Sum of TopPt weights = "<<SumOfTopPtReweights<<endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(LeptonJetFilter);
