#include <iostream>
#include <fstream>
#include <algorithm> 

using namespace std;


class MySignal{
    private:
    Float_t amp; //amplituda sygnału
    Int_t t0; //czas rozpoczęcia sygnału
    Float_t charge; //ładunek
    Int_t tot; //time over threshold

    public:

    MySignal() {} //konst. domyślny
    
    //konst. z parametrami
    MySignal(Float_t my_amp, Int_t my_t0, Float_t c, Int_t my_tot)
    : amp(my_amp), t0(my_t0), charge(c), tot(my_tot) {}
    
};


Bool_t Sig(string n_file = "wave_0.dat"){
    fstream scyn;

    scyn.open(n_file, ios::binary | ios::in); //otwieranie pliku 
    
    if (scyn.is_open()){ 
        cout << "plik poprawnie załadowany" << endl;
    }
    else{
        cout << "błąd w ładowaniu pliku" << endl;
    }
    
    float* x;
    char * temp = new char[sizeof(float)]; //zmienne do konwersji pliku binarnego

    const Int_t sig_time = 1024; //lczas jednego sygnalu
    vector<Float_t> sig_vec; //wektor przechowujący wartości jednego sygnału

    TFile* f_root = new TFile("signal.root", "RECREATE"); //tworzenie pliku root i drzewa
    TTree* tree = new TTree("sig_tree", "signal data");

    MySignal my_sig; // tworzenie obiektu klasy MySignal
    tree->Branch("sig_branch", &my_sig);

    const Int_t n_sig = 10000; //liczba analizowanych sygnalow

    for (Int_t i = 0; i < n_sig; i++) {
        for (Int_t j = 0; j < sig_time; j++) {
            scyn.read(temp, sizeof(float));
            x = (float*)temp;
            *x *= 1/4.096; // konwersja na mV
            sig_vec.push_back(*x);
        }

        Float_t sum = accumulate(sig_vec.begin(), sig_vec.begin() + 50, 0.0);
        Float_t avrg = sum/50; //srednia z pierwszych 50 punktów jako linia bazowa

        for (Int_t j = 0; j < sig_time; j++) {
            sig_vec[j] = abs(sig_vec[j] - avrg);
        }

        //amp. progowa do wyznaczania t0 i tot wyznaczona 
        //na podstawie kilku pierwszych sygnalow
        const Int_t A_threshold = 20;



        //find_if znajduje 1. wartość wektora
        //spełniajacą dany warunek
        // i zwraca do niej wskaźnik
        auto t0_p = find_if(sig_vec.begin(), sig_vec.end(), [A_threshold](int num) {
        return num > A_threshold; }); 

        //wyłuskanie indeksu, czyli tak naprawdę
        //wartości czasowej
        Int_t t0 = distance(sig_vec.begin(), t0_p); 

        auto tmax_p = find_if(sig_vec.begin() + t0, sig_vec.end(), [A_threshold](int num) {
        return num < A_threshold; });

        Int_t tmax = distance(sig_vec.begin(), tmax_p);



        //amplituda jako maksymalna wartość z wektora sig_vec
        Float_t A = *max_element(sig_vec.begin(), sig_vec.end()); 

        Int_t tot = tmax - t0;

        Float_t charge = accumulate(sig_vec.begin() + t0, sig_vec.begin() + tmax, 0.0);
        
        
        my_sig = MySignal(A, t0, charge, tot);
        tree->Fill();

        sig_vec.clear();
    } 

    scyn.close();
    f_root->Write();
    f_root->Close();

    return kTRUE;
}

Bool_t Hists(string file_name = "signal.root"){
    
    TFile* new_file = new TFile(file_name.c_str(), "READ");
    TTree* tree = (TTree*)new_file->Get("sig_tree");

    TH1F* hAmp = new TH1F("hAmp", "amplituda ", 100, 0, 1000);
    TH1F* ht0 = new TH1F("ht0", "czas t0", 100, 0, 500);
    TH1F* hChar = new TH1F("hChar", "ladunek", 100, 0, 100000);
    TH1F* htot = new TH1F("htot", "time over threshold", 100, 0, 500);

    TH2F* hAmpChar = new TH2F("hAmpChar", "amplituda vs ladunek", 100, 0, 1000,100, 0, 100000);
    TH2F* hCharT0 = new TH2F("hCharT0", "t0 vs ladunek", 100, 0, 100000,100, 0, 500);
    
    //tworzenie płótna i rysowanie histogramów
    TCanvas *can = new TCanvas("can", "can", 1300, 800);

    can->Divide(3,2);

    can->cd(1); 
    gStyle->SetOptStat(0);
    tree->Draw("sig_branch.amp>>hAmp","","");
    hAmp->Draw();
    hAmp->GetYaxis()->SetTitle("liczba zliczen");
    hAmp->GetXaxis()->SetTitle("amplituda [mV]");
    
    can->cd(2); 
    gStyle->SetOptStat(0);
    tree->Draw("sig_branch.t0>>ht0","","");
    ht0->Draw();
    ht0->GetYaxis()->SetTitle("liczba zliczen");
    ht0->GetXaxis()->SetTitle("t0 [ns]");

    can->cd(3); 
    gStyle->SetOptStat(0);
    tree->Draw("sig_branch.charge>>hChar","","");
    hChar->Draw();
    hChar->GetYaxis()->SetTitle("liczba zliczen");
    hChar->GetXaxis()->SetTitle("ladunek [pC]");

    can->cd(4); 
    gStyle->SetOptStat(0);
    tree->Draw("sig_branch.tot>>htot","","");
    htot->Draw();
    htot->GetYaxis()->SetTitle("liczba zliczen");
    htot->GetXaxis()->SetTitle("TOT [ns]");
    

    //can->Divide(2,1);
    
    can->cd(5);
    gStyle->SetOptStat(0);
    tree->Draw("sig_branch.charge:sig_branch.amp>>hAmpChar");
    hAmpChar->Draw("colz");
    hAmpChar->GetYaxis()->SetTitle("ladunek [pC]");
    hAmpChar->GetXaxis()->SetTitle("amplituda [mV]");

    can->cd(6);
    gStyle->SetOptStat(0);
    tree->Draw("sig_branch.t0:sig_branch.charge>>hCharT0","","");
    hCharT0->Draw("colz");
    hCharT0->GetXaxis()->SetTitle("ladunek [pC]");
    hCharT0->GetYaxis()->SetTitle("t0 [ns]");

    return kTRUE;
}

Bool_t zad2(){
    Sig();
    Hists();

    return kTRUE;
}