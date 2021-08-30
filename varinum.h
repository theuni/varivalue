// Copyright (c) 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __VARINUM_H__
#define __VARINUM_H__

#include <cstdint>
#include <string>

class VariNum
{
public:
    VariNum() = default;
    explicit VariNum(uint64_t val);
    explicit VariNum(int64_t val);
    explicit VariNum(int val);
    explicit VariNum(double val);

    bool setInt(uint64_t val);
    bool setInt(int64_t val);
    bool setInt(int val);
    bool setFloat(double val);

    int get_int() const;
    int64_t get_int64() const;
    double get_real() const;

    const std::string& getValStr() const;
    bool setNumStr(std::string val);
private:
    std::string m_value;
};

#endif // __VARINUM_H__
