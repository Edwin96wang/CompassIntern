void GraphProcess(){
    //TFile *file = new TFile("./hist/hist70195.root", "READ");
    TFile *file = new TFile("./hist_integrated.root", "READ");
    TH2D* graph_data = nullptr;
    file->GetObject("histo_integrate_spillnumber", graph_data);
    TCanvas* canvas = new TCanvas("c","c",1500,600);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    graph_data->SetTitle("");
    //graph_data->GetXaxis()->SetTitle("spill number");
    //graph_data->GetYaxis()->SetTitle("spill time (sec)");
    graph_data->GetXaxis()->SetTitleSize(0.06);
    graph_data->GetXaxis()->SetLabelSize(0.06);
    graph_data->GetXaxis()->SetTitleOffset(0.95);
    graph_data->GetYaxis()->SetTitleSize(0.06);
    graph_data->GetYaxis()->SetLabelSize(0.06);
    graph_data->GetYaxis()->SetTitleOffset(0.5);
    graph_data->GetXaxis()->SetLabelOffset(0.02);
    TPaletteAxis *palette = (TPaletteAxis*)graph_data->GetListOfFunctions()->FindObject("palette");
    graph_data->Draw("COLZ");
    canvas->SetBottomMargin(0.13);
    canvas->SetLeftMargin(0.06);
}