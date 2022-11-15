/**
  https://cplusplus.com/user/Anmol444/
  https://cplusplus.com/forum/general/102410/
  https://doraprojects.net/questions/1727881/how-to-use-the-pi-constant-in-c#
*/
#ifndef ANY_PI_PART
#define ANY_PI_PART
#include <ratio>
/// \brief wszystkie rodzaje u³amków pi z pe³n¹ dok³adnoœci¹
/// \details
/// Sposób u¿ycia:
/// ```
/// std::cout << dpipart<std::ratio<-1, 6>>() << std::endl;
/// ```
/// Wartoœci takie jak M_PI, M_PI_2, m_pi_4, itd.nie s¹ standardem C++, 
/// wiêc constexpr wydaje siê lepszym rozwi¹zaniem.
/// Mo¿na sformu³owaæ ró¿ne wyra¿enia const, które obliczaj¹ to samo pi.
/// Standard C++ nie wspomina wyraŸnie jak obliczyæ pi.
/// Dlatego wracam do rêcznego definiowania pi.
/// Chcia³bym podzieliæ siê poni¿szym rozwi¹zaniem.
/// \author: Jeroen Lammertink, 2018-07-31
template<typename RATIO>
constexpr double anyPIpart()
{
    long double const pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899863;
    return static_cast<double>(pi * RATIO::num / RATIO::den);
}
#endif //ANY_PI_PART

#if defined(_USE_MATH_DEFINES) && !defined(_MATH_DEFINES_DEFINED)
#define _MATH_DEFINES_DEFINED

/* Define _USE_MATH_DEFINES before including math.h to expose these macro
 * definitions for common math constants.  These are placed under an #ifdef
 * since these commonly-defined names are not part of the C/C++ standards.
 */

/* Definitions of useful mathematical constants
 * M_E        - e
 * M_LOG2E    - log2(e)
 * M_LOG10E   - log10(e)
 * M_LN2      - ln(2)
 * M_LN10     - ln(10)
 * M_PI       - pi
 * M_PI_2     - pi/2
 * M_PI_4     - pi/4
 * M_1_PI     - 1/pi
 * M_2_PI     - 2/pi
 * M_2_SQRTPI - 2/sqrt(pi)
 * M_SQRT2    - sqrt(2)
 * M_SQRT1_2  - 1/sqrt(2)
 */

#define M_E        (2.71828182845904523536)
#define M_LOG2E    (1.44269504088896340736)
#define M_LOG10E   (0.434294481903251827651)
#define M_LN2      (0.693147180559945309417)
#define M_LN10     (2.30258509299404568402)
#define M_PI       (3.14159265358979323846)
#define M_PI_2     (1.57079632679489661923)
#define M_PI_4     (0.785398163397448309616)
#define M_1_PI     (0.318309886183790671538)
#define M_2_PI     (0.636619772367581343076)
#define M_2_SQRTPI (1.12837916709551257390)
#define M_SQRT2    (1.41421356237309504880)
#define M_SQRT1_2  (0.707106781186547524401)

#endif  /* _USE_MATH_DEFINES */
