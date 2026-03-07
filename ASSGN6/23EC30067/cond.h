#ifndef CONDITION_VARIABLE_HEADER
#define CONDITION_VARIABLE_HEADER

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Structure definition
typedef struct _cond_t {
    // Semaphore at index 0 is mtx and semaphore at index 1 is cnd
    int semid;
    int shmid;
}cond_t;

// Function declarations
cond_t cond_create(key_t token1, key_t token2);
void cond_init(cond_t CV);
void cond_lock(cond_t CV);
void cond_unlock(cond_t CV);
void cond_wait(cond_t CV);
void cond_signal(cond_t CV);
void cond_broadcast(cond_t CV);
void cond_destroy(cond_t CV);

#ifdef __cplusplus
}
#endif

#endif