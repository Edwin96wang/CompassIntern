#include "TGraph.h"



{   
    TCanvas *c1 = new TCanvas("c1","example of two overlappingpads",1200,800);
    TPad *pad = new TPad("pad","",0,0,1,1);
    pad->Draw();
    pad->cd();
    Total_invariant_mass->Draw();
    Total_invariant_mass->GetYaxis()->SetRangeUser(0,4);
    c1->Update();
    TGaxis *axis = new TGaxis(pad->GetUxmax(),pad->GetUymin(),pad->GetUxmax(),pad->GetUymax(),10,30,510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->Draw();

    // TPad *pad2 = new TPad("pad2","",0,0,1,1);
    // pad2->Draw();
    // pad2->cd();
    Shrink_graph_y(ECOL_per,0.5)->Draw();

    //cout << gPad->GetUxmax() << endl;
}