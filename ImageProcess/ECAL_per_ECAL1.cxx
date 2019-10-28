TGraph* Shrink_graph_y(TGraph* g, double move, double ratio){
  double* xx = g->GetX();
  double* yy = g->GetY();
  int n= g->GetN();
  for(int i=0; i< n; i++){
    yy[i] = (yy[i]+move) * ratio;
  }
  return new TGraph(n,xx,yy);
}

TGraph* move_graph_y(TGraph* g, double move){
  double* xx = g->GetX();
  double* yy = g->GetY();
  int n= g->GetN();
  for(int i=0; i< n; i++){
    yy[i] = yy[i]+move;
  }
  return new TGraph(n,xx,yy);
}

void ECAL_per_ECAL1(){
    //TFile *file = new TFile("../hist/hist69811.root", "READ");
    TFile *file = new TFile("../hist_integrated.root", "READ");
    TGraph* g1 = nullptr;
    TGraph* g2 = nullptr;
    file->GetObject("ECAL1", g1);
    file->GetObject("ECAL_per", g2);

    // graph editting
    TCanvas* canvas = new TCanvas("c","c",1200,600);
    TMultiGraph* mg = new TMultiGraph("mg2","");
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    
    canvas->SetBottomMargin(0.11);
    canvas->SetLeftMargin(0.06);
    canvas->SetTopMargin(0.03);
    canvas->SetRightMargin(0.06);


    //integration
    double range_right[2] = {10,60};
    double range_left[2] = {0.5,2.5};
    g1->GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
    g2->GetYaxis()->SetRangeUser(range_right[0], range_right[1]);
    double ratio_trans = (double) (range_left[1] - range_left[0]) / (range_right[1] - range_right[0]);
    TGraph *new_g2 = Shrink_graph_y(g2, -range_right[0], ratio_trans);
    new_g2 = move_graph_y(new_g2, range_left[0]);
    new_g2->SetMarkerColor(2);
    g1->SetMarkerColor(kGreen+4);
    new_g2->SetMarkerStyle(22);
    canvas->cd();
    canvas->SetGrid();
    mg->Add(g1);
    mg->Add(new_g2);
    mg->GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
    
    mg->SetTitle("");
    mg->GetXaxis()->SetTitle("run number");
    mg->GetXaxis()->SetTitleSize(0.05);
    mg->GetXaxis()->SetLabelSize(0.05);
    mg->GetXaxis()->SetTitleOffset(1);
    mg->GetXaxis()->SetLabelOffset(0.02);

    mg->GetYaxis()->SetTitle("photon number");
    mg->GetYaxis()->SetTitleSize(0.05);
    mg->GetYaxis()->SetLabelSize(0.05);
    mg->GetYaxis()->SetTitleOffset(0.6);
    mg->Draw("APL");

    canvas->Update();
    canvas->Modified();
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),range_left[0],gPad->GetUxmax(),range_left[1],range_right[0],range_right[1],510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->Draw();
    axis->SetTitle("percentage (%)");
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(0.6);
    axis->SetLabelSize(0.05);
    axis->SetTitleColor(kRed);

    auto legend = new TLegend(0.57, 0.87, 0.89, 0.98);
    legend->AddEntry(g1, "Photon number per event from ECAL1", "lp");
    legend->AddEntry(new_g2, "Photon percentage #alpha");
    legend->Draw();

}
