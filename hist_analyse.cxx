#include <iostream>
#include <string.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraph.h>
#include <set>
#include "TCanvas.h"
#include "TGaxis.h"

using namespace std;
TGraph* Shrink_graph_y(TGraph* g, double move, double ratio);
TGraph* move_graph_y(TGraph* g, double move);
void combine_TGraphs(TCanvas* c, TMultiGraph* mg, double* range_left, double* range_right, TGraph* g1, TGraph* g2, string s1, string s2);
TH2D* shrink_TH2D_y(TH2D* his,double ymin, double ymax);

int get_int(string t)
{
    string s1 = "hist";
    string s2 = ".root";
    string::size_type i;
    i = t.find(s1);
    t.erase(i, s1.length());
    i = t.find(s2);
    t.erase(i, s2.length());
    return stoi(t);
}

int hist_analyse()
{
    DIR *dir;
    string path = "/localhome/ywang/analysis_compass/hist";

    const char *cstr = path.c_str();
    struct dirent *ent;
    set<int> removed_events = {70195, 70070};
    int Num_removed = removed_events.size();

    struct filenames_int
    {
        int num;
        string name;
    };
    vector<filenames_int> filenames;
    if ((dir = opendir(cstr)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                filenames.push_back({get_int((string)ent->d_name), (path + "/" + (string)ent->d_name)});
            }
        }
    }
    sort(filenames.begin(), filenames.end(), [](filenames_int const &a, filenames_int const &b) { return a.num < b.num; });

    //variables defined:
    int RunTime_range = filenames.back().num - filenames.front().num;
    int RunTime_first = filenames.front().num;
    int RunTime_last = filenames.back().num;
    double xq[3] = {0.16, 0.5, 0.68};
    double yq[3];
    int xbin; //bin number of histogram

    //begin reading the files:
    int filesize = filenames.size();
    TFile *files[filesize];
    files[0] = new TFile((filenames[0].name).c_str(), "READ");

    //filesize = 10;
    int sum_VerticeNum                  = 0;
    int sum_SpillNum                    = 0;
    int EventNum_sum_RunNum             = 0;
    TH2D *spillNum_VerticeNum[filesize];
    TH2D *spillNum_PriVerticeNum[filesize];
    TH2D *spillNum_NonPriVerticeNum[filesize];
    TH1D *ThreePion_inv_mass            = nullptr;
    TH1D *Total_inv_mass                = nullptr;
    TH2I *ECOL_hist                     = nullptr;
    TH2D *SpillTime_SpillNum_2D         = nullptr;

    files[0]->GetObject("spillNum_VerticeNum", spillNum_VerticeNum[0]);
    files[0]->GetObject("ECOL_hist", ECOL_hist);
    files[0]->GetObject("SpillTime_SpillNum_2D", SpillTime_SpillNum_2D);
    files[0]->GetObject("Total_inv_mass", Total_inv_mass);
    int VerticeNum = spillNum_VerticeNum[0]->GetNbinsY();
    int spillNum = spillNum_VerticeNum[0]->GetNbinsX();
    int Num_ECAL1 = ECOL_hist->GetNbinsX();
    int Num_ECAL2 = ECOL_hist->GetNbinsY();
    int spillTime = SpillTime_SpillNum_2D->GetNbinsY();
    int inv_mass = Total_inv_mass->GetNbinsX();

    // histograms instantiation
    TH2D *histo_integrate_spillnumber = new TH2D("histo_integrate_spillnumber", "event countings in certain Spillnum of certain runnumber",
                                                 RunTime_range, RunTime_first - 0.5, RunTime_last - 0.5, spillNum, -0.5, spillNum-0.5);
    histo_integrate_spillnumber->SetXTitle("Run Number");
    histo_integrate_spillnumber->SetYTitle("spillNum");

    TH1D *EventNum_RunNum = new TH1D("EventNum_RunNum", "Event countings of the RunNumber", RunTime_range,  RunTime_first - 0.5, RunTime_last - 0.5);
    EventNum_RunNum->SetXTitle("Run Number");
    EventNum_RunNum->SetYTitle("Event counting");

    TH2D *VerticeNum_RunNum_integrated = new TH2D("VerticeNum_RunNum_integrated",
                                                  "VerticeNumber through different run time", RunTime_range, RunTime_first-0.5, RunTime_last-0.5, VerticeNum, -0.5, VerticeNum-0.5);
    VerticeNum_RunNum_integrated->SetXTitle("Run Number");
    VerticeNum_RunNum_integrated->SetYTitle("VerticeNum");

    TH2D *PrimVerticeNum_RunNum_integrated = new TH2D("PrimVerticeNum_RunNum_integrated",
                                                      "PrimVerticeNumber through different run time", RunTime_range, RunTime_first-0.5, RunTime_last-0.5, VerticeNum, -0.5, VerticeNum-0.5);
    PrimVerticeNum_RunNum_integrated->SetXTitle("Run Number");
    PrimVerticeNum_RunNum_integrated->SetYTitle("PrimVerticeNum");

    TH2D *NonPrimVerticeNum_RunNum_integrated = new TH2D("NonPrimVerticeNum_RunNum_integrated",
                                                         "NonPrimVerticeNumber through different run time", RunTime_range, RunTime_first-0.5, RunTime_last-0.5, VerticeNum, -0.5, VerticeNum-0.5);
    NonPrimVerticeNum_RunNum_integrated->SetXTitle("Run Number");
    NonPrimVerticeNum_RunNum_integrated->SetYTitle("NonPrimVerticeNum");
    
    TH2D *Total_invariant_mass_His = new TH2D("Total_invariant_mass_His", "invariant mass of pions and photons (photon energy cutoff 1&4 GeV)", 
                                            RunTime_range, RunTime_first-0.5, RunTime_last-0.5, 200, 0, 8.0);
    Total_invariant_mass_His->SetXTitle("Run Number");
    Total_invariant_mass_His->SetYTitle("invariant mass (GeV)");

    // TGraph instantiation
    //three pion invariant mass:
    TGraphAsymmErrors *Three_pion_mass = new TGraphAsymmErrors(filesize - Num_removed);
    Three_pion_mass->SetMarkerColor(4);
    Three_pion_mass->SetMarkerStyle(21);
    Three_pion_mass->SetTitle("invariant mass of 3 pions");
    Three_pion_mass->SetName("Three_pion_mass");
    Three_pion_mass->GetXaxis()->SetTitle("Run Number");
    Three_pion_mass->GetYaxis()->SetTitle("s_3_pions (GeV)");

    //total invariant mass:
    TGraphAsymmErrors *Total_invariant_mass = new TGraphAsymmErrors(filesize - Num_removed);
    Total_invariant_mass->SetMarkerColor(4);
    Total_invariant_mass->SetMarkerStyle(21);
    Total_invariant_mass->SetTitle("invariant mass of pions and photons (photon energy cutoff 1&4 GeV)");
    Total_invariant_mass->SetName("Total_invariant_mass");
    Total_invariant_mass->GetXaxis()->SetTitle("Run Number");
    Total_invariant_mass->GetYaxis()->SetTitle("invariant mass (GeV)");

    //ratio of Col2 to Col1:
    TGraph *ECAL_per = new TGraph(filesize - Num_removed);
    ECAL_per->SetMarkerColor(2);
    ECAL_per->SetMarkerStyle(22);
    ECAL_per->SetTitle("Percentage of photons measured by ECAL1");
    ECAL_per->SetName("ECAL_per");
    ECAL_per->GetXaxis()->SetTitle("Run Number");
    ECAL_per->GetYaxis()->SetTitle("Percentage (%)");

    TGraph *ECAL1 = new TGraph(filesize - Num_removed);
    ECAL1->SetMarkerColor(2);
    ECAL1->SetMarkerStyle(22);
    ECAL1->SetTitle("photon number per event from ECAL1");
    ECAL1->SetName("ECAL1");
    ECAL1->GetXaxis()->SetTitle("Run Number");
    ECAL1->GetYaxis()->SetTitle("Photon number");

    TGraph *ECAL2 = new TGraph(filesize - Num_removed);
    ECAL2->SetMarkerColor(2);
    ECAL2->SetMarkerStyle(22);
    ECAL2->SetTitle("photon number per event from ECAL2");
    ECAL2->SetName("ECAL2");
    ECAL2->GetXaxis()->SetTitle("Run Number");
    ECAL2->GetYaxis()->SetTitle("Photon number");

    int ii = 0;
    //looping through loops:
    for (int i = 0; i < filesize; i++)
    {
        if (removed_events.count(filenames[i].num))
        {
            cout << "removed event: " << filenames[i].num << endl;
            continue;
        }

        files[i] = new TFile((filenames[i].name).c_str(), "READ");
        xbin = filenames[i].num + 1 - filenames.front().num;

        //event counting with spillnumber vs runnumber
        files[i]->GetObject("SpillTime_SpillNum_2D", SpillTime_SpillNum_2D);
        for (int i_spill = 0; i_spill < spillNum; i_spill++)
        {
            sum_SpillNum = 0;
            for (int i_time = 0; i_time < spillTime; i_time++)
            {
                sum_SpillNum += SpillTime_SpillNum_2D->GetBinContent(i_spill + 1, i_time + 1);
            }
            histo_integrate_spillnumber->SetBinContent(xbin, i_spill + 1, sum_SpillNum);
        }

        //event counting for each runnumber
        files[i]->GetObject("SpillTime_SpillNum_2D", SpillTime_SpillNum_2D);
        EventNum_sum_RunNum = 0;
        for (int i_spill = 0; i_spill < spillNum; i_spill++)
        {
            for (int i_time = 0; i_time < spillTime; i_time++)
            {
                EventNum_sum_RunNum += SpillTime_SpillNum_2D->GetBinContent(i_spill + 1, i_time + 1);
            }
        }
        EventNum_RunNum->SetBinContent(xbin, EventNum_sum_RunNum);

        //vertex number vs run number
        files[i]->GetObject("spillNum_VerticeNum", spillNum_VerticeNum[i]);

        for (int i_VerNum = 0; i_VerNum < VerticeNum; i_VerNum++)
        {
            sum_VerticeNum = 0;
            for (int i_spill = 0; i_spill < spillNum; i_spill++)
            {
                sum_VerticeNum += spillNum_VerticeNum[i]->GetBinContent(i_spill + 1, i_VerNum + 1);
            }
            VerticeNum_RunNum_integrated->SetBinContent(xbin, i_VerNum + 1, sum_VerticeNum);
        }

        //primary vertex number vs run number
        files[i]->GetObject("SpillNum_PrimVerticeNum", spillNum_PriVerticeNum[i]);
        for (int i_VerNum = 0; i_VerNum < VerticeNum; i_VerNum++)
        {
            sum_VerticeNum = 0;
            for (int i_spill = 0; i_spill < spillNum; i_spill++)
            {
                sum_VerticeNum += spillNum_PriVerticeNum[i]->GetBinContent(i_spill + 1, i_VerNum + 1);
            }
            PrimVerticeNum_RunNum_integrated->SetBinContent(xbin, i_VerNum + 1, sum_VerticeNum);
        }

        //non-primary vertex number vs run number
        files[i]->GetObject("SpillNum_NonPrimVerticeNum", spillNum_NonPriVerticeNum[i]);
        for (int i_VerNum = 0; i_VerNum < VerticeNum; i_VerNum++)
        {
            sum_VerticeNum = 0;
            for (int i_spill = 0; i_spill < spillNum; i_spill++)
            {
                sum_VerticeNum += spillNum_NonPriVerticeNum[i]->GetBinContent(i_spill + 1, i_VerNum + 1);
            }
            NonPrimVerticeNum_RunNum_integrated->SetBinContent(xbin, i_VerNum + 1, sum_VerticeNum);
        }

        //three pion invariant mass:
        files[i]->GetObject("ThreePion_inv_mass", ThreePion_inv_mass);
        ThreePion_inv_mass->GetQuantiles(3, yq, xq);
        Three_pion_mass->SetPoint(ii, filenames[i].num, yq[1]);
        Three_pion_mass->SetPointEYhigh(ii, yq[2] - yq[1]);
        Three_pion_mass->SetPointEYlow(ii, yq[1] - yq[0]);

        //total invariant mass:
        files[i]->GetObject("Total_inv_mass", Total_inv_mass);
        Total_inv_mass->GetQuantiles(3, yq, xq);
        //cout << "ii: " << ii << "filenumber: " << filenames[i].num << "value: " << yq[1] << endl;
        Total_invariant_mass->SetPoint(ii, filenames[i].num, yq[1]);
        Total_invariant_mass->SetPointEYhigh(ii, yq[2] - yq[1]);
        Total_invariant_mass->SetPointEYlow(ii, yq[1] - yq[0]);
        //Histogram:
        for (int i_mass = 0; i_mass < inv_mass; i_mass++)
        {
            Total_invariant_mass_His->SetBinContent(xbin, i_mass + 1, Total_inv_mass->GetBinContent(i_mass + 1));
        }

        //ECAL1 and ECAL2 ratio
        int totNum_ECAL1 = 0;
        int totNum_ECAL2 = 0;
        files[i]->GetObject("ECOL_hist", ECOL_hist);
        for (int i_ECAL1 = 0; i_ECAL1 < Num_ECAL1; i_ECAL1++)
        {
            for (int i_ECAL2 = 0; i_ECAL2 < Num_ECAL2; i_ECAL2++)
            {
                totNum_ECAL1 += ECOL_hist->GetBinContent(i_ECAL1 + 1, i_ECAL2 + 1) * i_ECAL1;
                totNum_ECAL2 += ECOL_hist->GetBinContent(i_ECAL1 + 1, i_ECAL2 + 1) * i_ECAL2;
            }
        }
        double ECAL_percent = (double)totNum_ECAL1 / (totNum_ECAL2 + totNum_ECAL1);
        ECAL_per->SetPoint(ii, filenames[i].num, 100 * ECAL_percent);
        ECAL1->SetPoint(ii, filenames[i].num, (double)totNum_ECAL1 / EventNum_sum_RunNum);
        ECAL2->SetPoint(ii, filenames[i].num, (double)totNum_ECAL2 / EventNum_sum_RunNum);

        //close the histogram
        ii++;
        files[i]->Close();
    }

    // vertice 3 over vertex 1
    int x_RunNum = VerticeNum_RunNum_integrated->GetNbinsX();
    TGraph *Vertice3_over_Vertice1 = new TGraph(filesize - Num_removed);
    Vertice3_over_Vertice1->SetTitle("counts of 3 Vertice over counts of 1 Vertex");
    Vertice3_over_Vertice1->SetName("Vertice3_over_Vertice1");
    Vertice3_over_Vertice1->SetMarkerColor(4);
    Vertice3_over_Vertice1->SetMarkerStyle(3);
    Vertice3_over_Vertice1->GetXaxis()->SetTitle("Run Number");
    Vertice3_over_Vertice1->GetYaxis()->SetTitle("ratio");

    // looping through the histogram with runnumber
    ii = 0;
    double ratio_vertex;
    for (int i = 0; i < filesize; i++)
    {
        if (removed_events.count(filenames[i].num))
            continue;
        if (VerticeNum_RunNum_integrated->GetBinContent(filenames[i].num - RunTime_first, 2) == 0)
        {
            ratio_vertex = 0;
        }
        else
        {
            ratio_vertex = VerticeNum_RunNum_integrated->GetBinContent(filenames[i].num - RunTime_first, 4) / VerticeNum_RunNum_integrated->GetBinContent(filenames[i].num - RunTime_first, 2);
        }
        Vertice3_over_Vertice1->SetPoint(ii, filenames[i].num, ratio_vertex);
        ii++;
    }

    TFile *file_integrated = new TFile("/localhome/ywang/analysis_compass/hist_integrated.root", "recreate");
    histo_integrate_spillnumber             ->Write();
    VerticeNum_RunNum_integrated            ->Write();
    PrimVerticeNum_RunNum_integrated        ->Write();
    NonPrimVerticeNum_RunNum_integrated     ->Write();
    Vertice3_over_Vertice1                  ->Write();
    Three_pion_mass                         ->Write();
    Total_invariant_mass                    ->Write();
    Total_invariant_mass_His                ->Write();
    ECAL_per                                ->Write();
    ECAL1                                   ->Write();
    ECAL2                                   ->Write();
    EventNum_RunNum                         ->Write();
    

    //creating canvas1: invariance mass and ECAL1 percentage
    double range_right[2];
    double range_left[2];
    TCanvas* c1 = new TCanvas("Inv_mass_ECAL_per","multigraph1",1500,800);
    TMultiGraph* mg1 = new TMultiGraph("mg","invariance mass and ECAL1 percentage");
    range_right[0]=0;
    range_right[1]=35;
    range_left[0]=1;
    range_left[1]=5;
    combine_TGraphs(c1, mg1, range_left, range_right, Total_invariant_mass, ECAL_per,"Invariant mass with quantiles (68%)","Percentage (ECAL1)");
    c1->Write();
    mg1->Write();

    //creating canvas2: invariance mass and ECAL1
    TCanvas* c2 = new TCanvas("Inv_mass_ECAL1","multigraph2",1500,800);
    TMultiGraph* mg2 = new TMultiGraph("mg2","invariance mass and ECAL1");
    range_right[0]=0;
    range_right[1]=0.5;
    range_left[0]=1;
    range_left[1]=5;
    combine_TGraphs(c2, mg2, range_left, range_right, Total_invariant_mass, ECAL1,"Invariant mass with quantiles (68%)","Photon number (ECAL1)");
    c2->Write();
    mg2->Write();

    //creating canvas3: invariance mass and ECAL2
    TCanvas* c3 = new TCanvas("Inv_mass_ECAL2","multigraph3",1500,800);
    TMultiGraph* mg3 = new TMultiGraph("mg3","invariance mass and ECAL2");
    range_right[0]=0;
    range_right[1]=1.3;
    range_left[0]=1;
    range_left[1]=5;
    combine_TGraphs(c3, mg3, range_left, range_right, Total_invariant_mass, ECAL2,"Invariant mass with quantiles (68%)","Photon number (ECAL2)");
    c3->Write();
    mg3->Write();

    file_integrated->Close();

    return 0;
}

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

void combine_TGraphs(TCanvas* c, TMultiGraph* mg, double* range_left, double* range_right, TGraph* g1, TGraph* g2, string s1, string s2){
    g1->GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
    g2->GetYaxis()->SetRangeUser(range_right[0], range_right[1]);
    double ratio_trans = (range_left[1] - range_left[0]) / (range_right[1] - range_right[0]);
    TGraph *new_g2 = Shrink_graph_y(g2, -range_right[0], ratio_trans);
    new_g2 = move_graph_y(new_g2, range_left[0]);
    new_g2->SetMarkerColor(2);
    new_g2->SetMarkerStyle(22);
    new_g2->SetTitle(g2->GetTitle());
    c->cd();
    c->SetGrid();
    mg->Add(g1);
    mg->Add(new_g2);
    mg->GetYaxis()->SetRangeUser(1,5);
    mg->GetXaxis()->SetTitle(g1->GetXaxis()->GetTitle());
    mg->GetYaxis()->SetTitle(g1->GetYaxis()->GetTitle());
    mg->Draw("apl");
    c->Update();
    c->Modified();
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),range_left[0],gPad->GetUxmax(),range_left[1],range_right[0],range_right[1],510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->Draw();
    axis->SetTitle(g2->GetYaxis()->GetTitle());
    axis->SetTitleOffset(0.7);
    axis->SetTitleColor(kRed);

    auto legend = new TLegend(0.1, 0.7, 0.48, 0.9);
    legend->AddEntry(g1, s1.c_str(), "lep");
    legend->AddEntry(new_g2, s2.c_str());
    legend->Draw();
}

// TH2D* shrink_TH2D_y(TH2D* his,double ymin, double ymax){
//     int his_binxw = (his->GetXaxis()->GetXmax()-his->GetXaxis()->GetXmin())/his->GetNbinsX();
//     int his_binyw = (his->GetYaxis()->GetXmax()-his->GetYaxis()->GetXmin())/his->GetNbinsY();
//     int yn = (int)(ymax-ymin)/his_binyw;
//     TH2D* his_shr = new TH2D();
//     his_shr->SetBins(his->GetNbinsX(), his->GetXaxis()->GetXmin(), his->GetXaxis()->GetXmax(), yn, ymin, ymax);
//     for(int i_s = 0; i_s < his->GetNbinsX(); i_s++){
//         for(int jj = 0; jj < his->GetNbinsY(); jj++){
//             his_shr->Se(his->GetBinContent(i_s+1,jj+1));
//         }
//     }
//     return his_shr;

// }
