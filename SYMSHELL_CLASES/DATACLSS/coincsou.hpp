///////////////////////////////////////////////////////////////////////////////
// Filtr liczacy koincydencje klas dwu serii i pochodne statystyki (Hi^2 itp)
///////////////////////////////////////////////////////////////////////////////
#ifndef __COINCIDENTION_SOUR_HPP__
#define __COINCIDENTION_SOUR_HPP__
#include "costatso.hpp"

#ifndef SIZE_MAX
#define SIZE_MAX (ULONG_MAX)
#endif

class coincidention_source:public co_statistics_source
//------------------------------------------------------------------------------
{
protected:
size_t N;//Required number of Class of First
size_t M;//Required number of Class of Second
wb_ptr<rectangle_geometry>   my_geometry;
wb_dynmatrix<unsigned long> arra;
int iHi;//indeks dla Hi-kwadrat

double _get(size_t index)
//Bezposrednio siega do tablicy arra
{
	assert(arra.IsOK());
	size_t  NNHeight=my_geometry->get_width();
	size_t i=index/NNHeight;//Który wiersz
	assert(arra[i].IsOK());
	size_t j=index%NNHeight;//Która kolumna
	return arra[i][j];
}

int _calculate() //Zwraca 1 jesli musial przeliczyc
{
	if(!co_statistics_source::_calculate()) 
		return 0;
	double Hi=0;//Suma skladowych Chi^2
	double Entropy=0;//Suma skladowych entropii
	double Za=0;//Ilosc bledow dla Tau_b
	double Zb=0;//Ilosc bledow dla Tau_a
	size_t NNHeight=0,MMWidth=0,columns=0,rows=0,Pairs=0;
	wb_dynarray<unsigned long> KolumnaSumm;
	wb_dynarray<unsigned long> WierszSumm;
	
	{//OBLICZANIE HISTOGRAMU 2D KOINCYDENCJI ZDARZEN
	assert(N==-1);//Tylko tryb integerowy zaimplementowany
	assert(M==-1);
	data_source_base* source1=get_first_source();
	data_source_base* source2=get_second_source();
	double min1,max1,min2,max2;
	size_t N1,N2;//Numbers of pairs,items & real numbers of classes
	size_t i,j;

	source1->bounds(N1,min1,max1);
	source2->bounds(N2,min2,max2);

	if(N1<=0 || N2<=0) goto ERROR;//Nie da sie dalej liczyc

	if(max1-min1<=double(size_t(-1)))//Czy w zakresie size_t
		NNHeight=size_t(max1-min1)+1;//Ile jednostek calkowitych zakresu
		else
		goto ERROR;
    if(max2-min2<=double(size_t(-1)))//Czy w zakresie size_t
		MMWidth=size_t(max2-min2)+1;//Ile jednostek calkowitych zakresu
		else
		goto ERROR;

	//Alokacja tablicy jesli potrzeba
	if(!my_geometry || 
		my_geometry->get_height()!=NNHeight || 
		my_geometry->get_width()!=MMWidth		)
		{
		if(arra.alloc(NNHeight,MMWidth)==0)//blad alokacji - za malo/za duzo?
				goto ERROR;
		
		my_geometry=new rectangle_geometry(MMWidth,NNHeight,0);
		}
	
	for(i=0;i<NNHeight;i++)
		{
		assert(arra[i].get_size()==MMWidth);
		memset(arra[i].get_ptr_val(),0, //Wypelnia za pomoca 0
				sizeof(unsigned long)*arra[i].get_size());
		}

	
	//PETLA ZLICZANIA
	KolumnaSumm.alloc(NNHeight);
	WierszSumm.alloc(MMWidth);

	memset(KolumnaSumm.get_ptr_val(),0, //Wypelnia za pomoca 0
				sizeof(unsigned long)*NNHeight);
	memset(WierszSumm.get_ptr_val(),0, //Wypelnia za pomoca 0
				sizeof(unsigned long)*MMWidth);

	iteratorh Ind1=source1->reset(); 
	iteratorh Ind2=source2->reset();
	while(Ind1 && Ind2) //Puki oba iteratory
		{
		double pom1=source1->get(Ind1);
		double pom2=source2->get(Ind2);
		if((!FromFirstIsMissing(pom1)) &&
		  (!FromSecondIsMissing(pom2)) 
			)
			{	
			Pairs++;   //Suma zdarzen!!!
			pom1-=min1;//NN
						assert(pom1<SIZE_MAX);
			pom2-=min2;//MM
					    assert(pom2<SIZE_MAX);
			KolumnaSumm[size_t(pom1)]++;//To niespecjalnie czysta metoda. WARNING!!!
			WierszSumm[size_t(pom2)]++;//To niespecjalnie czysta metoda. WARNING!!!
			arra[size_t(pom1)][size_t(pom2)]++;//To niespecjalnie czysta metoda. WARNING!!!
			}
		}
	source1->close(Ind1);	
	source2->close(Ind2);
   
	//PETLA	MIN/MAX, Hi^2
	if(Pairs==0) //Nie ma wspowystepowania np z powodu missing values
		goto ERROR;

	ymin=DBL_MAX;
	ymax=0;
	size_t licz=0,minp=0,maxp=0;
	
	for(i=0;i<NNHeight;i++)
		if(KolumnaSumm[i]!=0)
			rows++;
	
	for(j=0;j<MMWidth;j++)
		if(WierszSumm[j]!=0)
			columns++;

	if(rows<=1 || columns<=1) //Jesli któras ze zmiennych ma jedna klase to nie da sie ustalic zwiazku
			goto ERROR;

	for(i=0;i<NNHeight;i++)
		for(j=0;j<MMWidth;j++)
		{
		double pom=arra[i][j];//Aktualna wartosc 
		
		//if(pom>0) licz++;

		if(pom>ymax) 
			{
			ymax=pom;
			maxp=i;
			}

		if(pom<ymin) 
			{
			ymin=pom;
			minp=i;
			}

		//SKLADOWE Chi^2
		//Liczebnosc oczekiwana
		double fe=(double(KolumnaSumm[i])*WierszSumm[j])/double(Pairs);
		//Powiekrzenie sumy, tam gdzie nie jest to puste skrzyzowanie
		if(fe>0)
			Hi+=sqr(pom-fe)/fe;
		
		//SKLADOWE ENTROPI
		double qij=pom/double(Pairs);
		//Powiekrzenie sumy, tam gdzie nie jest to puste skrzyzowanie
		if(qij>0)
			Entropy+=qij*log(qij);

		//SKLADOWE Tau_b - ilosc bledów przy znajomosci cechy A
		double SumaWiersza=KolumnaSumm[i];
		if(SumaWiersza>0) //W niepustym wierszu
			Za+=pom*((SumaWiersza-pom)/SumaWiersza);

		//SKLADOWE Tau_a - ilosc bledów przy znajomosci cechy B
		double SumaKolumn=WierszSumm[j];
		if(SumaKolumn>0) //W niepustym wierszu
			Zb+=pom*((SumaKolumn-pom)/SumaKolumn);
		}

	}//Koniec obliczen skladowych
	
	if(table[iHi]!=NULL)
		table[iHi]->change_val(Hi);

	if(table[iHi+1]!=NULL)
		table[iHi+1]->change_val(Hi/(double(Pairs)*sqrt(double((rows-1)*(columns-1)))));

	if(table[iHi+2]!=NULL)
		table[iHi+2]->change_val(Hi/(double(Pairs)*min(rows-1,columns-1)));
	
	if(table[iHi+3]!=NULL)//Level of freedom
		table[iHi+3]->change_val((rows-1)*(columns-1));

	if(table[iHi+4]!=NULL)//Entropy
		{
		Entropy=-Entropy; //Z sumowania wychodzi ujemna bo ln z ulamków
		table[iHi+4]->change_val(Entropy);
		}

	if(table[iHi+5]!=NULL)//"Normalised Entropy"
		{
		if(Entropy<0)
			Entropy=-Entropy; //Z sumowania wychodzi ujemna bo ln z ulamków
		/*
		double q=1/double(rows*columns);//Ilosc mozliwych slupków sprawiedliwie obdzielona
		assert(Pairs>0 && q>0);
		double MaxS=-(double(rows*columns)*(q*log(q)));
		*/
		table[iHi+5]->change_val(Entropy/log(double(rows)*columns));//Dzielone przez maksymalna mozliwa dla tej liczby slupków
		}
	
	if(table[iHi+6]!=NULL)
		{
		double Na=0;
		for(unsigned j=0;j<MMWidth;j++)
			if(WierszSumm[j]!=0)
				{
				double pom=WierszSumm[j];
				Na+=pom*((Pairs-pom)/Pairs);
				}
		assert(Na>0);
		table[iHi+6]->change_val( (Na-Za)/Na );
		}

	if(table[iHi+7]!=NULL)
		{
		double Nb=0;
		for(unsigned i=0;i<NNHeight;i++)
			if(KolumnaSumm[i]!=0)
				{
				double pom=KolumnaSumm[i];
				Nb+=pom*((Pairs-pom)/Pairs);
				}
		assert(Nb>0);
		table[iHi+7]->change_val( (Nb-Zb)/Nb );
		}
	
	return 1;
ERROR:
	if(table[iHi]!=NULL)
		table[iHi]->change_val(table[iHi]->get_missing());
	if(table[iHi+1]!=NULL)
		table[iHi+1]->change_val(table[iHi+1]->get_missing());
	if(table[iHi+2]!=NULL)
		table[iHi+2]->change_val(table[iHi+2]->get_missing());
	if(table[iHi+3]!=NULL)
		table[iHi+3]->change_val(table[iHi+3]->get_missing());
	if(table[iHi+4]!=NULL)
		table[iHi+4]->change_val(table[iHi+4]->get_missing());
	if(table[iHi+5]!=NULL)
		table[iHi+5]->change_val(table[iHi+5]->get_missing());
	if(table[iHi+6]!=NULL)
		table[iHi+6]->change_val(table[iHi+7]->get_missing());
	if(table[iHi+7]!=NULL)
		table[iHi+7]->change_val(table[iHi+7]->get_missing());
	arra.dispose();
	my_geometry=NULL;
	ymin=ymax=0;
	return 1;
}

public:
virtual size_t number_of_subseries()
	{
	return co_statistics_source::number_of_subseries()+
			8;//Ma osiem podzrodel
	}

void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
{
	co_statistics_source::all_subseries_required();
	Chi2(); //HiKwadrat
	LevelOfFreedom();//Ilosc stopni swobody ukladu
	T2Czupurow(); //T Czupurowa  
	V2Cramer(); //V^2 Cramera - miernik zale¿noscia dla zredukowanej (min) liczby stopni swobody
	Entropy();
	NormEntropy();
	Tau_b_Goodman_Kruskal();//Tau_b zaleznosc B od A wg Goodman'a i Kruskal'a
	Tau_a_Goodman_Kruskal();//Tau_a zaleznosc A od B wg Goodman'a i Kruskal'a
}

//Acces to "childrens"
scalar_source<double>*      Chi2(const char* format="Chi^2(%s,%s)")	
{
	iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi,format);
}

scalar_source<double>*      T2Czupurow(const char* format="T^2(%s,%s)")	
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+1,format);
}


scalar_source<double>*      V2Cramer(const char* format="V^2(%s,%s)")	
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+2,format);
}

scalar_source<double>*      LevelOfFreedom(const char* format="Loff(%s,%s)")//Ilosc stopni swobody ukladu
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+3,format);
}

scalar_source<double>*      Entropy(const char* format="S(%s,%s)")	
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+4,format);
}

scalar_source<double>*      NormEntropy(const char* format="nS(%s,%s)")	
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+5,format);
}

scalar_source<double>*      Tau_b_Goodman_Kruskal(const char* format="Tau_b(%s,%s)")	
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+6,format);
}

scalar_source<double>*      Tau_a_Goodman_Kruskal(const char* format="Tau_a(%s,%s)")	
{
	if(iHi==-1)
		iHi=co_statistics_source::number_of_subseries();
	return GetMonoSource(iHi+7,format);
}

//Construction
	coincidention_source(data_source_base* ini1=NULL,
						 data_source_base* ini2=NULL,
						size_t NumberOfClass1=-1,//-1 oznacza tryb calkowitoliczbowy
						size_t NumberOfClass2=-1,//-1 oznacza tryb calkowitoliczbowy
						sources_menager_base* MyMenager=NULL,
						size_t table_size=1/*ZAPAS*/,
						const char* format="COINCIDENT(%s,%s)"):
	N(NumberOfClass1),
	M(NumberOfClass2),
	iHi(-1),
	co_statistics_source(ini1,ini2,
						MyMenager,
						8+table_size,//4 wlasne + z klas potomych
						format) 
	{}
	
	~coincidention_source(){}

// Accession Methods
void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
{
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();	
	min=ymin;max=ymax;
}

size_t get_size()
//ile elementów
{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	if(!my_geometry)
			return 0;
	return my_geometry->get_size();//Prawdziwy rozmiar tablicy koincydencji
}	

//Zwraca wskaznik do obowiazujacej geometri danych. NULL oznacza dane nie-zgeometryzowane	
geometry_base* getgeometry()
	{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	return my_geometry.get_ptr_val(); 
	}		  

iteratorh  reset()
//Umozliwia czytanie od poczatku
//tablicy lub wycinka
	{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	if(my_geometry)
		return my_geometry->make_global_iterator();
		else
		return NULL;
	}

void close(iteratorh& p)
//Wymuszony koniec iteracji
	{ 
	if(my_geometry)
		my_geometry->destroy_iterator(p);
	}

double get(iteratorh& p)
//Daje nastepna z NNHeight*MMWidth liczb!!! 
	{
	double ret=0;
	assert(p!=NULL);
	size_t pom=my_geometry->get_next(p);

	if(pom!=ULONG_MAX)
		ret=_get(pom);
		else
		ret=miss;
	
	return ret;
	}

double get(size_t index)
//Przetwarza index uzyskany z geometri na jedna z NNHeight*MMWidth liczb
	{ 
#ifdef CAREFULLY_GET //Raczej niepotrzebne bo robi to juz i get_geometry() i bounds() i get_size();
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy			
#endif
	assert(index<get_size());
	return _get( index );
	}	

};


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