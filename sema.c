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
static semd_t *ASL;
