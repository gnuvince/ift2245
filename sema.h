/* Vincent Foley-Bourgon (FOLV08078309)
 * Eric Thivierge        (THIE09016601) */

/* sema.h --- Functions to manipulate semaphores.

This file is part of Kaya OS.
Kaya OS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.  See <http://www.gnu.org/licenses/>.  */

#ifndef SEMA_H
#define SEMA_H

typedef struct pcb pcb_t;	/* Copied from proc.h.  */

/* The type of semaphore objects.  */
typedef struct semd semd_t;

/****** General manipulation of semaphore objects.  ******/

/* Initialize the semaphore module.  */
void initASL (void);

/* Initialize a new semaphore object `s'.  `val' is its initial value.  */
void initSemD (semd_t *s, int val);

/* Insert the process `p' at the tail of the queue of semaphore `s'.
   Add `s' to the ASL (active semaphore list), if not done yet.  */
void insertBlocked (semd_t *s, pcb_t *p);

/* Remove the first process from the queue of the semaphore `s' and
   return a pointer to it.  Return NULL if none found.
   If the process queue for this semaphore becomes empty,
   remove the semaphore from the ASL. */
pcb_t *removeBlocked (semd_t *s);

/* Remove the process `p' from the queue of the semaphore for which `p'
   is waiting.  Return NULL if `p' is not waiting for a semaphore and
   `p' otherwise.  */
pcb_t *outBlocked (pcb_t *p);

/* Return the process that is at the head of the queue associated with
   the semaphore `s'.  Return NULL if `s' is not active.  */
pcb_t *headBlocked (semd_t *s);

#endif
