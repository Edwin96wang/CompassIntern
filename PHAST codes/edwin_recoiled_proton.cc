#include <iostream>
#include "RPD_Helper.h"
#include <PaEvent.h>
#include <PaVertex.h>
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TVector3.h"

void UserEvent123(PaEvent &e){
  static PaVertex vertex;
  static RPD* proton_recoil;
  proton_recoil = &RPD::Instance();

  static int primary_vertex_num;
  static vector<TLorentzVector> protons;
  static TH1D* energy_proton = nullptr;
  static TH1D* angle_proton = nullptr;
  static TLorentzVector rest_proton(0.0,0.0,0.0,0.938272);
  static double mag=0.0;
  static bool first = true;

  if (first){
    energy_proton = new TH1D("energy_proton", "the energy of proton",300,-3,0);
    angle_proton = new TH1D("angle_proton","the angle distributin of proton", 300, 1.0,5.0);
    first = false;
  }

  primary_vertex_num = e.iBestPrimaryVertex();
  vertex = e.vVertex(primary_vertex_num);
  proton_recoil->Search(vertex);
  protons = proton_recoil->vTracks();

  mag = (protons[0]-rest_proton).M();
  if (protons.size() != 1 || mag >= -0.27 || mag <= -0.3) return;
  energy_proton -> Fill(mag);
  angle_proton -> Fill(5-protons[0].Theta());

  //cout << "theta: " << protons[0].Theta()<< endl;
  //cout << "Phi: " << protons[0].Phi() << endl;

  //cout << (protons[0]-rest_proton).M() << endl;

}
