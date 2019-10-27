void ImaProTwonarrow(){
    TFile *file = new TFile("./hist/hist70717.root", "READ");
    //TFile *file = new TFile("./hist_integrated.root", "READ");
    TH1I* graph_data = nullptr;
    TH1I* graph_data2 = nullptr;
    file->GetObject("Photon_Number_cut", graph_data);
    file->GetObject("Photon_Number", graph_data2);

    // graph editting
    TCanvas* canvas = new TCanvas("c","c",600,500);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    graph_data->SetTitle("");
    graph_data->GetXaxis()->SetTitle("Photon number per event");
    graph_data->GetXaxis()->SetTitleSize(0.05);
    graph_data->GetXaxis()->SetLabelSize(0.05);
    graph_data->GetXaxis()->SetTitleOffset(1.1);
    graph_data->GetXaxis()->SetLabelOffset(0.02);

    graph_data->GetYaxis()->SetTitle("entries");
    graph_data->GetYaxis()->SetTitleSize(0.05);
    graph_data->GetYaxis()->SetLabelSize(0.05);
    graph_data->GetYaxis()->SetTitleOffset(1.1);
    
    graph_data2->SetLineColor(kBlue);
    graph_data->SetLineColor(kRed);

    //TPaletteAxis *palette = (TPaletteAxis*)graph_data->GetListOfFunctions()->FindObject("palette");
    graph_data->Draw("");
    graph_data2->Draw("SAME");

    auto legend = new TLegend(0.1, 0.7, 0.48, 0.9);
    legend->AddEntry(graph_data2, "without energy cut", "lp");
    legend->AddEntry(graph_data, "with energy cut");
    legend->Draw();

    canvas->SetBottomMargin(0.13);
    canvas->SetLeftMargin(0.12);
    canvas->SetTopMargin(0.06);
    canvas->SetRightMargin(0.01);

    
}