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
template<typename T>
void combine_TGraphs(TCanvas* c, TMultiGraph* mg, double* range_left, double* range_right, T g1, TGraph* g2, string s1, string s2);
template<typename T>
void combine_his_graph(TCanvas* c, double* range_right, T g1, TGraph* g2, string s1, string s2);
//TH2D* shrink_TH2D_y(TH2D* his,double ymin, double ymax);



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
    string path = "./hist";

    const char *cstr = path.c_str();
    struct dirent *ent;
    set<int> removed_events = {69611, 70195, 70070};
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
    TH2I *ECAL_hist                     = nullptr;
    TH2D *SpillTime_SpillNum_2D         = nullptr;
    TH1D* angle_proton                  = nullptr;

    files[0]->GetObject("spillNum_VerticeNum", spillNum_VerticeNum[0]);
    files[0]->GetObject("ECAL_hist", ECAL_hist);
    files[0]->GetObject("SpillTime_SpillNum_2D", SpillTime_SpillNum_2D);
    files[0]->GetObject("Total_inv_mass", Total_inv_mass);
    int VerticeNum = spillNum_VerticeNum[0]->GetNbinsY();
    int spillNum = spillNum_VerticeNum[0]->GetNbinsX();
    int Num_ECAL1 = ECAL_hist->GetNbinsX();
    int Num_ECAL2 = ECAL_hist->GetNbinsY();
    int spillTime = SpillTime_SpillNum_2D->GetNbinsY();
    int inv_mass = Total_inv_mass->GetNbinsX();

    // histograms instantiation
    TH2D *histo_integrate_spillnumber = new TH2D("histo_integrate_spillnumber", "event countings in certain Spillnum of certain runnumber",
                                                 RunTime_range, RunTime_first - 0.5, RunTime_last - 0.5, spillNum, -0.5, spillNum-0.5);
    histo_integrate_spillnumber->SetXTitle("run number");
    histo_integrate_spillnumber->SetYTitle("spill number");

    TH1D *EventNum_RunNum = new TH1D("EventNum_RunNum", "Event countings of the RunNumber", RunTime_range,  RunTime_first - 0.5, RunTime_last - 0.5);
    EventNum_RunNum->SetXTitle("run rumber");
    EventNum_RunNum->SetYTitle("event counting");

    TH2D *VerticeNum_RunNum_integrated = new TH2D("VerticeNum_RunNum_integrated",
                                                  "VerticeNumber through different run time", RunTime_range, RunTime_first-0.5, RunTime_last-0.5, VerticeNum, -0.5, VerticeNum-0.5);
    VerticeNum_RunNum_integrated->SetXTitle("run rumber");
    VerticeNum_RunNum_integrated->SetYTitle("VerticeNum");

    TH2D *PrimVerticeNum_RunNum_integrated = new TH2D("PrimVerticeNum_RunNum_integrated",
                                                      "PrimVerticeNumber through different run time", RunTime_range, RunTime_first-0.5, RunTime_last-0.5, VerticeNum, -0.5, VerticeNum-0.5);
    PrimVerticeNum_RunNum_integrated->SetXTitle("run number");
    PrimVerticeNum_RunNum_integrated->SetYTitle("PrimVerticeNum");

    TH2D *NonPrimVerticeNum_RunNum_integrated = new TH2D("NonPrimVerticeNum_RunNum_integrated",
                                                         "NonPrimVerticeNumber through different run time", RunTime_range, RunTime_first-0.5, RunTime_last-0.5, VerticeNum, -0.5, VerticeNum-0.5);
    NonPrimVerticeNum_RunNum_integrated->SetXTitle("run number");
    NonPrimVerticeNum_RunNum_integrated->SetYTitle("NonPrimVerticeNum");
    
    TH2D *Three_pion_mass_His = new TH2D("Three_pion_mass_His", "invariant mass of three charged pions", 
                                            RunTime_range, RunTime_first-0.5, RunTime_last-0.5, 200, 0, 8.0);
    Three_pion_mass_His->SetXTitle("run number");
    Three_pion_mass_His->SetYTitle("invariant mass (GeV)");
    Three_pion_mass_His->SetStats(false);
    Three_pion_mass_His->SetOption("COLZ");

    TH2D *Total_invariant_mass_His = new TH2D("Total_invariant_mass_His", "invariant mass of pions and photons (photon energy cutoff 1&4 GeV)", 
                                            RunTime_range, RunTime_first-0.5, RunTime_last-0.5, 200, 0, 8.0);
    Total_invariant_mass_His->SetXTitle("run number");
    Total_invariant_mass_His->SetYTitle("invariant mass (GeV)");
    Total_invariant_mass_His->SetStats(false);
    Total_invariant_mass_His->SetOption("COLZ");

    // TGraph instantiation
    //three pion invariant mass:
    TGraphAsymmErrors *Three_pion_mass = new TGraphAsymmErrors(filesize - Num_removed);
    Three_pion_mass->SetMarkerColor(4);
    Three_pion_mass->SetMarkerStyle(21);
    Three_pion_mass->SetTitle("invariant mass of 3 pions");
    Three_pion_mass->SetName("Three_pion_mass");
    Three_pion_mass->GetXaxis()->SetTitle("run number");
    Three_pion_mass->GetYaxis()->SetTitle("invariant mass (GeV)");

    //total invariant mass:
    TGraphAsymmErrors *Total_invariant_mass = new TGraphAsymmErrors(filesize - Num_removed);
    Total_invariant_mass->SetMarkerColor(4);
    Total_invariant_mass->SetMarkerStyle(21);
    Total_invariant_mass->SetTitle("invariant mass of pions and photons (photon energy cutoff 1&4 GeV)");
    Total_invariant_mass->SetName("Total_invariant_mass");
    Total_invariant_mass->GetXaxis()->SetTitle("run number");
    Total_invariant_mass->GetYaxis()->SetTitle("invariant mass (GeV)");

    //ratio of Col2 to Col1:
    TGraph *ECAL_per = new TGraph(filesize - Num_removed);
    ECAL_per->SetMarkerColor(2);
    ECAL_per->SetMarkerStyle(22);
    ECAL_per->SetTitle("Percentage of photons measured by ECAL1");
    ECAL_per->SetName("ECAL_per");
    ECAL_per->GetXaxis()->SetTitle("run number");
    ECAL_per->GetYaxis()->SetTitle("Percentage (%)");

    TGraph *ECAL1 = new TGraph(filesize - Num_removed);
    ECAL1->SetMarkerColor(2);
    ECAL1->SetMarkerStyle(22);
    ECAL1->SetTitle("photon number per event from ECAL1");
    ECAL1->SetName("ECAL1");
    ECAL1->GetXaxis()->SetTitle("run number");
    ECAL1->GetYaxis()->SetTitle("Photon number");

    TGraph *ECAL2 = new TGraph(filesize - Num_removed);
    ECAL2->SetMarkerColor(2);
    ECAL2->SetMarkerStyle(22);
    ECAL2->SetTitle("photon number per event from ECAL2");
    ECAL2->SetName("ECAL2");
    ECAL2->GetXaxis()->SetTitle("run number");
    ECAL2->GetYaxis()->SetTitle("Photon number");

    TGraph *half_w_invMass = new TGraph(filesize - Num_removed);
    half_w_invMass->SetMarkerColor(4);
    half_w_invMass->SetMarkerStyle(21);
    half_w_invMass->SetTitle("half width of invariant mass distribution");
    half_w_invMass->SetName("half_w_invMass");
    half_w_invMass->GetXaxis()->SetTitle("run number");
    half_w_invMass->GetYaxis()->SetTitle("width (GeV)");

    TGraph *recProton_width = new TGraph(filesize - Num_removed);
    recProton_width->SetMarkerColor(2);
    recProton_width->SetMarkerStyle(22);
    recProton_width->SetTitle("width of recoil proton angular distribution at 23% of Max hight");
    recProton_width->SetName("recProton_width");
    recProton_width->GetXaxis()->SetTitle("run number");
    recProton_width->GetYaxis()->SetTitle("width (GeV)");

    

    int ii = 0;
    int bin_left = 0;
    int bin_right = 0;
    double pos_left = 0.0;
    double pos_right = 0.0;
    double center_mass = 0.0;
    gErrorIgnoreLevel = kWarning;
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
        bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum()*3/10);
        bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum()*3 / 10);
        pos_left = ThreePion_inv_mass->GetBinCenter(bin_left);
        pos_right = ThreePion_inv_mass->GetBinCenter(bin_right);
        TF1* f = new TF1("fit function", "[0]*(TMath::Gaus(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5]))", pos_left, pos_right);
        f->SetParameter(0, ThreePion_inv_mass->GetMaximum());
        f->SetParameter(1, 1);
        f->SetParameter(2, 1);
        f->SetParameter(3, 0.01);
        f->FixParameter(4, 1.49);
        f->SetParameter(5, 1);
        f->SetParLimits(5, 0.01, 0.1);
        ThreePion_inv_mass->Fit("fit function","RQ");
        
        // bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum()*9.5/10);
        // bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum()*9.5/10);
        //         //cout << "ii: " << ii << "filenumber: " << filenames[i].num << "value: " << yq[1] << endl;
        // center_mass = (ThreePion_inv_mass->GetBinCenter(bin_right) + ThreePion_inv_mass->GetBinCenter(bin_left))/2;
        Three_pion_mass->SetPoint(ii, filenames[i].num, f->GetParameter(1));
        bin_left = ThreePion_inv_mass->FindFirstBinAbove(ThreePion_inv_mass->GetMaximum()*5/10);
        bin_right = ThreePion_inv_mass->FindLastBinAbove(ThreePion_inv_mass->GetMaximum()*5/10);
        Three_pion_mass->SetPointEYhigh(ii, ThreePion_inv_mass->GetBinCenter(bin_right)-f->GetParameter(1));
        Three_pion_mass->SetPointEYlow(ii, f->GetParameter(1)-ThreePion_inv_mass->GetBinCenter(bin_left));
        if (filenames[i].num == 70654)
        {
            cout << f->GetParameter(1) << endl;
            cout << ThreePion_inv_mass->GetBinCenter(bin_right)-f->GetParameter(1) << endl;
            cout << f->GetParameter(1)-ThreePion_inv_mass->GetBinCenter(bin_left)<< endl;
        }
        //2DHistogram:
        for (int i_mass = 0; i_mass < inv_mass; i_mass++)
        {
            Three_pion_mass_His->SetBinContent(xbin, i_mass + 1, (int)(1000.0 * ThreePion_inv_mass->GetBinContent(i_mass + 1) / ThreePion_inv_mass->GetMaximum()));
        }

        //total invariant mass:
        files[i]->GetObject("Total_inv_mass", Total_inv_mass);
                //Total_inv_mass->GetQuantiles(3, yq, xq);
        bin_left = Total_inv_mass->FindFirstBinAbove(Total_inv_mass->GetMaximum()*9.5/10);
        bin_right = Total_inv_mass->FindLastBinAbove(Total_inv_mass->GetMaximum()*9.5/10);
                //cout << "ii: " << ii << "filenumber: " << filenames[i].num << "value: " << yq[1] << endl;
        center_mass = (Total_inv_mass->GetBinCenter(bin_right) + Total_inv_mass->GetBinCenter(bin_left))/2;
        Total_invariant_mass->SetPoint(ii, filenames[i].num, center_mass);
        bin_left = Total_inv_mass->FindFirstBinAbove(Total_inv_mass->GetMaximum()*5/10);
        bin_right = Total_inv_mass->FindLastBinAbove(Total_inv_mass->GetMaximum()*5/10);
        Total_invariant_mass->SetPointEYhigh(ii, Total_inv_mass->GetBinCenter(bin_right)-center_mass);
        Total_invariant_mass->SetPointEYlow(ii, center_mass-Total_inv_mass->GetBinCenter(bin_left));
        bin_left = Total_inv_mass->FindFirstBinAbove(Total_inv_mass->GetMaximum()/2);
        bin_right = Total_inv_mass->FindLastBinAbove(Total_inv_mass->GetMaximum()/2);
        half_w_invMass->SetPoint(ii, filenames[i].num, Total_inv_mass->GetBinCenter(bin_right) - Total_inv_mass->GetBinCenter(bin_left));
        //2DHistogram:
        for (int i_mass = 0; i_mass < inv_mass; i_mass++)
        {
            Total_invariant_mass_His->SetBinContent(xbin, i_mass + 1, (int)(1000.0 * Total_inv_mass->GetBinContent(i_mass + 1) / Total_inv_mass->GetMaximum()));
        }

        //ECAL1 and ECAL2 ratio
        int totNum_ECAL1 = 0;
        int totNum_ECAL2 = 0;
        files[i]->GetObject("ECAL_hist", ECAL_hist);
        for (int i_ECAL1 = 0; i_ECAL1 < Num_ECAL1; i_ECAL1++)
        {
            for (int i_ECAL2 = 0; i_ECAL2 < Num_ECAL2; i_ECAL2++)
            {
                totNum_ECAL1 += ECAL_hist->GetBinContent(i_ECAL1 + 1, i_ECAL2 + 1) * i_ECAL1;
                totNum_ECAL2 += ECAL_hist->GetBinContent(i_ECAL1 + 1, i_ECAL2 + 1) * i_ECAL2;
            }
        }
        double ECAL_percent = (double)totNum_ECAL1 / (totNum_ECAL2 + totNum_ECAL1);
        ECAL_per->SetPoint(ii, filenames[i].num, 100 * ECAL_percent);
        ECAL1->SetPoint(ii, filenames[i].num, (double)totNum_ECAL1 / EventNum_sum_RunNum);
        ECAL2->SetPoint(ii, filenames[i].num, (double)totNum_ECAL2 / EventNum_sum_RunNum);

        // recoiled proton:
        files[i]->GetObject("angle_proton", angle_proton);
        bin_left = angle_proton->FindFirstBinAbove(angle_proton->GetMaximum()*750/3200);
        bin_right = angle_proton->FindLastBinAbove(angle_proton->GetMaximum()*750/3200);
        recProton_width->SetPoint(ii, filenames[i].num, angle_proton->GetBinCenter(bin_right) - angle_proton->GetBinCenter(bin_left));

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
    Vertice3_over_Vertice1->GetXaxis()->SetTitle("run number");
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
    
    TFile *file_integrated = new TFile("hist_integrated.root", "recreate");
    histo_integrate_spillnumber             ->Write();
    VerticeNum_RunNum_integrated            ->Write();
    PrimVerticeNum_RunNum_integrated        ->Write();
    NonPrimVerticeNum_RunNum_integrated     ->Write();
    Vertice3_over_Vertice1                  ->Write();
    Three_pion_mass                         ->Write();
    Three_pion_mass_His                     ->Write();
    Total_invariant_mass                    ->Write();
    Total_invariant_mass_His                ->Write();
    half_w_invMass                          ->Write();
    ECAL_per                                ->Write();
    ECAL1                                   ->Write();
    ECAL2                                   ->Write();
    EventNum_RunNum                         ->Write();
    recProton_width                         ->Write();
    
    file_integrated->Close();
/*
    double range_right[2];
    double range_left[2];

       
    //creating canvas5: FWHM and ECAL1 percentage
    TCanvas* c5 = new TCanvas("FWHM_ECAL1_per","multigraph5",1500,800);
    TMultiGraph* mg5 = new TMultiGraph("mg5","comparison between FWHM and ECAL1 percentage");
    range_right[0]=10;
    range_right[1]=60;
    range_left[0]=1;
    range_left[1]=5;
    combine_TGraphs<TGraph*>(c5, mg5, range_left, range_right, half_w_invMass, ECAL_per,"FWHM","ECAL1 percentage");
    c5->Write();
    mg5->Write();


    //creating canvas1: invariance mass and ECAL1 percentage
    
    TCanvas* c1 = new TCanvas("Inv_mass_ECAL_per","multigraph1",1500,800);
    TMultiGraph* mg1 = new TMultiGraph("mg","invariance mass and ECAL1 percentage");
    range_right[0]=10;
    range_right[1]=60;
    range_left[0]=0.5;
    range_left[1]=4.5;
    combine_TGraphs<TGraphAsymmErrors*>(c1, mg1, range_left, range_right, Total_invariant_mass, ECAL_per,"Invariant mass with quantiles (68%)","Percentage (ECAL1)");
    c1->Write();
    mg1->Write();
    
    //creating canvas2: invariance mass and ECAL1
    TCanvas* c2 = new TCanvas("Inv_mass_ECAL1","multigraph2",1500,800);
    TMultiGraph* mg2 = new TMultiGraph("mg2","invariance mass and ECAL1");
    range_right[0]=0;
    range_right[1]=1.5;
    range_left[0]=0.5;
    range_left[1]=4.5;
    combine_TGraphs<TGraphAsymmErrors*>(c2, mg2, range_left, range_right, Total_invariant_mass, ECAL1,"Invariant mass with quantiles (68%)","Photon number (ECAL1)");
    c2->Write();
    mg2->Write();

    //creating canvas3: invariance mass and ECAL2
    TCanvas* c3 = new TCanvas("Inv_mass_ECAL2","multigraph3",1500,800);
    TMultiGraph* mg3 = new TMultiGraph("mg3","invariance mass and ECAL2");
    range_right[0]=0;
    range_right[1]=1.3;
    range_left[0]=0.5;
    range_left[1]=4.5;
    combine_TGraphs<TGraphAsymmErrors*>(c3, mg3, range_left, range_right, Total_invariant_mass, ECAL2,"Invariant mass with quantiles (68%)","Photon number (ECAL2)");
    c3->Write();
    mg3->Write();

    //creating canvas4: invariance mass and ECAL2
    TCanvas* c4 = new TCanvas("Inv_mass_recoiled","multigraph4",1500,800);
    TMultiGraph* mg4 = new TMultiGraph("mg4","comparison of two widths");
    range_right[0]=-2.5;
    range_right[1]=1.8;
    range_left[0]=1;
    range_left[1]=5;
    combine_TGraphs<TGraph*>(c4, mg4, range_left, range_right, half_w_invMass, recProton_width,"FWHM","width of recoilded proton at 23%");
    c4->Write();
    mg4->Write();

    
 */

    
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

template<typename T>
void combine_TGraphs(TCanvas* c, TMultiGraph* mg, double* range_left, double* range_right, T g1, TGraph* g2, string s1, string s2){
    //cout << "left0: " << range_left[0] << " left1: " << range_left[1] << endl;
    g1->GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
    g2->GetYaxis()->SetRangeUser(range_right[0], range_right[1]);
    double ratio_trans = (double) (range_left[1] - range_left[0]) / (range_right[1] - range_right[0]);
    TGraph *new_g2 = Shrink_graph_y(g2, -range_right[0], ratio_trans);
    new_g2 = move_graph_y(new_g2, range_left[0]);
    new_g2->SetMarkerColor(2);
    new_g2->SetMarkerStyle(22);
    new_g2->SetTitle(g2->GetTitle());
    c->cd();
    c->SetGrid();
    mg->Add(g1);
    mg->Add(new_g2);
    mg->GetYaxis()->SetRangeUser(range_left[0],range_left[1]);
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