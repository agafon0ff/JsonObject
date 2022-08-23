# JsonObject

Class implements serialization and deserialization of JSON-formatted text.

### Serialization json-object to text:
```Java
JsonObject jsonObject;
jsonObject.setValue("bool", true);
jsonObject.setValue("number", 123.4567);
jsonObject.setValue("text", "Hello world!");
cout << jsonObject.stringify(JsonObject::MODE_COMPACT) << endl;
// {"bool":true,"number":123.4567000000,"text":"Hello world!"}
```

### Deserialization text to json-object:
```Java
string data = "{\"bool\": false, \"num\":123.457, \"name\":\"Jane\"}";
JsonObject jsonObject;
jsonObject.parse(data.c_str(), data.size());
string name = jsonObject.value("name").toString();
double num = jsonObject.value("num").toNumber();
cout << "name: " << name << ", num: " << num << endl;
// name: Jane, num: 123.457
```
