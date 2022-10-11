/** @file virtualConstructor.h
*  Definition of virtual constructors template & macros..
*====================================================================================================================
*  \details Obiekty wirtualnych konstruktorów rejestrowane są na standardowej mapie z kluczem typu std::string.
*           File created by borkowsk on 12.08.22.
*  \copyright Now, this is a part of GuestXR project.
*  \date 2022-10-05 (last modification)
*/

#ifndef DYNEGO_VIRTUAL_CONSTRUCTOR_H
#define DYNEGO_VIRTUAL_CONSTRUCTOR_H

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY
/**
 * @defgroup VirtualConstruction Bazowa obsługa zapisu i odczytu obiektów heterogenicznych
 * \brief Narzędzia do definiowania wirtualnych konstruktorów
 */
/// @{


/// \brief   Wspólna klasa bazowa dla wszelkich obiektów z VMT.
/// \details Wymusza wirtualność destruktora, co właściwie powinno być zawsze użyteczne dla takich klas.
    class any_virtual_object {
    public:
        ///\brief  Konstruktor bezparametrowy musi być.
        any_virtual_object() {}

        /// \brief Wymuszenie wirtualności destruktorów.
        virtual ~any_virtual_object() {}
    };

/// \brief Prosty interface tworzenia heterogenicznych typów nadających się na składowe w kontenerze.
/// \details
///     Każdy obiekt tego typu musi być rejestrowany w konstruktorze
///     za pomocą funkcji registerThis().
    class InterfaceOfVirtualConstructor {
    protected:
        /// \brief Nazwa klasy.
        /// \details
        ///     Nieco redundantne w stosunku do implementacji registerThis() i getVirtualConstructorOf(),
        ///     ale nie szkodzi, bo to daje szanse na sprawdzenie poprawności działania rejestratorów.
        const char *className;

    public:
        /// \brief Bezparametrowy konstruktor.
        InterfaceOfVirtualConstructor() : className("***") {}

        /// \brief Dostęp do nazwy typu.
        const char *getTypeName() const { return className; }

        /// \brief   Tworzenie obiektu jakiegoś typu.
        /// \details
        ///      Jest to ten typ, do którego konstrukcji jest przeznaczony obiekt dziedziczący po tej klasie abstrakcyjnej.
        virtual any_virtual_object *newEmpty() const = 0;

        /// \brief Obsługa rejestracji obiektów klas potomnych.
        static void registerThis(InterfaceOfVirtualConstructor *ptr);

        /// \brief Obsługa odnajdywania obiektów klas potomnych wg. nazw.
        static InterfaceOfVirtualConstructor *getVirtualConstructorOf(const char *name);
    };

/// \brief Szablon obiektów służących tworzeniu obiektów obsługujących zadane typy heterogenicznej hierarchii.
/// \details
///     Dla każdego typu, którego obiekty mają być dynamicznie tworzone konieczny jest jeden obiekt
///     odpowiedniego typu kreowanego z tego szablonu.
///     W trakcie konstrukcji obiekt taki jest rejestrowany pod zadaną nazwą za pomocą funkcji
///     'InterfaceOfVirtualConstructor::getVirtualConstructorOf()',
///     i dzięki temu można generować domyślne obiekty danego typu z hierarchii.
    template<class TheType>
    class VirtualConstructorOf : public InterfaceOfVirtualConstructor {
    public:
        /// \brief Konstruktor wymagający nazwy dla klasy.
        /// \note  Nazwa tekstowa nie musi mieć nic wspólnego z nazwą w kodzie, choć to zazwyczaj pomaga.
        VirtualConstructorOf(const char *iClassName) {
            className = iClassName;
            registerThis(this);
        }

        /// \brief Tworzenie obiektu, którego typem opiekuje się ten typ "wirtualnego konstruktora".
        TheType *newEmpty() const override { return new TheType(); }


    };

} //end of namespace wbrtm

/// \brief Makro do deklarowania obowiązkowych składowych dla wirtualnej konstrukcji.
/// \details
///         Do użycia tylko we wnętrzu klasy, która ma posiadać "wirtualny konstruktor".
/// \param type - to C++ nazwa używającego (bez cudzysłowu!)
#define DECLARE_VIRTUAL_CONSTRUCTOR( type  ) \
            private:  \
            friend class VirtualConstructorOf<type>; \
            static const VirtualConstructorOf<type> virtualConstructor; \
            public: \
            const InterfaceOfVirtualConstructor* getVirtualConstructor() const override \
            { return  & type::virtualConstructor;}

/// \brief Makro do definiowania statycznego obiektu wirtualnego konstruktora dla konkretnego typu.
/// \param type to nazwa typu C++
/// \param shortTypeName to łańcuch tekstowy pod jakim ten typ jest pamiętany.
///                      Może być nazwa skrócona, albo cokolwiek, byle bez spacji/tabulacji.
#define DEFINE_VIRTUAL_CONSTRUCTOR( type , shortTypeName  ) \
            const wbrtm::VirtualConstructorOf<type> type::virtualConstructor{ (const char*)( shortTypeName ) };

//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//       Uzupełnienie do pakietu "Rozumek sieciowy" wersja 2022-10
//       (c) ISS UW 2010-2013-2016 Wojciech Borkowski, modyfikacja 2021/2022.01-08
//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @}
#endif //DYNEGO_VIRTUAL_CONSTRUCTOR_H


