/* Vincent Foley-Bourgon (FOLV08078309)
 * Eric Thivierge        (THIE09016601) */

/* sema.c --- Functions to manipulate semaphores.

This file is part of Kaya OS.
Kaya OS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.  See <http://www.gnu.org/licenses/>.  */

#include "proc.h"
#include "sema.h"

/* Semaphore descriptor.  */
struct semd {
   semd_t *s_next;		/* Next element on the ASL.  */
   int     s_value;		/* Current value of the semaphore.  */
   pcbq_t *s_procQ;		/* Queue of blocked processes.  */
};

/* The list of active semaphores,
   i.e. semaphores on which some process is blocked.  */
static semd_t *ASL = NULL;
static semd_t *semdFree;

static semd_t SEMA_POOL[MAXPROC];


void initASL(void) {
    int i;

    for (i = 0; i < MAXPROC - 1; ++i) {
        SEMA_POOL[i].s_next = &SEMA_POOL[i];
        SEMA_POOL[i].s_value = 0;
        SEMA_POOL[i].s_procQ = mkEmptyProcQ();
    }

    SEMA_POOL[MAXPROC-1].s_next = NULL;
    SEMA_POOL[MAXPROC-1].s_value = 0;
    SEMA_POOL[MAXPROC-1].s_procQ = mkEmptyProcQ();

    semdFree = &SEMA_POOL[0];
}


void initSemD (semd_t *s, int val) {
    s->s_value = val;
}


void insertBlocked (semd_t *s, pcb_t *p) {
    if (emptyProcQ(s->s_procQ)) {
        semd_t *curr = ASL;
        semd_t *prev = NULL;
        while (curr != NULL && curr->s_value < s->s_value) {
            curr = curr->s_next;
            if (prev == NULL)
                prev = ASL;
            else
                prev = prev->s_next;
        }

        if (curr == NULL) {
            prev->s_next = s;
            s->s_next = NULL;
        }
        else if (prev == NULL) {
            s->s_next = ASL;
            ASL = s;
        }
        else {
            s->s_next = curr;
            prev->s_next = s;
        }
    }

    insertProcQ(&s->s_procQ, p);
}



pcb_t *removeBlocked (semd_t *s) {
    pcb_t *p = removeProcQ(&s->s_procQ);

    if (emptyProcQ(s->s_procQ)) {
        semd_t *curr = ASL;
        semd_t *prev = NULL;
        while (curr != NULL && curr != s) {
            curr = curr->s_next;
            if (prev == NULL)
                prev = ASL;
            else
                prev = prev->s_next;
        }

        if (prev == NULL) {
            ASL = curr->s_next;
        }
        else if (curr != NULL) {
            prev->s_next = curr->s_next;
        }
    }

    return p;
}


pcb_t *outBlocked (pcb_t *p) {
    pcb_t *ret = NULL;
    semd_t *curr = ASL;
    semd_t *prev = NULL;

    while (curr != NULL) {
        ret = outProcQ(&curr->s_procQ, p);
        if (ret != NULL)
            break;
        curr = curr->s_next;
        if (prev == NULL)
            prev = ASL;
        else
            prev = prev->s_next;
    }

    if (ret != NULL) {
        if (emptyProcQ(curr->s_procQ)) {
            if (prev == NULL) {
                ASL = curr->s_next;
            }
            else if (curr != NULL) {
                prev->s_next = curr->s_next;
            }
        }
    }

    return ret;
}



pcb_t *headBlocked (semd_t *s) {
    return headProcQ(s->s_procQ);
}
