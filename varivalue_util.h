// Copyright (c) 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __VARIVALUE_UTIL_H__
#define __VARIVALUE_UTIL_H__

#include <string>

enum jtokentype {
    JTOK_ERR        = -1,
    JTOK_NONE       = 0,                           // eof
    JTOK_OBJ_OPEN,
    JTOK_OBJ_CLOSE,
    JTOK_ARR_OPEN,
    JTOK_ARR_CLOSE,
    JTOK_COLON,
    JTOK_COMMA,
    JTOK_KW_NULL,
    JTOK_KW_TRUE,
    JTOK_KW_FALSE,
    JTOK_NUMBER,
    JTOK_STRING,
};

constexpr bool jsonTokenIsValue(jtokentype jtt)
{
    return jtt == JTOK_KW_NULL || jtt == JTOK_KW_TRUE || jtt == JTOK_KW_FALSE || jtt == JTOK_NUMBER || jtt == JTOK_STRING;
}

constexpr bool json_isspace(int ch)
{
    return ch == 0x20 || ch == 0x09 || ch == 0x0a || ch == 0x0d;
}

jtokentype getJsonToken(std::string& tokenVal, unsigned int& consumed, const char *raw, const char *end);

bool validNumStr(const std::string& s);

#endif // __VARIVALUE_UTIL_H__
