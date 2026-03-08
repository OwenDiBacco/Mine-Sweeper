// Header
#include "../lib/cjson/cjson.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Represents a dynamic memory buffer to store the HTTP response
struct Memory
{
    char* memory;
    size_t size;
};

// Appends received chunks to the dynamic memory buffer
static size_t dbGetResponse (void* content, size_t size, size_t chunkCount, void* chunk)
{
    // Calculates the size of the chunk
    size_t chunkSize = size * chunkCount;
    struct Memory* memory = chunk;

    // Re-allocates the size of the memory buffer
    char* ptr = realloc (memory->memory, memory->size + chunkSize + 1);
    if (ptr == NULL) return 0;
    memory->memory = ptr;

    // Appends the content of the chunk to the memory buffer
    memcpy (&(memory->memory[memory->size]), content, chunkSize);
    memory->size += chunkSize;
    memory->memory [memory->size] = 0;

    return chunkSize;
}

static size_t dbSendRequest (const char* payload, char** response)
{

    // Creates a handle for the network transfer
    CURL* curl = curl_easy_init ();
    if(!curl) return -1;

    // Creates the header for the network transfer
    struct curl_slist* headers = NULL;
    headers = curl_slist_append (headers, "Content-Type: application/json");

    // Initializes the dynamic memory buffer
    struct Memory chunk;
    chunk.memory = malloc (1);
    chunk.size = 0;

    const char* API_GATEWAY_URL = "https://4glr37oafk.execute-api.us-east-1.amazonaws.com/dev/score";

    // Sets the configuration options for curl
    curl_easy_setopt (curl, CURLOPT_URL, API_GATEWAY_URL);
    curl_easy_setopt (curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt (curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt (curl, CURLOPT_POST, 1L);
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, dbGetResponse);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) &chunk);

    // Performs the network transfer
    CURLcode result = curl_easy_perform (curl);

    curl_slist_free_all (headers);
    curl_easy_cleanup (curl);

    *response = chunk.memory;

    return result;
}

ssize_t dbSetHighScore (char* name, size_t score)
{
    char* response;

    char* payload = malloc ( 64 + strlen (name));
    if (payload == NULL)
        return -1;

    sprintf (payload, "{\"action\":\"put\",\"score\":%zu,\"name\":\"%s\"}", score, name);

    if (dbSendRequest (payload, &response) == -1)
        return -1;

    free (response);
    free (payload);

    return 0;
}

ssize_t dbGetHighScore (char** name, size_t* score)
{
    char* response;
    char* payload = "{\"action\":\"get\"}";

    if (dbSendRequest (payload, &response) == -1)
        return -1;

    // Parses response with the cJson library
    cJSON* json = cJSON_Parse (response);
    size_t size = cJSON_GetArraySize ( json);

    if (size == 0)
    {
        *name = "";
        *score = 0;
        return -1;
    }

    cJSON* last = cJSON_GetArrayItem (json, size - 1);

    cJSON* nameFeild = cJSON_GetObjectItemCaseSensitive (last, "name");
    cJSON* scoreFeild = cJSON_GetObjectItemCaseSensitive (last, "score");

    *name = strdup (nameFeild->valuestring);
    *score = scoreFeild->valueint;

    cJSON_Delete (json);
    free (response);

    return 0;
}
