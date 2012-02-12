/* Vincent Foley-Bourgon (FOLV08078309)
 * Eric Thivierge        (THIE09016601) */

/* proc.h --- Functions to manipulate processes.

This file is part of Kaya OS.
Kaya OS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.  See <http://www.gnu.org/licenses/>.  */

#ifndef PROC_H
#define PROC_H

#define NULL ((void*)(0))

/* Since we don't have the C library at hand, we don't have malloc and
   friends, so we will have to make do with a hardcoded limit on the maximum
   number of processes.  */
#define MAXPROC 20


/* The type of process objects.  */
typedef struct pcb pcb_t;
/* Process queues are represented by one of their members.  */
typedef pcb_t pcbq_t;

typedef struct semd semd_t;

/****** General creation destruction of process objects.  ******/

/* Initialize the process handling module.  */
void initProc (void);
/* Allocate a new process.  Return NULL if there is no PCB left.  */
pcb_t *allocPcb (void);
/* Free a process.  */
void freePcb (pcb_t *p);

/****** Manipulating queues of processes.  ******/

/* Return a pointer to the tail of an empty process queue; i.e. NULL.  */
pcbq_t *mkEmptyProcQ (void);

/* Return TRUE iff the queue whose tail is pointed to by `pq'' is empty.  */
int emptyProcQ (pcbq_t *pq);

/* Insert the process pointed to by `p' into the process queue whose
   tail-pointer is pointed to by `pqp'.  Note the double indirection through
   `pqp' to allow for the possible updating of the tail pointer as well.  */
void insertProcQ (pcbq_t **pqp, pcb_t *p);

/* Remove the first (i.e. head) element from the process queue whose
   tail-pointer is pointed to by `pqp'.  Return NULL if the process queue
   was empty; otherwise return the pointer to the removed element.  */
pcb_t *removeProcQ (pcbq_t **pqp);

/* Remove the process `p' from the process queue whose tail-pointer
   is pointed to by `pqp'.  If the desired entry is not in the indicated queue
   (an error condition), return NULL; otherwise, return `p'.  */
pcb_t *outProcQ (pcbq_t **pqp, pcb_t *p);

/* Return a pointer to the first process from the process queue `pq'.
   Return NULL if the process queue is empty. */
pcb_t *headProcQ (pcbq_t *pq);


/****** Manipulating trees of processes.  ******/

/* Return TRUE iff the process `p' has no children.  */
int emptyChild (pcb_t *p);

/* Make the process `child' a child of the process `parent'.  */
void insertChild (pcb_t *parent, pcb_t *child);

/* Make the first child of the process `p' no longer a child of `p'.
   Return NULL if there were no children of `p'.
   Otherwise, return a pointer to this removed child. */
pcb_t *removeChild (pcb_t *p);

/* Make the process `p' no longer the child of its parent.
   If the process `p' has no parent, return NULL; otherwise, return `p'.  */
pcb_t *outChild (pcb_t *p);




#ifdef DEBUG

pcb_t *getFreeProcess(int);
int getMaxProcess(void);
pcb_t *getFreeProcess(int);
pcb_t *getPNext(pcb_t *);
pcb_t *getPParent(pcb_t *);
pcb_t *getPChild(pcb_t *);
pcb_t *getPSib(pcb_t *);
semd_t *getPSema(pcb_t *);
int getFreeProcessCount(void);


#endif




#endif
