// Copyright 2014 BitPay Inc.
// Copyright 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <iomanip>
#include <stdio.h>
#include "varivalue.h"
#include "univalue_escapes.h"

static std::string json_escape(const std::string& inS)
{
    std::string outS;
    outS.reserve(inS.size() * 2);

    for (unsigned int i = 0; i < inS.size(); i++) {
        unsigned char ch = inS[i];
        const char *escStr = escapes[ch];

        if (escStr)
            outS += escStr;
        else
            outS += ch;
    }

    return outS;
}

static void indentStr(unsigned int prettyIndent, unsigned int indentLevel, std::string& s)
{
    s.append(prettyIndent * indentLevel, ' ');
}

void writeArray(const array_t& arr, std::string& s, unsigned int prettyIndent, unsigned int indentLevel)
{
    s += "[";
    if (prettyIndent)
        s += "\n";

    for (unsigned int i = 0; i < arr.size(); i++) {
        if (prettyIndent)
            indentStr(prettyIndent, indentLevel, s);
        s += arr[i].write(prettyIndent, indentLevel + 1);
        if (i != (arr.size() - 1)) {
            s += ",";
        }
        if (prettyIndent)
            s += "\n";
    }

    if (prettyIndent)
        indentStr(prettyIndent, indentLevel - 1, s);
    s += "]";
}

void writeObject(const object_t& obj, std::string& s, unsigned int prettyIndent, unsigned int indentLevel)
{
    s += "{";
    if (prettyIndent)
        s += "\n";

    auto last = obj.rend()++.base();
    
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (prettyIndent)
            indentStr(prettyIndent, indentLevel, s);
        s += "\"" + json_escape(it->first) + "\":";
        if (prettyIndent)
            s += " ";
        s += it->second.write(prettyIndent, indentLevel + 1);
        if (it  != last)
            s += ",";
        if (prettyIndent)
            s += "\n";
    }

    if (prettyIndent)
        indentStr(prettyIndent, indentLevel - 1, s);
    s += "}";
}

void writeString(const std::string& str, std::string& s)
{
    s += "\"" + json_escape(str) + "\"";
}

void writeNum(num_t num, std::string& s)
{
    s += num.getValStr();
}

void writeBool(bool val, std::string& s)
{
    s += (val ? "true" : "false");
}

void writeNull(std::string& s)
{
    s += "null";
}
