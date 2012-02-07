#include <stdio.h>

#include "proc.h"

#define MAXPROCESS 20



void test(char *test_name, int (*f)(void)) {
    printf("%s: %s\n", test_name, (*f)() ? "OK" : "FAIL");
}

int test_initProc(void) {
    int i;
    int success = 1;
    pcb_t *p1, *p2;

    initProc();

    for (i = 0; i < MAXPROCESS-1; ++i) {
        p1 = getFreeProcess(i);
        p2 = getFreeProcess(i+1);

        /* Make sure p1 points to p2. */
        success &= getPNext(p1) == p2;
    }

    i = MAXPROCESS-1;
    p1 = getFreeProcess(i);
    success &= getPNext(p1) == NULL;



    return success;
}


int test_allocFreeCount(void) {
    int success = 1;
    pcb_t *procs[MAXPROCESS];
    int i;

    initProc();

    for (i = 1; i <= MAXPROCESS; ++i) {
        int j = 0;

        while (j < i && j < MAXPROCESS) {
            procs[j++] = allocPcb();
        }

        success &= getFreeProcessCount() == MAXPROCESS - i;

        while (j > 0) {
            freePcb(procs[--j]);
        }

        success &= getFreeProcessCount() == MAXPROCESS;
    }

    return success;
}

int test_allocFreeNull(void) {
    int success = 1;
    int i;

    initProc();

    /* Make sure freeing NULL is invalid. */
    freePcb(NULL);
    success &= getFreeProcessCount() == MAXPROCESS;

    /* Make sure you cannot allocate more than MAXPROCESS */
    for (i = 0; i < MAXPROCESS; i++) {
        allocPcb();
    }
    success &= allocPcb() == NULL;
    success &= allocPcb() == NULL;

    initProc();

    /* Make sure an invalid freePcb doesn't alter the state of free
     * processes. */
    allocPcb();
    success &= getFreeProcessCount() == MAXPROCESS - 1;
    freePcb(NULL);
    success &= getFreeProcessCount() == MAXPROCESS - 1;


    return success;
}



int test_EmptyProcQ(void) {
    int success = 1;
    pcb_t *p;
    pcbq_t *q;

    initProc();

    success &= emptyProcQ(NULL);

    q = mkEmptyProcQ();
    success &= emptyProcQ(q);

    p = allocPcb();
    insertProcQ(&q, p);
    success &= !emptyProcQ(q);
    success &= getPNext(p) == p;
    removeProcQ(&q);
    success &= emptyProcQ(q);

    return success;
}


int test_insertProcQ(void) {
    int success = 1;
    int i;
    pcbq_t *q = mkEmptyProcQ();

    initProc();

    success &= emptyProcQ(q);
    insertProcQ(&q, NULL);
    success &= emptyProcQ(q);

    for (i = 0; i < 3; ++i) {
        insertProcQ(&q, allocPcb());
    }

    for (i = 0; i < 3; ++i) {
        success &= removeProcQ(&q) != NULL;
    }
    success &= !removeProcQ(&q);


    return success;
}



int main(void) {
    test("test_initProc", test_initProc);
    test("test_allocFreeCount", test_allocFreeCount);
    test("test_allocFreeNull", test_allocFreeNull);
    test("test_EmptyProcQ", test_EmptyProcQ);
    test("test_insertProcQ", test_insertProcQ);

    return 0;
}
