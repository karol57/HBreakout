//
// Created by Karol on 18.08.2016.
//

#include "error_log.hpp"

//#include <freetype/fterrdef.h>
// FT_ERROR_DEFINITIONS_H

const char * ft_error_to_string(FT_Error error) noexcept {
    #undef FT_NOERRORDEF_
    #undef FT_ERRORDEF_
    #define FT_NOERRORDEF_(sym, code, str) case code: return str;
    #define FT_ERRORDEF_(sym, code, str) case code: return str;

    switch (error) {
        #include FT_ERROR_DEFINITIONS_H
        default:;
    }
    return "<unknown>";
}