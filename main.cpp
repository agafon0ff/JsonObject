#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include "jsonobject.h"

using namespace std;

int main()
{
    { // Serialization json-object to text

        JsonObject jsonObject;
        jsonObject.setValue("empty", JsonObject());
        jsonObject.setValue("bool", true);
        jsonObject.setValue("number", 123.456789123);
        jsonObject.setValue("text", "Hello world!");

        JsonObject jsonObectInArray;
        jsonObectInArray.setValue("number", 555.555);
        jsonObectInArray.setValue("text", "Another text");

        std::vector<JsonObject> jsonArray = {123.456, "qwe", false, jsonObectInArray};

        JsonObject jObj;
        jObj.setValue("obj", jsonObject);
        jObj.setValue("arr", jsonArray);

        // convert json-object to text
        cout << jObj.stringify() << endl;
    }

    { // Deserialization text to json-object

        std::string data = "{\"empty\": \"null\"}";

#ifdef TEST_JSON_PATH
        // Read json-file from build dir
        std::ifstream jsonFile(TEST_JSON_PATH);
        if(jsonFile) {
            ostringstream ss;
            ss << jsonFile.rdbuf();
            data = ss.str();
        }
#endif

        JsonObject jsonObject;
        // convert text to json-object

        size_t error = jsonObject.parse(data.c_str(), data.size());
        cout << "Error symbol num: " << error << endl << jsonObject.stringify() << endl;

        if (jsonObject.type() == JsonObject::JSON_ARRAY) {
            if (jsonObject.size() > 1) {

                JsonObject iObj = jsonObject.at(0);
                // Print all keys
                vector<string> keys = iObj.keys();
                for (auto &it: keys)
                    cout << "Key: " << it << endl;

                // get value
                cout << "name: " << iObj.value("name").toString() << endl;
            }
        }
    }

    return 0;
}
