#include "varivalue.h"
#include "varivalue_write.h"
#include "varivalue_util.h"

#include <cassert>
#include <stdexcept>

const VariValue NullUniValue;

VariValue::VariValue(UniValue::VType initialType, std::string initialStr) : VariValue(initialType)
{
    std::visit(varivalue::overloaded {
        [&](std::string& str) { str = std::move(initialStr); },
        [&](VariNum& num) { num.setNumStr(std::move(initialStr)); },
        [&](const auto&)  {},
        }, m_value);
}

VariValue:: VariValue(uint64_t val)
{
    m_value = num_t{val};
}

VariValue:: VariValue(int64_t val)
{
    m_value = num_t{val};
}

VariValue:: VariValue(int val)
{
    m_value = num_t{val};
}

VariValue:: VariValue(double val)
{
    m_value = num_t{val};
}

VariValue:: VariValue(std::string val)
{
    m_value = std::move(val);
}

VariValue:: VariValue(const char* val)
{
    if (val) {
        m_value = std::string(val);
    }
}

void VariValue::clear()
{
    m_value = std::monostate();
}

bool VariValue::setNull()
{
    m_value = std::monostate{};
    return true;
}

bool VariValue::setBool(bool val)
{
    m_value = val;
    return true;
}

bool VariValue::setInt(uint64_t val)
{
    if (num_t num; num.setInt(val)) {
        m_value = std::move(num);
        return true;
    }
    return false;
}

bool VariValue::setInt(int64_t val)
{
    if (num_t num; num.setInt(val)) {
        m_value = std::move(num);
        return true;
    }
    return false;
}

bool VariValue::setInt(int val)
{
    if (num_t num; num.setInt(val)) {
        m_value = std::move(num);
        return true;
    }
    return false;
}

bool VariValue::setFloat(double val)
{
    if (num_t num; num.setFloat(val)) {
        m_value = std::move(num);
        return true;
    }
    return false;
}

bool VariValue::setStr(std::string val)
{
    m_value = std::move(val);
    return true;
}

bool VariValue::setNumStr(std::string val)
{
    if (num_t num; num.setNumStr(std::move(val))) {
        m_value = std::move(num);
        return true;
    }
    return false;
}

bool VariValue::setArray()
{
    m_value = array_t();
    return true;
}

bool VariValue::setObject()
{
    m_value = object_t();
    return true;
}

enum VariValue::VType VariValue::getType() const
{
    return std::visit(varivalue::overloaded {
        [](const object_t&) { return VOBJ;},
        [](const array_t&) { return VARR;},
        [](const std::string&) { return VSTR;},
        [](num_t) { return VNUM;},
        [](bool) { return VBOOL;},
        [](std::monostate)  { return VNULL;},
        }, m_value);
}

std::string VariValue::getValStr() const
{
    return std::visit(varivalue::overloaded {
        [&](const std::string& val) { return val;},
        [&](const num_t& num) { return num.getValStr();},
        [&](const bool& val) -> std::string { return val ? "1" : "";},
        [&](const auto&) -> std::string { return "";},
        }, m_value);
}

bool VariValue::empty() const
{
    return std::visit(varivalue::overloaded {
        [&](const object_t& obj) { return obj.empty();},
        [&](const array_t& arr) { return arr.empty();},
        [&](const auto&)  { return true;},
        }, m_value);
}

size_t VariValue::size() const
{
    return std::visit(varivalue::overloaded {
        [&](const object_t& obj) { return obj.size();},
        [&](const array_t& arr) { return arr.size();},
        [&](const auto&) -> size_t  { return 0; },
        }, m_value);
}

void VariValue::reserve(size_t n) {

    std::visit(varivalue::overloaded {
        [&](object_t&) {/* TODO: fill in when object is unordered_map */ },
        [&](array_t& arr) { arr.reserve(n); },
        [&](std::string& str) {str.reserve(n); },
        [&](const auto&) {},
        }, m_value);
}

bool VariValue::getBool() const
{
    if(auto ret = std::get_if<bool>(&m_value)) {
        return *ret == true;
    }
    return false;
}


void VariValue::getObjMap(std::map<std::string,VariValue>& kv) const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        kv = *ret;
    }
}


bool VariValue::checkObject(const std::map<std::string,VariValue::VType>& keytypes) const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        for (const auto& [key, type] : keytypes) {
            if (auto match = ret->find(key); match != ret->end()) {
                if (match->second.getType() != type) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

const VariValue& VariValue::operator[](const std::string& key) const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        auto it = ret->find(key);
        if (it != ret->end()) {
            return it->second;
        }
    }
    return NullUniValue;
}

const VariValue& VariValue::operator[](size_t index) const
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        if (index < ret->size()) {
            return ret->operator[](index);
        }
    }
    return NullUniValue;
}



bool VariValue::exists(const std::string& key) const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        if (ret->find(key) != ret->end()) {
            return true;
        }
    }
    return false;
}

bool VariValue::isNull() const
{
    return std::holds_alternative<std::monostate>(m_value);
}

bool VariValue::isTrue() const
{
    if(auto ret = std::get_if<bool>(&m_value)) {
        return *ret;
    }
    return false;
}

bool VariValue::isFalse() const
{
    if(auto ret = std::get_if<bool>(&m_value)) {
        return *ret == false;
    }
    return false;
}

bool VariValue::isBool() const
{
    return std::holds_alternative<bool>(m_value);
}

bool VariValue::isStr() const
{
    return std::holds_alternative<std::string>(m_value);
}

bool VariValue::isNum() const
{
    return std::holds_alternative<num_t>(m_value);
}

bool VariValue::isArray() const
{
    return std::holds_alternative<array_t>(m_value);
}

bool VariValue::isObject() const
{
    return std::holds_alternative<object_t>(m_value);
}


bool VariValue::pushKV(std::string key, std::string val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{std::move(val)});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, const char *val) {
    if (val) {
        return pushKV(std::move(key), std::string(val));
    }
    return false;
}

bool VariValue::pushKV(std::string key, int64_t val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{val});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, uint64_t val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{val});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, bool val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{val});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, int val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{val});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, double val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{val});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, std::monostate) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), VariValue{});
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, VariValue obj)
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->insert_or_assign(std::move(key), std::move(obj));
        return true;
    }
    return false;
}

bool VariValue::pushKVs(VariValue obj)
{
    if(auto lhs = std::get_if<object_t>(&m_value)) {
        if(auto rhs = std::get_if<object_t>(&obj.m_value)) {
            lhs->merge(std::move(*rhs));
            return true;
        }
    }
    return false;
}

bool VariValue::push_back(VariValue val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->push_back(std::move(val));
        return true;
    }
    return false;
}

bool VariValue::push_back(std::string val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->emplace_back(std::move(val));
        return true;
    }
    return false;
}

bool VariValue::push_back(const char *val)
{
    if (val) {
        return push_back(std::string(val));
    }
    return false;
}

bool VariValue::push_back(uint64_t val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->emplace_back(val);
        return true;
    }
    return false;
}

bool VariValue::push_back(int64_t val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->emplace_back(val);
        return true;
    }
    return false;
}

bool VariValue::push_back(bool val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->emplace_back(val);
        return true;
    }
    return false;
}

bool VariValue::push_back(int val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->emplace_back(val);
        return true;
    }
    return false;
}

bool VariValue::push_back(double val)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->emplace_back(val);
        return true;
    }
    return false;
}

bool VariValue::push_back(std::monostate)
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        ret->push_back(VariValue{});
        return true;
    }
    return false;
}

bool VariValue::push_backV(std::vector<VariValue> vec)
{
    if(auto lhs = std::get_if<array_t>(&m_value)) {
        lhs->insert(lhs->end(), vec.begin(), vec.end());
        return true;
    }
    return false;
}


std::vector<std::string> VariValue::getKeys() const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        std::vector<std::string> keys;
        for (auto&& i : *ret) {
            keys.push_back(i.first);
        }
        return keys;
    }
    throw std::runtime_error("JSON value is not an object as expected");
}

std::vector<VariValue> VariValue::getValues() const
{
    return std::visit(varivalue::overloaded {
        [&](const array_t& arr) { return arr;},
        [&](const object_t& obj) {
            std::vector<VariValue> values;
            values.reserve(obj.size());
            for (const auto& i : obj) {
                values.push_back(i.second);
            }
            return values;
        },
        [&](const auto&) -> std::vector<VariValue> {
            throw std::runtime_error("JSON value is not an object as expected");
        },
        }, m_value);
}

bool VariValue::get_bool() const
{
    if(auto ret = std::get_if<bool>(&m_value)) {
        return *ret;
    }
    throw std::runtime_error("JSON value is not a boolean as expected");
}

const std::string& VariValue::get_str() const
{
    
    if(auto ret = std::get_if<std::string>(&m_value)) {
        return *ret;
    }
    throw std::runtime_error("JSON value is not a string as expected");
}

int VariValue::get_int() const
{
    if(auto num = std::get_if<num_t>(&m_value)) {
        return num->get_int();
    }
    throw std::runtime_error("JSON value is not an integer as expected");
}

int64_t VariValue::get_int64() const
{
    if(auto num = std::get_if<num_t>(&m_value)) {
        return num->get_int64();
    }
    throw std::runtime_error("JSON value is not an integer as expected");
}

double VariValue::get_real() const
{
    if(auto num = std::get_if<num_t>(&m_value)) {
        return num->get_real();
    }
    throw std::runtime_error("JSON value is not a number as expected");
}

const VariValue& VariValue::get_obj() const
{
    if(auto num = std::get_if<object_t>(&m_value)) {
        return *this;
    }
    throw std::runtime_error("JSON value is not an object as expected");
}

const VariValue& VariValue::get_array() const
{
    if(auto num = std::get_if<array_t>(&m_value)) {
        return *this;
    }
    throw std::runtime_error("JSON value is not an array as expected");
}

VariValue::VType VariValue::type() const
{
    return getType();
}

const VariValue& find_value(const VariValue& obj, const std::string& name)
{
    if(auto ret = std::get_if<object_t>(&obj.m_value)) {
        auto it = ret->find(name);
        if (it != ret->end()) {
            return it->second;
        }
    }
    return NullUniValue;
}

std::string VariValue::write(unsigned int prettyIndent, unsigned int indentLevel) const
{
    std::string s;
    s.reserve(1024);

    unsigned int modIndent = indentLevel;
    if (modIndent == 0)
        modIndent = 1;

    std::visit(varivalue::overloaded {
        [&](const object_t& val) { return writeObject(val, s, prettyIndent, modIndent);},
        [&](const array_t& val) { return writeArray(val, s, prettyIndent, modIndent);},
        [&](const std::string val) { return writeString(val, s);},
        [&](const num_t& val) { return writeNum(val, s);},
        [&](bool val) { return writeBool(val, s);},
        [&](std::monostate) { return writeNull(s);}
        }, m_value);

    return s;
}
