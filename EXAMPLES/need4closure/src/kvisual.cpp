/// @file
/// @brief
///  @EN{ Visualisation of "kWorld" (old example for SymShell implementing a Kruglanski's like model). }
///  @PL{ Wizualizacja "kWorld" (starego przykładu dla SymShell-a implementujący model podobny do modelu Kruglańskiego). }
/// @date 2026-05-29 (modified)
/// =================================================================================================
/// @details ...
//======================================================================================================================

#include "krand.h"
#include "kWorld.h"
#include "dhistosou.hpp" //Nowsza wersja — poprawiona
#include "clstsour.hpp"  //Jest też alternatywne statsour
#include "spatcors.hpp"
#include "coincsou.hpp"
#include "funcsour.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"
#include "toitoutoll.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2;
using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

const int RAMKA=4;
extern const char* SIMULATION_NAME;

// KONSTRUKCJA	WIZUALIZACJI
//===================================
extern unsigned internal_log;
extern unsigned spatial_correlation_mode;


// Generuje podstawowe źródła dla wbudowanego zarządcy danych lub innego
//===========================================================================
void kWorld::make_basic_sources()
{
    sources_manager& WhatSourMen=this->Sources;

    world::make_basic_sources(); //Odziedziczone
    
    //Główne serie
    Firsts=Agenci.make_source("Attitude",&kAgent::First);
    if(Firsts)
        Firsts->set_min_max(-1, 1);
    Seconds=Agenci.make_source("Prev. attitude",&kAgent::Second);
    if(Seconds)
        Seconds->set_min_max(-1, 1);
    
    ForLeft=Agenci.make_source("For left",&kAgent::ForLeft);
    ForRight=Agenci.make_source("For right",&kAgent::ForRight);
    Powers=Agenci.make_source("Power",&kAgent::Power);
    
    /*
    Pressure=Agents.make_source("Pressure",&kAgent::Press);
    MaxPressure=long(MaxSila)*WeightOfSelf;
    MaxPressure+=long(MaxSila)*NeedForClosure * (NeighDens>0 ? NeighDens : sqr(NeighRadius*2+1)-1   ); //Prymitywna implementacja
    Pressure->set_missing(-1);
    Pressure->set_min_max(0, MaxPressure  );
    */
    
    ptrLastChanged=new ptr_to_scalar_source<int>(nullptr,"Change cnt.");
    ptrLastMigration=new ptr_to_scalar_source<int>(nullptr,"Migration cnt.");
    
    //DZIAŁA PRAWIDŁOWO. Na `CountCh` i `CountMig` pojawia się '-1'.
    ptrLastChanged->set_missing(-1.);
    ptrLastMigration->set_missing(-1.);
    CountCh=toi(ptrLastChanged->get_missing());
    CountMig=toi(ptrLastMigration->get_missing());
    
    //Classif=Agents.make_source("Classification",&kAgent::classify); //Z PIERWOWZORU "LANGUAGES". ZBĘDNE.
    //if(classify)
    //	classify->set_min_max(0,NofCat*NofCat*NofCat-1); //Max class ==NofCat^3 bo trzy niezależne płaszczyzny
    
    //Umieszczenie głównych serii w managerze serii
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
    WhatSourMen.insert(ForLeft);
    WhatSourMen.insert(ForRight);
    //WhatSourMen.insert(Pressure);
    WhatSourMen.insert(ptrLastChanged);
    WhatSourMen.insert(ptrLastMigration);
    
    //WhatSourMen.insert(classify);
}


// Współpraca z managerem wyświetlania, a także logiem
//------------------------------------------------------------------
// Wypisywanie/dopisywanie na konsole statusu
void    kWorld::actualize_out_area()
{
    world::actualize_out_area();
    //ptrStres; ptrClsSize; - Do przekazywania aktualnie najważniejszych danych na okno statusu
    if(OutArea)
    {
        wb_pchar bufor(1024); //ze sporym zapasem
        //assert(ptrStres->);
        double Stres=ptrStres->get();      //Zakładamy, że to źródła jednowartościowe
        double ClsSiz=ptrClsSize->get();
        bufor.prn("Stress: %g \nApproximated cluster size: %g",Stres,ClsSiz);
        OutArea->add_text(bufor.get_ptr_val());
    }
}


void kWorld::make_default_visualisation()
//Rejestruje pochodne serie, tworzy domyślne "lufciki" i wkłada w "Manager"
{
    area_manager_base& Manager=this->MyAreaManager();
    int iFirst=0,iSecond=0,iPower=0,iPressure=0,iChangeCnt,iMigrationCnt;

    //Uzyskanie indeksów podstawowych serii z managera
    {
        if(Firsts) iFirst=Sources.search(Firsts->name());
        else  goto ERROR;
        
        if(Seconds) iSecond=Sources.search(Seconds->name());
        else  goto ERROR;
        
        if(Powers)   iPower=Sources.search(Powers->name());
        else  goto ERROR;
        
        //if(Pressure)  iPressure=Sources.search(Pressure->name());
        
        
        //Oraz utworzenie pochodnych serii statystycznych
        fifo_source<int>* LastChangedLog=new fifo_source<int>(ptrLastChanged,internal_log);
        //if(!LastChangedLog) goto ERROR;
        iChangeCnt=Sources.insert(LastChangedLog);
        
        fifo_source<int>* LastMigrationLog=new fifo_source<int>(ptrLastMigration,internal_log);
        //if(!LastMigrationLog) goto ERROR;
        iMigrationCnt=Sources.insert(LastMigrationLog);
        
        generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
        //if(!FirstStat) goto ERROR;
        Sources.insert(FirstStat);
        
        generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
        //if(!SecondStat) goto ERROR;
        Sources.insert(SecondStat);
        
        //generic_basic_statistics_source* PressureStat=new generic_basic_statistics_source(Pressure);
        //if(!PressureStat) goto ERROR;
        //Sources.insert(PressureStat);
        
        
        //Źródło liczące statystykę i histogram z klasyfikacji
        //= new  generic_histogram_source(Firsts);
        generic_discrete_histogram_source*  ClassStat=new generic_discrete_histogram_source(-1,3,Firsts);
        //if(!ClassStat) goto ERROR;
        Sources.insert(ClassStat);  //cerr<<ClassStat->name();
        
        generic_spatial_correlation_source* SpatialCorr=new generic_spatial_correlation_source(Firsts,-1,toi(spatial_correlation_mode));
        //if(!SpatialCorr) goto ERROR;
        //int iSpatialCorr=
                Sources.insert(SpatialCorr);
        
        //A także utworzenie seri liczących ich wzajemne ko-statystyki
        coincidence_source* CorrFS=new coincidence_source(Firsts, Seconds);
        //if(!CorrFS) goto ERROR;
        Sources.insert(CorrFS); //Żeby została kiedyś zwolniona, a poza tym może ktoś kiedyś...
        
        fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(),internal_log);
        //if(!EntropyFSLog) goto ERROR;
        int iEntropyFS=Sources.insert(EntropyFSLog);
        
        fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(),internal_log); //FIFO z korelacji pierwszych z drugimi
        //if(!CorrFSLogR) goto ERROR;
        //int iCorrFSR=
                Sources.insert(CorrFSLogR);
        
        
        //I utworzenie seri liczących ich statystyki
        /*
        fifo_source<double>* MeanPressLog=new fifo_source<double>(PressureStat->Mean(),internal_log); //FIFO ze średniego chwilowego stresu
        if(!MeanPressLog) goto ERROR;
        int iMeanPress=Sources.insert(MeanPressLog);
        
          fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(),internal_log); //FIFO ze stresu klasycznego
          if(!StressFirstLog) goto ERROR;
          int iSFirst=Sources.insert(StressFirstLog);
          
            fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(),internal_log); //FIFO ze starego stresu
            if(!StressSecondLog) goto ERROR;
            int iSSecond=Sources.insert(StressSecondLog);
        */
        
        fifo_source<double>* ClusterSizeLog=new fifo_source<double>(SpatialCorr->ApproximatedClusterSize(),internal_log); //FIFO z rozmiaru klastra
        //if(!ClusterSizeLog) goto ERROR;
        int iClusterSize=Sources.insert(ClusterSizeLog);
        
        //iMainClassF,iWhichMainF,iNumClassF,			
        fifo_source<double>* NumClassLog=new fifo_source<double>(ClassStat->NumOfClass(),internal_log);
        //if(!NumClassLog) goto ERROR;
        int iNumClassF=Sources.insert(NumClassLog);
        
        fifo_source<double>* ClassEntropyLog=new fifo_source<double>(ClassStat->Entropy(),internal_log);
        //if(!ClassEntropyLog) goto ERROR;
        int iClassEntropy=Sources.insert(ClassEntropyLog);
        
        fifo_source<double>* MainClassLog=new fifo_source<double>(ClassStat->MainClass(),internal_log);
        //if(!MainClassLog) goto ERROR;
        int iMainClassF=Sources.insert(MainClassLog);
        
        fifo_source<double>* WhichMainLog=new fifo_source<double>(ClassStat->WhichMain(),internal_log);
        //if(!WhichMainLog) goto ERROR;
        int iWhichMainF=Sources.insert(WhichMainLog);
        
        ptrStres=FirstStat->Stress();
        ptrClsSize=SpatialCorr->ApproximatedClusterSize();
        
        //I umieszczanie w logu tego co trzeba
        Log.insert(ClassStat->NumOfClass());
        Log.insert(ClassStat->Entropy());
        Log.insert(ClassStat->NormEntropy());
        Log.insert(ClassStat->MainClass());
        Log.insert(ClassStat->WhichMain());
        //Log.insert(FirstStat->Stress());
        //Log.insert(PressureStat->Mean());
        Log.insert(ptrLastChanged);
        Log.insert(ptrLastMigration);
        Log.insert(CorrFS->Entropy());
        Log.insert(CorrFS->NormEntropy());
        Log.insert(CorrFS->Chi2());
        Log.insert(CorrFS->LevelOfFreedom());
        Log.insert(CorrFS->V2Cramer());
        Log.insert(CorrFS->T2Czupurow());
        Log.insert(CorrFS->Tau_b_Goodman_Kruskal());
        Log.insert(CorrFS->Tau_a_Goodman_Kruskal());
        Log.insert(SpatialCorr->ApproximatedClusterSize());
        for(int k=0;k<3;k++) //Źródła histogramu — muszą być na końcu, bo zmienia na liczbę
        {
            Log.insert(ClassStat->Categories(k));
        }
        
        //PODSTAWOWA WIZUALIZACJA SERII DANYCH
        //WYMIARY DOMYŚLNEGO OKNA
        int lW= Manager.get_width();
        int lH= Manager.get_height();                     assert(lW > 50 && lH > 40); //Najmniejsze sensowne okno
        int chHX= toi(char_height('X'));

        //Obszary domyślne z klasy bazowej — np. obszar STATUSU
        world::make_default_visualisation();
        if(OutArea) 
        {
            OutArea->set(1, 1, lW / 2. - 1, lH / 2. - 1);
            Manager.as_original(Manager.search(OutArea->name()));
        }
        
        //WŁAŚCIWE LUFCIKI
        graph* pom1=new sequence_graph(lW / 2 - 1, lH / 4, lW - 50, lH / 2 - 1,
                                       3, Sources.make_series_info(
                                                                    iNumClassF,iMainClassF,iWhichMainF,
                                                                    -1
                                                                    ).get_ptr_val(),
                                       0//* Z reskalowaniem
                                        );
        //if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF CLASSIFICATION");
        Manager.insert(pom1);
        
        //inne mniej potrzebne
        graph* pom2=new sequence_graph(lW / 2 - 1, 1, lW - 50, lH / 4 - 1,	//domyślne współrzędne
                                        3, Sources.make_series_info(
                                                                        //iSSecond,
                                                                        iClusterSize,
                                                                        iChangeCnt,
                                                                        iMigrationCnt,
                                                                        //		iMeanPress,
                                                                        -1
                                                                        ).get_ptr_val(),
                                       0// Z reskalowaniem
                                        //1//Wspólne minimum/maximum
                                        );
        //if(!pom2) goto ERROR;
        pom2->set_frame(128);
        pom2->set_title("HISTORY OF CLUSTERIZATION");
        Manager.insert(pom2);

        pom2=new carpet_graph(1, lH / 2, lW / 3, lH - 1, //domyślne współrzędne
            Firsts); //I źródło danych
        pom2->set_data_colors(0, 255);
        pom2->set_title("Map of current attitude");
        Manager.insert(pom2);


        pom2=new carpet_graph(lW / 3 + 1, lH / 2, lW / 3 * 2, lH - 1, //domyślne współrzędne
                                ForLeft);
        pom2->set_data_colors(0, 255);
        pom2->set_title("Map of left counters");
        Manager.insert(pom2);

        pom2=new carpet_graph(lW / 3 * 2 + 1, lH / 2, lW, lH - 1, //domyślne współrzędne
                                 ForRight);
        pom2->set_data_colors(0, 255);
        pom2->set_title("Map of right counters");
        Manager.insert(pom2);
        
        /*
        pom2 = new carpet_graph(lW/3*2+1,lH/2,lW,lH-1, //domyślne współrzędne, //domyślne współrzędne  lW-49,7*char_height('X')+7,lW,8*char_height('X')+9
        Pressure);
        pom2->set_data_colors(0,255);
        pom2->set_title("Map of instantaneous social pressure");
        Manager.insert(pom2);
        */
        
        //PRZYCISKI
        pom2=new carpet_graph(lW - 49, 5 * (chHX + RAMKA), lW, 6 * (chHX + RAMKA), //domyślne współrzędne
                                Seconds); //I źródło danych
        pom2->set_data_colors(0, 255);
        pom2->set_frame(32);
        pom2->set_title("Map of previous attitude");
        Manager.insert(pom2);


        pom2=new carpet_graph(lW - 49, 6 * (chHX + RAMKA), lW, 7 * (chHX + RAMKA), //domyślne współrzędne
                                Powers); //I źródło danych
        pom2->set_data_colors(0, 255);
        pom2->set_frame(32);
        pom2->set_title("Map of power");
        Manager.insert(pom2);

        pom2=new manhattan_graph(lW - 49, 7 * (chHX + RAMKA), lW, 8 * (chHX + RAMKA), //domyślne współrzędne
                                    Powers, 0, //I źródło danych o wysokościach, niezarządzane
                                    Firsts, 0, //Źródło danych o kolorach — niezarządzane
                                    1,		//Słupki zaczynają się co najmniej od 0!
                                    //Jeśli 0 to zaczynają się od min>0
                                    0.22,		//Ułamek szerokości przeznaczony na perspektywę
                                    0.77		//Ułamek wysokości  przeznaczony na perspektywę
                                    ); //I źródło danych
        pom2->set_data_colors(0, 255);
        pom2->set_frame(32);
        pom2->set_title("Composed map of strength and attitude of agents");
        Manager.insert(pom2);

        pom2=new bars_graph(lW - 49, 8 * (chHX + RAMKA), lW, 9 * (chHX + RAMKA),
                            ClassStat);
        pom2->set_data_colors(0, 255);
        pom2->set_frame(128 + 64);
        pom2->set_title("Histogram of attitude");
        Manager.insert(pom2);

        pom2=new manhattan_graph(lW - 49, 9 * (chHX + RAMKA), lW, 10 * (chHX + RAMKA), //domyślne współrzędne //
                                    CorrFS, 0,	//I źródło danych
                                    CorrFS, 0,
                                 1,
                                 0.22,		//Ułamek szerokości przeznaczony na perspektywę
                                    0.77);		//Ułamek wysokości  przeznaczony na perspektywę
        pom2->set_data_colors(0, 255);
        pom2->set_text_colors(0);
        pom2->set_frame(128 + 64);
        pom2->set_title("Dynamism: curr. attitude vers. prev. attitude");
        Manager.insert(pom2);
        
        
        pom1=new sequence_graph(lW - 49, 10 * (chHX + RAMKA), lW, 11 * (chHX + RAMKA),
                                1, Sources.make_series_info(
                                                            iClassEntropy,
                                                            -1
                                                            ).get_ptr_val(),
                                1/*Wspólne minimum/maximum*/);
        //if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CLASSIFICATION");
        Manager.insert(pom1);
        
        pom1=new sequence_graph(lW - 49, 11 * (chHX + RAMKA), lW, 12 * (chHX + RAMKA),
                                1, Sources.make_series_info(
                                                            iEntropyFS,
                                                            -1
                                                            ).get_ptr_val(),
                                1/*Wspólne minimum/maximum*/);
        //if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CHANGE");
        Manager.insert(pom1);
        
        /*
        pom2=new sequence_graph(lW-49, 11*(char_height('X')+RAMKA),lW,12*(char_height('X')+RAMKA),
        1,Sources.make_series_info(
        iCorrFSR, //iCorrFS,
        -1
        ).get_ptr_val(),
                                1//Wspólne minimum/maximum
                                );
                                if(!pom2) goto ERROR;
                                pom2->set_frame(128);
                                pom2->set_title("HISTORY OF Prev.TO Curr. CORRELATION");
                                Manager.insert(pom2);
        */
        /*
        pom1=new sequence_graph(lW-49, 12*(char_height('X')+RAMKA),lW,13*(char_height('X')+RAMKA),
        1,Sources.make_series_info(
        iSpatialCorr,
        -1
        ).get_ptr_val(),
        0);
        
        */ 
        
        function_source_base* Linear=new function_source<y_eq_x>(SpatialCorr->get_size(), 0., double(SpatialCorr->get_size()), "length");
        Sources.insert(Linear);
        pom1=new scatter_graph(lW - 49, 12 * (chHX + RAMKA), lW, 13 * (chHX + RAMKA),
                               Linear, 0,
                               SpatialCorr, 0);
        
        //if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("SPATIAL CORRELATION");
        Manager.insert(pom1);
        
        //Tworzenie obszaru sterującego
        {
            wb_dynarray<rectangle_source_base*> tmp(4,  (rectangle_source_base*)Sources.get(iFirst),
                                                        (rectangle_source_base*)Sources.get(iSecond),
                                                        //(rectangle_source_base*)Sources.get(iThird),
                                                        (rectangle_source_base*)Sources.get(iPower),
                                                        (rectangle_source_base*)Sources.get(iPressure),
                                                        -1
                                                        );
            drawable_base* pom=new steering_wheel(lW - 49, 0, lW, 5 * (chHX + RAMKA), tmp);
            assert(pom!=nullptr);
            pom->set_background(10);
            Manager.insert(pom);
            pom->set_title(" ");
        }
    } // Koniec klamry umożliwiającej "goto"

    Sources.new_data_version(1,1); //Oznajmia seriom, że dane się uaktualniły	(po inicjacji)
    return; //Normalne wyjście.
ERROR: //Tu "akcja na niepogodę"
    // error_message(...)
    ; exit(-111);
}

#pragma clang diagnostic pop
/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */


