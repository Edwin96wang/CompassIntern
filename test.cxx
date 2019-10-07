void test(){
    TFile *file = new TFile("/localhome/ywang/analysis_compass/hist/hist69853.root", "READ");
    TH1D* ThreePion_inv_mass = nullptr;
    gErrorIgnoreLevel = kWarning;
    file->GetObject("ThreePion_inv_mass", ThreePion_inv_mass);
    int bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum()*3/10);
    int bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum()*3/10);
    double pos_left = ThreePion_inv_mass->GetBinCenter(bin_left);
    double pos_right = ThreePion_inv_mass->GetBinCenter(bin_right);
    TF1* f = new TF1("fit function", "[0]*(TMath::Gaus(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5]))", pos_left, pos_right);
    cout << pos_left << endl;
    cout << pos_right << endl;
    f->SetParameter(0, ThreePion_inv_mass->GetMaximum());
    f->SetParameter(1, 1);
    f->SetParameter(2, 1);
    f->SetParameter(3, 0.01);
    f->FixParameter(4, 1.49);
    f->SetParameter(5, 1);
    f->SetParLimits(5, 0.01, 0.1);
    auto c1 = new TCanvas();
    ThreePion_inv_mass -> Draw();
    ThreePion_inv_mass->Fit("fit function","R");

    // auto c2 = new TCanvas();
    // f->Draw();
    // TF1* f1 = new TF1("f1", "[0]*TMath::Gaus(x,[1],[2])", pos_left, pos_right);
    // f1 -> SetParameter(0, f->GetParameter(0));
    // f1 -> SetParameter(1, f->GetParameter(1));
    // f1 -> SetParameter(2, f->GetParameter(2));
    // f1->SetRange(0,8);
    // f1->Draw("SAME");
    // TF1* f2 = new TF1("f2", "[0]*TMath::Gaus(x,[1],[2])", pos_left, pos_right);
    // f2 -> SetParameter(0, f->GetParameter(0)*f->GetParameter(3));
    // f2 -> SetParameter(1, f->GetParameter(4));
    // f2 -> SetParameter(2, f->GetParameter(5));
    // f2 -> Draw("SAME");

    // c1->cd();
    // ThreePion_inv_mass -> Draw();
    // ThreePion_inv_mass->Fit("fit function","R");
    
    cout << f->GetParameter(0)<< endl;
    cout << f->GetParameter(1)<< endl;
    cout << f->GetParameter(2)<< endl;

    
}

// files[i]->GetObject("ThreePion_inv_mass", ThreePion_inv_mass);
// bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum() * 3 / 10);
// bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum() * 3 / 10);
// pos_left = ThreePion_inv_mass->GetBinCenter(bin_left);
// pos_right = ThreePion_inv_mass->GetBinCenter(bin_right);
// static TF1 *f = new TF1("fit function", "[0]*(TMath::Gaus(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5]))", pos_left, pos_right);
// f->SetParameter(0, ThreePion_inv_mass->GetMaximum());
// f->SetParameter(1, 1);
// f->SetParameter(2, 1);
// f->SetParameter(3, 0.01);
// f->FixParameter(4, 1.49);
// f->SetParameter(5, 1);
// f->SetParLimits(5, 0.01, 0.1);
// ThreePion_inv_mass->Draw();
// ThreePion_inv_mass->Fit("fit function", "RQ");
// // bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum()*9.5/10);
// // bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum()*9.5/10);
// //         //cout << "ii: " << ii << "filenumber: " << filenames[i].num << "value: " << yq[1] << endl;
// // center_mass = (ThreePion_inv_mass->GetBinCenter(bin_right) + ThreePion_inv_mass->GetBinCenter(bin_left))/2;
// Three_pion_mass->SetPoint(ii, filenames[i].num, f->GetParameter(1));
// bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum() * 5 / 10);
// bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum() * 5 / 10);
// Three_pion_mass->SetPointEYhigh(ii, ThreePion_inv_mass->GetBinCenter(bin_right) - center_mass);
// Three_pion_mass->SetPointEYlow(ii, center_mass - ThreePion_inv_mass->GetBinCenter(bin_left));