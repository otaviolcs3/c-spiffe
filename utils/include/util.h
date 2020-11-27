#ifndef __INCLUDE_UTILS_UTIL_H__
#define __INCLUDE_UTILS_UTIL_H__

#include <stdbool.h>
#include <openssl/evp.h>

// typedef bool err_t;
typedef char* string_t;
typedef char** string_arr_t;
typedef char* URL_t;
typedef unsigned char byte;

typedef struct URL_t
{
    string_t scheme;
	string_t host;
    // string_t host_ip;
    // int host_exists;
	string_t path;
    string_t raw_query;
    string_t fragment;
    string_t user;
    // string_t protocol;
	string_t port;
} URL_t;

typedef struct map_string_EVP_PKEY
{
    string_t key;
    EVP_PKEY *value;
} map_string_EVP_PKEY;

enum enum_err_t
{
    NO_ERROR = 0,
    ERROR1,
    ERROR2
};

typedef enum enum_err_t err_t;

// void util_string_t_Free(string_t *strptr);
void util_string_t_Free(string_t str, bool alloc);
void util_URL_t_Free(URL_t *url, bool alloc);

#endif