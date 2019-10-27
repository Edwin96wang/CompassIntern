void Total_mass_comp(){
    TFile *file = new TFile("../hist/hist70717.root", "READ");
    TFile *file2 = new TFile("../hist/hist70650.root", "READ");
    //TFile *file = new TFile("../hist_integrated.root", "READ");
    TH1D* graph_data = nullptr;
    TH1D* graph_data2 = nullptr;
    file->GetObject("Total_inv_mass", graph_data);
    file2->GetObject("Total_inv_mass", graph_data2);

    //data editting
    double max = graph_data->GetMaximum();
    for(int i = 1; i <= graph_data->GetNbinsX(); i++){
        graph_data->SetBinContent(i, 1000*graph_data->GetBinContent(i)/max);
    }
    max = graph_data2->GetMaximum();
    for(int i = 1; i <= graph_data2->GetNbinsX(); i++){
        graph_data2->SetBinContent(i, 1000*graph_data2->GetBinContent(i)/max);
    }
    // graph editting
    TCanvas* canvas = new TCanvas("c","c",600,500);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    graph_data->SetTitle("");
    graph_data->GetXaxis()->SetTitle("invariant mass (GeV)");
    graph_data->GetXaxis()->SetTitleSize(0.05);
    graph_data->GetXaxis()->SetLabelSize(0.05);
    graph_data->GetXaxis()->SetTitleOffset(1.1);
    graph_data->GetXaxis()->SetLabelOffset(0.02);

    graph_data->GetYaxis()->SetTitle("entries");
    graph_data->GetYaxis()->SetTitleSize(0.05);
    graph_data->GetYaxis()->SetLabelSize(0.05);
    graph_data->GetYaxis()->SetTitleOffset(1.3);
    
    graph_data2->SetLineColor(kBlue);
    graph_data->SetLineColor(kRed);

    //TPaletteAxis *palette = (TPaletteAxis*)graph_data->GetListOfFunctions()->FindObject("palette");
    graph_data->Draw();
    graph_data2->Draw("SAMEAPL");

    auto legend = new TLegend(0.65, 0.7, 0.98, 0.9);
    legend->AddEntry(graph_data, "Normal run (70717)", "lp");
    legend->AddEntry(graph_data2, "Abnormal run (70650)");
    legend->Draw();

    canvas->SetBottomMargin(0.13);
    canvas->SetLeftMargin(0.12);
    canvas->SetTopMargin(0.06);
    canvas->SetRightMargin(0.01);

    
}