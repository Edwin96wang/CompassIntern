void test(){
    TFile *file1 = new TFile("/localhome/ywang/analysis_compass/hist/hist70650.root", "READ");
    TFile *file2 = new TFile("/localhome/ywang/analysis_compass/hist/hist69853.root", "READ");
    TH1D* angle_proton1 = nullptr;
    TH1D* angle_proton2 = nullptr;
    file1->GetObject("angle_proton", angle_proton1);
    file2->GetObject("angle_proton", angle_proton2);
    double max1 = angle_proton1->GetMaximum();
    double max2 = angle_proton2->GetMaximum();
    angle_proton1->Scale(1000.0/max1);
    angle_proton1->SetXTitle("theta");
    angle_proton2->Scale(1000.0/max2);
    angle_proton1->Draw("H");
    gStyle->SetOptStat(0);
    angle_proton2->Draw("HSAME");
    angle_proton2->SetLineColor(kRed);

    auto legend = new TLegend();
    legend->AddEntry(angle_proton2, "Run Number = 69853 (normal)", "lp");
    legend->AddEntry(angle_proton1, "Run Number = 70650 (abnormal)");
    legend->Draw();
    
}