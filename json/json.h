/*
 * C based JSON Serializer/Deserializer
 *
 *  Limited in size because of the lack of OO nature of C *
 *
 */
#ifndef __JSON_H_
#define __JSON_H_


#include "CustomStructs/KeyValuePair.h"



/*Would be an example of a JSON formated as:
 * {
 * 	"testParent":{
 * 	"testChild1":"testValue",
 * 	"testChild2":"testValue",
 * 	"testChild3":"testValue",
 * 	"testChild4":"testValue",
 * 	(etc....)
 * 	}
 *
 *
 * }
 */
typedef struct{
	char *key;
	KeyValuePair_String_String value[];
}JObject_String__String;




/*Would be an example of a JSON formated as:
 * {
 * 	"testParent":[
 * 	"testValue1",
 * 	"testValue2",
 * 	"testValue3",
 * 	"testValue4",
 * 	(etc....)
 * 	]
 * }
 */
struct JArray{
	char *key;
	void *value[];
};






#endif


