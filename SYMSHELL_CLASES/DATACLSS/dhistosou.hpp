///////////////////////////////////////////////////////////////////////////
// Filtr liczacy liczebnosc dyskretnych klas serii i pochodne statystyki
///////////////////////////////////////////////////////////////////////////
#ifndef __DISCR_HISTOGRAM_SOUR_HPP__
#define __DISCR_HISTOGRAM_SOUR_HPP__
#include "statsour.hpp"

template<class DATA_SOURCE> //UWAGA NA DZIEDZICZENIE - N jest zmienne wiec ilosc serii pochodnych tez!
class discrete_histogram_source:public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------------------------------
{
protected:
size_t Num;//Number of Class;
int    Sta;//Lowest integer class (offset)
//int RealLowest;//Historycznie-aktualnie najnizsza wartosc ze zrodla
//int RealHighest;//Historycznie-aktualnie najwyzsza wartosc ze zrodla

wb_dynarray<unsigned long> arra;

// Przemieszcza iterator o jednostke. Zeruje jesli koniec tablicy
size_t _next(iteratorh& p)
{
	assert(p!=NULL);//Nie wolno wywolac dla NULL
	size_t pom=((size_t)p)-1;

	if(pom+1>=Num)
		p=NULL;
	else
		p=(iteratorh)(pom+2);
	return pom;
}


int _calculate() //Zwraca 1 jesli musial przeliczyc
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
        this->Source->bounds(SN,smin,smax);

        if(smax-smin>double(size_t(-1)) )//Czy w zakresie size_t
            goto ERROR;

        arra.fill(0);

        //PETLA ZLICZANIA
        iteratorh Ind=this->Source->reset();
        this->source_miss=this->Source->get_missing();//Trzeba to zrobić zeby FromSourceIsMissing dzialalo poprawnie!

        size_t Licz=0,Poza=0;
        for(size_t j=0;j<SN;j++)
        {
            double pom=this->Source->get(Ind);
            if(!filter_source_base::FromSourceIsMissing(pom))
            {
                pom=std::trunc(pom);//Wlasne trunc?

                if(Sta<=pom && pom<Sta+Num)
                {
                    Licz++;//Tylko te ktore faktycznie weszly w histogram
                    arra[size_t(pom-Sta)]++;//Wczesniej sprawdzono ze pom w zakresie size_t i ze >=Sta
                }
                else
                {
                    Poza++;//Te ktore nie weszly w histogram
                }
            }
        }
        this->Source->close(Ind);

        //PETLA	LICZENIA STATYSTYK
        double Entropy=0;
        size_t licz_klasy=0,maxp=0;;

        if(Licz>0)//Jest cokolwiek do liczenia
        {
            size_t minp=0;
            this->ymin=DBL_MAX;
            this->ymax=0;

            for(size_t i=0;i<Num;i++)
            {
                double pom=arra[i];

                if(pom>0)
                    licz_klasy++;

                if(pom>this->ymax)
                {
                    this->ymax=pom;
                    maxp=i;
                }

                if(pom<this->ymin)
                {
                    this->ymin=pom;
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
        if(this->table[6]!=NULL)
        {
            this->table[6]->change_val(this->ymax);
        }

        if(this->table[7]!=NULL)
        {
            this->table[7]->change_val(licz_klasy);
        }

        if(this->table[8]!=NULL)
        {
            this->table[8]->change_val(maxp+smin+0.5);//0.5 bo srodek przedzialu calkowitego
        }

        if(this->table[9]!=NULL)
        {
            if(Entropy!=0)//Jesli cos sie zsumowalo
                this->table[9]->change_val(-Entropy);
            else
                this->table[9]->change_val(this->table[9]->get_missing());
        }

        if(this->table[10]!=NULL)
        {
            int KL=size_t(smax-smin)+1;//Ile jednostek calkowitych zakresu realnego
                                        assert(KL>0);
            if(Entropy!=0)//Jesli cos sie zsumowalo
                this->table[10]->change_val(-Entropy/log(double(KL)));//A moze powinno byc dla zadanego?
            else
                this->table[10]->change_val(this->table[10]->get_missing());
        }

        //Jesli zdefiniowana liczba klas
        for(size_t k=0;k<Num;k++)
        {
            if(this->table[11+k]!=NULL) //"i" moze byc 0!
            {
                this->table[11+k]->change_val(arra[k]);//Jeden do jednego,
            }
        }

        return 1;
    }//Musial przeliczyc

ERROR:
        if(this->table[10]=NULL)
            this->table[10]->change_val(this->table[9]->get_missing());
        if(this->table[9]=NULL)
            this->table[9]->change_val(this->table[9]->get_missing());
        if(this->table[8]!=NULL)
            this->table[8]->change_val(this->table[8]->get_missing());
        if(this->table[7]!=NULL)
            this->table[7]->change_val(this->table[7]->get_missing());
        if(this->table[6]!=NULL)
            this->table[6]->change_val(this->table[6]->get_missing());
        arra.dispose();
        this->ymin=this->ymax=0;
        return 1;
}

public:
scalar_source<double>*      MainClass(const char* format="MainClass(%s)")
{
        return this->GetMonoSource(6,format);
}

scalar_source<double>*      NumOfClass(const char* format="NumOfClass(%s)")
{
        return this->GetMonoSource(7,format);
}

scalar_source<double>*      WhichMain(const char* format="WhichMain(%s)")
{
        return this->GetMonoSource(8,format);
}

scalar_source<double>*      Entropy(const char* format="S(%s)")
{
        return this->GetMonoSource(9,format);
}

scalar_source<double>*      NormEntropy(const char* format="nS(%s)")
{
        return this->GetMonoSource(10,format);
}

scalar_source<double>*      Class(size_t number,const char* format="N%d(%s)")
{
    char bufor[200];//Z duzym zapasem
    int pom=int(number)+Sta;
    sprintf(bufor,format,pom,"%s");
    return this->GetMonoSource(10+1+number,bufor);//+1 bo "number" moze byc 0!!!
}

discrete_histogram_source(
		int    LowestClass,     //Najnizsza klasa
                size_t HowManyClass,    //Ile klas od niej
                DATA_SOURCE* ini=NULL,  //Klasa zrodlowa
                                      //Jesli nie pokrywa sie z minX-maxX to faktycznie liczony jest wycinek
                const char* format="DISCR.DISTRIBUTION(%s[%d..%d])",
		sources_menager_base* MyMenager=NULL,
		size_t table_size=11/*BEZ ZAPASU*/
		):
	    Num(HowManyClass),
            Sta(LowestClass),
            basic_statistics_source<DATA_SOURCE>(ini,MyMenager,
                                                table_size+HowManyClass,//Alokuje miejsce na zrodelka klasowe
                                                format)
	{
            wb_pchar bufor(strlen(format)+2*100);//Z za duzym zapasem jak na dwa integery, ale...
            bufor.prn(format,"%s",Sta,Sta+Num-1);
            basic_statistics_source<DATA_SOURCE>::settitle(bufor.get());
            arra.alloc(Num);//Liczba klas zafiksowana
        }

	~discrete_histogram_source(){}

// Methods
size_t get_size()
{
        this->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
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

iteratorh  reset()
//Umozliwia czytanie od poczatku
{
        this->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	return (iteratorh)1;
}

void close(iteratorh& p)
{
	p=NULL;
}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
{
        this->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();
	min=this->ymin;max=this->ymax;
}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!!
{
	assert(ptr_to_iterator!=NULL);
	return arra[ _next(ptr_to_iterator) ];
}

double get(size_t index)//Przetwarza index uzyskany z geometri
{ //na wartosc z serii, o ile jest mozliwe czytanie losowe
        this->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	assert(index<get_size());
	return arra[ index ];
}


};

typedef discrete_histogram_source<data_source_base> generic_discrete_histogram_source;


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
