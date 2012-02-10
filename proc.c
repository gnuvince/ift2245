/* Vincent Foley-Bourgon (FOLV08078309)
 * Eric Thivierge        (THIE09016601) */

/* proc.c --- Functions to manipulate processes.

   This file is part of Kaya OS.
   Kaya OS is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.  See <http://www.gnu.org/licenses/>.  */

#include "proc.h"
#include "sema.h"

/* Process Control Block.  */
struct pcb {
    /* Process queue fields.  */
    pcb_t   *p_next;                      /* Pointer to next entry.  */

    /* Process tree fields.  */
    pcb_t   *p_parent;                /* Pointer to parent.  */
    pcb_t   *p_child;               /* Pointer to first child.  */
    pcb_t   *p_sib;                     /* Pointer to sibling.  */

    /* Semaphore fields.  */
    semd_t  *p_sema;  /* Pointer to semaphore on which process is blocked.  */

    /* ...other fields will come later... */
};




/* Array of MAXPROC pcb's. */
static pcb_t PROCESS_POOL[MAXPROC];

/* Pointer to the head of free (unused) pcb linked list. */
static pcb_t *pcb_free_h;





/* Create the linked list of unused pcb's.' */
void initProc (void) {
    int i;

    for (i = 0; i < MAXPROC-1; ++i)
        PROCESS_POOL[i].p_next = &PROCESS_POOL[i+1];
    PROCESS_POOL[MAXPROC-1].p_next = NULL;
    pcb_free_h = &PROCESS_POOL[0];
}

/* Return a pcb to the unused pcb list */
void freePcb(pcb_t *p) {
    if (p == NULL)
        return;
    p->p_next = pcb_free_h;
    pcb_free_h = p;
}


/* Allocate a new process.  Return NULL if there is no PCB left.  */
pcb_t *allocPcb(void) {
    pcb_t *p;

    if (pcb_free_h == NULL)
        return NULL;

    /* Obtain the pcb at the head of the unused pcb list and return it.
     * Change the head of unused pcb list to be the next free pcb. */
    p = pcb_free_h;
    pcb_free_h = pcb_free_h->p_next;

    p->p_next   = NULL;
    p->p_parent = NULL;
    p->p_child  = NULL;
    p->p_sib    = NULL;
    p->p_sema   = NULL;

    return p;
}





/* An empty queue is simply a null pointer. */
pcbq_t *mkEmptyProcQ(void) {
    return NULL;
};


/* A queue is empty if it points to null. */
int emptyProcQ(pcbq_t *pq) {
    return NULL == pq;
}


/* To insert a process in a queue: if the queue is empty, the queue is
 * the process pointing to itself, otherwise the process will insert
 * itself between the tail and its previous neighbor. */
void insertProcQ(pcbq_t **pqp, pcb_t *p) {
    if (pqp == NULL || p == NULL) {
        return;
    }
    else if (emptyProcQ(*pqp)) {
        p->p_next = p;
        *pqp = p;
    }
    else {
        p->p_next = (*pqp)->p_next;
        (*pqp)->p_next = p;
    }
}


/* To remove the head from a queue, call outProcQ. */
pcb_t *removeProcQ(pcbq_t **pqp) {
    if (pqp == NULL)
        return NULL;

    return outProcQ(pqp, *pqp);
}



/* Remove a given pcb from the pcb queue.  Return null if pcb does not
 * exist. */
pcb_t *outProcQ(pcbq_t **pqp, pcb_t *p) {
    pcb_t *prev;
    pcb_t *curr;

    if (pqp == NULL || emptyProcQ(*pqp) || p == NULL)
        return NULL;

    prev = *pqp;
    curr = *pqp;

    /* Try to find p in pqp. */
    while (curr != p) {
        curr = curr->p_next;

        /* If curr == *pqp, then we have looped and p is not in
         * pqp. */
        if (curr == *pqp)
            return NULL;
    }


    /* Get the element before p. */
    while (prev->p_next != p)
        prev = prev->p_next;

    /* Update the queue. */
    if (curr == prev) {
        /* Queue has a single element. */
        *pqp = mkEmptyProcQ();
    }
    else {
        prev->p_next = p->p_next;
        *pqp = prev;
    }

    return p;
}


/* Return the next element to be popped from the list. */
pcb_t *headProcQ(pcbq_t *pq) {
    if (pq == NULL)
        return NULL;
    else
        return pq;
}


/* Return TRUE iff the process `p' has no children.  */
int emptyChild(pcb_t *p) {
    return p != NULL && p->p_child == NULL;
}


/* Insert a new child at the head of the list of children of parent. */
void insertChild(pcb_t *parent, pcb_t *child) {
    if (parent == NULL || child == NULL || child->p_parent != NULL)
        return;

    if (parent->p_child != NULL) {
        child->p_sib = parent->p_child;
    }

    child->p_parent = parent;
    parent->p_child = child;
}


/* Remove the first child of p.  If this child has children, remove
 * them all (apply this operation recursively).*/
pcb_t *removeChild(pcb_t *p) {
    pcb_t *child;

    if (p == NULL || emptyChild(p))
        return NULL;


    child = p->p_child;
    p->p_child = child->p_sib;
    child->p_parent = NULL;
    child->p_sib = NULL;

    /* If child has children of his own, remove them. */
    while (!emptyChild(child))
        removeChild(child);

    return child;
}

pcb_t *outChild(pcb_t *p) {
    pcb_t *prev;

    if (p == NULL || p->p_parent == NULL)
        return NULL;


    if (p->p_parent->p_child == p) {
        p->p_parent->p_child = NULL;
        p->p_parent = NULL;
        return p;
    }
    else {
        prev = p->p_parent->p_child;
        while (prev->p_sib != NULL || prev->p_sib != p)
            prev = prev->p_sib;

        if (prev->p_sib == NULL) {
            return NULL;
        }

        p->p_parent = NULL;
        prev->p_sib = p->p_sib;
        return p;
    }
}




/* Functions for debugging and testing. */
#ifdef DEBUG

pcb_t *getFreeProcess(int i) {
    if (i < 0 || i >= MAXPROC)
        return NULL;
    return &PROCESS_POOL[i];
}


pcb_t *getPNext(pcb_t *p) { return p->p_next; }
pcb_t *getPParent(pcb_t *p) { return p->p_parent; }
pcb_t *getPChild(pcb_t *p) { return p->p_child; }
pcb_t *getPSib(pcb_t *p) { return p->p_sib; }
pcb_t *getPSema(pcb_t *p) { return p->p_sema; }
int getFreeProcessCount(void) {
    int count = 0;
    pcb_t *curr = pcb_free_h;
    while (curr != NULL) {
        count++;
        curr = curr->p_next;
    }
    return count;
}

#endif
