#include "tcdb_drivers.h"
#include <malloc.h>


TcdbDriver* tcdbDriverCreate(TcdbType *type) {
    TcdbDriver *driver = (TcdbDriver *)calloc(1, sizeof(TcdbDriver));
    driver->type = type;

    return driver;
}


void tcdbDriverRelease(TcdbDriver *driver) {
    /* Cause driver->type is static
     * so no need to free it */

    void (*release)(TcdbDriver *);
    release = driver->type->release;

    /* First, do type dependently release */
    if (release != NULL) {
        release(driver);
    }

    /* Then release driver itself */
    free(driver);
}
