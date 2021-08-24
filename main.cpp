#include "varivalue.h"

int main()
{
    using UniValue = VariValue;
    UniValue numval(175l);
    UniValue stringval("foo");
    UniValue obj(UniValue::VOBJ);
    obj.pushKV("foo", "bar");
    UniValue arr(UniValue::VARR);
    arr.push_back(true);
    return 0;
}
