#include "general.h"
#include "list.h"

#ifndef _TCDB_DRIVERS_H_
#define _TCDB_DRIVERS_H_

typedef struct TcdbInfo {
    /* Path to directory to place Tc scripts that
     * from TCDB */
    char *local_directory;
    char *dest;
    /* Tc types that defined in this DB
     * TcdbDriver should create this list after active */
    List *types;
} TcdbInfo;


struct TcdbDriver;
typedef struct TcdbType {
     int (*getTcByIdent)(struct TcdbDriver *, char *ident);
    int (*getTcsByType)(struct TcdbDriver *, char *type);
    _Bool (*isAlive)(struct TcdbDriver *, void *);
    _Bool (*active)(struct TcdbDriver *, void *);
    void (*release)(struct TcdbDriver *);
    List* (*typesOfTcs)(struct TcdbDriver *, void *);
} TcdbType;


typedef struct TcdbDriver {
    TcdbInfo info;
    TcdbType *type;
} TcdbDriver;


/* Tcdb member functions as macros */
#define tcdbInfo(db) ((db)->info)
#define getTcByIdent(db, ident) \
    ((db)->type->getTcByIdent(db, ident))
#define getTcsByTypes(db, type) \
    ((db)->type->getTcsByType(db, type))
#define isTcdbAlive(db) ((db)->type->isAlive(db))
#define typesOfTcdb(db) ((db)->type->typesOfTcs(db))
#define tcdbDriverSetDest(db, dest) ((db)->info.dest = dest)
#define tcdbDriverSetLocalDir(db, path) ((db)->info.local_directory = path)
#define tcdbDriverSetTypes(db, type) ((db)->info.types = type)


/* Prototypes */
TcdbDriver* tcdbDriverCreate(TcdbType *type);
void tcdbDriverRelease(TcdbDriver *driver);

#endif /* _TCDB_DRIVERS_H_ */
