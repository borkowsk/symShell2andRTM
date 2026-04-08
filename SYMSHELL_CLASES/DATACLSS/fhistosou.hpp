//////////////////////////////////////////////////////////////////////////////
// Filtr liczacy liczebnosc okre�lonej liczby klas serii i pochodne statystyki
//////////////////////////////////////////////////////////////////////////////
#ifndef __FIXED_CLAS_HISTOGRAM_SOUR_HPP__
#define __FIXED_CLAS_HISTOGRAM_SOUR_HPP__
#include "statsour.hpp"

template<class DATA_SOURCE> //UWAGA NA DZIEDZICZENIE - N jest zmienne wiec ilosc serii pochodnych tez!
class fix_histogram_source:public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------------------------------
{
    typedef basic_statistics_source<DATA_SOURCE> base_class;
protected:
size_t Num;     //Number of Class;
double FixMin;  //Ustalone z gory minimum
double FixMax;  //Ustalone z gory maximum
unsigned SubRange:1; //Jezeli zakres realny wykracza poza zadany to robi hist z czesci danych

wb_dynarray<unsigned long> arra;

// Przemieszcza iterator o jednostke. Zeruje jesli koniec tablicy
size_t _next(iteratorh& p);

int _calculate() override
//Zwraca 1 jesli musial przeliczyc
{
    if(!basic_statistics_source<DATA_SOURCE>::_calculate()) 
    {
        return 0;//NIC DO ROBOTY BO NIE BYLO ZMIAN
    }
    else
    {//OBLICZANIE HISTOGRAMU
        assert(Num>1);      //Musza byc jakies klasy
        assert(arra.IsOK());//Musi byc zaalokowana tablica
        
        size_t SN;
        double smin,smax;	
        base_class::Source->bounds(SN,smin,smax);
        
        if((!SubRange)&&(smin<FixMin || FixMax<smax))//Czy w zakresie
            goto ERROR;
        
        arra.fill(0);
        
        //PETLA ZLICZANIA
        iteratorh Ind=base_class::Source->reset();
        base_class::source_miss=base_class::Source->get_missing();//Trzeba to zrobi� zeby FromSourceIsMissing dzialalo poprawnie!
        
        size_t Licz=0,Poza=0;        
        for(size_t j=0;j<SN;j++)
        {
            double pom=base_class::Source->get(Ind);
            if(!base_class::FromSourceIsMissing(pom))
            {	
                if(FixMin<=pom && pom<FixMax)
                {
                    Licz++;//Tylko te ktore faktycznie weszly w histogram
                    pom=(pom-FixMin)/(FixMax-FixMin);//Wczesniej sprawdzono ze pom w zakresie
                             assert(0<=pom && pom<1);
                    arra[size_t(trunc(pom*Num))]++;
                }
                else 
                if(pom==FixMax)//Wylapanie maksimum jesli jest
                {
                    Licz++;
                    arra[Num-1]++;//Arbitralnie do ostatniego koszyka
                }
                else
                {
                    Poza++;//Te ktore nie weszly w histogram
                }
            }
        }
        base_class::Source->close(Ind);
        
        //PETLA	LICZENIA STATYSTYK
        double Entropy=0;
        size_t licz_klasy=0,maxp=0;;
        
        if(Licz>0)//Jest cokolwiek do liczenia
        {
            size_t minp=0; 
            base_class::ymin=DBL_MAX;
            base_class::ymax=0;
                                      
            for(size_t i=0;i<Num;i++)
            {
                double pom=arra[i];
                
                if(pom>0)
                    licz_klasy++;
                
                if(pom>base_class::ymax)
                {
                    base_class::ymax=pom;
                    maxp=i;
                }
                
                if(pom<base_class::ymin)
                {
                    base_class::ymin=pom;
                    minp=i;
                }
                
                //Liczenie skladowych entropi
                double qi=pom/double(Licz);
                
                //Powiekrzenie sumy, tam gdzie nie jest to puste skrzyzowanie
                if(qi>0)
                    Entropy+=qi*log(qi);
                
            }
        }
        
        //AKTUALIZACJA AKTYWNYCH ZRODEL STATYSTYCZNYCH
        if(base_class::table[6]!=NULL)
        {
            base_class::table[6]->change_val(base_class::ymax);
        }
        
        if(base_class::table[7]!=NULL)
        {
            base_class::table[7]->change_val(licz_klasy);
        }
        
        if(base_class::table[8]!=NULL)
        {
            base_class::table[8]->change_val(maxp+smin+0.5);//0.5 bo srodek przedzialu calkowitego
        }
        
        if(base_class::table[9]!=NULL)
        { 
            if(Entropy!=0)//Jesli cos sie zsumowalo
                base_class::table[9]->change_val(-Entropy);
            else
                base_class::table[9]->change_val(base_class::table[9]->get_missing());
        }
        
        if(base_class::table[10]!=NULL)
        {
            int KL=size_t(smax-smin)+1;//Ile jednostek calkowitych zakresu realnego
                                        assert(KL>0);
            if(Entropy!=0)//Jesli cos sie zsumowalo
                base_class::table[10]->change_val(-Entropy/log(double(KL)));//A moze powinno byc dla zadanego?
            else
                base_class::table[10]->change_val(base_class::table[10]->get_missing());
        }
        
        //Jesli zdefiniowana liczba klas
        for(size_t k=0;k<Num;k++)
        {
            if(base_class::table[11+k]!=NULL) //"i" moze byc 0!
            {
                base_class::table[11+k]->change_val(arra[k]);//Jeden do jednego,
            }
        }
        
        return 1;
    }//Musial przeliczyc
        
ERROR:
        if(base_class::table[10]=NULL)
            base_class::table[10]->change_val(base_class::table[9]->get_missing());
        if(base_class::table[9]=NULL)
            base_class::table[9]->change_val(base_class::table[9]->get_missing());
        if(base_class::table[8]!=NULL)
            base_class::table[8]->change_val(base_class::table[8]->get_missing());
        if(base_class::table[7]!=NULL)
            base_class::table[7]->change_val(base_class::table[7]->get_missing());
        if(base_class::table[6]!=NULL)
            base_class::table[6]->change_val(base_class::table[6]->get_missing());
        arra.dispose();
        base_class::ymin=base_class::ymax=0;
        return 1;
}

public:
scalar_source<double>*      MainClass(const char* format="MainClass(%s)")	
{
	return base_class::GetMonoSource(6,format);
}

scalar_source<double>*      NumOfClass(const char* format="NumOfClass(%s)")	
{
	return base_class::GetMonoSource(7,format);
}

scalar_source<double>*      WhichMain(const char* format="WhichMain(%s)")	
{
	return base_class::GetMonoSource(8,format);
}

scalar_source<double>*      Entropy(const char* format="S(%s)")	
{
	return base_class::GetMonoSource(9,format);
}

scalar_source<double>*      NormEntropy(const char* format="nS(%s)")	
{
	return base_class::GetMonoSource(10,format);
}

scalar_source<double>*      Class(size_t number,const char* format="C<%g,%g)(%s)")
{
    char bufor[500];//Z duzym zapasem
    double step=(FixMax-FixMin)/Num;
    double min=number*step;
    double max=(number+1)*step;
    sprintf(bufor,format,min,max,"%s");
    return base_class::GetMonoSource(10+1+number,bufor);//+1 bo "number" moze byc 0!!!
}	

	fix_histogram_source(
		size_t HowManyClass,    //Number of Class;
        double iFixMin,         //Ustalone z gory minimum
        double iFixMax,         //Ustalone z gory maximum        
        DATA_SOURCE* ini=NULL,  //Klasa zrodlowa
                                //Jesli nie pokrywa sie z minX-maxX to faktycznie liczony jest wycinek
        const char* format="DISTR_%d_CLASS(%s[%g..%g])",
        bool iSubRange=false, //Jezeli zakres realny wykracza poza zadany to robi hist z czesci danych
		sources_menager_base* MyMenager=NULL,
		size_t table_size=11/*BEZ ZAPASU*/
		):
	    Num(HowManyClass),
        FixMin(iFixMin),FixMax(iFixMax),SubRange(iSubRange),
        basic_statistics_source<DATA_SOURCE>(ini,MyMenager,
                                             table_size+HowManyClass,//Alokuje miejsce na zrodelka klasowe
                                             format) 
	{
            wb_pchar bufor(strlen(format)+2*100);//Z za duzym zapasem jak na dwa integery, ale...
            bufor.prn(format,Num,"%s",FixMin,FixMax);
            basic_statistics_source<DATA_SOURCE>::settitle(bufor.get());
            arra.alloc(Num);//Liczba klas zafiksowana
        }
	
	~fix_histogram_source(){}

// Methods
size_t get_size()
{ 
	base_class::check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	return arra.get_size();
}	

void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
{
	basic_statistics_source<DATA_SOURCE>::all_subseries_required();
	//MAX CLASS
	MainClass();
	WhichMain();
	NumOfClass();
	Entropy();
    for(size_t i=0;i<Num;i++)
        Class(i); //Alokacja zrodel liczebnosci klas
}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
{
	base_class::check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();	
	min=base_class::ymin;max=base_class::ymax;
}

iteratorh  reset();//Umozliwia czytanie po iteratorze od poczatku
void close(iteratorh& p);//Usuwa iterator
double get(iteratorh& ptr_to_iterator);//Daje nastepna z N liczb!!! 
double get(size_t index);//Przetwarza index uzyskany z geometri
};

// Przemieszcza iterator o jednostke. Zeruje jesli koniec tablicy
template<class DATA_SOURCE> 
size_t fix_histogram_source<DATA_SOURCE>::_next(iteratorh& p)
{
	assert(p!=NULL);//Nie wolno wywolac dla NULL
	size_t pom=((size_t)p)-1;

	if(pom+1>=Num)
		p=NULL;
	else
		p=(iteratorh)(pom+2);
	return pom;	
}

//Przetwarza index uzyskany z geometri

template<class DATA_SOURCE> inline
double fix_histogram_source<DATA_SOURCE>::get(size_t index)
{ //na wartosc z serii, o ile jest mozliwe czytanie losowe	
	base_class::check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	assert(index<get_size());
	return arra[ index ];
}	

//Daje nastepna z N liczb!!! 
template<class DATA_SOURCE> inline
double fix_histogram_source<DATA_SOURCE>::get(iteratorh& ptr_to_iterator)
{
	assert(ptr_to_iterator!=NULL);
	return arra[ _next(ptr_to_iterator) ];
}


template<class DATA_SOURCE> inline
void  fix_histogram_source<DATA_SOURCE>::close(iteratorh& p)
{
	p=NULL;
}


template<class DATA_SOURCE> inline
iteratorh  fix_histogram_source<DATA_SOURCE>::reset()//Umozliwia czytanie po iteratorze od poczatku
{ 
	base_class::check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	return (iteratorh)1;
}


typedef fix_histogram_source<data_source_base> generic_fix_histogram_source;


#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/