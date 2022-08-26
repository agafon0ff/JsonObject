/*
 * Copyright (c) 2022 Sergey Agafonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <cstring>
#include <sstream>
#include <iomanip>

#include "jsonobject.h"

const char NUM_SYMBOLS[15] = {'.','0','1','2','3','4','5','6','7','8','9','-','+','e','E'};

JsonObject::JsonObject()
{
    m_keys.clear();
}

JsonObject::JsonObject(bool value) : JsonObject()
{
    m_type = JsonObject::JSON_BOOL;
    m_value = value ? "true" : "false";
}

JsonObject::JsonObject(int value) : JsonObject()
{
    m_type = JsonObject::JSON_NUMBER;
    m_value = std::to_string(value);
}

JsonObject::JsonObject(double value, uint16_t precision) : JsonObject()
{
    m_type = JsonObject::JSON_NUMBER;

    std::ostringstream streamObj;
    streamObj << std::fixed << std::setprecision(precision) << value;

    m_value = streamObj.str();
}

JsonObject::JsonObject(const char *value) : JsonObject()
{
    m_type = JsonObject::JSON_STRING;
    m_value = std::string(value);
}

JsonObject::JsonObject(const std::string &value) : JsonObject()
{
    m_type = JsonObject::JSON_STRING;
    m_value = value;
}

JsonObject::JsonObject(std::string &&value) : JsonObject()
{
    m_type = JsonObject::JSON_STRING;
    m_value = std::move(value);
}

JsonObject::JsonObject(const std::vector<JsonObject> &value) : JsonObject()
{
    m_type = JsonObject::JSON_ARRAY;
    m_array = value;
}

JsonObject::JsonObject(std::vector<JsonObject> &&value) : JsonObject()
{
    m_type = JsonObject::JSON_ARRAY;
    m_array = std::move(value);
}

size_t JsonObject::parse(const char *data, size_t len)
{
    if (len < 2) return 1;

    clear();

    size_t step = 0, errPos = 1, end = 0;
    for (step = 0; step < len; ++step) {
        char symbol = data[step];

        if (symbol == '{') {
            errPos = _parseObject(data + step, len - step, end, *this);
            break;
        }
        else if (symbol == '[') {
            errPos = _parseArray(data + step, len - step, end, *this);
            break;
        }
        else if (symbol == ' ' || symbol == '\n' || symbol == '\r') {
            continue;
        }
        else {
            errPos = step;
            break;
        }
    }

    return errPos;
}

size_t JsonObject::parse(const std::string &data)
{
    return parse(data.data(), data.size());
}

std::string JsonObject::stringify(JsonObject::StringifyMode mode)
{
    return _stringify(0, mode);
}

JsonObject::Type JsonObject::type()
{
    return m_type;
}

std::vector<std::string> JsonObject::keys()
{
    if (m_keys.size() != m_map.size()) {
        m_keys.reserve(m_map.size());

        for (auto &it: m_map)
            m_keys.push_back(it.first);
    }

    return m_keys;
}

bool JsonObject::exist(const char *key)
{
    return exist(std::string(key));
}

bool JsonObject::exist(const std::string &key)
{
    return m_map.find(key) != m_map.end();
}

JsonObject JsonObject::value(const char *key)
{
    return value(std::string(key));
}

JsonObject JsonObject::value(const std::string &key)
{
    auto it = m_map.find(key);
    if (it != m_map.end())
        return it->second;

    return JsonObject();
}

void JsonObject::setValue(const char *key, const JsonObject &value)
{
    setValue(std::string(key), value);
}

void JsonObject::setValue(const std::string &key, const JsonObject &value)
{
    if (m_type != JsonObject::JSON_OBJECT)
        clear();

    m_type = JsonObject::JSON_OBJECT;
    m_map[key] = value;
}

void JsonObject::append(const JsonObject &value)
{
    if (m_type != JsonObject::JSON_ARRAY)
        clear();

    m_type = JsonObject::JSON_ARRAY;
    m_array.push_back(value);
}

JsonObject JsonObject::at(size_t index)
{
    if (index >= m_array.size())
        return {};

    return m_array.at(index);
}

size_t JsonObject::size()
{
    if (m_type == JsonObject::JSON_OBJECT)
        return m_map.size();
    else if (m_type == JsonObject::JSON_ARRAY)
        return m_array.size();
    else return 0;
}

void JsonObject::clear()
{
    m_type = JsonObject::JSON_NULL;
    m_map.clear();
    m_value.clear();
    m_array.clear();
    m_keys.clear();
}

void JsonObject::remove(const char *key)
{
    remove(std::string(key));
}

void JsonObject::remove(const std::string &key)
{
    m_keys.clear();
    auto it = m_map.find(key);
    if (it != m_map.end())
        m_map.erase(it);
}

bool JsonObject::toBool(bool defVal)
{
    if (m_type == JsonObject::JSON_BOOL)
        return m_value == "true";

    return defVal;
}

double JsonObject::toNumber(double defVal)
{
    if (m_type == JsonObject::JSON_NUMBER)
        return std::stod(m_value);

    return defVal;
}

std::string JsonObject::toString(const std::string defVal)
{
    if (m_type == JsonObject::JSON_STRING)
        return m_value;

    return defVal;
}

std::vector<JsonObject> JsonObject::toArray()
{
    if (m_type == JsonObject::JSON_ARRAY)
        return m_array;

    return {};
}

std::map<std::string, JsonObject> JsonObject::toMap()
{
     if (m_type == JsonObject::JSON_OBJECT)
        return m_map;

    return {};
}

std::string JsonObject::_stringify(size_t indent, StringifyMode mode)
{
    std::string result;
    result.reserve(100);
    int8_t spaces = static_cast<int8_t>(mode);
    const char* newLine = (mode == MODE_COMPACT) ? "" : "\n";

    switch (m_type) {
    case JsonObject::JSON_NULL:
        result = std::string("null");
        break;
    case JsonObject::JSON_BOOL:
        result = m_value;
        break;
    case JsonObject::JSON_NUMBER:
        result = m_value;
        break;
    case JsonObject::JSON_STRING:
        result += "\"";
        result += m_value;
        result += "\"";
        break;
    case JsonObject::JSON_ARRAY:
        result += "[";
        result += newLine;
        ++indent;

        for (auto it = m_array.begin(); it != m_array.end(); ++it)
        {
            result += std::string(indent * spaces, ' ');
            result += it->_stringify(indent, mode);

            if (it != m_array.end() - 1) {
                result += ",";
                result += newLine;
            }
        }

        --indent;
        result += newLine;
        result += std::string(indent * spaces, ' ');
        result += "]";

        break;
    case JsonObject::JSON_OBJECT: {
        result += "{";
        result += newLine;
        ++indent;

        size_t i = 0, last = m_map.size() - 1;
        for (auto it = m_map.begin(); it != m_map.end(); ++it, ++i)
        {
            result += std::string(indent * spaces, ' ');
            result += "\"";
            result += it->first;
            result += "\":";

            if (mode != MODE_COMPACT)
                result += " ";

            result += it->second._stringify(indent, mode);
            if (i < last) {
                result += ",";
                result += newLine;
            }
        }

        --indent;
        result += newLine;
        result += std::string(indent * spaces, ' ');
        result += "}";
        break;
    }
    default: break;
    }

    return result;
}

size_t JsonObject::_parseObject(const char *data, size_t len, size_t &end, JsonObject &obj)
{
    if (len < 2) {
        obj.m_type = JsonObject::JSON_ERROR;
        return 1;
    }

    size_t step = 0, errPos = 1;
    char symbol = data[step];
    std::string key, valueStr;
    end = 0;

    if (symbol != '{') {
        obj.m_type = JsonObject::JSON_ERROR;
        return 1;
    }

    obj.m_type = JsonObject::JSON_OBJECT;
    bool stepValue = false;

    for (step = 1; step < len; ++step) {
        symbol = data[step];

        if (stepValue) { // value parsing
            if (symbol == 'n') { // null
                errPos = _compareWord(data + step, len - step, "null");
                if (errPos > 0 ) break;

                step += 3;
                obj.setValue(key, JsonObject());
            }
            else if (symbol == 't') { // true
                errPos = _compareWord(data + step, len - step, "true");
                if (errPos > 0 ) break;

                step += 3;
                obj.setValue(key, true);
            }
            else if (symbol == 'f') { // false
                errPos = _compareWord(data + step, len - step, "false");
                if (errPos > 0 ) break;

                step += 4;
                obj.setValue(key, false);
            }
            else if (symbol == '"') { // text
                errPos = _parseText(data + step, len - step, end);

                if (errPos > 0) break;
                if (end == 0) break;

                valueStr = std::string(data + step + 1, end - 1);
                step += end;

                obj.setValue(key, valueStr);
            }
            else if (isCharNumber(symbol)) { // number

                errPos = _parseNumber(data + step, len - step, end);

                if (errPos > 0) break;
                if (end == 0) break;

                valueStr = std::string(data + step, end);
                step += end - 1;

                JsonObject jsonNum(valueStr);
                jsonNum.m_type = JsonObject::JSON_NUMBER;
                obj.m_map[key] = std::move(jsonNum);
            }
            else if (symbol == '{') { // object
                JsonObject jsonObj;
                errPos = _parseObject(data + step, len - step, end, jsonObj);

                if (errPos > 0) break;
                if (end == 0) break;

                step += end;
                obj.m_map[key] = std::move(jsonObj);
            }
            else if (symbol == '[') { // array
                JsonObject jsonObj;
                errPos = _parseArray(data + step, len - step, end, jsonObj);

                if (errPos > 0) break;
                if (end == 0) break;

                step += end;
                obj.m_map[key] = std::move(jsonObj);
            }
            else if (symbol == ',') {
                stepValue = false;
            }
            else if (symbol == '}') {
                break;
            }
        }
        else { // key parsing
            if (symbol == '"') {
                errPos = _parseText(data + step, len - step, end);

                if (errPos > 0) break;
                if (end == 0) break;

                key = std::string(data + step + 1, end - 1);
                step += end;
            }
            else if (symbol == ':') {
                stepValue = true;
            }
        }
    }

    if (errPos == 0) {
        end = step;
        return 0;
    }
    else obj.m_type = JsonObject::JSON_ERROR;
    return errPos + step;
}

size_t JsonObject::_parseArray(const char *data, size_t len, size_t &end, JsonObject &obj)
{
    if (len < 2) {
        obj.m_type = JsonObject::JSON_ERROR;
        return 1;
    }

    size_t step = 0, errPos = 1;
    char symbol = data[step];
    std::string valueStr;
    end = 0;

    if (symbol != '[') {
        obj.m_type = JsonObject::JSON_ERROR;
        return 1;
    }

    obj.m_type = JsonObject::JSON_ARRAY;
    bool stepValue = true;

    for (step = 1; step < len; ++step) {
        symbol = data[step];

        if (stepValue) { // value parsing

            if (symbol == 'n') { // null
                errPos = _compareWord(data + step, len - step, "null");
                stepValue = false;
                if (errPos > 0 ) break;

                step += 3;
                obj.m_array.push_back(JsonObject());
            }
            else if (symbol == 't') { // true
                errPos = _compareWord(data + step, len - step, "true");
                stepValue = false;
                if (errPos > 0 ) break;

                step += 3;
                obj.m_array.push_back(true);
            }
            else if (symbol == 'f') { // false
                errPos = _compareWord(data + step, len - step, "false");
                stepValue = false;
                if (errPos > 0 ) break;

                step += 4;
                obj.m_array.push_back(false);
            }
            else if (symbol == '"') { // text
                errPos = _parseText(data + step, len - step, end);
                stepValue = false;

                if (errPos > 0) break;
                if (end == 0) break;

                valueStr = std::string(data + step + 1, end - 1);
                step += end;

                obj.m_array.push_back(valueStr);
            }
            else if (isCharNumber(symbol)) { // number

                errPos = _parseNumber(data + step, len - step, end);
                stepValue = false;

                if (errPos > 0) break;
                if (end == 0) break;

                valueStr = std::string(data + step, end);
                step += end - 1;

                JsonObject jsonNum(valueStr);
                jsonNum.m_type = JsonObject::JSON_NUMBER;
                obj.m_array.push_back(std::move(jsonNum));
            }
            else if (symbol == '{') { // object
                JsonObject jsonObj;
                errPos = _parseObject(data + step, len - step, end, jsonObj);
                stepValue = false;

                if (errPos > 0) break;
                if (end == 0) break;

                step += end;
                obj.m_array.push_back(std::move(jsonObj));
            }
            else if (symbol == '[') { // array
                JsonObject jsonArr;
                errPos = _parseArray(data + step, len - step, end, jsonArr);

                if (errPos > 0) break;
                if (end == 0) break;

                step += end;
                obj.m_array.push_back(std::move(jsonArr));
            }
        }
        if (symbol == ',') {
            stepValue = true;
        }
        else if (symbol == ']') {
            break;
        }
    }

    if (errPos == 0) {
        end = step;
        return 0;
    }
    else obj.m_type = JsonObject::JSON_ERROR;
    return errPos + step;
}

size_t JsonObject::_parseText(const char *data, size_t len, size_t &end)
{
    if (len < 3) return 1;

    end = 0;
    size_t step = 0;
    bool started = false;

    for (step = 0; step < len; ++step) {
        if (data[step] == '"' && data[step-1] != '\\') {
            if (started) {
                end = step;
                return 0;
            }
            else {
                started = true;
            }
        }
    }

    return step;
}

size_t JsonObject::_parseNumber(const char *data, size_t len, size_t &end)
{
    if (len < 2) return 1;

    end = 0;

    for (int i = 0; i < len; ++i) {
        char symbol = data[i];
        if (isCharNumber(symbol)) {
            continue;
        }
        else if (symbol == ' ' || symbol == ',' ||
                 symbol == '\r' || symbol == '\n') {
            end = i;
            return 0;
        }
        else {
            end = i;
            break;
        }
    }

    return end + 1;
}

size_t JsonObject::_compareWord(const char *data, size_t len, const char* word)
{
    size_t size = strlen(word);
    if (len < size) return 1;

    for (int i = 0; i < size; ++i) {
        if (data[i] != word[i])
            return i + 1;
    }

    char lastSymbol = data[size];

    if (lastSymbol == ' ' || lastSymbol == ',' ||
         lastSymbol == '\r' || lastSymbol == '\n') {
        return 0;
    }
    else return size;
}

bool JsonObject::isCharNumber(char symbol)
{
    for (int i=0; i<15; ++i)
        if (NUM_SYMBOLS[i] == symbol)
            return true;

    return false;
}
