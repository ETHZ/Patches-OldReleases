
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/Jet.h"


class TrackingFailureFilter : public edm::EDFilter {

  public:

    explicit TrackingFailureFilter(const edm::ParameterSet & iConfig);
    ~TrackingFailureFilter() {}

  private:

    virtual bool filter(edm::Event & iEvent, const edm::EventSetup & iSetup);
    
    const edm::InputTag jetSrc_, trackSrc_, vertexSrc_;
    const double dzTrVtxMax_, dxyTrVtxMax_, minSumPtOverHT_;
    const bool taggingMode_, quiet_;

};


TrackingFailureFilter::TrackingFailureFilter(const edm::ParameterSet & iConfig) 
  : jetSrc_        (iConfig.getParameter<edm::InputTag>("JetSource"))
  , trackSrc_      (iConfig.getParameter<edm::InputTag>("TrackSource"))
  , vertexSrc_     (iConfig.getParameter<edm::InputTag>("VertexSource"))
  , dzTrVtxMax_    (iConfig.getParameter<double>("DzTrVtxMax"))
  , dxyTrVtxMax_   (iConfig.getParameter<double>("DxyTrVtxMax"))
  , minSumPtOverHT_(iConfig.getParameter<double>("MinSumPtOverHT"))
  , taggingMode_   (iConfig.getParameter<bool> ("taggingMode"))
  , quiet_(iConfig.getParameter<bool>("quiet"))
{
  produces<bool>();
}


bool TrackingFailureFilter::filter(edm::Event & iEvent, const edm::EventSetup & iSetup) {

  edm::Handle<edm::View<reco::Jet> > jets;
  iEvent.getByLabel(jetSrc_, jets);
  edm::Handle<std::vector<reco::Track> > tracks;
  iEvent.getByLabel(trackSrc_, tracks);
  edm::Handle<std::vector<reco::Vertex> > vtxs;
  iEvent.getByLabel(vertexSrc_, vtxs);

  double ht = 0;
  for (edm::View<reco::Jet>::const_iterator j = jets->begin(); j != jets->end(); ++j) {
    ht += j->pt();
  }
  double sumpt = 0;
  if (vtxs->size() > 0) {
    const reco::Vertex * vtx = &((*vtxs)[0]);
    for (std::vector<reco::Track>::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr) {
      if (fabs(tr->dz(vtx->position())) > dzTrVtxMax_) continue;
      if (fabs(tr->dxy(vtx->position())) > dxyTrVtxMax_) continue;
      sumpt += tr->pt();
    }
  }
  const bool ok = minSumPtOverHT_ <= (sumpt/ht);

  if ( !ok && !quiet_ )
    std::cout << "TRACKING FAILURE: "
              << iEvent.id().run() << " : " << iEvent.id().luminosityBlock() << " : " << iEvent.id().event()
              << " HT=" << ht
              << " SumPt=" << sumpt
              << std::endl;

  iEvent.put( std::auto_ptr<bool>(new bool(ok)) ); 
  return taggingMode_ || ok ; // return false if filtering and not enough tracks in event

}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(TrackingFailureFilter);
