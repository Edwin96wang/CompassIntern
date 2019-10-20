void GraphProcess(){
    TFile *file = new TFile("./hist/hist69811.root", "READ");
    //TFile *file = new TFile("./hist_integrated.root", "READ");
    TH1D* graph_data = nullptr;
    file->GetObject("ThreePion_inv_mass", graph_data);

    //fitting
    gErrorIgnoreLevel = kWarning;
    file->GetObject("ThreePion_inv_mass", graph_data);
    int bin_left = graph_data->FindFirstBinAbove(graph_data->GetMaximum()* 3 / 10);
    int bin_right = graph_data->FindLastBinAbove(graph_data->GetMaximum()*3 / 10);
    double pos_left = graph_data->GetBinCenter(bin_left);
    double pos_right = graph_data->GetBinCenter(bin_right);
    TF1* f = new TF1("fit function", "[0]*(TMath::Gaus(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5]))", pos_left, pos_right);
    cout << pos_left << endl;
    cout << pos_right << endl;

    f->SetParameter(0, graph_data->GetMaximum());
    f->SetParameter(1, 1);
    f->SetParameter(2, 1);
    f->SetParameter(3, 0.01);
    f->FixParameter(4, 1.49);
    f->SetParameter(5, 1);
    f->SetParLimits(5, 0.01, 0.1);
    auto c1 = new TCanvas();
    graph_data -> Draw();
    graph_data->Fit("fit function","R");

    // graph editting
    TCanvas* canvas = new TCanvas("c","c",900,900);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    graph_data->SetTitle("");
    graph_data->GetXaxis()->SetTitle("invariant mass (GeV)");
    graph_data->GetXaxis()->SetTitleSize(0.04);
    graph_data->GetXaxis()->SetLabelSize(0.04);
    graph_data->GetXaxis()->SetTitleOffset(1.2);
    graph_data->GetXaxis()->SetLabelOffset(0.02);
    graph_data->GetXaxis()->SetRangeUser(0, 5);

    graph_data->GetYaxis()->SetTitle("entries");
    graph_data->GetYaxis()->SetTitleSize(0.04);
    graph_data->GetYaxis()->SetLabelSize(0.04);
    graph_data->GetYaxis()->SetTitleOffset(1.6);
    
    TPaletteAxis *palette = (TPaletteAxis*)graph_data->GetListOfFunctions()->FindObject("palette");
    graph_data->Draw("COLZ");
    canvas->SetBottomMargin(0.13);
    canvas->SetLeftMargin(0.13);
}