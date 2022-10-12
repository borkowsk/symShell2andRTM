/// \file optParam.hpp
/// \brief KLASY potrzebne do obsługi parametrów wywołania programu
/// \details
///     Obsługiwane są typy standardowe numeryczne oraz
///      inne typy mające implementacje << >> na strumienie i
///     operator porównania <=
///     Obsługiwany jest też typ "const char*" , ale na poziomie wskaźników, czyli
///     wskaźnik do zawartości linii komend może zostać przypisany na zmienną
///     typu "const char*"
///     NIESTETY nie można używać typu 'string' bo nie ma on obsługi strumieni!
/// \note Chyba żeby już była? TODO CHECK IT!
/// \author borkowsk
/// \date  2022-10-12 (last modification)

#ifndef OPTIONAL_PARAMETERS_HPP
#define  OPTIONAL_PARAMETERS_HPP

#ifndef OLD_FASHION_CPP
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#else
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstrea.h>
#endif

#include "wb_ptr.hpp"

using namespace std;

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// \brief   Baza klas dla wszystkich typów parametrów wywołania.
/// \details Definiuje interfejs parametru i funkcje statyczne do obsługi całej listy możliwych parametrów
class OptionalParameterBase
{
public:
    /// \brief  Funkcja interfejsu sprawdzania parametrów obiektami hierarchii OptionalParametr
virtual
    int CheckStr(const char* argv,char sep='=')
    { return 0;} //0: Nie moja sprawa; 1: moje i dobre; -1: Moje, ale zly format

    /// \brief  Funkcja interfejsu  drukowania linii HELP'u
virtual
    void HelpPrn(ostream& o) {o<<"UPSSss..."<<endl;}

    /// \brief  Funkcja interfejsu pobierania nazwy parametru
virtual
    const char* getName() { return "#"; }

    /// \brief  funkcja interfejsu pobierająca wartość parametru
virtual
    const char* getVal(char* buff=NULL)  { return "...";}

/// \brief   Metoda statyczna do obsługi listy parametrów.
/// \details Musi przeanalizować parametry wywołania. Trzeba wywołać ją w main() i to gdzieś w miarę wcześnie.
static
    int parse_options(const int argc,
                      const char* argv[],
                      OptionalParameterBase* Parameters[],
                      int Len
                      );

/// \brief Metoda statyczna zapisu parametrów do pliku jako raport
static
    void report(ostream& Out,
                OptionalParameterBase* Parameters[],
                int  Len,
                const char* SeparatorTab="\t=\t",
                const char* SeparatorLine="\n"
                        );

/// \brief Metoda statyczna zapisu parametrów do pliku jako tabela
static
    void table(ostream& Out,
               OptionalParameterBase* Parameters[],
               int  Len,
               const char* SeparatorTab="\t",
               const char* Head="$Parameters",
               const char* ValHd="values"
                       );
};

// Klasy potomne
// /////////////////

/// \brief  Klasa do rozdzielania parametrów w tablicy i w helpie
class ParameterLabel:public OptionalParameterBase
{
protected:
	wb_pchar	Lead;
	wb_pchar	Info;

    /// \brief  Implementacja metody drukowania linii HELP'u wymaganej przez klasę bazową
	void HelpPrn(ostream& o)
	{o<<Lead.get()<<' '<<Info.get()<<endl;}

  public:
    /// \brief  Konstruktor
	ParameterLabel(const char* iInfo,const char* iLead="\n#"):
			Lead(clone_str(iLead)),Info(clone_str(iInfo))
            {}
    /// \brief  Destruktor
	~ParameterLabel() = default;
};

/// \brief Szablon klasy opcjonalnego parametru.
template<class T>
class OptionalParameter:public OptionalParameterBase
{
  protected:
	wb_pchar	Name;   ///< Identyfikator (mnemonik) parametru
	wb_pchar	Info;   ///< Tekst informacyjny dla użytkownika
        T& 		Value;  ///< Referencja to zmiennej, która będzie modyfikowana
        T 		LBound; ///< Najmniejsza dozwolona wartość
        T 		HBound; ///< Największa dozwolona wartość. Dla stringów/tekstów może mieć inne znaczenie

    /// \brief Drukowanie helpu do tego parametru \details Metoda wymagane przez klasę bazową
	void HelpPrn(ostream& o)
	{ o<<Name.get()<<": "<<Info.get()<<" Range: <"<<LBound<<','<<HBound<<">; Default: "<<Value<<endl; }

  public:
    /// \brief  Konstruktor
	OptionalParameter(T& iV,const T& iLB,const T& iHB,const char* iName,const char* iInfo):
		  Value(iV),LBound(iLB),HBound(iHB),Name(clone_str(iName)),Info(clone_str(iInfo)){}

    /// \brief  Destruktor
	~OptionalParameter(){}

    /// \brief  Metoda przetworzenia konkretnego parametru
	int CheckStr(const char* argv,char sep='=');

    /// \brief Metoda konwersji.
    /// \note Muszą być dostarczone różne implementacje tej metody w zależności od typu T
	virtual T convert(const char* str);

    /// \brief   Sprawdzenie domyślne wartości.
    /// \details Sprawdza czy wartość w zakresie, ale może być zmienione dla danego typu T
    ///          (np. wb_pchar czy char* wymagają zupełnie innego sprawdzenia)
	virtual bool check(const T& _val);
};

/// \brief  Szablon klasy opcjonalnego parametru będącego WYLICZENIEM
template<class T>
class OptEnumParametr:public OptionalParameter<T>
{
 protected:
	unsigned     NofEn;    ///< Ile nazw dla typu zdefiniowano
	const char** EnNames;  ///< Jakie to nazwy
	const T*     EnVals;   ///< Domyślna wartość ???

	void HelpPrn(ostream& o);     ///< Podstawienie metody dostarczonej przez klasę bazową (TODO TEST?)

 public:
      /// \brief  Konstruktor
	  OptEnumParametr(T& iV,const T& iLB,const T& iHB,
	                  const char* iName,const char* iInfo,
					  unsigned NofNames,const char** EnumNames,const T* EnumValues=NULL):
					  OptionalParameter<T>(iV,iLB,iHB,iName,iInfo),
					  NofEn(NofNames),EnNames(EnumNames),EnVals(EnumValues)
                      {}

      /// \brief  Destruktor
	  ~OptEnumParametr() = default;

    /// \brief Metoda konwersji. \details Musi być implementacja zależna od typu EnNames i EnVals
	T convert(const char* str);
};

// /////////////////////////////
// IMPLEMENTACJE on line
// /////////////////////////////

//  Funkcje sprawdzania poprawności
// ////////////////////////////////////

/// \details  Implementacja ogólna sprawdzania poprawności
/// \return   Czy wartość mieści się w zadanym zakresie.
template<class T>
bool OptionalParameter<T>::check(const T& _val)
{
	return (LBound<=_val) && (_val<=HBound);
}

/// \details  Implementacja sprawdzania poprawności dla typu 'string'
/// \return   Czy zawartość jest zaalokowana i nie jest pustym łańcuchem.
template<> inline
bool OptionalParameter<string>::check(const string& val)
{
	return val.c_str()!=NULL &&  *val.c_str()!='\0';
}

/// \details Implementacja sprawdzania poprawności dla typu 'wb_pchar'
/// \return   Czy zawartość jest zaalokowana i nie jest pustym łańcuchem.
template<> inline
bool OptionalParameter<wb_pchar>::check(const wb_pchar& val)
{
	return val.get()!=NULL && *val.get()!='\0';
}

/// \details Implementacja sprawdzania poprawności dla typu 'char*'
/// \return  Czy zawartość nie jest NULL i nie jest pustym łańcuchem.
template<> inline
bool OptionalParameter<char*>::check(char* const& val)
{
	return (val!=NULL) && (*val!='\0');
}

/// \details Implementacja sprawdzania poprawności dla typu 'const char*'
/// \return  Czy zawartość nie jest NULL i nie jest pustym łańcuchem.
template<> inline
bool OptionalParameter<const char*>::check(const char* const& val)
{
	return (val!=NULL) && (*val!='\0');
}


// Funkcje konwersji
// //////////////////////////////////////////////////////////////////////////////

/// \details Implementacja konwersji dla typu  'char*'. \return użyteczny łańcuch alokowany na stercie.
template<> inline
char* OptionalParameter<char*>::convert(const char* str)
{
	return clone_str(str);//Bez zwalniania pamięci, bo to przecież parametr wywołania!
}

/// \details Implementacja konwersji dla typu  'const char*'.
/// \return  Zwraca po prostu ten sam łańcuch. To przecież kawałek parametru wywołania, więc nie może się zmienić!
template<> inline
const char* OptionalParameter<const char*>::convert(const char* str)
{
    return str;
}

/// \details Implementacja konwersji dla typu 'wb_pchar'
/// \return  Zapewne kopie tego, co dostanie (czyli kawałka parametru wywołania)
template<> inline
wb_pchar OptionalParameter<wb_pchar>::convert(const char* str)
{
	return wb_pchar(str);
}

/// \details Implementacja konwersji dla typu  'string'
template<> inline
string OptionalParameter<string>::convert(const char* str)
{
	return string(str);
}

/// \details Implementacja konwersji dla typu  'double'
template<> inline
double OptionalParameter<double>::convert(const char* str)
{
	return atof(str);
}

/// \details Implementacja konwersji dla typu  'float'
template<> inline
float OptionalParameter<float>::convert(const char* str)
{
	return (float)atof(str);// conversion from 'double' to 'float', possible loss of data
}

/// \details Implementacja konwersji dla typu  'long'
template<> inline
long OptionalParameter<long>::convert(const char* str)
{
	return atol(str);
}

/// \details Implementacja konwersji dla typu  'long long'
template<> inline
long long OptionalParameter<long long>::convert(const char* str)
{
#if defined(_MSC_VER)
	return (long long)atof(str);
#else
	return atoll(str);
#endif
}

/// \details Implementacja konwersji dla typu  'unsigned long long'.
/// \return  Wynik działania na \p str funkcji atoll() lub atof() (w MSVC++)
template<> inline
unsigned long long OptionalParameter<unsigned long long>::convert(const char* str)
{
#if defined(_MSC_VER)
	return (unsigned long long)atof(str);
#else
	return atoll(str);
#endif
}

/// \details Implementacja konwersji dla typu  'unsigned int'
/// \return  Wynik działania na \p str funkcji atol()
template<> inline
unsigned OptionalParameter<unsigned>::convert(const char* str)
{
	return atol(str);
}

/// \details Implementacja konwersji dla typu  'int'
/// \return  Wynik działania na \p str funkcji atoi()
template<> inline
int OptionalParameter<int>::convert(const char* str)
{
	return atoi(str);
}

/// \details Implementacja konwersji dla typu  'bool'
/// \return true, jeżeli pierwszy ZNAK jest 'Y','y','T','t' albo '1'.
template<> inline
bool OptionalParameter<bool>::convert(const char* str)
{
	return toupper(*str)=='Y' || toupper(*str)=='T' || (*str)=='1';
}

/// \brief   Podstawienie konwersji dostarczonej przez klasę bazową
/// \details Szablon konwersji dla pozostałych typów. Czyli WYLICZENIOWYCH (enum)!
template<class T> inline
T OptEnumParametr<T>::convert(const char* str)
{
    if(('A'<=str[0] && str[0]<='Z')
            || ('a'<=str[0] && str[0]<='z')
            || (str[0] == '_') )//Czy jest identyfikator
    {
        for(unsigned i=0;i<NofEn;i++)
            if(std::strcmp(str,EnNames[i])==0) //Jest?
            {
                if(EnVals) return EnVals[i];
                else return T(this->LBound+i);
            }
        return T(-9999);
    }
    else //W przeciwnym razie probujemy liczbowo
    {
        int pom=atol(str);
        return T(pom);
    }
}

/// \brief   WERSJA OGÓLNA konwersji ZGŁASZAJĄCA AWARIE
/// \details Zostaje użyta jedynie przy próbie konwersji jakiegoś typu,
///          który nie ma przygotowanej implementacji funkcji 'convert'.
///          Podstawowe typy i tak są obsłużone oddzielnie,
///          więc sprawa dotyczy nietypowych enum-s i jakichś pomysłów na używanie klas użytkownika
///          Na razie nic bardziej ogólnego nie wymyśliłem.
/// \return  wartość -9999 rzutowana na dany typ. Najprawdopodobniej w tym miejscu będzie już błąd kompilacji!
template<class T> inline
T OptionalParameter<T>::convert(const char* str)
{
  /*
    istrstream Strm(str); //Gdyby dało się ogólnie
	T Val;
	Strm>>Val; //Ale wiele typów i tak nie ma, np. różne enum
  */
	return T(-9999);//Zazwyczaj -9999 nie będzie poprawną daną.
}


//*????
//typedef OptionalParameter<class T> OptPar<class T>;  ??? TODO HOW TO DECLARE THAT?

//	Główne metody
// //////////////////////////////////////////////////////////////////////////////

/// \details Zapis parametrów do pliku "raportu"
/// \param Out - strumień do zapisu
/// \param Parameters - tablica parametrów
/// \param Len - liczba parametrów w tablicy \p Parameters
/// \param SeparatorTab - separator komórek tabeli
/// \param SeparatorLine - separator wierszy tabeli
inline
void OptionalParameterBase::report(ostream& Out,OptionalParameterBase* Parameters[],
                                   int  Len,const char* SeparatorTab,const char* SeparatorLine)
{
  //Out<<Len;
  Out<<endl;
  for(int j=0;j<Len;j++)
  {
    char buff[1024];
    Out<<Parameters[j]->getName();
    Out<<SeparatorTab;
    Out<<Parameters[j]->getVal(buff);
    Out<<SeparatorLine;
  }
}

/// \brief Zapis parametrów w formie tabeli
/// \param Out - strumień do zapisu
/// \param Parameters - tablica parametrów
/// \param Len - liczba parametrów w tablicy \p Parameters
/// \param SepTab - separator komórek tabeli
/// \param Head - pierwsze pole nagłówka tabeli
/// \param ValHd - ???
inline
void OptionalParameterBase::table(ostream& Out,OptionalParameterBase* Parameters[],
                                  int  Len,const char* SepTab,const char* Head,const char* ValHd)
{
  //Out<<Len;
   Out<<Head<<SepTab;
   for(int j=0;j<Len;j++)
   {
       const char* pom=Parameters[j]->getName();
       if(pom[0]!='#') //POMIJA ETYKIETKI
         Out<<pom<<SepTab;
   }

   Out<<endl<<ValHd<<SepTab;
   for(int j=0;j<Len;j++)
   {
       const char* pom=Parameters[j]->getName();
       if(pom[0]!='#') //POMIJA ETYKIETKI
         {
             char buff[1024];
             Out<<Parameters[j]->getVal(buff)<<SepTab;
         }
   }
}

/// \brief Główna funkcja parsująca listę parametrów wywołania
/// \param argc - liczba argumentów programu ( z main() )
/// \param argv - wartości argumentów programu ( z main() )
/// \param Parameters - tabela parametrów
/// \param Len - rozmiar tabeli parametrów
/// \return 0 jeśli OK, -1 gdy jakiś "syntax error"
inline
int OptionalParameterBase::parse_options(const int argc,const char* argv[],
                                         OptionalParameterBase* Parameters[],int  Len)
{
    for(int i=1;i<argc;i++)
    {
        if( *argv[i]=='-' )
            continue; // Opcja X11 lub symshell'a, czy inne zaczynające się zwykle na '-', które chcemy obsłużyć inaczej
        if(std::strcmp(argv[i],"HELP")==0) //Bez kwalifikacji std:: może być kłopot w zakresie działania definicji friend'ów wb_pchar
        {
            cout<<endl<<"*** NAMES OF PARAMETERS:"<<endl<<flush;
            for(int j=0;j<Len;j++) Parameters[j]->HelpPrn(cout);
            cout<<"* JUST USE LIST OF SPACE SEPARATED PAIRS PARAMETER=VALUE"<<endl;
            cout<<"<<press the return/enter key>>"<<endl;
            cin.get();
            return 1;
        }
        for(int j=0;j<Len;j++)
        {
            int ret=Parameters[j]->CheckStr(argv[i]);
            if(ret==1) goto CONTINUE; //Odnaleziono
            if(ret==-1)
            {
                cerr<<"* SORRY (Press ENTER)*"<<endl;
                cin.ignore(0xffffffff,'\n');
                return -1;
            }
        }
        cerr<<"Unknown parameter "<<argv[i]<<endl;
        return -1;
CONTINUE:;
    }
    return 0;
}

/// \brief Szablon funkcji sprawdzania łańcucha parametru
/// \tparam T - jaki typ
/// \param argv - wartość parametru programu
/// \param sep - znak separatora
/// \return 1 if OK, -1 on error, 0 for ignoring
template<class T> inline
int OptionalParameter<T>::CheckStr(const char* argv,char sep/*arator*/)
{
    const char* pom=NULL;
    if((pom=std::strstr(argv,Name.get()))!=NULL)
    {
        pom+=std::strlen(Name.get());
        if(*pom!=sep)
        {
            cerr<<argv<<" is malformed parameter value for "<<Name.get()<<endl;
            return -1;
        }

        T temp=convert(++pom); //Musi istnieć taka funkcja pośrednicząca zwracająca wartość niemodyfikowalną

        if(check(temp))
        {
            Value=temp;
            cout<<"* Value "<<Name.get()<<" was changed into '"<<Value<<'\''<<endl;
            return 1;//Moja poprawna wartość
        }
        else
        {
            cerr<<"* Value "<<Name.get()<<" cannot be changed into '"<<Value<<'\''<<endl;
            cerr<<"** Proper values should be beetween "<<LBound<<" and "<<HBound<<endl;
            cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
            return -1;//Moja, ale niepoprawna wartość
        }
    }
    return 0;//Nie MOJA wartość. Szukaj dalej!
}

/// \brief  funkcji sprawdzania łańcucha parametru dla typu 'string'
/// \param argv - wartość parametru programu
/// \param sep - znak separatora
/// \return 1 if OK, -1 on error, 0 for ignoring
template<> inline
int OptionalParameter<string>::CheckStr(const char* argv,char sep/*arator*/)
{
    const char* pom=NULL;
    if((pom=std::strstr(argv,Name.get()))!=NULL) //Bez kwalifikacji std:: może być kłopot w zakresie działania definicji friend'ów wb_pchar
    {
        pom+=std::strlen(Name.get());
        if(*pom!=sep)
        {
            cerr<<argv<<" is malformed parameter value for "<<Name.get()<<endl;
            return -1;
        }

        string temp=convert(++pom);//Musi istnieć taka funkcja pośrednicząca zwracająca wartość niemodyfikowalną

        if(check(temp))
        {
            Value=temp;
            cout<<"* Value "<<Name.get()<<" was changed into '"<<Value.c_str()<<'\''<<endl;
            return 1;//Moja poprawna warto��
        }
        else
        {
            cerr<<"* Value "<<Name.get()<<" cannot be changed into \""<<Value.c_str()<<'"'<<endl;
            cerr<<"** Proper value may looks like \""<<LBound.c_str()<<"\" and \""<<HBound.c_str()<<"\""<<endl;
            cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
            return -1;//Moja, ale niepoprawna warto��
        }
    }
    return 0;//Nie MOJA sprawa
}

// NIETYPOWE METODY DRUKOWANIA HELPU DO PARAMETRÓW TEKSTOWYCH
// //////////////////////////////////////////////////////////////////////////////

/// \brief  funkcja drukowania helpu parametru dla typu 'string'
template<> inline
void OptionalParameter<string>::HelpPrn(ostream& o)
	{
		o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.c_str()<<"\" or \""<<HBound.c_str()
         <<"\"; Default: \""<<Value.c_str()<<'"'<<endl;
	}

/// \brief  funkcja drukowania helpu parametru dla typu 'wb_pchar'
template<> inline
void OptionalParameter<wb_pchar>::HelpPrn(ostream& o)
	{
		o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.get()<<"\" or \""<<HBound.get()
         <<"\"; Default: \""<<Value.get()<<'"'<<endl;
	}

/// \brief  funkcja drukowania helpu parametru dla typu 'char*'
template<> inline
void OptionalParameter<char*>::HelpPrn(ostream& o)
	{
	  o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound<<"\" or \""<<HBound<<"\"; Default: \""<<Value<<'"'<<endl;
	}

/// \brief  funkcja drukowania helpu parametru dla typu 'bool'
template<> inline
void OptionalParameter<bool>::HelpPrn(ostream& o)
	{ o<<Name.get()<<": "<<Info.get()<<"; allowed are: 0,1,Yes,No,Tak,Nie; Default: "<<Value<<endl; }

/// \brief  Szablon funkcji drukowania helpu parametru dla typu WYLICZENIOWEGO 'enum'
template<class T> inline
void OptEnumParametr<T>::HelpPrn(ostream& o)
{ //G++ chciało tu kiedyś wszędzie this-> ??? TODO - MOŻE JUŻ NIEPOTRZEBNE?
    o<<this->Name.get()<<": "<<this->Info.get()<<"; allowed are: ";
    for(unsigned i=0;i<this->NofEn;i++) //Tablica definiuje nazwy od LBound do HBound!
    {
        o<<this->EnNames[i];
        if(this->EnVals)
            o<<"="<<this->EnVals[i];
        o<<' ';
    }
    o<<"or integers range: <"<<this->LBound<<','<<this->HBound<<"> Default: "
     <<this->EnNames[this->Value-this->LBound]<<"="<<this->Value<<endl;
}

}//NAMESPACE WBRTM

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2022                         */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif
