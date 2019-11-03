void PrimVertex(){
    TFile *file = new TFile("../hist/hist70171.root", "READ");
    //TFile *file = new TFile("./hist_integrated.root", "READ");
    TH2D* graph_data = nullptr;
    file->GetObject("PrimVertexXY_3", graph_data);

    // graph editting
    TCanvas* canvas = new TCanvas("c","c",800,600);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    graph_data->SetTitle("");
    graph_data->GetXaxis()->SetTitle("x (cm)");
    graph_data->GetXaxis()->SetTitleSize(0.05);
    graph_data->GetXaxis()->SetLabelSize(0.05);
    graph_data->GetXaxis()->SetTitleOffset(1);
    graph_data->GetXaxis()->SetLabelOffset(0.02);

    graph_data->GetYaxis()->SetTitle("y (cm)");
    graph_data->GetYaxis()->SetTitleSize(0.05);
    graph_data->GetYaxis()->SetLabelSize(0.05);
    graph_data->GetYaxis()->SetTitleOffset(1);
    
    //TPaletteAxis *palette = (TPaletteAxis*)graph_data->GetListOfFunctions()->FindObject("palette");
    graph_data->Draw("COLZ");
    canvas->SetBottomMargin(0.11);
    canvas->SetLeftMargin(0.1);
    canvas->SetTopMargin(0.02);
    canvas->SetRightMargin(0.09);
}