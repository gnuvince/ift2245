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


enum semd_state { ST_FREE, ST_ACQUIRED, ST_ASL };

/* Semaphore descriptor.  */
struct semd {
    semd_t *s_next;		/* Next element on the ASL.  */
    int     s_value;		/* Current value of the semaphore.  */
    pcbq_t *s_procQ;		/* Queue of blocked processes.  */

    enum semd_state s_state;
};

/* The list of active semaphores,
   i.e. semaphores on which some process is blocked.  */
static semd_t *ASL;
static semd_t *semdFree;

static semd_t SEMA_POOL[MAXPROC];


void initASL(void) {
    int i;

    for (i = 0; i < MAXPROC - 1; ++i) {
        SEMA_POOL[i].s_next = &SEMA_POOL[i+1];
        SEMA_POOL[i].s_state = ST_FREE;
    }

    SEMA_POOL[MAXPROC-1].s_next = NULL;
    SEMA_POOL[MAXPROC-1].s_state = ST_FREE;

    semdFree = &SEMA_POOL[0];
    ASL = NULL;
}

/* Take a semd from semdFree and "give it" to s.  Return 0 if semd is
 * empty. */
int initSemD (semd_t **s, int val) {
    if (semdFree != NULL) {
        /* Get a semd from the free list. */
        *s = semdFree;
        semdFree = semdFree->s_next;

        /* Initialize it. */
        (*s)->s_procQ = mkEmptyProcQ();
        (*s)->s_value = val;
        (*s)->s_next = NULL;
        (*s)->s_state = ST_ACQUIRED;
        return 1;
    }
    return 0;
}


/* Insert the pcb p into s's procQ.  If s was not in the ASL, insert
 * it by order of its value field. */
void insertBlocked (semd_t *s, pcb_t *p) {
    if (s == NULL || p == NULL || s->s_state == ST_FREE)
        return;

    if (s->s_state == ST_ACQUIRED) {
        semd_t *curr = ASL;
        semd_t *prev = NULL;

        /* Find which ASL node is going to be s's left neighbor. */
        while (curr != NULL && curr->s_value < s->s_value) {
            curr = curr->s_next;
            if (prev == NULL)
                prev = ASL;
            else
                prev = prev->s_next;
        }

        /* s is inserted at the beginning of the ASL. */
        if (prev == NULL) {
            s->s_next = ASL;
            ASL = s;
        }
        /* s is inserted at the end of the ASL. */
        else if (curr == NULL) {
            prev->s_next = s;
            s->s_next = NULL;
        }
        /* s is inserted in the middle of the ASL. */
        else {
            s->s_next = curr;
            prev->s_next = s;
        }

        s->s_state = ST_ASL;
    }

    /* Add the process p to s's procQ. */
    insertProcQ(&s->s_procQ, p);
}



/* Remove the head process from s's procQ and return it. */
pcb_t *removeBlocked (semd_t *s) {
    if (s == NULL || s->s_state == ST_FREE || s->s_state == ST_ACQUIRED)
        return NULL;

    pcb_t *p = removeProcQ(&s->s_procQ);

    /* Remove s from ASL if its procQ is now empty. */
    if (s->s_state == ST_ASL && emptyProcQ(s->s_procQ)) {
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

        /* Return s to the semdFree list. */
        s->s_next = semdFree;
        s->s_state = ST_FREE;
        semdFree = s;
    }

    return p;
}


/* Given a process, remove it from its semaphore's queue and return
 * it. */
pcb_t *outBlocked (pcb_t *p) {
    pcb_t *ret = NULL;
    semd_t *curr = ASL;
    semd_t *prev = NULL;

    /* Find the semaphore containing p. */
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

    /* Remove p's containing semaphore from ASL if its procQ is now empty. */
    if (ret != NULL) {
        if (curr->s_state == ST_ASL && emptyProcQ(curr->s_procQ)) {
            if (prev == NULL) {
                ASL = curr->s_next;
            }
            else if (curr != NULL) {
                prev->s_next = curr->s_next;
            }
        }
        curr->s_next = semdFree;
        semdFree = curr;
        curr->s_state = ST_FREE;
    }

    return ret;
}


/* Return the process at the head of s's procQ. */
pcb_t *headBlocked (semd_t *s) {
    if (s == NULL)
        return NULL;

    return headProcQ(s->s_procQ);
}




#ifdef DEBUG
semd_t *getSema(int i) {return &SEMA_POOL[i];}
semd_t *getASL(void) {return ASL;}
semd_t *getSemdFree(void) {return semdFree;}
semd_t *getSNext(semd_t *s) { return s->s_next; }
int     getSValue(semd_t *s) { return s->s_value; }
pcbq_t *getSProcQ(semd_t *s) { return s->s_procQ; }
#endif
