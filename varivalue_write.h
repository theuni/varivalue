// Copyright (c) 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __VARIVALUE_WRITE_H__
#define __VARIVALUE_WRITE_H__

void writeArray(const array_t& arr, std::string& s, unsigned int prettyIndent, unsigned int indentLevel);
void writeObject(const object_t& obj, std::string& s, unsigned int prettyIndent, unsigned int indentLevel);
void writeString(const std::string& str, std::string& s);
void writeNum(num_t num, std::string& s);
void writeBool(bool val, std::string& s);
void writeNull(std::string& s);

#endif // __VARIVALUE_WRITE_H__
