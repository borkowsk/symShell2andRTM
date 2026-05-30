/// @file
/// @brief
/// @EN{ Implementation of "the world of the convinced" (D. Stauffer idea). }
/// @PL{ Implementacja "świata tych przekonanych" (idea D. Stauffer-a). }
/// @date 2026-05-30 (modified)
///       =================================================================
/// @details ...
//======================================================================================================================

#include <cstring>
#include <cmath>

#include "crand.h"
#include "cworld.h"
#include "histosou.hpp"
#include "clstsour.hpp" //There is also statsour!
#include "coincsou.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"
#include "toitoutoll.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2;

extern const char* SIMULATION_NAME;
const int FRAME=4; ///< @brief @PL{ GRUBOŚĆ RAMKI. } @EN{ FRAME THICKNESS. }

// Construction of agents:
//========================

convAgent::convAgent(const convAgent& ini)
{
    First=ini.First;
    Second=ini.Second;
    if(MinStrength < MaxStrength)
        Power= tos( MinStrength + RANDOM(MaxStrength - MinStrength + 1) );
    else
        Power=ini.Power;
}

convAgent::convAgent(const convAgent* ini)
{
    if(ini!=NULL)
    {
        First=ini->First;
        Second=ini->Second;
        if(MinStrength < MaxStrength)
            Power= tos( MinStrength + RANDOM(MaxStrength - MinStrength + 1) );
        else
            Power=ini->Power;
    }
    else
        _clean();
}

convAgent::convAgent()
{
    _clean();

    if(DRAND()<ToBeNewProb)
        First=tos( 1+RANDOM(NumOfCate - 1) );
    else
        First=0;

    Second=0;
    if(MinStrength < MaxStrength)
        Power= tos( MinStrength + RANDOM(MaxStrength - MinStrength + 1) );
    else
        Power=MaxStrength;
}

// Static Agent Fields for Initialization:
//========================================

short   convAgent::MinStrength=10;
short	convAgent::MaxStrength=100;	//Maximum agent power/force.
short	convAgent::NumOfCate=2;	    //Number of categories in maps.
short	convAgent::CateShift=0;	    //Offset for loading states from a GIF file.

double	convAgent::ToBeNewProb=0;	    //The probability of a loner spontaneously changing his views on a new type of entertainment/sport.
double  convAgent::NewInfectProb=0.01;	//Probability of "infection" from a pair of "infected" individuals.
double  convAgent::ReverseProb=1;	    //Probability of reversal of views to 0 - no idea for entertainment.
double  convAgent::SupportLevel=0.5;    //The power of support when you have some companions.

istream &operator>>(istream &i, convAgent &a)        //!< @brief @EN{ Deserialization. } @PL{ Deserializacja. }
{
    char pom;
    i>>pom;		//ignoring `{`
    i>>a.Power>>a.First>>a.Second;
    i>>pom;		//ignoring `}`
    return i;
}

ostream &operator<<(ostream &o, const convAgent &a)    //!< @brief @EN{ Serialization. } @PL{ Serializacja. }
{
    o<<'{';
    o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' ';
    o<<'}';
    return o;
}



// CONSTRUCTION OF THE WORLD:
//===========================

extern unsigned InternalLogLen;

convWorld::convWorld(
        unsigned iWidth,			//Width of the torus of the agent matrix.
        double iToBeNewProb,		//=0.1, //Likelihood of spontaneous change of opinion.
        double iInfectProb,			//=0.9, //Probability of reversal of views on 0.
        double iSupportLevel,		//=0.5, //The power of support when he has some friends.
        const char* iLog_name,		//="convince.log", //File name for saving history.
        const char* iMapL_name,		//=NULL,	//The name of the bitmap initializing the "components".
        const char* iMapP_name,		//=NULL,	//The name of the agent force initialization bitmap.
        const char* iLive_mask,		//=NULL,	//The name of the bitmap defining uninhabited areas.
        short iMax_strength,		//=100,	//Maximum agent power/strength.
        short iMin_strength			//,=10	//Minimum agent strength.
               )
: world(iLog_name, 50),
  MapLName(clone_str(iMapL_name)),
  MappName(clone_str(iMapP_name)),
  MaskName(clone_str(iLive_mask)),
  //Sub-objects specific to this simulation:
  MyWidth(iWidth),
  Agents(iWidth, iWidth, NULL),
  //Pointers to basic data series
  Firsts(NULL), Seconds(NULL), Powers(NULL)
{// There is not too much that can be done because we cannot rely on virtual methods of the world class yet.
    convAgent::MinStrength=iMin_strength;
    convAgent::MaxStrength=iMax_strength;
    convAgent::NumOfCate=2;
    convAgent::CateShift=0;

    convAgent::ToBeNewProb=iToBeNewProb;
    convAgent::NewInfectProb=iInfectProb;
    convAgent::ReverseProb= 1 - iToBeNewProb;
    convAgent::SupportLevel=iSupportLevel;

    world::set_simulation_name(SIMULATION_NAME);
}


void convWorld::make_basic_sources()
// Generates basic sources for the built-in data manager:
{
    sources_manager& WhatSourMen=this->Sources;
    world::make_basic_sources(); //Odziedziczone

    //Creation of the main data series:
    Firsts=Agents.make_source("Attitude", &convAgent::First);
    if(Firsts)
        Firsts->set_min_max(0, convAgent::NumOfCate - 1);

    Seconds=Agents.make_source("Prev. attitude", &convAgent::Second);
    if(Seconds)
        Seconds->set_min_max(0, convAgent::NumOfCate - 1);

    Powers=Agents.make_source("Power", &convAgent::Power);

    //Placing the main series in the series manager:
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
}


void convWorld::make_default_visualisation()
// Works with the display manager and the log.
// Registers derived series, creates default "windows" and places them in the "Manager".
{
    area_manager_base& Manager=this->MyAreaManager();
    int iFirst=0,iSecond=0,iPower=0,iClassify=0;

    // Getting the indexes of basic series from the manager:
    {
    if(Firsts) iFirst=Sources.search(Firsts->name());
    else  goto ERROR;

    if(Seconds) iSecond=Sources.search(Seconds->name());
    else  goto ERROR;

    if(Powers)   iPower=Sources.search(Powers->name());
    else  goto ERROR;

    if(Firsts) iClassify=Sources.search(Firsts->name());
    else  goto ERROR;


    // Creation of derived statistical series:
    generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
    //if(!FirstStat) goto ERROR;
    Sources.insert(FirstStat);

    generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
    //if(!SecondStat) goto ERROR;
    Sources.insert(SecondStat);

    // Source for calculating statistics and histogram from classification:
    generic_histogram_source*  ClassStat=new generic_histogram_source(Firsts);
    //if(!ClassStat) goto ERROR;
    Sources.insert(ClassStat);

    // And also creating a series counting their mutual co-statistics:
    coincidence_source* CorrFS=new coincidence_source(Firsts, Seconds);
    //if(!CorrFS) goto ERROR;
    Sources.insert(CorrFS); // Registered to be released at the end

    fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(), InternalLogLen);
    //if(!EntropyFSLog) goto ERROR;
    int iEntropyFS=Sources.insert(EntropyFSLog);

    fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(), InternalLogLen);
    //if(!CorrFSLogR) goto ERROR;
    int iCorrFSR=Sources.insert(CorrFSLogR);

    // Creating series counting statistics from basic series:
    fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(), InternalLogLen);
    //if(!StressFirstLog) goto ERROR;
    int iSFirst=Sources.insert(StressFirstLog);

    fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(), InternalLogLen);
    //if(!StressSecondLog) goto ERROR;
    //int iSSecond=
            Sources.insert(StressSecondLog);

    //iMainClassF,iWhichMainF,iNumClassF,
    fifo_source<double>* NumClassLog=new fifo_source<double>(ClassStat->NumOfClass(), InternalLogLen);
    //if(!NumClassLog) goto ERROR;
    int iNumClassF=Sources.insert(NumClassLog);

    fifo_source<double>* ClassEntropyLog=new fifo_source<double>(ClassStat->Entropy(), InternalLogLen);
    //if(!ClassEntropyLog) goto ERROR;
    int iClassEntropy=Sources.insert(ClassEntropyLog);

    fifo_source<double>* MainClassLog=new fifo_source<double>(ClassStat->MainClass(), InternalLogLen);
    //if(!MainClassLog) goto ERROR;
    int iMainClassF=Sources.insert(MainClassLog);


    fifo_source<double>* WhichMainLog=new fifo_source<double>(ClassStat->WhichMain(), InternalLogLen);
    //if(!WhichMainLog) goto ERROR;
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
    //==============================================

    // WE GET THE DEFAULT WINDOW DIMENSIONS:
    unsigned lW= Manager.get_width();
    unsigned lH= Manager.get_height();                        assert(lW > 50 && lH > 40); //The smallest possible window

    // Creating default areas, such as the STATUS area:
    world::make_default_visualisation();
    if(OutArea)
    {
        OutArea->set(1, 1, lW / 2. - 1, lH / 2. - 1);
        Manager.as_original(Manager.search(OutArea->name()));
    }

    // RIGHT "LUFTIES" FOR THIS SIMULATION:
    // (LUFTIES are small windows built into a large window.)
    graph* pom1=new sequence_graph( toi(lW / 2. - 1),toi(lH / 4.),toi(lW - 50),toi(lH / 2. - 1),
                                    3, Sources.make_series_info(
                                            iNumClassF,iMainClassF,iWhichMainF,
                                                -1
                                            ).get_ptr_val(),
                                    0 // This 0 means that with rescaling
                                   );
    //if(!pom1) goto ERROR;
    pom1->set_frame(128);
    pom1->set_title("HISTORY OF CLASSIFICATION");
    Manager.insert(pom1);

    //inne mniej potrzebne
    graph* pom=new sequence_graph( toi(lW / 2 - 1), 1,toi(lW - 50),toi(lH / 4 - 1),	//default area coordinates
                                    1, Sources.make_series_info(
                                            iSFirst,	// Pointer to data source.
                                                -1
                                            ).get_ptr_val(),
                                  1); // This 1 means that the minimum and maximum are common.
    //if(!pom) goto ERROR;
    pom->set_frame(128);
    pom->set_title("HISTORY OF STRESS");
    Manager.insert(pom);

    pom=new carpet_graph(1,toi(lH / 2),toi(lW / 3),toi(lH - 1),	//default area coordinates
                            Firsts);	// Pointer to data source.
    pom->set_data_colors(0, 255);
    pom->set_title("Map of current attitude");
    Manager.insert(pom);

    pom=new bars_graph(toi(lW / 3 + 1),toi(lH / 2),toi(lW / 3 * 2),toi(lH - 1),	//default area coordinates
                            ClassStat);	// Pointer to data source.
    pom->set_data_colors(0, 255);
    pom->set_title("Histogram of attitude");
    Manager.insert(pom);

    pom=new manhattan_graph(toi(lW / 3 * 2 + 1),toi(lH / 2),toi(lW),toi(lH - 1),	//default area coordinates
                                CorrFS, 0,	// Pointer to data source.
                                CorrFS, 0,	// Pointer to data source.
                                1,
                                0.22,		//A fraction of the width is allocated to perspective
                                0.77);		//A fraction of the height is dedicated to perspective
    pom->set_data_colors(0, 255);
    pom->set_text_colors(0);
    pom->set_title("Dynamism: curr. attitude vers. prev. attitude");
    Manager.insert(pom);

    //PRZYCISKI
    pom=new carpet_graph(toi(lW - 49), toi(5 * (char_height('X') + FRAME)), toi(lW), toi(6 * (char_height('X') + FRAME)),	//default area coordinates
                            Seconds);	// Pointer to data source.
    pom->set_data_colors(0, 255);
    pom->set_frame(0);
    pom->set_title("Map of previous attitude");
    Manager.insert(pom);


    pom=new carpet_graph(toi(lW - 49), toi(6 * (char_height('X') + FRAME)), toi(lW), toi(7 * (char_height('X') + FRAME)),	//default area coordinates
                            Powers);	// Pointer to data source.
    pom->set_data_colors(0, 255);
    pom->set_frame(0);
    pom->set_title("Map of power");
    Manager.insert(pom);

    pom=new manhattan_graph(toi(lW - 49), toi(7 * (char_height('X') + FRAME)), toi(lW), toi(8 * (char_height('X') + FRAME)),	//default area coordinates
                            Powers, 0,	// Pointer to a data source (but unmanaged because someone else releases them)
                            Firsts, 0,	// Pointer to a data source (but unmanaged because someone else releases them)
                            1,		//Bars start at least from 0!
                                            //If it was 0, then they start from min>0
                            0.22,		//A fraction of the width is allocated to perspective
                            0.77		//A fraction of the height is dedicated to perspective
                            );	// Pointer to data source.
    pom->set_data_colors(0, 255);
    pom->set_frame(0);
    pom->set_title("The composed map of strength and attitude of agents");
    Manager.insert(pom);

    pom1=new sequence_graph(toi(lW - 49), toi(9 * (char_height('X') + FRAME)), toi(lW), toi(10 * (char_height('X') + FRAME)),
                            1, Sources.make_series_info(
                                            iClassEntropy,
                                                -1
                                            ).get_ptr_val(),
                            1 /*Common minimum and maximum*/);
    //if(!pom1) goto ERROR;
    pom1->set_frame(128);
    pom1->set_title("HISTORY OF ENTROPY OF CLASSIFICATION");
    Manager.insert(pom1);


    pom1=new sequence_graph(toi(lW - 49), toi(10 * (char_height('X') + FRAME)), toi(lW), toi(11 * (char_height('X') + FRAME)),
                            1, Sources.make_series_info(
                                            iEntropyFS,
                                                -1
                                            ).get_ptr_val(),
                            1 /*Common minimum and maximum*/);
    //if(!pom1) goto ERROR;
    pom1->set_frame(128);
    pom1->set_title("HISTORY OF ENTROPY OF CHANGE");
    Manager.insert(pom1);


    pom=new sequence_graph(toi(lW - 49), toi(11 * (char_height('X') + FRAME)), toi(lW), toi(12 * (char_height('X') + FRAME)),
                           1, Sources.make_series_info(
                                            iCorrFSR, //iCorrFS,
                                                -1
                                            ).get_ptr_val(),
                           1
                                   );
    //if(!pom) goto ERROR;
    pom->set_frame(128);
    pom->set_title("HISTORY OF Prev.TO Curr. CORRELATION");
    Manager.insert(pom);

    // Creating a control area:
    {
    wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
                                              (rectangle_source_base*)Sources.get(iSecond),
                                              //(rectangle_source_base*)Sources.get(iThird),
                                              (rectangle_source_base*)Sources.get(iPower),
                                              (rectangle_source_base*)Sources.get(iClassify),
                                              -1
                                              );
    drawable_base* l_pom=new steering_wheel(toi(lW - 49), 0, toi(lW), toi(5 * (char_height('X') + FRAME)), tmp); //assert(l_pom!=NULL);
    l_pom->set_background(10);
    Manager.insert(l_pom);
    }

    }
    Sources.new_data_version(1,1); // Notifies the series that data has been updated (after initialization)

    ERROR:
    cerr<<"Error during construction of data series or data areas!"<<endl; //error_message(...)
}



// SIMULATION ACTIONS:
//=====================

void convWorld::after_read_from_image()
//Action after loading the initialization file. NOTE! Also updating the agent class's static fields!!!
{
    switch(convAgent::NumOfCate)
    {
    case   2:convAgent::CateShift=7;break;
    case   4:convAgent::CateShift=6;break;
    case   8:convAgent::CateShift=5;break;
    case  16:convAgent::CateShift=4;break;
    case  32:convAgent::CateShift=3;break;
    case  64:convAgent::CateShift=2;break;
    case 128:convAgent::CateShift=1;break;
    case 256:convAgent::CateShift=0;break;
    default:
        convAgent::NumOfCate=256;
            convAgent::CateShift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }
}


void convWorld::initialize_layers()
// Prepares the starting state of the simulation
{
    static int first=1; // TEMPORARY PRINTING DISABLEMENT!!!
    if(first)
        Log.GetStream()<<"convince SIMULATION:";

    switch(convAgent::NumOfCate)
    {
    case   2:convAgent::CateShift=7;break;
    case   4:convAgent::CateShift=6;break;
    case   8:convAgent::CateShift=5;break;
    case  16:convAgent::CateShift=4;break;
    case  32:convAgent::CateShift=3;break;
    case  64:convAgent::CateShift=2;break;
    case 128:convAgent::CateShift=1;break;
    case 256:convAgent::CateShift=0;break;
    default:
        convAgent::NumOfCate=256;
            convAgent::CateShift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }

    // Printout of simulation parameter values:
    if(first)
      Log.GetStream()
              << "\n Change to new Pn=" << Log.separator() << convAgent::ToBeNewProb
              << "\n Reverse Pr=" << Log.separator() << convAgent::ReverseProb
              << "\n Infection Pr=" << Log.separator() << convAgent::NewInfectProb
              << "\n Nei. Support S=" << Log.separator() << convAgent::SupportLevel

              << "\nMin Power=" << Log.separator() << convAgent::MinStrength
              << "\nMax Power=" << Log.separator() << convAgent::MaxStrength
              << "\nNum of Kl=" << Log.separator() << convAgent::NumOfCate;

    // AGENT STATES DETERMINATION:
    //============================

    //It loads using the constructor, so it initializes the rest of the fields as well:
    int from1= Agents.init_from_bitmap(MappName.get_ptr_val(), &convAgent::assignPow);
    //And here it changes some fields:
    int from2= Agents.init_from_bitmap(MapLName.get_ptr_val(), &convAgent::assign123);

    //However, if not initialized, then temporary initialization via constructors or cloning:
    if(from1!=1 && from2!=1)
        Agents.reallocate_all();

    // Removes the agent if the mask contains black color.
    if(Agents.init_from_bitmap(MaskName.get_ptr_val(), &convAgent::killBlack) == 1 )
        Agents.deallocate_not_OK();

    first=0; //End of first run of initialization. There will be prints in the next runs.
}


void convWorld::simulate_one_step()
// Single simulation step:
{
    const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agents.get_geometry());	assert(MyGeom != NULL);
    for(size_t len=MyGeom->get_size(),i=0;i<len;i++)
    {
        convAgent& a = Agents.get(i);
        a.save_state();
    }

    size_t MonteSteps=MyWidth*MyWidth; //Maybe less?
    for(int m=0;m<MonteSteps;m++)
    {
        long x=RANDOM(MyWidth);																assert(x<MyWidth);
        long y=RANDOM(MyWidth);																assert(y<MyWidth);
        convAgent& CenterAgent=Agents(x, y);

        if(CenterAgent.First==0) //no view on sports/entertainment
        {
            if(DRAND() < convAgent::ToBeNewProb)
                CenterAgent.First=tos(1+RANDOM(convAgent::NumOfCate - 1));
            continue;
        }
        else
        {
            //You need to check if it's not lonely and then change it along with the surroundings...
            unsigned index=MyGeom->get(x,y);

            //We check what our neighbors are like:
            int koledzy[8][2];
            unsigned ilu=0;
            iterator_h Neigh=MyGeom->make_neighbour_iterator(index, 1);
            while(Neigh)
            {
                size_t index2=MyGeom->get_next(Neigh); //We get the neighbor index
                assert(index2!=any_layer_base::FULL); //There must always be a torus!

                if( index2==index)	//If it was him, it would still be pointless.
                    continue;

                size_t nx,ny; //Local variables to retrieve data by reference.
                MyGeom->WhatCoordinates(index2,nx,ny);
                if(Agents(nx, ny).First == Agents(x, y).First)
                {
                    koledzy[ilu][0]=toi(nx);
                    koledzy[ilu][1]=toi(ny);
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
                if(DRAND() < convAgent::ReverseProb - convAgent::SupportLevel)
                {
                    CenterAgent.First=0;
                    continue;
                }

                // And now the hardest part, convincing unconvinced neighbors:
                //============================================================
                //int Inni[20][2];
                //int innych=0;

                for(int i=toi(min(x,nx));i<=max(x,nx);i++)
                {
                    for(int j=toi(min(y,ny));j<=max(y,ny);j++)
                    {
                        if(i==x && j==y)
                            continue; //The first of the pair of contagious
                        if(i==nx && j==ny)
                            continue; //Or the second of the pair
                        if(abs(i-x)<2 && abs(i-nx)<2 &&
                           abs(j-y)<2 && abs(j-ny)<2)
                        {
                            size_t ConvertedIndex=MyGeom->get(i,j);              assert(ConvertedIndex!=geometry::FULL);
                                                                                 assert(ConvertedIndex<MyWidth*MyWidth);
                            convAgent& ForModify=Agents.get(ConvertedIndex);
                            if(ForModify.First==0 && DRAND() < convAgent::NewInfectProb)
                                ForModify.First=CenterAgent.First; //TMP?
                        }
                    }
                }

            }
            else
            {
                //When there is no neighbor with your view, there is a quick random loss of view.
                if(DRAND() < convAgent::ReverseProb)
                    CenterAgent.First=0;
                continue;
            }
        }
    }
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
