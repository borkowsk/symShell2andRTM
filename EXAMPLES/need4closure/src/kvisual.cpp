#include <string.h>
#include <math.h>

#include "krand.h"
#include "kworld.h"
#include "dhistosou.hpp"//Nowsza wersja - poprawiona
#include "clstsour.hpp" //Jest tez statsour
#include "spatcors.hpp"
#include "coincsou.hpp"
#include "funcsour.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"

const RAMKA=4;
extern const char* SYMULATION_NAME;

//KONSTRUKCJA	WIZUALIZACJI
////////////////////////////////////
extern unsigned internal_log;
extern unsigned spatial_correlation_mode;


//Generuje podstawowe zrodla dla wbudowanego menagera danych lub innego
////////////////////////////////////////////////////////////////////////////
void kworld::make_basic_sources(sources_menager& WhatSourMen)
{
    world::make_basic_sources(WhatSourMen);//Odziedziczone
    
    //Glowne serie 
    Firsts=Agenci.make_source("Attitude",&kagent::First);	
    if(Firsts)
        Firsts->setminmax(-1,1);	
    Seconds=Agenci.make_source("Prev. attitude",&kagent::Second);
    if(Seconds)
        Seconds->setminmax(-1,1);
    
    ForLeft=Agenci.make_source("For left",&kagent::ForLeft);
    ForRight=Agenci.make_source("For right",&kagent::ForRight);
    Powers=Agenci.make_source("Power",&kagent::Power);
    
    /*
    Pressure=Agenci.make_source("Pressure",&kagent::Press);
    MaxPressure=long(MaxSila)*WeightOfSelf;
    MaxPressure+=long(MaxSila)*NeedForClosure * (IleSasiad>0 ? IleSasiad : sqr(OdlSasiad*2+1)-1   );//Prymitywna implementacja
    Pressure->set_missing(-1);
    Pressure->setminmax(0, MaxPressure  );
    */
    
    ptrLastChanged=new ptr_to_scalar_source<int>(NULL,"Change cnt.");
    ptrLastMigration=new ptr_to_scalar_source<int>(NULL,"Migration cnt.");
    
    //NIE DZIALA PRAWIDLOWO (?)
    ptrLastChanged->set_missing(-1);
    ptrLastMigration->set_missing(-1);
    CountCh=ptrLastChanged->get_missing();
    CountMig=ptrLastMigration->get_missing();
    
    //Classif=Agenci.make_source("Classification",&kagent::Classif); //Z PIERWOWZORU "LANGUAGES"
    //if(Classif)
    //	Classif->setminmax(0,IleKate*IleKate*IleKate-1);//Max class ==IleKate^3 bo trzy niezalezne plaszczyzny
    
    //Umieszczenie glownych serii w menagerze serii
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
    WhatSourMen.insert(ForLeft);
    WhatSourMen.insert(ForRight);
    //WhatSourMen.insert(Pressure);
    WhatSourMen.insert(ptrLastChanged);
    WhatSourMen.insert(ptrLastMigration);
    
    //WhatSourMen.insert(Classif);
}


//Wspolpraca z menagerem wyswietlania a takze logiem
//------------------------------------------------------------------
//Wypisywanie/dopisywanie na konsole statusu
void    kworld::actualize_out_area()
{
    world::actualize_out_area();
    //ptrStres; ptrClsSize; - Do przekazywania aktualnie najwazniejszych danych na okno statusu
    if(OutArea)
    {
        wb_pchar bufor(1024);//ze sporym zapsem
        //assert(ptrStres->);
        double Stres=ptrStres->get();      //Zakladamy ze to zrodla jednowartosciowe
        double ClsSiz=ptrClsSize->get();
        bufor.prn("Stress: %g \nAproximated cluster size: %g",Stres,ClsSiz);
        OutArea->add_text(bufor.get_ptr_val());
    }
}


void kworld::make_default_visualisation(area_menager_base& Menager)
//Rejestruje pochodne serie, tworzy domyslne "lufciki" i wklada w "Menager"
{
    int iFirst=0,iSecond=0,iPower=0,iPressure=0,iChangeCnt,iMigratCnt;
    //Uzyskanie indeksow podstawowych serii z menagera
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
        
        
        //Zrodlo liczace statystyke i histogram z klasyfikacji
        //=new  generic_histogram_source(Firsts);   
        generic_discrete_histogram_source*  ClassStat=new generic_discrete_histogram_source(-1,3,Firsts);
        if(!ClassStat) goto ERROR;
        else	Sources.insert(ClassStat);  //cerr<<ClassStat->name();
        
        generic_spatial_correlation_source* SpatialCorr=new generic_spatial_correlation_source(Firsts,-1,spatial_correlation_mode);
        if(!SpatialCorr) goto ERROR;
        int iSpatialCorr=Sources.insert(SpatialCorr);
        
        //A takze utworzenie seri liczacych ich wzajemne ko-statystyki
        coincidention_source* CorrFS=new coincidention_source(Firsts,Seconds);
        if(!CorrFS) goto ERROR;
        Sources.insert(CorrFS);//Zeby zostala kiedys zwolniona, a poza tym moze ktos kiedys...
        
        fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(),internal_log);
        if(!EntropyFSLog) goto ERROR;
        int iEntropyFS=Sources.insert(EntropyFSLog);
        
        fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(),internal_log);//Fifo korelacji pierwszych z drugimi
        if(!CorrFSLogR) goto ERROR;
        int iCorrFSR=Sources.insert(CorrFSLogR);
        
        
        //I utworzenie seri liczacych ich statystyki
        /*
        fifo_source<double>* MeanPressLog=new fifo_source<double>(PressureStat->Mean(),internal_log);//Fifo ze sredniego chwilowego stresu
        if(!MeanPressLog) goto ERROR;
        int iMeanPress=Sources.insert(MeanPressLog);
        
          fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(),internal_log);//Fifo ze stresu klasycznego
          if(!StressFirstLog) goto ERROR;
          int iSFirst=Sources.insert(StressFirstLog);
          
            fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(),internal_log);//Fifo ze starego stresu
            if(!StressSecondLog) goto ERROR;
            int iSSecond=Sources.insert(StressSecondLog);
        */
        
        fifo_source<double>* ClusterSizeLog=new fifo_source<double>(SpatialCorr->ApproximatedClusterSize(),internal_log);//Fifo z rozmiaru klastra
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
        for(int k=0;k<3;k++) //Zrodla histogramu - musza byc na koncu bo zmienia na liczba
        {
            Log.insert(ClassStat->Class(k));
        }
        
        //PODSTAWOWA WIZUALIZACJA SERII DANYCH
        //WYMIARY DOMYSLNEGO OKNA
        unsigned szer=Menager.getwidth();
        unsigned wyso=Menager.getheight();
        assert(szer>50 && wyso>40);//Najmniejsze sensowne okno
        
        //Obszary domyœlne - np obszar STATUSU
        world::make_default_visualisation(Menager);
        if(OutArea) 
        {
            OutArea->set(1,1,szer/2-1,wyso/2-1);
            Menager.as_orginal(Menager.search(OutArea->name()));
        }
        
        //WLASCIWE LUFCIKI
        graph* pom1=new sequence_graph(szer/2-1,wyso/4,szer-50,wyso/2-1,
            3,Sources.make_series_info(
            iNumClassF,iMainClassF,iWhichMainF,									
            -1
            ).get_ptr_val(),
            0//* Z reskalowaniem 
            );
        if(!pom1) goto ERROR;
        pom1->setframe(128);
        pom1->settitle("HISTORY OF CLASSIFICATION");
        Menager.insert(pom1);
        
        //inne mniej potrzebne
        graph* pom=new sequence_graph(szer/2-1,1,szer-50,wyso/4-1,	//domyslne wspolrzedne
            3,Sources.make_series_info(
            //iSSecond,
            iClusterSize,
            iChangeCnt,
            iMigratCnt,
            //		iMeanPress,                                        
            -1
            ).get_ptr_val(),
            0// Z reskalowaniem 
            //1//Wspolne minimum/maximum
            );
        if(!pom) goto ERROR;
        pom->setframe(128);
        pom->settitle("HISTORY OF CLUSTERISATION");
        Menager.insert(pom);
        
        pom=new carpet_graph(1,wyso/2,szer/3,wyso-1,//domyslne wspolrzedne
            Firsts);//I zrodlo danych
        pom->setdatacolors(0,255);
        pom->settitle("Map of current attitude");
        Menager.insert(pom);
        
        
        pom=new carpet_graph(szer/3+1,wyso/2,szer/3*2,wyso-1,//domyslne wspolrzedne
            ForLeft);
        pom->setdatacolors(0,255);
        pom->settitle("Map of left counters");
        Menager.insert(pom);
        
        pom=new carpet_graph(szer/3*2+1,wyso/2,szer,wyso-1,//domyslne wspolrzedne
            ForRight);
        pom->setdatacolors(0,255);
        pom->settitle("Map of right counters");
        Menager.insert(pom);
        
        /*
        pom=new carpet_graph(szer/3*2+1,wyso/2,szer,wyso-1,//domyslne wspolrzedne,//domyslne wspolrzedne  szer-49,7*char_height('X')+7,szer,8*char_height('X')+9
        Pressure);
        pom->setdatacolors(0,255);
        pom->settitle("Map of instantaneous social pressure");
        Menager.insert(pom);
        */
        
        //PRZYCISKI
        pom=new carpet_graph(szer-49,5*(char_height('X')+RAMKA),szer,6*(char_height('X')+RAMKA),//domyslne wspolrzedne 
            Seconds);//I zrodlo danych
        pom->setdatacolors(0,255);
        pom->setframe(32);
        pom->settitle("Map of previous attitude");
        Menager.insert(pom);
        
        
        pom=new carpet_graph(szer-49,6*(char_height('X')+RAMKA),szer,7*(char_height('X')+RAMKA),//domyslne wspolrzedne
            Powers);//I zrodlo danych
        pom->setdatacolors(0,255);
        pom->setframe(32);
        pom->settitle("Map of power");
        Menager.insert(pom);
        
        pom=new manhattan_graph(szer-49, 7*(char_height('X')+RAMKA),szer,8*(char_height('X')+RAMKA),//domyslne wspolrzedne
            Powers,0,//I zrodlo danych o wysokosciach, miezazadzane
            Firsts,0,//Zrodlo danych o kolorach - niezazadzane
            1,		//Slupki zaczynaja sie conajmniej od 0!
            //Jesli 0 to zaczynaja sie od min>0
            0.22,		//Ulamek szerokosci przeznaczony na perspektywe
            0.77		//Ulamek wysokosci  przeznaczony na perspektywe
            );//I zrodlo danych
        pom->setdatacolors(0,255);
        pom->setframe(32);
        pom->settitle("Composed map of strength & attitude of agents");
        Menager.insert(pom);
        
        pom=new bars_graph(szer-49, 8*(char_height('X')+RAMKA),szer,9*(char_height('X')+RAMKA),
            ClassStat);
        pom->setdatacolors(0,255);
        pom->setframe(128+64);
        pom->settitle("Histogram of attitude");
        Menager.insert(pom);
        
        pom=new manhattan_graph(szer-49, 9*(char_height('X')+RAMKA),szer,10*(char_height('X')+RAMKA),//domyslne wspolrzedne //
            CorrFS,0,	//I zrodlo danych
            CorrFS,0,
            1,
            0.22,		//Ulamek szerokosci przeznaczony na perspektywe
            0.77);		//Ulamek wysokosci  przeznaczony na perspektywe
        pom->setdatacolors(0,255);
        pom->settextcolors(0);
        pom->setframe(128+64);
        pom->settitle("Dynamism: curr. attit. vers. prev. attitude");
        Menager.insert(pom);
        
        
        pom1=new sequence_graph(szer-49, 10*(char_height('X')+RAMKA),szer,11*(char_height('X')+RAMKA),						
            1,Sources.make_series_info(
            iClassEntropy,
            -1
            ).get_ptr_val(),
            1/*Wspolne minimum/maximum*/);
        if(!pom1) goto ERROR;
        pom1->setframe(128);
        pom1->settitle("HISTORY OF ENTROPY OF CLASIFICATION");
        Menager.insert(pom1);
        
        pom1=new sequence_graph(szer-49,11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),  						
            1,Sources.make_series_info(
            iEntropyFS,
            -1
            ).get_ptr_val(),
            1/*Wspolne minimum/maximum*/);
        if(!pom1) goto ERROR;
        pom1->setframe(128);
        pom1->settitle("HISTORY OF ENTROPY OF CHANGE");
        Menager.insert(pom1);
        
        /*
        pom=new sequence_graph(szer-49, 11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),
        1,Sources.make_series_info(
        iCorrFSR,//iCorrFS,						
        -1
        ).get_ptr_val(),
								1//Wspolne minimum/maximum
                                );
                                if(!pom) goto ERROR;
                                pom->setframe(128);
                                pom->settitle("HISTORY OF Prev.TO Curr. CORRELATION");
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
        
        function_source_base* Linear=new function_source<yeqx>(SpatialCorr->get_size(),0,SpatialCorr->get_size(),"lenght");
        Sources.insert(Linear);
        pom1=new scatter_graph(szer-49, 12*(char_height('X')+RAMKA),szer,13*(char_height('X')+RAMKA),
            Linear,0,
            SpatialCorr,0);	
        
        if(!pom1) goto ERROR;
        pom1->setframe(128);
        pom1->settitle("SPATIAL CORRELATION");
        Menager.insert(pom1);
        
        //Tworzenie obszaru sterujacego
        {
            wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
                (rectangle_source_base*)Sources.get(iSecond),
                //(rectangle_source_base*)Sources.get(iThird),
                (rectangle_source_base*)Sources.get(iPower),
                (rectangle_source_base*)Sources.get(iPressure),
                -1
                );
            drawable_base* pom=new steering_wheel(szer-49,0,szer,5*(char_height('X')+RAMKA),tmp);			
            assert(pom!=NULL);
            pom->setbackground(10);
            Menager.insert(pom);
            pom->settitle(" ");
        }
        
}
Sources.new_data_version(1,1);//Oznajmia seriom ze dane sie uaktualnily	(po inicjacji)

ERROR://... tu akcja na niepogode
;//error_message(...)
}



/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                                                                  */
/*      Instytut Studiow Spolecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk                  */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
