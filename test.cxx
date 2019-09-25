void combine_TGraphs(TCanvas* c, TMultiGraph* mg, int* range_left, int* range_right, TGraph g1, TGraph g2){
    c->cd();
    g1.GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
    g2.GetYaxis()->SetRangeUser(range_right[0], range_right[1]);
    double ratio_trans = (range_left[1] - range_left[0]) / (range_right[1] - range_right[0]);
    TGraph *new_g2 = Shrink_graph_y(&g2, -range_right[0], ratio_trans);
    new_g2 = move_graph_y(new_g2, range_left[0]);
    new_g2->SetMarkerColor(2);
    new_g2->SetMarkerStyle(22);
    new_g2->SetTitle(g2.GetTitle());
    mg->Add(g1);
    mg->Add(new_g2);
    mg->Draw("apl");
    mg->GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
    mg->GetXaxis()->SetTitle(g1.GetXaxis()->GetTitle);
    mg->GetYaxis()->SetTitle(g1.GetYaxis()->GetTitle);
    c->Update();
    c->Modified();
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),range_left[0],gPad->GetUxmax(),range_left[1],range_right[0],range_right[1],510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->Draw();
    axis->SetTitle(g2.GetYaxis()->GetTitle);
    axis->SetTitleOffset(0.7);
    axis->SetTitleColor(kRed);
}