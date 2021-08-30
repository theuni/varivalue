#include "varinum.h"
#include "varivalue_util.h"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace
{
static bool ParsePrechecks(const std::string& str)
{
    if (str.empty()) // No empty string allowed
        return false;
    if (str.size() >= 1 && (json_isspace(str[0]) || json_isspace(str[str.size()-1]))) // No padding allowed
        return false;
    if (str.size() != strlen(str.c_str())) // No embedded NUL characters allowed
        return false;
    return true;
}

bool ParseInt32(const std::string& str, int32_t *out)
{
    if (!ParsePrechecks(str))
        return false;
    char *endp = NULL;
    errno = 0; // strtol will not set errno if valid
    long int n = strtol(str.c_str(), &endp, 10);
    if(out) *out = (int32_t)n;
    // Note that strtol returns a *long int*, so even if strtol doesn't report an over/underflow
    // we still have to check that the returned value is within the range of an *int32_t*. On 64-bit
    // platforms the size of these types may be different.
    return endp && *endp == 0 && !errno &&
        n >= std::numeric_limits<int32_t>::min() &&
        n <= std::numeric_limits<int32_t>::max();
}

bool ParseInt64(const std::string& str, int64_t *out)
{
    if (!ParsePrechecks(str))
        return false;
    char *endp = NULL;
    errno = 0; // strtoll will not set errno if valid
    long long int n = strtoll(str.c_str(), &endp, 10);
    if(out) *out = (int64_t)n;
    // Note that strtoll returns a *long long int*, so even if strtol doesn't report a over/underflow
    // we still have to check that the returned value is within the range of an *int64_t*.
    return endp && *endp == 0 && !errno &&
        n >= std::numeric_limits<int64_t>::min() &&
        n <= std::numeric_limits<int64_t>::max();
}

bool ParseDouble(const std::string& str, double *out)
{
    if (!ParsePrechecks(str))
        return false;
    if (str.size() >= 2 && str[0] == '0' && str[1] == 'x') // No hexadecimal floats allowed
        return false;
    std::istringstream text(str);
    text.imbue(std::locale::classic());
    double result;
    text >> result;
    if(out) *out = result;
    return text.eof() && !text.fail();
}
}

VariNum::VariNum(uint64_t val)
{
    setInt(val);
}

VariNum::VariNum(int64_t val)
{
    setInt(val);
}

VariNum::VariNum(int val)
{
    setInt(val);
}

VariNum::VariNum(double val)
{
    setFloat(val);
}

const std::string& VariNum::getValStr() const
{
    return m_value;
}

bool VariNum::setNumStr(std::string val)
{
    if (!validNumStr(val))
        return false;

    m_value = std::move(val);
    return true;
}

bool VariNum::setInt(uint64_t val_)
{
    std::ostringstream oss;

    oss << val_;

    return setNumStr(oss.str());
}

bool VariNum::setInt(int64_t val_)
{
    std::ostringstream oss;

    oss << val_;

    return setNumStr(oss.str());
}

bool VariNum::setInt(int val_)
{
    return setInt(static_cast<int64_t>(val_));
}

bool VariNum::setFloat(double val_)
{
    std::ostringstream oss;

    oss << std::setprecision(16) << val_;

    return setNumStr(oss.str());
}


int VariNum::get_int() const
{
    int32_t retval;
    if (!ParseInt32(m_value, &retval))
        throw std::runtime_error("JSON integer out of range");
    return retval;
}

int64_t VariNum::get_int64() const
{
    int64_t retval;
    if (!ParseInt64(m_value, &retval))
        throw std::runtime_error("JSON integer out of range");
    return retval;
}

double VariNum::get_real() const
{
    double retval;
    if (!ParseDouble(m_value, &retval))
        throw std::runtime_error("JSON double out of range");
    return retval;
}
