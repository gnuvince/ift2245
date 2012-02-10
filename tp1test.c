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

    for (i = 0; i < MAXPROCESS; ++i) {
        insertProcQ(&q, allocPcb());
    }

    for (i = 0; i < MAXPROCESS; ++i) {
        success &= removeProcQ(&q) != NULL;
    }
    success &= !removeProcQ(&q);


    return success;
}


int test_removeProcQ(void) {
    int success = 1;
    pcb_t *p1, *p2;
    pcbq_t *q;

    initProc();
    p1 = allocPcb();
    p2 = allocPcb();
    q = mkEmptyProcQ();

    success &= removeProcQ(NULL) == NULL;

    insertProcQ(&q, p1);
    success &= !emptyProcQ(q);
    success &= removeProcQ(&q) == p1;
    success &= emptyProcQ(q);

    insertProcQ(&q, p1);
    insertProcQ(&q, p2);
    success &= removeProcQ(&q) == p1;
    success &= removeProcQ(&q) == p2;

    return success;
}


int test_outProcQ(void) {
    int success = 1;
    pcb_t *p1, *p2, *p3;
    pcbq_t *q;

    initProc();
    q = mkEmptyProcQ();
    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();

    success &= outProcQ(NULL, p1) == NULL;
    success &= outProcQ(&q, NULL) == NULL;
    success &= outProcQ(&q, p1) == NULL;
    success &= outProcQ(&q, p2) == NULL;
    success &= outProcQ(&q, p3) == NULL;

    insertProcQ(&q, p1);
    insertProcQ(&q, p2);
    success &= outProcQ(&q, p3) == NULL;
    insertProcQ(&q, p3);
    success &= outProcQ(&q, p3) == p3;
    success &= outProcQ(&q, p1) == p1;
    success &= removeProcQ(&q) == p2;

    return success;
}


int test_headProcQ(void) {
    int success = 1;
    pcb_t *p1, *p2;
    pcbq_t *q;

    initProc();
    q = mkEmptyProcQ();
    p1 = allocPcb();
    p2 = allocPcb();

    success &= headProcQ(q) == NULL;
    insertProcQ(&q, p1);
    success &= headProcQ(q) == p1;
    insertProcQ(&q, p2);
    success &= headProcQ(q) == p1;
    outProcQ(&q, p1);
    success &= headProcQ(q) == p2;

    return success;
}



int test_emptyChild(void) {
    int success = 1;
    pcb_t *p1, *p2;

    initProc();
    p1 = allocPcb();
    p2 = allocPcb();

    success &= !emptyChild(NULL);
    success &= emptyChild(p1);
    success &= emptyChild(p2);

    insertChild(p1, p2);
    success &= !emptyChild(p1);
    removeChild(p1);
    success &= emptyChild(p1);

    return success;
}



int test_removeChild(void) {
    int success = 1;
    pcb_t *p1, *p2, *p3, *p4, *p5;

    initProc();
    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();
    p4 = allocPcb();
    p5 = allocPcb();

    success &= removeChild(NULL) == NULL;
    success &= removeChild(p1) == NULL;

    insertChild(p1, p2);
    insertChild(p1, p3);
    success &= removeChild(p1) == p3;
    success &= removeChild(p1) == p2;
    success &= removeChild(p1) == NULL;

    insertChild(p1, p2);
    insertChild(p2, p3);
    insertChild(p2, p4);
    insertChild(p4, p5);
    success &= removeChild(p1) == p2;
    success &= emptyChild(p2);
    success &= emptyChild(p3);
    success &= emptyChild(p4);
    success &= emptyChild(p5);

    return success;
}




int main(void) {
    test("test_initProc", test_initProc);
    test("test_allocFreeCount", test_allocFreeCount);
    test("test_allocFreeNull", test_allocFreeNull);
    test("test_EmptyProcQ", test_EmptyProcQ);
    test("test_insertProcQ", test_insertProcQ);
    test("test_removeProcQ", test_removeProcQ);
    test("test_outProcQ", test_outProcQ);
    test("test_headProcQ", test_headProcQ);
    test("test_emptyChild", test_emptyChild);
    test("test_removeChild", test_removeChild);

    return 0;
}
