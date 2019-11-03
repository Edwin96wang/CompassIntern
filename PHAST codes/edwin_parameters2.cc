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
#include "PaHit.h"
#include "PaDetect.h"
#include "PaDigit.h"
#include "PaCaloClus.h"
#include <set>
#include <algorithm>  

using namespace std;

static PaVertex vertex;

void UserEvent126(PaEvent &e)
{
    static bool first = true;
    static PaVertex vertex;

    static TH2D *vertex_charge = nullptr;
    static TH1D *PrimVer_ParNum = nullptr;
    static TH1D *NonPrimVer_ParNum = nullptr;
    static TH1D *ParNum_NoVertex = nullptr;
    static TH1D *Par_NoVertex_Z= nullptr;

    if (first)
    {
        vertex_charge = new TH2D("vertex_charge", "particle charge out of non primary vertices", 8, -0.5, 8.5, 3, -1.5, 1.5);
        vertex_charge->SetXTitle("the number of nonprimary vertices in the event");
        vertex_charge->SetYTitle("charge of particles going out of nonprimary vertices");

        PrimVer_ParNum = new TH1D("PrimVer_ParNum", "particle number of primary vertices", 8, -0.5, 7.5);
        PrimVer_ParNum->SetXTitle("the number of outgoing particles");
        PrimVer_ParNum->SetYTitle("Entry");

        NonPrimVer_ParNum = new TH1D("NonPrimVer_ParNum", "particle number of non primary vertices", 8, -0.5, 7.5);
        NonPrimVer_ParNum->SetXTitle("the number of outgoing particles");
        NonPrimVer_ParNum->SetYTitle("Entry");

        ParNum_NoVertex = new TH1D("ParNum_NoVertex", "number of charged particles with no vertex", 8, -0.5, 7.5);
        ParNum_NoVertex->SetXTitle("the number of charged particles");
        ParNum_NoVertex->SetYTitle("Entry");

        Par_NoVertex_Z = new TH1D("Par_NoVertex_Z", "Z measurement of charged particles with no vertex", 200, -400, 1600);
        Par_NoVertex_Z->SetXTitle("z of charged particles");
        Par_NoVertex_Z->SetYTitle("Entry");

        first = false;
    }

    //charged partilces not from vertex
    
    set<int> Partilce_ver;
    for (int i = 0; i < e.NVertex(); i++)
    {   
        vertex = e.vVertex(i);
        for (int j = 0; j < vertex.NOutParticles(); j++)
            {
                Partilce_ver.insert(vertex.iOutParticle(j));
            }
        if(vertex.IsBestPrimary()){
            Partilce_ver.insert(vertex.InParticle());
        }
    }
    //cout <<"**************" << endl;
    // int i_particle_nonvertex = 0;
    // 
    // for(int i=0; i < e.NParticle(); i++){
    //     if(!Partilce_ver.count(i)){
    //         if(abs(e.vParticle(i).Q())==1) {
    //             i_particle_nonvertex++;
    //             Par_NoVertex_ZFirst->Fill(e.vTrack(e.vParticle(i).iTrack()).ZFirst());
    //             //cout << (e.vCaloClus(e.vParticle(i).iCalorim(0))).E() << endl;
    //         }
    //     }
    // }
    int i_particle_nonvertex = 0;
    for (int i = 0; i < e.NParticle(); i++)
    {   
        if(e.vParticle(i).NVertex()==0 && abs(e.vParticle(i).Q())==1){
            i_particle_nonvertex++;
            double min_value = min(e.vTrack(e.vParticle(i).iTrack()).ZFirst(),e.vTrack(e.vParticle(i).iTrack()).ZLast());
            Par_NoVertex_Z->Fill(min_value);
        }
    }
    if(i_particle_nonvertex != 0) ParNum_NoVertex->Fill(i_particle_nonvertex);

    //non primary vertex and charge investigation
    int i_nonPri = 0;
    for (int i = 0; i < e.NVertex(); i++)
    {
        vertex = e.vVertex(i);
        if (!vertex.IsPrimary())
            i_nonPri++;
    }

    for (int i = 0; i < e.NVertex(); i++)
    {
        vertex = e.vVertex(i);
        if (!vertex.IsPrimary())
        {   
            NonPrimVer_ParNum->Fill(vertex.NOutParticles());
            for (int j = 0; j < vertex.NOutParticles(); j++)
            {
                vertex_charge->Fill(i_nonPri, e.vParticle(vertex.iOutParticle(j)).Q());
            }
        }
        else{
            PrimVer_ParNum->Fill(vertex.NOutParticles());
        }
    }
}

void UserJobEnd126()
{
}