#include <stdlib.h>
#include "parson.h"
#include "parson.h"
#include "message.h"

char** message_deserializing(char *message) {
    JSON_Value *value = json_parse_string(message);
    JSON_Array *array = json_array(value);
    int fields = json_array_get_count(array);

    int i;
    char** contents = (char**) malloc(MESSAGE_MAX_FIELDS * sizeof (char**));
    for (i = 0; i <= fields - 1; i++) {
        contents[i] = (char*) json_array_get_string(array, i);
    }
    return contents;
}

char* message_serializing(int fields, char* contents[]) {
    if (fields == 0) return NULL;

    JSON_Value *values = json_value_init_array();
    JSON_Array *array = json_value_get_array(values);
    int i;
    for (i = 0; i < fields; i++) {
        char* c = contents[i];
        json_array_append_string(array, contents[i]);
    }

    return json_serialize_to_string(values);
}
