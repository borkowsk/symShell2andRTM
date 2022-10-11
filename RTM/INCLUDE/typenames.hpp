/**
* @file typenames.hpp
*
* \brief    How do I convert T to a string that says what T is?
* \details  While playing with templates in c++ I encountered a problem converting typename T to string.
*	    There is no built-in mechanism for this!
*           typeid(T)::name() can give some info, but the standard does not mandate this string to be human-readable; 
*           just that it has to be distinct for each type. (E.x. Microsoft Visual C++ uses human-readable strings; GCC does not.)
*      	  You can build your own system though. 
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
*/
#ifndef _DEFINED_TypeName
#define _DEFINED_TypeName

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

/// \brief specjalisation for type 'int'.
template <>
struct TypeName<int>
{
    static const char* Get()
    {
        return "int";
    }
};

/// \brief Macro for anabling specialised TypeName
/// \details Specialization for each type of those you want to support
///          and don't like the string returned by typeid.
///
/// Usage:
/// \code{C++}
/// 	ENABLE_TYPENAME(A) 	//< \brief specjalisation for type 'A'.
/// \endcode
#define ENABLE_TYPENAME(A) template<> struct TypeName<A> { static const char *Get() { return #A; }};

// Default specialisation for simple types
ENABLE_TYPENAME(bool)         ///< \brief specjalisation for type 'bool'.
ENABLE_TYPENAME(char)         ///< \brief specjalisation for type 'char'.
ENABLE_TYPENAME(float)        ///< \brief specjalisation for type 'float'.
ENABLE_TYPENAME(double)       ///< \brief specjalisation for type 'float'.
ENABLE_TYPENAME(long) 	       ///< \brief specjalisation for type 'float'.

#endif // _DEFINED_TypeName


