/**
* @file     typenames.hpp
* \brief    How do I convert T to a string that says what T is?
*           ---------------------------------------------------
* \details  While playing with templates in c++ I encountered a problem converting typename T to string.
*	        There is no built-in mechanism for this!
*           typeid(T)::name() can give some info, but the standard does not mandate this string to be human-readable; 
*           just that it has to be distinct for each type. (E.x. Microsoft Visual C++ uses human-readable strings; GCC does not.)
*      	    You can build your own system though.
*
* 	    usage:
* \code{C++}
*	         const char* name = TypeName<MyType>::Get();
* \endcode
*
*           see also:
*
*	     - https://stackoverflow.com/questions/4484982/how-to-convert-typename-t-to-string-in-c
*	     - https://en.cppreference.com/w/cpp/types/type_info/name
*
* \date    2022-10-12 (last modification)
*/
#warning  "This code is OBSOLETE and not tested in C++11 standard"

#ifndef _DEFINED_TypeName
#define _DEFINED_TypeName

#include <typeinfo>

/// \brief default implementation.
/// \details
/// 	You need a specialization for each type of those you want to support
/// 	and don't like the string returned by typeid.
template <typename T>
struct TypeName
{
    static const char* Get()
    {
        return typeid(T).name();
    }
};

/// \brief specialization for type 'int'.
template <>
struct TypeName<int>
{
    static const char* Get()
    {
        return "int";
    }
};

/// \brief Macro for enabling specialised TypeName
/// \details Specialization for each type of those you want to support
///          and don't like the string returned by typeid.
///
/// Usage:
/// \code{C++}
/// 	ENABLE_TYPENAME(A) 	// specialization for type 'A'.
/// \endcode
#define ENABLE_TYPENAME(A) template<> struct TypeName<A> { static const char *Get() { return #A; }};

// Default specialisation for simple types
ENABLE_TYPENAME(bool)         ///< \brief specialization for type 'bool'.
ENABLE_TYPENAME(char)         ///< \brief specialization for type 'char'.
ENABLE_TYPENAME(float)        ///< \brief specialization for type 'float'.
ENABLE_TYPENAME(double)       ///< \brief specialization for type 'double'.
ENABLE_TYPENAME(long) 	      ///< \brief specialization for type 'long'.

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
#endif // _DEFINED_TypeName


