#include "../include/trustdomain.h"

spiffeid_TrustDomain spiffeid_TrustDomainFromString(string_t str, err_t *err)
{
    const char spiffe_scheme[] = "spiffe://";
    const size_t spiffe_scheme_len = sizeof spiffe_scheme - 1;

    if(!string_contains(str, "://"))
    {
        //inserts spiffe scheme on the string beginning
        arrinsn(str, 0, spiffe_scheme_len);
        strncpy(str, spiffe_scheme, spiffe_scheme_len);
    }

    err_t err2;
    spiffeid_ID id = spiffeid_FromString(str, &err2);

    if(err2)
    {
        *err = err2;
        arrfree(id.path);

        return id.td;
    }
}

#if !__TRUSTDOMAIN_BY_POINTER__
const string_t spiffeid_TrustDomain_String(const spiffeid_TrustDomain td)
{
    return td.name;
}

spiffeid_ID spiffeid_TrustDomain_ID(const spiffeid_TrustDomain td)
{
    // spiffeid_ID id = {NULL, NULL};

    // id.td.name = string_push(id.td.name, td.name);
    // id.path = string_push(id.path, "");

    return (spiffeid_ID){
        .td = (spiffeid_TrustDomain){
            string_push(NULL, td.name)
        }, 
        .path = string_push(NULL, "")
    };
}

string_t spiffeid_TrustDomain_IDString(const spiffeid_TrustDomain td)
{
    spiffeid_ID id = spiffeid_TrustDomain_ID(td);
    string_t str = spiffeid_ID_String(id);
    spiffeid_ID_Free(&id, false);

    return str;
}

spiffeid_ID spiffeid_TrustDomain_NewID(const spiffeid_TrustDomain td, const string_t path)
{
    // spiffeid_ID id = {NULL, NULL};
    // id.td.name = string_push(id.td.name, td.name);
    // id.path = string_push(id.path, path);

    return (spiffeid_ID){
        .td = (spiffeid_TrustDomain){
            string_push(NULL, td.name)
        }, 
        .path = string_push(NULL, path)
    };
}

bool spiffeid_TrustDomain_IsZero(const spiffeid_TrustDomain td)
{
    return empty_str(td.name);
}

int spiffeid_TrustDomain_Compare(const spiffeid_TrustDomain td1, const spiffeid_TrustDomain td2)
{
    return strcmp(td1.name, td2.name);
}
#else

#endif

void spiffeid_TrustDomain_Free(spiffeid_TrustDomain *td, bool alloc)
{
    if(td)
    {
        arrfree(td->name);
        if(alloc)
            free(td);
    }
}