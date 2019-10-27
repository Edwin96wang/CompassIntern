void ImaProGraphWide(){
    //TFile *file = new TFile("./hist/hist69811.root", "READ");
    TFile *file = new TFile("./hist_integrated.root", "READ");
    TGraph* graph_data = nullptr;
    file->GetObject("half_w_invMass", graph_data);

    // graph editting
    TCanvas* canvas = new TCanvas("c","c",1200,500);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    graph_data->SetTitle("");
    graph_data->GetXaxis()->SetTitle("run number");
    graph_data->GetXaxis()->SetTitleSize(0.05);
    graph_data->GetXaxis()->SetLabelSize(0.05);
    graph_data->GetXaxis()->SetTitleOffset(1);
    graph_data->GetXaxis()->SetLabelOffset(0.02);

    graph_data->GetYaxis()->SetTitle("invariant mass (GeV)");
    graph_data->GetYaxis()->SetTitleSize(0.05);
    graph_data->GetYaxis()->SetLabelSize(0.05);
    graph_data->GetYaxis()->SetTitleOffset(0.5);
    
    //TPaletteAxis *palette = (TPaletteAxis*)graph_data->GetListOfFunctions()->FindObject("palette");
    graph_data->Draw("APL");
    canvas->SetBottomMargin(0.13);
    canvas->SetLeftMargin(0.05);
    canvas->SetTopMargin(0.03);
    canvas->SetRightMargin(0.01);
}