
#ifndef phreadTest__h
#define phreadTest__h

typedef struct pCommonSt
{
    int age;
    char *name;
} pCommon;

typedef struct arrTestSt {
    void *handle;
    void *private_data;
    char *plugin_name;
} arrTestSt;

static void *original_owner_thread(void *ptr);
static void *bad_thread(void *ptr);
static void *second_thread(void *ptr);
int phread_main();

#endif
