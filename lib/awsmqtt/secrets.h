#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "Random"                     //change this

const char AWS_IOT_ENDPOINT[] = "INSERT YOUR AWS END-POINT";       //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
        INSERT 
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
        INSERT
-----END CERTIFICATE-----
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
        INSERT
-----END RSA PRIVATE KEY-----
)KEY";