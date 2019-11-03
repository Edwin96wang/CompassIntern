#include "PaHit.h"
#include <iostream>
#include "RPD_Helper.h"
#include <PaEvent.h>
#include <PaVertex.h>
#include <PaParticle.h>
#include <PaTPar.h>
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH1I.h"
#include "TVector3.h"
#include "TF1.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <math.h>
#include "PaDetect.h"
#include "PaDigit.h"
#include "PaCaloClus.h"

using namespace std;

static TH2D* Pion_Beam_P                = nullptr;
static TH2D* Pion_Beam_Pt               = nullptr;
static TH2D* spillNum_VerticeNum        = nullptr;
static TH2D* SpillNum_PrimVerticeNum    = nullptr;
static TH2D* SpillNum_NonPrimVerticeNum = nullptr;
static TH1D* energy_proton              = nullptr;
static TH1D* angle_proton               = nullptr;
static TH2D* angle_proton_z_position    = nullptr;
static TH2D* SecVertexXY                = nullptr;
static TH2D* SecVertexZY                = nullptr;
static TH2D* PrimVertexXY               = nullptr;
static TH2D* PrimVertexXY_1             = nullptr;
static TH2D* PrimVertexXY_3             = nullptr;
static TH2D* PrimVertexZY               = nullptr;
static TH1D* SpillNum_SpillTime         = nullptr;
static TH2D* SpillTime_SpillNum_2D      = nullptr;
static TH1D* ThreePion_inv_mass         = nullptr;
static TH1D* Total_inv_mass             = nullptr;
static TH1D* Photon_Energy              = nullptr;
static TH1D* Photon_Energy_cut          = nullptr;
static TH1I* Photon_Number              = nullptr;
static TH1I* Photon_Number_cut          = nullptr;
static TH2I* ECAL_hist                  = nullptr;
static TH1D* ECAL1_SpillNum             = nullptr;
static TH1D* ECAL2_SpillNum             = nullptr;

static int primary_vertex_num;
static vector<TLorentzVector> protons;
static TLorentzVector rest_proton(0.0,0.0,0.0,0.938272);
static double mag=0.0;

static vector<vector<double> > spilltime (220,vector<double >(2,0.0));

void UserEvent120(PaEvent &e){
    static bool first = true;
    static PaVertex vertex;
    static PaParticle pion_in;

    static double mass_pion = 0.13957;

    static RPD* proton_recoil;
    proton_recoil = &RPD::Instance();

    map<string, int> ECAL_map = {{"EC01",1},{"EC02",2}};

    if(first){
        Pion_Beam_P = new TH2D("Pion_Beam_P ", "Beam momentum for each spill", 220, -0.5, 220 - 0.5, 200, 189.8, 190.2);
        Pion_Beam_P->SetXTitle("spill number");
        Pion_Beam_P->SetYTitle("momentum");

        Pion_Beam_Pt = new TH2D("Pion_Beam_Pt ", "Beam transverse momentum for each spill", 220, -0.5, 220 - 0.5, 200, 0, 0.5);
        Pion_Beam_Pt->SetXTitle("spill number");
        Pion_Beam_Pt->SetYTitle("Pt");

        spillNum_VerticeNum = new TH2D("spillNum_VerticeNum ", "vertice num of each spill", 220, -0.5, 220 - 0.5, 8, -0.5, 8 - 0.5);
        spillNum_VerticeNum->SetXTitle("spill number");
        spillNum_VerticeNum->SetYTitle("num of vertices");

        SpillNum_PrimVerticeNum = new TH2D("SpillNum_PrimVerticeNum ", " primary vertice num of each spill", 220, -0.5, 220 - 0.5, 8, -0.5, 8.0 - 0.5);
        SpillNum_PrimVerticeNum->SetXTitle("spill number");
        SpillNum_PrimVerticeNum->SetYTitle("num of primary vertices");

        SpillNum_NonPrimVerticeNum = new TH2D("SpillNum_NonPrimVerticeNum ", " non-primary vertice num of each spill", 220, -0.5, 220 - 0.5, 8, -0.5, 8.0 - 0.5);
        SpillNum_NonPrimVerticeNum->SetXTitle("spillNum");
        SpillNum_NonPrimVerticeNum->SetYTitle("num of non-primary vertices");

        SecVertexXY = new TH2D("SecVertexXY", " x and y position of non-primary vertex", 500, -2, 2., 500, -2, 2);
        SecVertexXY->SetXTitle("x (cm)");
        SecVertexXY->SetYTitle("y (cm)");

        SecVertexZY = new TH2D("SecVertexZY ", "z and y position of non-primary vertex", 500, -250 - 0.5, 250 - 0.5, 500, -2, 2);
        SecVertexZY->SetXTitle("z (cm)");
        SecVertexZY->SetYTitle("y (cm)");

        PrimVertexXY = new TH2D("PrimVertexXY", " x and y position of primary vertex", 500, -2, 2., 500, -2, 2);
        PrimVertexXY->SetXTitle("x (cm)");
        PrimVertexXY->SetYTitle("y (cm)");

        PrimVertexXY_1 = new TH2D("PrimVertexXY_1", " x and y position of primary vertex", 500, -2, 2., 500, -2, 2);
        PrimVertexXY_1->SetXTitle("x (cm)");
        PrimVertexXY_1->SetYTitle("y (cm)");

        PrimVertexXY_3 = new TH2D("PrimVertexXY_3", " x and y position of primary vertex", 500, -2, 2., 500, -2, 2);
        PrimVertexXY_3->SetXTitle("x (cm)");
        PrimVertexXY_3->SetYTitle("y (cm)");

        PrimVertexZY = new TH2D("PrimVertexZY ", "z and y position of primary vertex", 500, -250 - 0.5, 250 - 0.5, 500, -2, 2);
        PrimVertexZY->SetXTitle("z (cm)");
        PrimVertexZY->SetYTitle("y (cm)");

        energy_proton = new TH1D("energy_proton", "the energy of proton", 300, -3, 0);
        energy_proton->SetXTitle("The energy of recoilded proton");
        energy_proton->SetYTitle("entries");

        angle_proton = new TH1D("angle_proton", "the angle distributin of proton", 300, 0, 4);
        angle_proton->SetXTitle("angle of recoided proton (Rad)");
        angle_proton->SetYTitle("entries");

        angle_proton_z_position = new TH2D("angle_proton_z_position", "the angle distributin of proton of z position", 100, -80, -10, 300, 0, 4);
        angle_proton_z_position->SetXTitle("z (cm)");
        angle_proton_z_position->SetYTitle("angle of recoided proton (Rad)");

        SpillNum_SpillTime = new TH1D("SpillNum_SpillTime", "spill time of each spill number", 220, -0.5, 220 - 0.5);
        SpillNum_SpillTime->SetXTitle("number of spill");
        SpillNum_SpillTime->SetYTitle("duration of spill (s)");

        SpillTime_SpillNum_2D = new TH2D("SpillTime_SpillNum_2D", "spill time distributiobn for each spill number", 220, -0.5, 220 - 0.5, 100, 0, 30.0);
        SpillTime_SpillNum_2D->SetXTitle("number of spill");
        SpillTime_SpillNum_2D->SetYTitle("spill time");

        ThreePion_inv_mass = new TH1D("ThreePion_inv_mass", "invariance mass of three pions", 200, 0.0, 8.0);
        ThreePion_inv_mass->SetXTitle("mass of three pions (GeV)");
        ThreePion_inv_mass->SetYTitle("Entries");

        Total_inv_mass = new TH1D("Total_inv_mass", "invariance mass", 200, 0.0, 8.0);
        Total_inv_mass->SetXTitle("mass (GeV)");
        Total_inv_mass->SetYTitle("Entries");

        Photon_Energy = new TH1D("Photon_Energy", "photon energy", 200, 0, 10);
        Photon_Energy->SetXTitle("Photon energy (GeV)");
        Photon_Energy->SetYTitle("Entries");

        Photon_Energy_cut = new TH1D("Photon_Energy_cut", "photon energy", 200, 0, 10);
        Photon_Energy_cut->SetXTitle("Photon energy (GeV)");
        Photon_Energy_cut->SetYTitle("Entries");

        Photon_Number = new TH1I("Photon_Number", "photon number", 40, -0.5, 40 - 0.5);
        Photon_Number->SetXTitle("Photon number of each event");
        Photon_Number->SetYTitle("Entries");

        Photon_Number_cut = new TH1I("Photon_Number_cut", "photon number", 40, -0.5, 40 - 0.5);
        Photon_Number_cut->SetXTitle("Photon number of each event");
        Photon_Number_cut->SetYTitle("Entries");

        ECAL_hist = new TH2I("ECAL_hist", "spill time distributiobn for each spill number", 15, -0.5, 14.5, 15, -0.5, 14.5);
        ECAL_hist->SetXTitle("the photon number of ECAL1");
        ECAL_hist->SetYTitle("the photon number of ECAL2");

        ECAL1_SpillNum = new TH1D("ECAL1_SpillNum", "spill time distributiobn for each spill number", 220, -0.5, 220 - 0.5);
        ECAL1_SpillNum->SetXTitle("spill number");
        ECAL1_SpillNum->SetYTitle("the photon number of ECAL1");

        ECAL2_SpillNum = new TH1D("ECAL2_SpillNum", "spill time distributiobn for each spill number", 220, -0.5, 220 - 0.5);
        ECAL2_SpillNum->SetXTitle("spill number");
        ECAL2_SpillNum->SetYTitle("the photon number of ECAL2");

        first = false;
    }

    //spillnumber vs vertice number:
    static int spillnum = 0;
    spillnum = e.SpillNum();
    spillNum_VerticeNum->Fill(spillnum,e.NVertex());

    //Checking the energy of the beam
    vertex = e.vVertex(e.iBestPrimaryVertex());
    pion_in = e.vParticle(vertex.InParticle());
    if(pion_in.IsBeam()){
        Pion_Beam_P->Fill(spillnum, pion_in.ParInVtx(e.iBestPrimaryVertex()).Mom());
        Pion_Beam_Pt->Fill(spillnum, pion_in.ParInVtx(e.iBestPrimaryVertex()).Pt());
    }

    //spillnumber vs spilltime:

    SpillTime_SpillNum_2D->Fill(spillnum, e.TimeInSpill());

    if(spilltime[spillnum][0]==0 || spilltime[spillnum][1] == 0){
        spilltime[spillnum][0] = e.TimeInSpill();
        spilltime[spillnum][1] = e.TimeInSpill();
    }
    else{
        if(e.TimeInSpill() > spilltime[spillnum][0]){
            spilltime[spillnum][0] = e.TimeInSpill();
        }
        else if (e.TimeInSpill() < spilltime[spillnum][1]){
            spilltime[spillnum][1] = e.TimeInSpill();
        }
    }

    //spillnumber vs primary vertice number: 
    // recording the position of non promary vertex:
    static int vertex_index;
    static int vertex_count;
    static int vertex_index_primary;
    vertex_count = 0;
    for (vertex_index = 0; vertex_index < e.NVertex(); vertex_index++){
        if(e.vVertex(vertex_index).IsPrimary()){
            vertex_count++;
            if(e.vVertex(vertex_index).NOutParticles() == 1) PrimVertexXY_1->Fill(e.vVertex(vertex_index).X(), e.vVertex(vertex_index).Y());
            if(e.vVertex(vertex_index).NOutParticles() == 3) PrimVertexXY_3->Fill(e.vVertex(vertex_index).X(), e.vVertex(vertex_index).Y());
            PrimVertexXY->Fill(e.vVertex(vertex_index).X(), e.vVertex(vertex_index).Y());
            PrimVertexZY->Fill(e.vVertex(vertex_index).Z(), e.vVertex(vertex_index).Y());
            vertex_index_primary = vertex_index;
        }
        else{
            SecVertexXY->Fill(e.vVertex(vertex_index).X(), e.vVertex(vertex_index).Y());
            SecVertexZY->Fill(e.vVertex(vertex_index).Z(), e.vVertex(vertex_index).Y());
        }
    }
    SpillNum_PrimVerticeNum->Fill(spillnum,vertex_count);
    SpillNum_NonPrimVerticeNum->Fill(spillnum,e.NVertex()-vertex_count);
    vertex_count = 0;

    //invariant mass of three pions and total mass:
    static int Photon_N;
    static int Photon_N_cut;
    static double Photon_E = 0;
    vector<TLorentzVector> Photon_E_vec = vector<TLorentzVector> (0);
    static TVector3 vec3;
    static TLorentzVector vec4;
    Photon_N = 0;
    Photon_N_cut = 0;
    primary_vertex_num = e.iBestPrimaryVertex();
    vertex = e.vVertex(primary_vertex_num);
    static TLorentzVector vec;

    int i_ECA1 = 0;
    int i_ECA2 = 0;
    if(vertex.NOutParticles() == 3 ){

        for (int i = 0; i < e.NParticle(); i++)
        {

            if (e.vParticle(i).Q() != 0)
                continue;
            if (e.vParticle(i).NCalorim() > 1)
            {
                cout << "more than 1 calorimeter has occured" << endl;
                continue;
            }
            if ((e.vCaloClus(e.vParticle(i).iCalorim(0))).CalorimName().substr(0, 2) != "EC")
                continue;

            
            Photon_E = (e.vCaloClus(e.vParticle(i).iCalorim(0))).E();
            string ecal = e.vCaloClus(e.vParticle(i).iCalorim(0)).CalorimName().substr(0, 4);
            if((Photon_E > 1 && ECAL_map.at(ecal) == 1) || (Photon_E > 4 && ECAL_map.at(ecal) == 2) ){
                vec3=TVector3();
                vec4=TLorentzVector();
                vec3 = TVector3((e.vCaloClus(e.vParticle(i).iCalorim(0))).X(),(e.vCaloClus(e.vParticle(i).iCalorim(0))).Y(),
                (e.vCaloClus(e.vParticle(i).iCalorim(0))).Z());
                vec3.SetMag(Photon_E);
                vec4.SetPxPyPzE(vec3.x(),vec3.y(),vec3.z(),Photon_E);
                Photon_E_vec.push_back(vec4);
                Photon_N_cut++;
                Photon_Energy_cut->Fill(Photon_E);
                if (ECAL_map.at(ecal) == 1)
                {
                    i_ECA1++;
                    ECAL1_SpillNum->Fill(spillnum);
                }
                if (ECAL_map.at(ecal) == 2)
                {
                    i_ECA2++;
                    ECAL2_SpillNum->Fill(spillnum);
                }
            }
            Photon_Energy->Fill(Photon_E);
            Photon_N++;
        }
        ECAL_hist->Fill(i_ECA1,i_ECA2);
        if(Photon_N != 0){
            Photon_Number -> Fill(Photon_N);
        }
        if(Photon_N_cut != 0){
            Photon_Number_cut -> Fill(Photon_N_cut);
        }

        vec = TLorentzVector();
        for(int i = 0; i < vertex.NOutParticles(); i++){
            vec += (e.vParticle(vertex.iOutParticle(i))).ParInVtx(primary_vertex_num).LzVec(mass_pion);
        }
        ThreePion_inv_mass->Fill(vec.M());
        for(unsigned int i = 0; i < Photon_E_vec.size(); i++){
            vec += Photon_E_vec[i];
        }
        Total_inv_mass->Fill(vec.M());
    }

    //recoiled proton:

    primary_vertex_num = e.iBestPrimaryVertex();
    vertex = e.vVertex(primary_vertex_num);
    proton_recoil->Search(vertex);
    protons = proton_recoil->vTracks();

    mag = (protons[0]-rest_proton).M();
    if (protons.size() != 1 /*|| mag >= -0.27 || mag <= -0.3*/) return;
    energy_proton -> Fill(mag);
    if (protons[0].Theta() != 0)
    {
        angle_proton->Fill(protons[0].Theta());
        angle_proton_z_position->Fill(e.vVertex(vertex_index_primary).Z(), protons[0].Theta());
    }
}


void UserJobEnd120(){
    cout << "new run time" << endl;
    for(int i=0; i < (int)spilltime.size(); i++){
        SpillNum_SpillTime->SetBinContent(i+1, spilltime[i][0]-spilltime[i][1]);
    }
    //cout << "the maximum t: " << spilltime[67][0] << ", " << spilltime[67][1] << endl; 
}


// gStyle->SetOptStat(0);
// gStyle->SetPalette(55);