/// @file
/// Implementation of the world of the convinced (D. Stauffer idea).
// /////////////////////////////////////////////////////////////////////
/// @date 2026-04-07 (modified)

#include <cstring>
#include <cmath>

#include "crand.h"
#include "cworld.h"
#include "histosou.hpp"
#include "clstsour.hpp" //There is also statsour!
#include "coincsou.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"

const int RAMKA=4;
extern const char* SIMULATION_NAME;

// Construction of agents:
// ///////////////////////

anAgent::anAgent(const anAgent& ini)
{
    if(&ini!=NULL)
    {
        First=ini.First;
        Second=ini.Second;
        if(min_sila<max_sila)
            Power=min_sila+RANDOM(max_sila-min_sila+1);
        else
            Power=ini.Power;
    }
    else
        _clean();
}

anAgent::anAgent()
{
    _clean();

    if(DRAND()<ToBeNewProb)
        First=1+RANDOM(ile_kate-1);
    else
        First=0;

    Second=0;
    if(min_sila<max_sila)
        Power=min_sila+RANDOM(max_sila-min_sila+1);
    else
        Power=max_sila;
}

// Static Agent Fields for Initialization:
// ///////////////////////////////////////

short	anAgent::ruchsily=1;	//Maximum force jump.
short   anAgent::min_sila=10;
short	anAgent::max_sila=100;	//Maximum agent power/force.
short	anAgent::ile_kate=2;	//Number of categories in maps.
short	anAgent::kate_shift=0;	//Offset for loading states from a GIF file.

double	anAgent::ToBeNewProb=0;	//The probability of a loner spontaneously changing his views on a new type of entertainment/sport.
double  anAgent::NewInfectProb=0.01;	//Probability of "infection" from a pair of "infected" individuals.
double  anAgent::ReverseProb=1;	//Probability of reversal of views to 0 - no idea for entertainment.
double  anAgent::SupportLevel=0.5;	//The power of support when you have some companions.

// CONSTRUCTION OF THE WORLD:
// //////////////////////////

extern unsigned internal_log;

aWorld::aWorld(	
        unsigned iWidth,		//Width of the torus of the agent matrix.
        double iToBeNewProb,	//=0.1,//Likelihood of spontaneous change of opinion.
        double iInfectProb,		//=0.9,//Probability of reversal of views on 0.
        double iSupportLevel,	//=0.5,//The power of support when he has some friends.
        const char* ilog_name,	//="convince.log", //File name for saving history.
        const char* imapl_name,	//=NULL,	//The name of the bitmap initializing the "components".
        const char* imapp_name,	//=NULL,	//The name of the agent force initialization bitmap.
        const char* ilive_mask,	//=NULL,	//The name of the bitmap defining uninhabited areas.
        short imax_sila,		//=100,	//Maximum agent power/strength.
        short imin_sila			//,=10	//Minimum agent strength.
               ):
        world(ilog_name,50),
        MaplName(clone_str(imapl_name)),
        MappName(clone_str(imapp_name)),
        MaskName(clone_str(ilive_mask)),
        //Sub-objects specific to this simulation:
        MyWidth(iWidth),
        Agenci(iWidth,iWidth,NULL),
        //Pointers to basic data series
        Firsts(NULL),
        Seconds(NULL),
        Powers(NULL)
{// There is not too much that can be done because we cannot rely on virtual methods of the world class yet.
    anAgent::ruchsily=1;
    anAgent::min_sila=imin_sila;
    anAgent::max_sila=imax_sila;
    anAgent::ile_kate=2;
    anAgent::kate_shift=0;

    anAgent::ToBeNewProb=iToBeNewProb;
    anAgent::NewInfectProb=iInfectProb;
    anAgent::ReverseProb=1-iToBeNewProb;
    anAgent::SupportLevel=iSupportLevel;

    world::set_simulation_name(SIMULATION_NAME);
}


void aWorld::make_basic_sources()
// Generates basic sources for the built-in data manager:
{
    sources_menager& WhatSourMen=this->Sources;
    world::make_basic_sources(); //Odziedziczone

    //Creation of the main data series:
    Firsts=Agenci.make_source("Attitude",&anAgent::First);
    if(Firsts)
        Firsts->setminmax(0,anAgent::ile_kate-1);

    Seconds=Agenci.make_source("Prev. attitude",&anAgent::Second);
    if(Seconds)
        Seconds->setminmax(0,anAgent::ile_kate-1);

    Powers=Agenci.make_source("Power",&anAgent::Power);

    //Placing the main series in the series manager:
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
}


void aWorld::make_default_visualisation()
// Works with the display manager and the log.
// Registers derived series, creates default "windows" and places them in the "Manager".
{
    area_menager_base& Manager=this->MyAreaMenager();
    int iFirst=0,iSecond=0,iPower=0,iClassif=0;

    // Getting the indexes of basic series from the manager:
    {
    if(Firsts) iFirst=Sources.search(Firsts->name());
        else  goto ERROR;

    if(Seconds) iSecond=Sources.search(Seconds->name());
        else  goto ERROR;

    if(Powers)   iPower=Sources.search(Powers->name());
        else  goto ERROR;

    if(Firsts)  iClassif=Sources.search(Firsts->name());
        else  goto ERROR;


    // Creation of derived statistical series:
    generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
    if(!FirstStat) goto ERROR;
        else	Sources.insert(FirstStat);

    generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
    if(!SecondStat) goto ERROR;
        else	Sources.insert(SecondStat);

    // Source for calculating statistics and histogram from classification:
    generic_histogram_source*  ClassStat=new generic_histogram_source(Firsts);
    if(!ClassStat) goto ERROR;
        else	Sources.insert(ClassStat);

    // And also creating a series counting their mutual co-statistics:
    coincidention_source* CorrFS=new coincidention_source(Firsts,Seconds);
    if(!CorrFS) goto ERROR;
    Sources.insert(CorrFS); // Registered to be released at the end

    fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(),internal_log);
    if(!EntropyFSLog) goto ERROR;
    int iEntropyFS=Sources.insert(EntropyFSLog);

    fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(),internal_log);
    if(!CorrFSLogR) goto ERROR;
    int iCorrFSR=Sources.insert(CorrFSLogR);


    // Creating series counting statistics from basic series:
    fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(),internal_log);
    if(!StressFirstLog) goto ERROR;
    int iSFirst=Sources.insert(StressFirstLog);

    fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(),internal_log);
    if(!StressSecondLog) goto ERROR;
    int iSSecond=Sources.insert(StressSecondLog);

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


    // And putting what you need in the log:
    Log.insert(ClassStat->NumOfClass());
    Log.insert(ClassStat->Entropy());
    Log.insert(ClassStat->NormEntropy());
    Log.insert(ClassStat->MainClass());
    Log.insert(ClassStat->WhichMain());
    Log.insert(FirstStat->Stress());
    Log.insert(SecondStat->Stress());
    Log.insert(CorrFS->Entropy());
    Log.insert(CorrFS->NormEntropy());
    Log.insert(CorrFS->Chi2());
    Log.insert(CorrFS->LevelOfFreedom());
    Log.insert(CorrFS->V2Cramer());
    Log.insert(CorrFS->T2Czupurow());
    Log.insert(CorrFS->Tau_b_Goodman_Kruskal());
    Log.insert(CorrFS->Tau_a_Goodman_Kruskal());

    // BASIC DATA SERIES VISUALIZATION CONSTRUCTION:
    // /////////////////////////////////////////////

    // WE GET THE DEFAULT WINDOW DIMENSIONS:
    unsigned szer=Manager.getwidth();
    unsigned wyso=Manager.getheight();
    assert(szer>50 && wyso>40); //The smallest possible window

    // Creating default areas, such as the STATUS area:
    world::make_default_visualisation();
    if(OutArea)
    {
        OutArea->set(1,1,szer/2-1,wyso/2-1);
        Manager.as_orginal(Manager.search(OutArea->name()));
    }

    // RIGHT "LUFTIES" FOR THIS SIMULATION:
    // (LUFTIES are small windows built into a large window.)
    graph* pom1=new sequence_graph(szer/2-1,wyso/4,szer-50,wyso/2-1,
                                    3,Sources.make_series_info(
                                            iNumClassF,iMainClassF,iWhichMainF,
                                                -1
                                            ).get_ptr_val(),
                                    0 // This 0 means that with rescaling
                                   );
    if(!pom1) goto ERROR;
    pom1->setframe(128);
    pom1->settitle("HISTORY OF CLASSIFICATION");
    Manager.insert(pom1);

    //inne mniej potrzebne
    graph* pom=new sequence_graph(szer/2-1,1,szer-50,wyso/4-1,	//default area coordinates
                                    1,Sources.make_series_info(
                                            iSFirst,	// Pointer to data source.
                                                -1
                                            ).get_ptr_val(),
                                   1); // This 1 means that the minimum and maximum are common.
    if(!pom) goto ERROR;
    pom->setframe(128);
    pom->settitle("HISTORY OF STRESS");
    Manager.insert(pom);

    pom=new carpet_graph(1,wyso/2,szer/3,wyso-1,	//default area coordinates
                            Firsts);	// Pointer to data source.
    pom->setdatacolors(0,255);
    pom->settitle("Map of current attitude");
    Manager.insert(pom);

    pom=new bars_graph(szer/3+1,wyso/2,szer/3*2,wyso-1,	//default area coordinates
                            ClassStat);	// Pointer to data source.
    pom->setdatacolors(0,255);
    pom->settitle("Histogram of attitude");
    Manager.insert(pom);

    pom=new manhattan_graph(szer/3*2+1,wyso/2,szer,wyso-1,	//default area coordinates
                                CorrFS,0,	// Pointer to data source.
                                CorrFS,0,	// Pointer to data source.
                                1,
                                0.22,		//A fraction of the width is allocated to perspective
                                0.77);	//A fraction of the height is dedicated to perspective
    pom->setdatacolors(0,255);
    pom->settextcolors(0);
    pom->settitle("Dynamism: curr. attit. vers. prev. attitude");
    Manager.insert(pom);

    //PRZYCISKI
    pom=new carpet_graph(szer-49,5*(char_height('X')+RAMKA),szer,6*(char_height('X')+RAMKA),	//default area coordinates
                            Seconds);	// Pointer to data source.
    pom->setdatacolors(0,255);
    pom->setframe(0);
    pom->settitle("Map of previous attitude");
    Manager.insert(pom);


    pom=new carpet_graph(szer-49,6*(char_height('X')+RAMKA),szer,7*(char_height('X')+RAMKA),	//default area coordinates
                            Powers);	// Pointer to data source.
    pom->setdatacolors(0,255);
    pom->setframe(0);
    pom->settitle("Map of power");
    Manager.insert(pom);

    pom=new manhattan_graph(szer-49, 7*(char_height('X')+RAMKA),szer,8*(char_height('X')+RAMKA),	//default area coordinates
                            Powers,0,	// Pointer to a data source (but unmanaged because someone else releases them)
                            Firsts,0,	// Pointer to a data source (but unmanaged because someone else releases them)
                            1,		//Bars start at least from 0!
                                            //If it was 0, then they start from min>0
                            0.22,		//A fraction of the width is allocated to perspective
                            0.77		//A fraction of the height is dedicated to perspective
                            );	// Pointer to data source.
    pom->setdatacolors(0,255);
    pom->setframe(0);
    pom->settitle("The composed map of strength & attitude of agents");
    Manager.insert(pom);

    pom1=new sequence_graph(szer-49, 9*(char_height('X')+RAMKA),szer,10*(char_height('X')+RAMKA),

                                    1,Sources.make_series_info(
                                            iClassEntropy,
                                                -1
                                            ).get_ptr_val(),
                                   1 /*Common minimum and maximum*/);
    if(!pom1) goto ERROR;
    pom1->setframe(128);
    pom1->settitle("HISTORY OF ENTROPY OF CLASSIFICATION");
    Manager.insert(pom1);


    pom1=new sequence_graph(szer-49, 10*(char_height('X')+RAMKA),szer,11*(char_height('X')+RAMKA),
                                    1,Sources.make_series_info(
                                            iEntropyFS,
                                                -1
                                            ).get_ptr_val(),
                                   1 /*Common minimum and maximum*/);
    if(!pom1) goto ERROR;
    pom1->setframe(128);
    pom1->settitle("HISTORY OF ENTROPY OF CHANGE");
    Manager.insert(pom1);


    pom=new sequence_graph(szer-49, 11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),
                                    1,Sources.make_series_info(
                                            iCorrFSR,//iCorrFS,
                                                -1
                                            ).get_ptr_val(),
                                    1
                                   );
    if(!pom) goto ERROR;
    pom->setframe(128);
    pom->settitle("HISTORY OF Prev.TO Curr. CORRELATION");
    Manager.insert(pom);

    // Creating a control area:
    {
    wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
                                              (rectangle_source_base*)Sources.get(iSecond),
                                              //(rectangle_source_base*)Sources.get(iThird),
                                              (rectangle_source_base*)Sources.get(iPower),
                                              (rectangle_source_base*)Sources.get(iClassif),
                                              -1
                                              );
    drawable_base* pom=new steering_wheel(szer-49,0,szer,5*(char_height('X')+RAMKA),tmp);
    assert(pom!=NULL);
    pom->setbackground(10);
    Manager.insert(pom);
    }

    }
    Sources.new_data_version(1,1); // Notifies the series that data has been updated (after initialization)

    ERROR:
    cerr<<"Error during construction of data series or data areas!"<<endl; //error_message(...)
}



// SIMULATION ACTIONS:
// ////////////////////

void aWorld::after_read_from_image()
//Action after loading the initialization file. NOTE! Also updating the agent class's static fields!!!
{
    switch(anAgent::ile_kate)
    {
    case   2:anAgent::kate_shift=7;break;
    case   4:anAgent::kate_shift=6;break;
    case   8:anAgent::kate_shift=5;break;
    case  16:anAgent::kate_shift=4;break;
    case  32:anAgent::kate_shift=3;break;
    case  64:anAgent::kate_shift=2;break;
    case 128:anAgent::kate_shift=1;break;
    case 256:anAgent::kate_shift=0;break;
    default:
        anAgent::ile_kate=256;
        anAgent::kate_shift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }
}


void aWorld::initialize_layers()
// Prepares the starting state of the simulation
{
    static int first=1; // TEMPORARY PRINTING DISABLEMENT!!!
    if(first)
        Log.GetStream()<<"convince SIMULATION:";

    switch(anAgent::ile_kate)
    {
    case   2:anAgent::kate_shift=7;break;
    case   4:anAgent::kate_shift=6;break;
    case   8:anAgent::kate_shift=5;break;
    case  16:anAgent::kate_shift=4;break;
    case  32:anAgent::kate_shift=3;break;
    case  64:anAgent::kate_shift=2;break;
    case 128:anAgent::kate_shift=1;break;
    case 256:anAgent::kate_shift=0;break;
    default:
        anAgent::ile_kate=256;
        anAgent::kate_shift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }

    // Printout of simulation parameter values:
    if(first)
      Log.GetStream()
        <<"\n Change to new Pn="<<Log.separator()<<anAgent::ToBeNewProb
        <<"\n Reverse Pr="<<Log.separator()<<anAgent::ReverseProb
        <<"\n Infection Pr="<<Log.separator()<<anAgent::NewInfectProb
        <<"\n Nei. Support S="<<Log.separator()<<anAgent::SupportLevel

        <<"\nMin Power="<<Log.separator()<<anAgent::min_sila
        <<"\nMax Power="<<Log.separator()<<anAgent::max_sila
        <<"\nMov. Power="<<Log.separator()<<anAgent::ruchsily
        <<"\nNum of Kl="<<Log.separator()<<anAgent::ile_kate;

    //	ACTUAL AGENT STATES DETERMINATION:
    // ///////////////////////////////////

    //It loads using the constructor, so it initializes the rest of the fields as well:
    int from1= Agenci.init_from_bitmap(MappName.get_ptr_val(),&anAgent::assignPow);
    //And here it changes some fields:
    int from2= Agenci.init_from_bitmap(MaplName.get_ptr_val(),&anAgent::assign123);

    //However, if not initialized, then temporary initialization via constructors or cloning:
    if(from1!=1 && from2!=1)
        Agenci.reallocate_all();

    // Removes the agent if the mask contains black color.
    if(Agenci.init_from_bitmap(MaskName.get_ptr_val(),&anAgent::killBlack)==1 )
        Agenci.deallocate_not_OK();

    first=0; //End of first run of initialization. There will be prints in the next runs.
}


void aWorld::simulate_one_step()
// Single simulation step:
{
    const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agenci.get_geometry());	assert(MyGeom!=NULL);
    size_t MonteSteps=MyWidth*MyWidth;
    for(int m=0;m<MonteSteps;m++)
    {
        long x=RANDOM(MyWidth);																assert(x<MyWidth);
        long y=RANDOM(MyWidth);																assert(y<MyWidth);
        anAgent& CenterAgent=Agenci(x,y);

        if(CenterAgent.First==0) //no view on sports/entertainment
        {
            if(DRAND()<anAgent::ToBeNewProb)
                CenterAgent.First=1+RANDOM(anAgent::ile_kate-1);
            continue;
        }
        else
        {
            //You need to check if it's not lonely and then change it along with the surroundings...
            unsigned index=MyGeom->get(x,y);

            //We check what our neighbors are like:
            int koledzy[8][2],ilu=0;
            iteratorh Neigh=MyGeom->make_neighbour_iterator(index,1);
            while(Neigh)
            {
                size_t index2=MyGeom->get_next(Neigh); //We get the neighbor index
                assert(index2!=any_layer_base::FULL); //There must always be a torus!

                if( index2==index)	//If it was him, it would still be pointless.
                    continue;

                size_t nx,ny; //Local variables to retrieve data by reference.
                MyGeom->WhatCoordinates(index2,nx,ny);
                if(Agenci(nx,ny).First==Agenci(x,y).First)
                {
                    koledzy[ilu][0]=nx;
                    koledzy[ilu][1]=ny;
                    ilu++;
                }
            }
            MyGeom->destroy_iterator(Neigh);

            if(ilu==8) //There is no one left to infect
                continue;

            if(ilu>0) //When he has the support of at least one neighbor with the same attitude
            {
                long nx,ny;
                if(ilu>1)
                {
                    int pom=RANDOM(ilu);
                    nx=koledzy[pom][0];
                    ny=koledzy[pom][1];
                }
                else
                {
                    nx=koledzy[0][0];
                    ny=koledzy[0][1];
                }

                //Random loss of interest despite support.
                if(DRAND()<anAgent::ReverseProb-anAgent::SupportLevel)
                {
                    CenterAgent.First=0;
                    continue;
                }

                //And now the hardest part, convincing unconvinced neighbors:
                int Inni[20][2],innych=0;
                for(int i=min(x,nx);i<=max(x,nx);i++)
                {
                    for(int j=min(y,ny);j<=max(y,ny);j++)
                    {
                        if(i==x && j==y)
                            continue; //The first of the pair of contagious
                        if(i==nx && j==ny)
                            continue; //Or the second of the pair
                        if(abs(i-x)<2 && abs(i-nx)<2 &&
                           abs(j-y)<2 && abs(j-ny)<2)
                        {
                            size_t ConvertedIndex=MyGeom->get(i,j);    assert(ConvertedIndex!=geometry_base::FULL);
                                                                                 assert(ConvertedIndex<MyWidth*MyWidth);
                            anAgent& ForModify=Agenci.get(ConvertedIndex);
                            if(ForModify.First==0 && DRAND()<anAgent::NewInfectProb)
                                ForModify.First=CenterAgent.First; //TMP
                        }
                    }
                }

            }
            else
            {
                //When there is no neighbor with your view, there is a quick random loss of view.
                if(DRAND()<anAgent::ReverseProb)
                    CenterAgent.First=0;
                continue;
            }
        }
    }
}

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */


