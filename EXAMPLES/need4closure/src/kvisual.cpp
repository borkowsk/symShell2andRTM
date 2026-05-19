/// @file
/// @brief
///  @EN{ Visualisation of KWORLD (old example for SymShell implementing a Kruglanskis like model). }
///  @PL{  }
/// @date 2026-05-19 (modified)
/// =================================================================================================
/// @details ...
//======================================================================================================================

#include <cstring>
#include <cmath>

#include "krand.h"
#include "kworld.h"
#include "dhistosou.hpp" //Nowsza wersja — poprawiona
#include "clstsour.hpp"  //Jest też statsour
#include "spatcors.hpp"
#include "coincsou.hpp"
#include "funcsour.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"

using namespace sym2;
using namespace sym2::data;

const int RAMKA=4;
extern const char* SIMULATION_NAME;

// KONSTRUKCJA	WIZUALIZACJI
//===================================
extern unsigned internal_log;
extern unsigned spatial_correlation_mode;


// Generuje podstawowe źródła dla wbudowanego zarządcy danych lub innego
//===========================================================================
void kworld::make_basic_sources()
{
    sources_manager& WhatSourMen=this->Sources;
    world::make_basic_sources(); //Odziedziczone
    
    //Główne serie
    Firsts=Agenci.make_source("Attitude",&kagent::First);	
    if(Firsts)
        Firsts->set_min_max(-1, 1);
    Seconds=Agenci.make_source("Prev. attitude",&kagent::Second);
    if(Seconds)
        Seconds->set_min_max(-1, 1);
    
    ForLeft=Agenci.make_source("For left",&kagent::ForLeft);
    ForRight=Agenci.make_source("For right",&kagent::ForRight);
    Powers=Agenci.make_source("Power",&kagent::Power);
    
    /*
    Pressure=Agenci.make_source("Pressure",&kagent::Press);
    MaxPressure=long(MaxSila)*WeightOfSelf;
    MaxPressure+=long(MaxSila)*NeedForClosure * (IleSasiad>0 ? IleSasiad : sqr(OdlSasiad*2+1)-1   ); //Prymitywna implementacja
    Pressure->set_missing(-1);
    Pressure->set_min_max(0, MaxPressure  );
    */
    
    ptrLastChanged=new ptr_to_scalar_source<int>(nullptr,"Change cnt.");
    ptrLastMigration=new ptr_to_scalar_source<int>(nullptr,"Migration cnt.");
    
    //NIE DZIAŁA PRAWIDŁOWO (?)
    ptrLastChanged->set_missing(-1);
    ptrLastMigration->set_missing(-1);
    CountCh=ptrLastChanged->get_missing();
    CountMig=ptrLastMigration->get_missing();
    
    //Classif=Agenci.make_source("Classification",&kagent::classif); //Z PIERWOWZORU "LANGUAGES"
    //if(classif)
    //	classif->set_min_max(0,IleKate*IleKate*IleKate-1); //Max class ==IleKate^3 bo trzy niezależne płaszczyzny
    
    //Umieszczenie głównych serii w managerze serii
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
    WhatSourMen.insert(ForLeft);
    WhatSourMen.insert(ForRight);
    //WhatSourMen.insert(Pressure);
    WhatSourMen.insert(ptrLastChanged);
    WhatSourMen.insert(ptrLastMigration);
    
    //WhatSourMen.insert(classif);
}


// Współpraca z managerem wyświetlania, a także logiem
//------------------------------------------------------------------
// Wypisywanie/dopisywanie na konsole statusu
void    kworld::actualize_out_area()
{
    world::actualize_out_area();
    //ptrStres; ptrClsSize; - Do przekazywania aktualnie najważniejszych danych na okno statusu
    if(OutArea)
    {
        wb_pchar bufor(1024); //ze sporym zapasem
        //assert(ptrStres->);
        double Stres=ptrStres->get();      //Zakładamy, że to źródła jednowartościowe
        double ClsSiz=ptrClsSize->get();
        bufor.prn("Stress: %g \nAproximated cluster size: %g",Stres,ClsSiz);
        OutArea->add_text(bufor.get_ptr_val());
    }
}


void kworld::make_default_visualisation()
//Rejestruje pochodne serie, tworzy domyślne "lufciki" i wkłada w "Manager"
{
    area_manager_base& Menager=this->MyAreaManager();
    int iFirst=0,iSecond=0,iPower=0,iPressure=0,iChangeCnt,iMigratCnt;
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
        if(!LastChangedLog) goto ERROR;
        iChangeCnt=Sources.insert(LastChangedLog);
        
        fifo_source<int>* LastMigrationLog=new fifo_source<int>(ptrLastMigration,internal_log);
        if(!LastMigrationLog) goto ERROR;
        iMigratCnt=Sources.insert(LastMigrationLog);
        
        generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
        if(!FirstStat) goto ERROR;
        else	Sources.insert(FirstStat);
        
        generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
        if(!SecondStat) goto ERROR;
        else	Sources.insert(SecondStat);
        
        //generic_basic_statistics_source* PressureStat=new generic_basic_statistics_source(Pressure);
        //if(!PressureStat) goto ERROR;
        //	else	Sources.insert(PressureStat);
        
        
        //Źródło liczące statystykę i histogram z klasyfikacji
        //=new  generic_histogram_source(Firsts);   
        generic_discrete_histogram_source*  ClassStat=new generic_discrete_histogram_source(-1,3,Firsts);
        if(!ClassStat) goto ERROR;
        else	Sources.insert(ClassStat);  //cerr<<ClassStat->name();
        
        generic_spatial_correlation_source* SpatialCorr=new generic_spatial_correlation_source(Firsts,-1,spatial_correlation_mode);
        if(!SpatialCorr) goto ERROR;
        int iSpatialCorr=Sources.insert(SpatialCorr);
        
        //A także utworzenie seri liczących ich wzajemne ko-statystyki
        coincidence_source* CorrFS=new coincidence_source(Firsts, Seconds);
        if(!CorrFS) goto ERROR;
        Sources.insert(CorrFS); //Żeby została kiedyś zwolniona, a poza tym może ktoś kiedyś...
        
        fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(),internal_log);
        if(!EntropyFSLog) goto ERROR;
        int iEntropyFS=Sources.insert(EntropyFSLog);
        
        fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(),internal_log); //FIFO z korelacji pierwszych z drugimi
        if(!CorrFSLogR) goto ERROR;
        int iCorrFSR=Sources.insert(CorrFSLogR);
        
        
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
        if(!ClusterSizeLog) goto ERROR;
        int iClusterSize=Sources.insert(ClusterSizeLog);
        
        //iMainClassF,iWhichMainF,iNumClassF,			
        fifo_source<double>* NumClassLog=new fifo_source<double>(ClassStat->NumOfClass(),internal_log);
        if(!NumClassLog) goto ERROR;
        int iNumClassF=Sources.insert(NumClassLog);
        
        fifo_source<double>* ClassEntropyLog=new fifo_source<double>(ClassStat->Entropy(),internal_log);
        if(!ClassEntropyLog) goto ERROR;
        int iClassEntropy=Sources.insert(ClassEntropyLog);
        
        fifo_source<double>* MainClassLog=new fifo_source<double>(ClassStat->MainClass(),internal_log);
        if(!MainClassLog) goto ERROR;
        int iMainClassF=Sources.insert(MainClassLog);
        
        fifo_source<double>* WhichMainLog=new fifo_source<double>(ClassStat->WhichMain(),internal_log);
        if(!WhichMainLog) goto ERROR;
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
        unsigned szer= Menager.get_width();
        unsigned wyso= Menager.get_height();
        assert(szer>50 && wyso>40); //Najmniejsze sensowne okno
        
        //Obszary domyślne — np. obszar STATUSU
        world::make_default_visualisation();
        if(OutArea) 
        {
            OutArea->set(1,1,szer/2-1,wyso/2-1);
            Menager.as_original(Menager.search(OutArea->name()));
        }
        
        //WŁAŚCIWE LUFCIKI
        graph* pom1=new sequence_graph(szer/2-1,wyso/4,szer-50,wyso/2-1,
            3,Sources.make_series_info(
            iNumClassF,iMainClassF,iWhichMainF,									
            -1
            ).get_ptr_val(),
            0//* Z reskalowaniem 
            );
        if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF CLASSIFICATION");
        Menager.insert(pom1);
        
        //inne mniej potrzebne
        graph* pom=new sequence_graph(szer/2-1,1,szer-50,wyso/4-1,	//domyślne współrzędne
            3,Sources.make_series_info(
            //iSSecond,
            iClusterSize,
            iChangeCnt,
            iMigratCnt,
            //		iMeanPress,                                        
            -1
            ).get_ptr_val(),
            0// Z reskalowaniem 
            //1//Wspólne minimum/maximum
            );
        if(!pom) goto ERROR;
        pom->set_frame(128);
        pom->set_title("HISTORY OF CLUSTERISATION");
        Menager.insert(pom);
        
        pom=new carpet_graph(1,wyso/2,szer/3,wyso-1, //domyślne współrzędne
            Firsts); //I źródło danych
        pom->set_data_colors(0, 255);
        pom->set_title("Map of current attitude");
        Menager.insert(pom);
        
        
        pom=new carpet_graph(szer/3+1,wyso/2,szer/3*2,wyso-1, //domyślne współrzędne
            ForLeft);
        pom->set_data_colors(0, 255);
        pom->set_title("Map of left counters");
        Menager.insert(pom);
        
        pom=new carpet_graph(szer/3*2+1,wyso/2,szer,wyso-1, //domyślne współrzędne
            ForRight);
        pom->set_data_colors(0, 255);
        pom->set_title("Map of right counters");
        Menager.insert(pom);
        
        /*
        pom=new carpet_graph(szer/3*2+1,wyso/2,szer,wyso-1, //domyślne współrzędne, //domyślne współrzędne  szer-49,7*char_height('X')+7,szer,8*char_height('X')+9
        Pressure);
        pom->set_data_colors(0,255);
        pom->set_title("Map of instantaneous social pressure");
        Menager.insert(pom);
        */
        
        //PRZYCISKI
        pom=new carpet_graph(szer-49,5*(char_height('X')+RAMKA),szer,6*(char_height('X')+RAMKA), //domyślne współrzędne
            Seconds); //I źródło danych
        pom->set_data_colors(0, 255);
        pom->set_frame(32);
        pom->set_title("Map of previous attitude");
        Menager.insert(pom);
        
        
        pom=new carpet_graph(szer-49,6*(char_height('X')+RAMKA),szer,7*(char_height('X')+RAMKA), //domyślne współrzędne
            Powers); //I źródło danych
        pom->set_data_colors(0, 255);
        pom->set_frame(32);
        pom->set_title("Map of power");
        Menager.insert(pom);
        
        pom=new manhattan_graph(szer-49, 7*(char_height('X')+RAMKA),szer,8*(char_height('X')+RAMKA), //domyślne współrzędne
            Powers,0, //I źródło danych o wysokościach, niezarządzane
            Firsts,0, //Źródło danych o kolorach — niezarządzane
            1,		//Słupki zaczynają się co najmniej od 0!
            //Jeśli 0 to zaczynają się od min>0
            0.22,		//Ułamek szerokości przeznaczony na perspektywę
            0.77		//Ułamek wysokości  przeznaczony na perspektywę
            ); //I źródło danych
        pom->set_data_colors(0, 255);
        pom->set_frame(32);
        pom->set_title("Composed map of strength & attitude of agents");
        Menager.insert(pom);
        
        pom=new bars_graph(szer-49, 8*(char_height('X')+RAMKA),szer,9*(char_height('X')+RAMKA),
            ClassStat);
        pom->set_data_colors(0, 255);
        pom->set_frame(128 + 64);
        pom->set_title("Histogram of attitude");
        Menager.insert(pom);
        
        pom=new manhattan_graph(szer-49, 9*(char_height('X')+RAMKA),szer,10*(char_height('X')+RAMKA), //domyślne współrzędne //
            CorrFS,0,	//I źródło danych
            CorrFS,0,
            1,
            0.22,		//Ułamek szerokości przeznaczony na perspektywę
            0.77);		//Ułamek wysokości  przeznaczony na perspektywę
        pom->set_data_colors(0, 255);
        pom->set_text_colors(0);
        pom->set_frame(128 + 64);
        pom->set_title("Dynamism: curr. attit. vers. prev. attitude");
        Menager.insert(pom);
        
        
        pom1=new sequence_graph(szer-49, 10*(char_height('X')+RAMKA),szer,11*(char_height('X')+RAMKA),						
            1,Sources.make_series_info(
            iClassEntropy,
            -1
            ).get_ptr_val(),
            1/*Wspolne minimum/maximum*/);
        if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CLASIFICATION");
        Menager.insert(pom1);
        
        pom1=new sequence_graph(szer-49,11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),  						
            1,Sources.make_series_info(
            iEntropyFS,
            -1
            ).get_ptr_val(),
            1/*Wspolne minimum/maximum*/);
        if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CHANGE");
        Menager.insert(pom1);
        
        /*
        pom=new sequence_graph(szer-49, 11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),
        1,Sources.make_series_info(
        iCorrFSR, //iCorrFS,
        -1
        ).get_ptr_val(),
                                1//Wspólne minimum/maximum
                                );
                                if(!pom) goto ERROR;
                                pom->set_frame(128);
                                pom->set_title("HISTORY OF Prev.TO Curr. CORRELATION");
                                Menager.insert(pom);
        */
        /*
        pom1=new sequence_graph(szer-49, 12*(char_height('X')+RAMKA),szer,13*(char_height('X')+RAMKA),						
        1,Sources.make_series_info(
        iSpatialCorr,
        -1
        ).get_ptr_val(),
        0);
        
        */ 
        
        function_source_base* Linear=new function_source<y_eq_x>(SpatialCorr->get_size(), 0, SpatialCorr->get_size(), "lenght");
        Sources.insert(Linear);
        pom1=new scatter_graph(szer-49, 12*(char_height('X')+RAMKA),szer,13*(char_height('X')+RAMKA),
            Linear,0,
            SpatialCorr,0);	
        
        if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("SPATIAL CORRELATION");
        Menager.insert(pom1);
        
        //Tworzenie obszaru sterującego
        {
            wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
                (rectangle_source_base*)Sources.get(iSecond),
                //(rectangle_source_base*)Sources.get(iThird),
                (rectangle_source_base*)Sources.get(iPower),
                (rectangle_source_base*)Sources.get(iPressure),
                -1
                );
            drawable_base* pom=new steering_wheel(szer-49,0,szer,5*(char_height('X')+RAMKA),tmp);			
            assert(pom!=nullptr);
            pom->set_background(10);
            Menager.insert(pom);
            pom->set_title(" ");
        }
        
}
Sources.new_data_version(1,1); //Oznajmia seriom, że dane się uaktualniły	(po inicjacji)

ERROR://... tu akcja na niepogodę
; //error_message(...)
}

/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */


