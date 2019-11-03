#include <iostream>
#include <PaTrack.h>
#include <PaEvent.h>
#include <PaVertex.h>
#include <PaParticle.h>
#include <PaTPar.h>
#include <TH2.h>
#include <math.h>
#include <TLorentzVector.h>

using namespace std;

void UserEvent124(PaEvent &e){
  static TH2F* xyhis_pri = nullptr;
  static TH1D* energy_out = nullptr;

  static const PaParticle* outparticle = nullptr;
  static const PaVertex* vertex = nullptr;
  static const PaTPar* PaPar = nullptr;

  static int n_out = 0;
  static int PrimaryVertex = 0;
  static double mass_out = 0.1396;
  static TLorentzVector vec;
  static bool first  = true;

  if (first){
    xyhis_pri = new TH2F("xyhis_pri","xyhis_pri",200,-5.0,5.0,200,-5.0,5.0);
    energy_out = new TH1D("energy_out", "invariance mass",100,0.0,8.0);
    xyhis_pri->SetXTitle("xposition of primary vertex");
    xyhis_pri->SetYTitle("yposition of primary vertex");
    xyhis_pri->SetMarkerColor(kRed);
    energy_out->SetXTitle("Mass (GeV)");
    energy_out->SetYTitle("Entry");
    first = false;
  }

  PrimaryVertex = e.iBestPrimaryVertex();
  vertex = &e.vVertex(PrimaryVertex);
  n_out = vertex->NOutParticles();
  if(n_out>1){
    vec = TLorentzVector();
    for(int i = 0; i < n_out; i++){
      outparticle = &e.vParticle(vertex->iOutParticle(i));
      PaPar = &outparticle->ParInVtx(PrimaryVertex);
      vec += PaPar->LzVec(mass_out);
    }

    xyhis_pri->Fill(vertex->X(),vertex->Y());
    energy_out->Fill(vec.M());
  }
}

void UserJobEnd124(){

}
