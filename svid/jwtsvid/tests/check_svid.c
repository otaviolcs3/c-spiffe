#include <check.h>
#include <openssl/pem.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include "svid/jwtsvid/src/svid.h"

// Check common fields
void test_fields_verify(jwtsvid_SVID *svid, err_t err, char *token)
{
    ck_assert_uint_eq(err, NO_ERROR);
    ck_assert_ptr_eq(svid->audience, NULL);
    ck_assert_ptr_ne(svid->claims, NULL);
    ck_assert_uint_eq(shlenu(svid->claims), 4);
    ck_assert_int_ge(shgeti(svid->claims, "sub"), 0);
    ck_assert_int_ge(shgeti(svid->claims, "name"), 0);
    ck_assert_int_ge(shgeti(svid->claims, "iat"), 0);
    ck_assert_int_ge(shgeti(svid->claims, "exp"), 0);
    ck_assert_int_eq(svid->expiry, 1620000000);
    ck_assert_ptr_ne(svid->id.path, NULL);
    ck_assert_str_eq(svid->id.path, "/workload1");
    ck_assert_ptr_ne(svid->id.td.name, NULL);
    ck_assert_str_eq(svid->id.td.name, "example.com");
    ck_assert_ptr_ne(svid->token, NULL);
    ck_assert_str_eq(svid->token, token);
}

/*
Each test named 'test_jwtsvid_<function name>' tests
jwtsvid_<function name> function.
*/

// precondition: valid jwt token
// postcondition: valid jwt svid corresponding to the
// token without claims map
START_TEST(test_jwtsvid_parse)
{
    // spiffeid_TrustDomain td = {"example.com"};
    // jwtbundle_Bundle *bundle = jwtbundle_Load(td,
    // "./resources/jwk_keys.json", &err);

    char token[]
        = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW1lIjoiSm9"
          "obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTYyMDAwMDAwMH0."
          "sbbqzgX6d3gH2O2tBAHdmehfHBv3QH29WOIDrPmuyOl6FfFxJaBmo6D3jX3Fm7_"
          "Wh0gM7GagbC5hkPBKZlUYR-DYg5lvp9QbHP9r1BLIqB-zfhHGYgfq_"
          "cbCh0ud1ytv9AjQw9k1oUyJUZfkB8kC1IfTZPVQQIgnKFeauT3lmPxIpEjueyn-"
          "98Qbbnv705wKlrU0KMGK7ac1Sj78yclqdmcfnT7oEE8zDdSs27Uh4lEIsO58zW6fEe_"
          "NE_M6BnaubI35eOoegwSkfCWT54fWa8jwn1OjLF_"
          "K0e5FxF4i8YJHlpY54rge6grAPAJiKKRei__-ZC8osYOEpmhGltu2BQ";
    err_t err;
    jwtsvid_SVID *svid = jwtsvid_parse(token, NULL, NULL, &err);

    ck_assert_uint_eq(err, NO_ERROR);
    ck_assert_ptr_eq(svid->audience, NULL);
    ck_assert_ptr_eq(svid->claims, NULL);
    ck_assert_int_eq(svid->expiry, 1620000000);
    ck_assert_ptr_ne(svid->id.path, NULL);
    ck_assert_str_eq(svid->id.path, "/workload1");
    ck_assert_ptr_ne(svid->id.td.name, NULL);
    ck_assert_str_eq(svid->id.td.name, "example.com");
    ck_assert_ptr_ne(svid->token, NULL);
    ck_assert_str_eq(svid->token, token);

    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: valid jwt token
// postcondition: valid jwt svid corresponding to the
// token with valid claims map
START_TEST(test_jwtsvid_ParseInsecure)
{
    char token[]
        = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW1lIjoiSm9"
          "obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTYyMDAwMDAwMH0."
          "sbbqzgX6d3gH2O2tBAHdmehfHBv3QH29WOIDrPmuyOl6FfFxJaBmo6D3jX3Fm7_"
          "Wh0gM7GagbC5hkPBKZlUYR-DYg5lvp9QbHP9r1BLIqB-zfhHGYgfq_"
          "cbCh0ud1ytv9AjQw9k1oUyJUZfkB8kC1IfTZPVQQIgnKFeauT3lmPxIpEjueyn-"
          "98Qbbnv705wKlrU0KMGK7ac1Sj78yclqdmcfnT7oEE8zDdSs27Uh4lEIsO58zW6fEe_"
          "NE_M6BnaubI35eOoegwSkfCWT54fWa8jwn1OjLF_"
          "K0e5FxF4i8YJHlpY54rge6grAPAJiKKRei__-ZC8osYOEpmhGltu2BQ";

    err_t err;
    jwtsvid_SVID *svid = jwtsvid_ParseInsecure(token, NULL, &err);

    test_fields_verify(svid, err, token);

    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: valid jwt token
// postcondition: valid jwt svid corresponding to the
// token with valid claims map and correctly verified signature
START_TEST(test_jwtsvid_ParseAndValidate)
{
    spiffeid_TrustDomain td = { "example.com" };
    jwtbundle_Bundle *bundle = jwtbundle_New(td);

    FILE *f = fopen("./resources/privkey.pem", "r");
    EVP_PKEY *pkey = PEM_read_PrivateKey(f, NULL, NULL, NULL);
    fclose(f);

    err_t err = jwtbundle_Bundle_AddJWTAuthority(
        bundle, "ff3c5c96-392e-46ef-a839-6ff16027af78", pkey);

    ck_assert_uint_eq(err, NO_ERROR);

    char token[]
        = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW1lIjoiSm9"
          "obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTYyMDAwMDAwMH0."
          "sbbqzgX6d3gH2O2tBAHdmehfHBv3QH29WOIDrPmuyOl6FfFxJaBmo6D3jX3Fm7_"
          "Wh0gM7GagbC5hkPBKZlUYR-DYg5lvp9QbHP9r1BLIqB-zfhHGYgfq_"
          "cbCh0ud1ytv9AjQw9k1oUyJUZfkB8kC1IfTZPVQQIgnKFeauT3lmPxIpEjueyn-"
          "98Qbbnv705wKlrU0KMGK7ac1Sj78yclqdmcfnT7oEE8zDdSs27Uh4lEIsO58zW6fEe_"
          "NE_M6BnaubI35eOoegwSkfCWT54fWa8jwn1OjLF_"
          "K0e5FxF4i8YJHlpY54rge6grAPAJiKKRei__-ZC8osYOEpmhGltu2BQ";
    jwtbundle_Source *source = jwtbundle_SourceFromBundle(bundle);
    jwtsvid_SVID *svid = jwtsvid_ParseAndValidate(token, source, NULL, &err);

    test_fields_verify(svid, err, token);

    jwtbundle_Source_Free(source);
    EVP_PKEY_free(pkey);
    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: valid elliptic curve jwt token
// postcondition: valid jwt svid corresponding to the
// token with valid claims map
START_TEST(test_jwtsvid_EC)
{
    spiffeid_TrustDomain td = { "example.com" };
    jwtbundle_Bundle *bundle = jwtbundle_New(td);

    FILE *f = fopen("./resources/ec-secp256k1-priv-key.pem", "r");
    ck_assert_ptr_ne(f, NULL);
    EVP_PKEY *pkey = PEM_read_PrivateKey(f, NULL, NULL, NULL);
    fclose(f);

    err_t err = jwtbundle_Bundle_AddJWTAuthority(
        bundle, "ff3c5c96-392e-46ef-a839-6ff16027af78", pkey);

    ck_assert_uint_eq(err, NO_ERROR);

    char token[]
        = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LT"
          "M5MmUtNDZlZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW"
          "1lIjoiSm9obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTYyMDAwMDAwMH0."
          "WA7x2GiNZvh5BoLkvS7BBGIHz6ULTCsX7DBJo8kDoPla4wbo4G2157WWCZ"
          "Lx6zPE8Qpvvb11kMk0Ivk_G0gMeA";
    jwtbundle_Source *source = jwtbundle_SourceFromBundle(bundle);
    jwtsvid_SVID *svid = jwtsvid_ParseAndValidate(token, source, NULL, &err);

    test_fields_verify(svid, err, token);

    jwtbundle_Source_Free(source);
    EVP_PKEY_free(pkey);
    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: valid elliptic curve jwt token
// postcondition:  valid jwt svid corresponding
// to the token with valid
START_TEST(test_jwtsvid_Marshal)
{
    spiffeid_TrustDomain td = { "example.com" };
    jwtbundle_Bundle *bundle = jwtbundle_New(td);

    FILE *f = fopen("./resources/ec-secp256k1-priv-key.pem", "r");
    ck_assert_ptr_ne(f, NULL);
    EVP_PKEY *pkey = PEM_read_PrivateKey(f, NULL, NULL, NULL);
    fclose(f);

    err_t err = jwtbundle_Bundle_AddJWTAuthority(
        bundle, "ff3c5c96-392e-46ef-a839-6ff16027af78", pkey);

    ck_assert_uint_eq(err, NO_ERROR);

    char token[]
        = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW1lIjoiSm9"
          "obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTYyMDAwMDAwMH0."
          "WA7x2GiNZvh5BoLkvS7BBGIHz6ULTCsX7DBJo8kDoPla4wbo4G2157WWCZLx6zPE8Qp"
          "vvb11kMk0Ivk_G0gMeA";
    jwtbundle_Source *source = jwtbundle_SourceFromBundle(bundle);
    jwtsvid_SVID *svid = jwtsvid_ParseAndValidate(token, source, NULL, &err);

    const char *marshal = jwtsvid_SVID_Marshal(svid);

    ck_assert_str_eq(marshal, token);

    jwtbundle_Source_Free(source);
    EVP_PKEY_free(pkey);
    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: invalid jwt token with invalid signature
// postcondition: valid jwt svid corresponding to the
// token with invalid signuture
START_TEST(test_jwtsvid_error_invalid_signature)
{
    spiffeid_TrustDomain td = { "example.com" };
    jwtbundle_Bundle *bundle = jwtbundle_New(td);

    FILE *f = fopen("./resources/ec-secp256k1-priv-key.pem", "r");
    ck_assert_ptr_ne(f, NULL);
    EVP_PKEY *pkey = PEM_read_PrivateKey(f, NULL, NULL, NULL);
    fclose(f);

    err_t err = jwtbundle_Bundle_AddJWTAuthority(
        bundle, "ff3c5c96-392e-46ef-a839-6ff16027af78", pkey);

    ck_assert_uint_eq(err, NO_ERROR);

    char token[]
        = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW1lIjoiSm9"
          "obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTYyMDAwMDAwMH0."
          "WA7x2GiNZvh5BoLkvS7BBGIHz6ULTCsX7DBJo8kDoPla4wbo4G2157WWCZLx6zPE8Qp"
          "vvb11kMk0Ivk";
    jwtbundle_Source *source = jwtbundle_SourceFromBundle(bundle);
    jwtsvid_SVID *svid = jwtsvid_ParseAndValidate(token, source, NULL, &err);

    ck_assert_uint_eq(err, ERROR5);

    jwtbundle_Source_Free(source);
    EVP_PKEY_free(pkey);
    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: invalid jwt token with subject not spifeeid
// postcondition: invalid jwt svid corresponding to the
// token with subject not spiffeeid
START_TEST(test_jwtsvid_error_subject_not_spiffeid)
{
    char token[]
        = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJodHRwOi8vZXhhbXBsZS5jb20vd29ya2xvYWQxIiwibmFtZSI6IkpvaG4"
          "gRG9lIiwiaWF0IjoxNTE2MjM5MDIyLCJleHAiOjE2MjAwMDAwMDB9."
          "cVivPwGf8M9AkHP9ekqeeWeDpeVHMV6A4TrdbHx285Ga07C4o47UHPqQvuttQACKnT7"
          "h_uhujJEFGUph0oJwqg";

    err_t err;
    jwtsvid_SVID *svid = jwtsvid_ParseInsecure(token, NULL, &err);
    ck_assert_uint_eq(err, ERROR4);
    ck_assert_ptr_eq(svid, NULL);
    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: invalid jwt token without expiration date
// postcondition: invalid jwt svid corresponding to the
// token without expiry
START_TEST(test_jwtsvid_error_without_exp)
{
    char token[] = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LT"
                   "M5MmUtNDZlZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
                   "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW"
                   "1lIjoiSm9obiBEb2UiLCJpYXQiOjE1MTYyMzkwMjJ9."
                   "tIyeP2Alj6OE1KN3sLGDA89EdkTPqoHMi1hMyARyJZ_"
                   "wpOpL57dgeeMSXWjbHYLW6tKITiOA9pcQVrCQRbcY9A";

    err_t err;
    jwtsvid_SVID *svid = jwtsvid_ParseInsecure(token, NULL, &err);
    ck_assert_uint_eq(err, ERROR3);
    ck_assert_ptr_eq(svid, NULL);
    jwtsvid_SVID_Free(svid);
}
END_TEST

// precondition: invalid jwt token without issuer, jti, audience
// postcondition: invalid jwt svid corresponding to the
// token without subject
START_TEST(test_jwtsvid_error_issuer_jti_aud)
{
    char token[]
        = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImZmM2M1Yzk2LTM5MmUtNDZ"
          "lZi1hODM5LTZmZjE2MDI3YWY3OCJ9."
          "eyJzdWIiOiJzcGlmZmU6Ly9leGFtcGxlLmNvbS93b3JrbG9hZDEiLCJuYW1lIjoiSm9"
          "obiBEb2UiLCJpc3MiOiJIUEUiLCJpYXQiOjE1MTYyMzkwMjIsImp0aSI6IkFCQ0RFRk"
          "dIIiwibmJmIjoxNDAwMDAwMDAwLCJhdWQiOlsiYXVkMSIsImF1ZDIiXX0."
          "afcc79kL3JYqwrh1ABQe7ogCy-WxWGb-G2YdmIKIeej3RMT-usawYCC8c1OH30Fi-_"
          "ygfzk_qLJnOs1t30tcEQ";

    err_t err;
    jwtsvid_SVID *svid = jwtsvid_ParseInsecure(token, NULL, &err);
    ck_assert_uint_eq(err, ERROR3);
    ck_assert_ptr_eq(svid, NULL);
    jwtsvid_SVID_Free(svid);
}
END_TEST

Suite *svid_suite(void)
{
    Suite *s = suite_create("svid");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_jwtsvid_parse);
    tcase_add_test(tc_core, test_jwtsvid_ParseInsecure);
    tcase_add_test(tc_core, test_jwtsvid_ParseAndValidate);
    tcase_add_test(tc_core, test_jwtsvid_EC);
    tcase_add_test(tc_core, test_jwtsvid_Marshal);
    tcase_add_test(tc_core, test_jwtsvid_error_invalid_signature);
    tcase_add_test(tc_core, test_jwtsvid_error_subject_not_spiffeid);
    tcase_add_test(tc_core, test_jwtsvid_error_without_exp);
    tcase_add_test(tc_core, test_jwtsvid_error_issuer_jti_aud);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    Suite *s = svid_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    const int number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}