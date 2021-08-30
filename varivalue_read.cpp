// Copyright 2014 BitPay Inc.
// Copyright 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <string.h>
#include <vector>
#include <stdio.h>
#include "varivalue.h"
#include "varivalue_util.h"

/*
 * According to stackexchange, the original json test suite wanted
 * to limit depth to 22.  Widely-deployed PHP bails at depth 512,
 * so we will follow PHP's lead, which should be more than sufficient
 * (further stackexchange comments indicate depth > 32 rarely occurs).
 */
static constexpr size_t MAX_JSON_DEPTH = 512;

enum expect_bits {
    EXP_OBJ_NAME = (1U << 0),
    EXP_COLON = (1U << 1),
    EXP_ARR_VALUE = (1U << 2),
    EXP_VALUE = (1U << 3),
    EXP_NOT_VALUE = (1U << 4),
};

#define expect(bit) (expectMask & (EXP_##bit))
#define setExpect(bit) (expectMask |= EXP_##bit)
#define clearExpect(bit) (expectMask &= ~EXP_##bit)

bool VariValue::read(const char *raw, size_t size)
{
    clear();

    uint32_t expectMask = 0;
    std::vector<UniValue*> stack;

    std::string tokenVal;
    unsigned int consumed;
    enum jtokentype tok = JTOK_NONE;
    enum jtokentype last_tok = JTOK_NONE;
    const char* end = raw + size;


    std::string cur_key;
    bool have_key{false};
    do {
        last_tok = tok;

        tok = getJsonToken(tokenVal, consumed, raw, end);
        if (tok == JTOK_NONE || tok == JTOK_ERR)
            return false;
        raw += consumed;

        bool isValueOpen = jsonTokenIsValue(tok) ||
            tok == JTOK_OBJ_OPEN || tok == JTOK_ARR_OPEN;

        if (expect(VALUE)) {
            if (!isValueOpen)
                return false;
            clearExpect(VALUE);

        } else if (expect(ARR_VALUE)) {
            bool isArrValue = isValueOpen || (tok == JTOK_ARR_CLOSE);
            if (!isArrValue)
                return false;

            clearExpect(ARR_VALUE);

        } else if (expect(OBJ_NAME)) {
            bool isObjName = (tok == JTOK_OBJ_CLOSE || tok == JTOK_STRING);
            if (!isObjName)
                return false;

        } else if (expect(COLON)) {
            if (tok != JTOK_COLON)
                return false;
            clearExpect(COLON);

        } else if (!expect(COLON) && (tok == JTOK_COLON)) {
            return false;
        }

        if (expect(NOT_VALUE)) {
            if (isValueOpen)
                return false;
            clearExpect(NOT_VALUE);
        }

        switch (tok) {

        case JTOK_OBJ_OPEN:
        case JTOK_ARR_OPEN: {
            VType utyp = (tok == JTOK_OBJ_OPEN ? VOBJ : VARR);
            if (!stack.size()) {
                if (utyp == VOBJ)
                    setObject();
                else
                    setArray();
                stack.push_back(this);
            } else {
                UniValue* top = stack.back();
                UniValue tmpVal(utyp);
                std::visit(varivalue::overloaded {
                    [&](array_t& arr) {
                        auto& newTop = arr.emplace_back(std::move(tmpVal));
                        stack.push_back(&newTop);
                    },
                    [&](object_t& obj) {
                        const auto& [iter, inserted] = obj.emplace(std::move(cur_key), std::move(tmpVal));
                        have_key = false;
                        cur_key.clear();
                        stack.push_back(&iter->second);
                    },
                    [&](const auto&) {},
                }, m_value);
            }

            if (stack.size() > MAX_JSON_DEPTH)
                return false;

            if (utyp == VOBJ)
                setExpect(OBJ_NAME);
            else
                setExpect(ARR_VALUE);
            break;
            }

        case JTOK_OBJ_CLOSE:
        case JTOK_ARR_CLOSE: {
            if (!stack.size() || (last_tok == JTOK_COMMA))
                return false;

            VType utyp = (tok == JTOK_OBJ_CLOSE ? VOBJ : VARR);
            UniValue *top = stack.back();
            if (utyp != top->getType())
                return false;

            stack.pop_back();
            clearExpect(OBJ_NAME);
            setExpect(NOT_VALUE);
            break;
            }

        case JTOK_COLON: {
            if (!stack.size())
                return false;

            UniValue *top = stack.back();
            if (top->getType() != VOBJ)
                return false;

            setExpect(VALUE);
            break;
            }

        case JTOK_COMMA: {
            if (!stack.size() ||
                (last_tok == JTOK_COMMA) || (last_tok == JTOK_ARR_OPEN))
                return false;

            UniValue *top = stack.back();
            if (top->getType() == VOBJ)
                setExpect(OBJ_NAME);
            else
                setExpect(ARR_VALUE);
            break;
            }

        case JTOK_KW_NULL: {
            if (!stack.size()) {
                m_value = std::monostate();
                break;
            }
            UniValue* top = stack.back();
            UniValue tmpVal;
            std::visit(varivalue::overloaded {
                [&](array_t& arr) {
                    auto& newTop = arr.emplace_back(std::move(tmpVal));
                    stack.push_back(&newTop);
                },
                [&](object_t& obj) {
                    const auto& [iter, inserted] = obj.emplace(std::move(cur_key), std::move(tmpVal));
                    have_key = false;
                    cur_key.clear();
                    stack.push_back(&iter->second);
                    },
                [&](const auto&) {},
            }, m_value);
            break;
            }
        case JTOK_KW_TRUE:
        case JTOK_KW_FALSE: {
            bool val;
            switch (tok) {
            case JTOK_KW_TRUE:
                val = true;
                break;
            case JTOK_KW_FALSE:
                val = false;
                break;
            default: /* impossible */ break;
            }

            if (!stack.size()) {
                m_value = val;
                break;
            }

            UniValue* top = stack.back();
            std::visit(varivalue::overloaded {
                [&](array_t& arr) {
                    auto& newTop = arr.emplace_back(val);
                    stack.push_back(&newTop);
                },
                [&](object_t& obj) {
                    const auto& [iter, inserted] = obj.emplace(std::move(cur_key), val);
                    have_key = false;
                    cur_key.clear();
                    stack.push_back(&iter->second);
                },
                [&](const auto&) {},
            }, m_value);

            setExpect(NOT_VALUE);
            break;
            }

        case JTOK_NUMBER: {
            if (!stack.size()) {
                setNumStr(std::move(tokenVal));
                break;
            }
            VariValue tmpVal(VNUM, std::move(tokenVal));
            UniValue *top = stack.back();
            std::visit(varivalue::overloaded {
                [&](array_t& arr) {
                    auto& newTop = arr.emplace_back(std::move(tmpVal));
                    stack.push_back(&newTop);
                },
                [&](object_t& obj) {
                    const auto& [iter, inserted] = obj.emplace(std::move(cur_key), std::move(tmpVal));
                    have_key = false;
                    cur_key.clear();
                    stack.push_back(&iter->second);
                },
                [&](const auto&) {},
            }, m_value);

            setExpect(NOT_VALUE);
            break;
            }

        case JTOK_STRING: {
            if (expect(OBJ_NAME)) {
                cur_key = std::move(tokenVal);
                have_key = true;
                clearExpect(OBJ_NAME);
                setExpect(COLON);
            } else {
                if (!stack.size()) {
                    m_value = std::move(tokenVal);
                    break;
                }
                UniValue tmpVal(std::move(tokenVal));
                UniValue *top = stack.back();
                std::visit(varivalue::overloaded {
                    [&](array_t& arr) {
                        auto& newTop = arr.emplace_back(std::move(tmpVal));
                        stack.push_back(&newTop);
                    },
                    [&](object_t& obj) {
                        const auto& [iter, inserted] = obj.emplace(std::move(cur_key), std::move(tmpVal));
                        have_key = false;
                        cur_key.clear();
                        stack.push_back(&iter->second);
                    },
                    [&](const auto&) {},
                }, m_value);
            }
            setExpect(NOT_VALUE);
            break;
            }

        default:
            return false;
        }
    } while (!stack.empty ());

    /* Check that nothing follows the initial construct (parsed above).  */
    tok = getJsonToken(tokenVal, consumed, raw, end);
    if (tok != JTOK_NONE)
        return false;

    return true;
}

bool VariValue::read(const char *raw)
{
    return read(raw, strlen(raw));
}

bool VariValue::read(const std::string& rawStr)
{
    return read(rawStr.data(), rawStr.size());
}
