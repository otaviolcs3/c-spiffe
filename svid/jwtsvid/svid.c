#include "svid/jwtsvid/svid.h"
#include "bundle/jwtbundle/source.h"
#include <cjose/cjose.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>

// one minute leeway
const time_t DEFAULT_LEEWAY = 60L;

static void jwtsvid_JWT_Free(jwtsvid_JWT *jwt)
{
    if(jwt) {
        free(jwt->header);
        free(jwt->payload);
        arrfree(jwt->header_str);
        arrfree(jwt->payload_str);
        arrfree(jwt->signature);

        free(jwt);
    }
}

const char *jwtsvid_SVID_Marshal(jwtsvid_SVID *svid)
{
    if(svid)
        return svid->token;
    return NULL;
}

void jwtsvid_SVID_Free(jwtsvid_SVID *svid)
{
    if(svid) {
        // free spiffe id
        spiffeid_ID_Free(&(svid->id));
        // free array of strings
        util_string_arr_t_Free(svid->audience);
        // free each json object
        for(size_t i = 0, size = shlenu(svid->claims); i < size; ++i) {
            free(svid->claims[i].value);
        }
        shfree(svid->claims);
        // free token string
        arrfree(svid->token);
    }
}
