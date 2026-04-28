/** @file
 * @brief  A control value/field template with a given length and uniq value.
 * @date 2026-04-28 (last modification)
 */
#ifndef MEMORY_GUARD_H
#define MEMORY_GUARD_H

#include <cassert>

namespace merry_tools::memory {

    /** @brief A simple control field template with a given length and value
     *  @details The default value is given in the constructor, and shifted to a single hexadecimal digit in the
     *           destructor. The only functions check if the value is still valid. Checking whether it is the shifted
     *           value or something completely different allows you to distinguish whether the object containing
     *           the field has simply been destroyed or erased.
     *           Typically, a block of memory freed by delete is modified in the first few leading bytes,
     *           but the rest of the data is left intact.
     *  @tparam UnsignedType - internal field type. Expected any unsigned int types
     *  @tparam DEF_VALUE - default value for internal field. Should be unique as much as possible.
     *                      Do not use 0 , 0xDEADBEEF or any similar, often used in debugging.
     *
     *  @note Use with MEMORY_GUARD macro. This ensures that all testing burden
     *        will disappear when compiling with _NDEBUG or when defining NO_MEMORY_GUARDS
     *        before including this header. But You have to be informed, that this macro works only
     *        for one adjacent inclusion of "guard.h"!
     */
    template<typename UnsignedType,UnsignedType DEF_VALUE,unsigned DESTR_SHIFT=4>
    class [[maybe_unused]] guard {

        UnsignedType value=DEF_VALUE; //!< If everything is OK, this field has the value `DEF_VALUE`
                                 //!< for the entire lifetime of the containing object.
    public:
        guard()=default; //!< @brief Only available default constructor.

        /// @brief Digit shift in the destructor.
        ~guard() {                                                                             assert(is_constructed());
            value>>=DESTR_SHIFT;                                                                 // PUT BREAKPOINT HERE!
        }

        [[maybe_unused]] [[nodiscard]] /// @brief It checks for construction and not destruction or accidental overwrite.
        bool is_constructed() const {
            // ReSharper disable once CppTooWideScope
            bool flag=(value==DEF_VALUE);                                                        // indeed for a debug!
            if(flag) {
                return true;                                                                     // PUT BREAKPOINT HERE!
            } else {
                return false;                                                                    // PUT BREAKPOINT HERE!
            }
        }

        [[maybe_unused]] [[nodiscard]] /// @brief It checks for destruction but not destroying of the data.
        bool is_destructed() const {
            bool flag=(value==(DEF_VALUE>>DESTR_SHIFT));
            return flag;                                                                         // PUT BREAKPOINT HERE!
        }
    }; //Guard<>

} //namespace merry_tools::memory

#ifdef NO_MEMORY_GUARDS
/// Explicitly disable MEMORY_GUARDs for this file.
#warning "Header for memory guards are used, but disabled by NO_MEMORY_GUARDS definition."
#endif


#ifdef _NDEBUG //Jeśli jest _NDEBUG to musi byc NO_MEMORY_GUARDS
#   ifndef NO_MEMORY_GUARDS
/// It prevents MEMORY_GUARD macro from generating memory guard markers.
/// @note This macro works only for one adjacent inclusion of "guard.h"!
#       define NO_MEMORY_GUARDS
#   endif
#endif

#ifdef NO_MEMORY_GUARDS
/// _NDEBUG safe memory guard field placing.

// It may have already been defined differently and the preprocessor/compiler will argue!
// PL: Mogło byc już wcześniej zdefiniowane inaczej i preprocesor/kompilator będzie się kłócić!
#   ifdef MEMORY_GUARD
#       undef MEMORY_GUARD
#   endif

#   define MEMORY_GUARD(type,value) bool valid_memory(const void* ptr) const { return ptr!=nullptr; }

#else

// It may have already been defined differently and the preprocessor/compiler will argue!
// PL: Mogło byc już wcześniej zdefiniowane inaczej i preprocesor/kompilator będzie się kłócić!
#   ifdef MEMORY_GUARD
#       undef MEMORY_GUARD
#   endif

/// _NDEBUG safe memory guard field placing.
/// @note Written in a lengthy manner on purpose so that there would be somewhere to insert brake-points!
#   define MEMORY_GUARD(type,value) ::merry_tools::memory::guard<type,value> _debug_memory_marker; \
                            bool valid_memory(const void* ptr=(void*)1) const      \
                                    {                                              \
                                      if(ptr!=nullptr) {                           \
                                        if(_debug_memory_marker.is_constructed())  \
                                                       return true;                \
                                      }                                            \
                                      /*WB_TRAP("invalid memory of the object!");*/\
                                      return false;                                \
                                    }

#endif

// This macro works only for one adjacent inclusion of "guard.h"!
#   ifdef NO_MEMORY_GUARDS
#   undef NO_MEMORY_GUARDS
#   endif

/* ****************************************************************** */
/*               MERRY TOOLS by WOJCIECH BORKOWSKI                    */
/*    See: https://github.com/borkowsk                                */
/*     or: https://www.researchgate.net/profile/Wojciech-Borkowski    */
/*    ☕☕☕: https://buycoffee.to/adalbertus                            */
/* ****************************************************************** */
#endif //MEMORY_GUARD_H
