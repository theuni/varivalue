// Copyright (c) 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __JSON_H__
#define __JSON_H__

#include <variant>
#include <cstddef>
#include <vector>
#include <string>
#include <map>

class VariValue;
using num_t = std::variant<int64_t, uint64_t, double>;
using array_t = std::vector<VariValue>;
using object_t = std::map<std::string, VariValue>;
using json_t = std::variant<std::monostate, object_t, array_t, std::string, num_t, bool>;

class VariValue {
public:
    enum VType { VNULL, VOBJ, VARR, VSTR, VNUM, VBOOL, };

    VariValue();
    VariValue(VType initialType);

    VariValue(uint64_t val);
    VariValue(int64_t val);
    VariValue(bool val);
    VariValue(int val);
    VariValue(double val);
    VariValue(std::string val);
    void clear();

    bool setNull();
    bool setBool(bool val);
    bool setInt(uint64_t val);
    bool setInt(int64_t val);
    bool setInt(int val);
    bool setFloat(double val);
    bool setStr(std::string val);
    bool setArray();
    bool setObject();

    enum VType getType() const;
    std::string getValStr() const;
    bool empty() const;

    size_t size() const;

    bool getBool() const;
    void getObjMap(std::map<std::string,VariValue>& kv) const;
    bool checkObject(const std::map<std::string,VariValue::VType>& memberTypes) const;
    const VariValue& operator[](const std::string& key) const;
    const VariValue& operator[](size_t index) const;
    bool exists(const std::string& key) const;

    bool isNull() const;
    bool isTrue() const;
    bool isFalse() const;
    bool isBool() const;
    bool isStr() const;
    bool isNum() const;
    bool isArray() const;
    bool isObject() const;

    bool pushKV(std::string key, std::string val);
    bool pushKV(std::string key, const char *val_);
    bool pushKV(std::string key, int64_t val_);
    bool pushKV(std::string key, uint64_t val_);
    bool pushKV(std::string key, bool val_);
    bool pushKV(std::string key, int val_);
    bool pushKV(std::string key, double val_);
    bool pushKVs(VariValue obj);


    bool push_back(VariValue val);
    bool push_back(std::string val_);
    bool push_back(const char *val_);
    bool push_back(uint64_t val_);
    bool push_back(int64_t val_);
    bool push_back(bool val_);
    bool push_back(int val_);
    bool push_back(double val_);
    bool push_backV(std::vector<VariValue> vec);

    // Strict type-specific getters, these throw std::runtime_error if the
    // value is of unexpected type
    std::vector<std::string> getKeys() const;
    std::vector<VariValue> getValues() const;
    bool get_bool() const;
    const std::string& get_str() const;
    int get_int() const;
    int64_t get_int64() const;
    double get_real() const;
    const VariValue& get_obj() const;
    const VariValue& get_array() const;

    enum VType type() const;
    friend const VariValue& find_value( const VariValue& obj, const std::string& name);

private:
    json_t m_value;
};
const VariValue& find_value( const VariValue& obj, const std::string& name);
#endif // __JSON_H__
