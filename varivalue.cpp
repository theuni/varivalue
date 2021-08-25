#include "varivalue.h"
#include <cassert>
#include <stdexcept>

static const VariValue NULLVALUE;

// visitor helper type. From: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

VariValue::VariValue(VType initialType) {
    switch (initialType) {
        case VNULL: m_value = std::monostate(); break;
        case VOBJ: m_value = object_t{}; break;
        case VARR: m_value = array_t{}; break;
        case VSTR: m_value = std::string(); break;
        case VNUM: m_value = num_t{}; break;
        case VBOOL: m_value = bool{}; break;
        default: m_value = std::monostate(); break;
    }
}

VariValue::VariValue() {
}

VariValue::VariValue(uint64_t val) {
    m_value = num_t{val};
}

VariValue::VariValue(int64_t val) {
    m_value = num_t{val};
}

VariValue::VariValue(bool val) {
    m_value = val;
}

VariValue::VariValue(int val) {
    m_value = num_t{static_cast<int64_t>(val)};
}

VariValue::VariValue(double val) {
    m_value = num_t{val};
}

VariValue::VariValue(std::string val) {
    m_value = std::move(val);
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
    m_value = num_t(val);
    return true;
}

bool VariValue::setInt(int64_t val)
{
    m_value = num_t(val);
    return true;
}

bool VariValue::setInt(int val)
{
    m_value = num_t(static_cast<int64_t>(val));
    return true;
}

bool VariValue::setFloat(double val)
{
    m_value = num_t(val);
    return true;
}

bool VariValue::setStr(std::string val)
{
    m_value = std::move(val);
    return true;
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
    return std::visit(overloaded {
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
    return {};
}

bool VariValue::empty() const
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        return ret->empty();
    }
    return true;
}

size_t VariValue::size() const
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        return ret->size();
    }
    return 0;
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


bool VariValue::checkObject(const std::map<std::string,VariValue::VType>&) const
{
    // TODO?
    assert(false);
}

const VariValue& VariValue::operator[](const std::string& key) const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        auto it = ret->find(key);
        if (it != ret->end()) {
            return it->second;
        }
    }
    return NULLVALUE;
}

const VariValue& VariValue::operator[](size_t index) const
{
    if(auto ret = std::get_if<array_t>(&m_value)) {
        if (index < ret->size()) {
            return ret->operator[](index);
        }
    }
    return NULLVALUE;
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
    if(auto ret = std::get_if<bool>(&m_value))
        return *ret;
    return false;
}

bool VariValue::isFalse() const
{
    if(auto ret = std::get_if<bool>(&m_value))
        return *ret == false;
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
        ret->emplace(std::move(key), std::move(val));
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
        ret->emplace(std::move(key), val);
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, uint64_t val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->emplace(std::move(key), val);
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, bool val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->emplace(std::move(key), val);
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, int val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->emplace(std::move(key), val);
        return true;
    }
    return false;
}

bool VariValue::pushKV(std::string key, double val) {
    if(auto ret = std::get_if<object_t>(&m_value)) {
        ret->emplace(std::move(key), val);
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
        for (auto i : *ret) {
            keys.push_back(i.first);
        }
        return keys;
    }
    throw std::runtime_error("JSON value is not an object as expected");
}

std::vector<VariValue> VariValue::getValues() const
{
    if(auto ret = std::get_if<object_t>(&m_value)) {
        std::vector<VariValue> values;
        for (auto i : *ret) {
            values.push_back(i.second);
        }
        return values;
    }
    throw std::runtime_error("JSON value is not an object as expected");
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
        if(auto ret = std::get_if<int64_t>(num)) {
            return *ret;
        }
    }
    throw std::runtime_error("JSON value is not an integer as expected");
}

int64_t VariValue::get_int64() const
{
    if(auto num = std::get_if<num_t>(&m_value)) {
        if(auto ret = std::get_if<int64_t>(num)) {
            return *ret;
        }
    }
    throw std::runtime_error("JSON value is not an integer as expected");
}

double VariValue::get_real() const
{
    if(auto num = std::get_if<num_t>(&m_value)) {
        if(auto ret = std::get_if<double>(num)) {
            return *ret;
        }
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
    return NULLVALUE;
}
