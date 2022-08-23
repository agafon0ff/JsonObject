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

#pragma once

#include <string>
#include <vector>
#include <map>


/// \brief The JsonObject class implements serialization and
/// deserialization of JSON-formatted text.
class JsonObject
{
public:

    /// \brief The Type enum describes the state of the object's content.
    /// to get type use method: type();
    enum Type
    {
        JSON_NULL,      /// 'null'
        JSON_BOOL,      /// 'true' or 'false'
        JSON_NUMBER,    /// such numbers as: 123, 0.56, -5, 1.234e-23
        JSON_STRING,    /// "text in double quotes"
        JSON_ARRAY,     /// array of JsonObjects [obj1, obj2, obj3, ...]
        JSON_OBJECT,    /// sequence of key-value pairs: {"key1": obj1, "key2": obj2 ...}
        JSON_ERROR      /// parsing error
    };

    /// \brief The StringifyMode enum describes text representation mode
    enum StringifyMode
    {
        MODE_COMPACT = 0,   /// without any spaces and new lines
        MODE_2_SPACES = 2,  /// 2 spaces indent and new lines
        MODE_4_SPACES = 4   /// 4 spaces indent and new lines
    };

    /// \brief JsonObject Creates an object with the appropriate content:
    JsonObject();                                       /// 'null' content
    JsonObject(bool value);                             /// 'true' or 'false'
    JsonObject(int value);                              /// integer
    JsonObject(double value, uint16_t precision = 10);  /// double and number of decimal places
    JsonObject(const char* value);                      /// text
    JsonObject(const std::string &value);               /// text
    JsonObject(std::string &&value);                    /// text
    JsonObject(const std::vector<JsonObject> &value);   /// array of JsonObjects
    JsonObject(std::vector<JsonObject> &&value);        /// array of JsonObjects

    /// \brief parse - Converts text to JsonObject
    /// \param data - pinter to the beginning of the text array
    /// \param len - text size
    /// \return returns 0 if success, otherwise parsing error character index
    size_t parse(const char *data, size_t len);

    /// \brief parse - Converts text to JsonObject
    /// \param data - text array
    /// \return returns 0 if success, otherwise parsing error character index
    size_t parse(const std::string &data);

    /// \brief stringify - Converts JsonObject to text
    /// \param mode - StringifyMode describes text representation mode
    /// \return convertation result
    std::string stringify(JsonObject::StringifyMode mode = MODE_2_SPACES);

    /// \brief type - returns type of the content.
    JsonObject::Type type();

    /// \brief keys - returns array of keys if type is JSON_OBJECT
    std::vector<std::string> keys();

    /// \brief exist - returns 'true' if given key is exist in object
    bool exist(const char* key);
    bool exist(const std::string &key);

    /// \brief value - returns JsonObject if key exist, otherwise JsonObject with type JSON_NULL
    JsonObject value(const char* key);
    JsonObject value(const std::string &key);

    /// \brief setValue - add key-value pair to JsonObject
    /// convert oblect to JSON_OBJECT type if it's not, with loss of previous data
    void setValue(const char* key, const JsonObject &value);
    void setValue(const std::string &key, const JsonObject &value);

    /// \brief append - add value to JsonObject if type is JSON_ARRAY
    /// convert oblect to JSON_ARRAY if it's not, with loss of previous data
    void append(const JsonObject &value);

    /// \brief at - returns JsonObject by index if type is JSON_ARRAY
    /// otherwise JsonObject with 'null'
    JsonObject at(size_t index);

    /// \brief size - returns the number of stored elements if type is JSON_OBJECT or JSON_ARRAY
    size_t size();

    /// \brief clear - remove all contained data and set type to JSON_NULL
    void clear();

    /// \brief remove - remove contained value with given key if type is JSON_OBJECT
    void remove(const char* key);
    void remove(const std::string &key);

    /// \brief toBool - returns contained value if type is JSON_BOOL
    bool toBool(bool defVal = false);

    /// \brief toNumber - returns contained value if type is JSON_NUMBER
    double toNumber(double defVal = 0.);

    /// \brief toNumber - returns contained value if type is JSON_STRING
    std::string toString(const std::string defVal = "");

    /// \brief toNumber - returns contained value if type is JSON_ARRAY
    std::vector<JsonObject> toArray();

private:
    std::string m_value;
    std::vector<JsonObject> m_array;

    JsonObject::Type m_type = JsonObject::JSON_NULL;
    std::map<std::string, JsonObject> m_map;
    std::vector<std::string> m_keys;

    std::string _stringify(size_t indent, JsonObject::StringifyMode mode);
    size_t _parseObject(const char* data, size_t len, size_t &end, JsonObject& obj);
    size_t _parseArray(const char* data, size_t len, size_t &end, JsonObject &obj);
    size_t _parseText(const char* data, size_t len, size_t &end);
    size_t _parseNumber(const char* data, size_t len, size_t &end);
    size_t _compareWord(const char *data, size_t len, const char* word);
    bool isCharNumber(char symbol);
};
