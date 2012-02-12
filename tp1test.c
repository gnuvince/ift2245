#include <stdio.h>

#include "proc.h"
#include "sema.h"

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


int test_outChild(void) {
    int success = 1;
    pcb_t *p1, *p2, *p3, *p4, *p5;

    initProc();
    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();
    p4 = allocPcb();
    p5 = allocPcb();

    success &= outChild(NULL) == NULL;
    success &= outChild(p1) == NULL;

    insertChild(p1, p2);
    insertChild(p1, p3);
    insertChild(p3, p4);
    insertChild(p1, p5);

    success &= outChild(p3) == p3;
    success &= emptyChild(p3);
    success &= getPSib(p5) == p2;
    success &= getPSib(p2) == NULL;

    success &= outChild(p5) == p5;
    success &= getPChild(p1) == p2;
    success &= getPSib(p2) == NULL;

    return success;
}



int test_initASL(void) {
    int i;
    int success = 1;
    semd_t *s1, *s2;

    initASL();

    for (i = 0; i < MAXPROCESS-1; ++i) {
        s1 = getSema(i);
        s2 = getSema(i+1);

        success &= getSNext(s1) == s2;
    }
    s1 = getSema(MAXPROCESS - 1);
    success &= getSNext(s1) == NULL;

    success &= getSemdFree() == getSema(0);

    return success;
}


int test_initSemD(void) {
    int success = 1;
    semd_t *s1;

    initASL();

    initSemD(s1, 42);
    success &= getSValue(s1) == 42;

    return success;
}

int test_initSemDExhaustion(void) {
    int success = 1;
    int i = 0;
    semd_t *s;

    initASL();

    for (i = 0; i < MAXPROCESS; ++i)
        success &= initSemD(s, i);
    success &= !initSemD(s, MAXPROCESS);

    return success;
}


int test_insertBlocked(void) {
    int success = 1;
    semd_t *s1, *s2, *s3;
    pcb_t *p1, *p2, *p3;
    pcbq_t *q;

    initASL();
    initProc();

    s1 = getSema(0); initSemD(s1, 1);
    s2 = getSema(1); initSemD(s2, 2);
    s3 = getSema(12); initSemD(s3, 3);

    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();

    success &= getASL() == NULL;
    insertBlocked(NULL, p1);
    success &= getASL() == NULL;
    insertBlocked(s1, NULL);
    success &= getASL() == NULL;

    insertBlocked(s2, p2);
    q = getSProcQ(s2);
    success &= !emptyProcQ(q);
    success &= getASL() == s2;

    insertBlocked(s3, p3);
    q = getSProcQ(s3);
    success &= !emptyProcQ(q);
    success &= getASL() == s2;
    success &= getSNext(s2) == s3;

    insertBlocked(s1, p1);
    q = getSProcQ(s1);
    success &= !emptyProcQ(q);
    success &= getASL() == s1;
    success &= getSNext(s1) == s2;

    return success;
}


int test_removeBlocked(void) {
    int success = 1;
    semd_t *s1, *s2;
    pcb_t *p1, *p2, *p3;


    initASL();
    initProc();

    s1 = getSema(0); initSemD(s1, 1);
    s2 = getSema(1); initSemD(s2, 2);

    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();


    insertBlocked(s1, p1);
    insertBlocked(s2, p2);
    insertBlocked(s1, p3);

    success &= removeBlocked(NULL) == NULL;

    success &= getSNext(s1) == s2;
    success &= removeBlocked(s2) == p2;
    success &= getSemdFree() == s2;
    success &= getSNext(s1) == NULL;

    success &= removeBlocked(s1) == p1;
    success &= removeBlocked(s1) == p3;
    success &= getSemdFree() == s1;
    success &= getASL() == NULL;

    return success;
}

int test_outBlocked(void) {
    int success = 1;
    semd_t *s1, *s2;
    pcb_t *p1, *p2, *p3;


    initASL();
    initProc();

    s1 = getSema(0); initSemD(s1, 1);
    s2 = getSema(1); initSemD(s2, 2);

    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();


    insertBlocked(s1, p1);
    insertBlocked(s2, p2);
    insertBlocked(s1, p3);

    success &= outBlocked(NULL) == NULL;

    success &= getSNext(s1) == s2;
    success &= removeBlocked(s2) == p2;
    success &= getSemdFree() == s2;
    success &= getSNext(s1) == NULL;

    success &= removeBlocked(s1) == p1;
    success &= getASL() == s1;
    success &= removeBlocked(s1) == p3;
    success &= getSemdFree() == s1;
    success &= getASL() == NULL;

    return success;
}


int test_headBlocked(void) {
    int success = 1;
    semd_t *s1, *s2;
    pcb_t *p1, *p2, *p3;


    initASL();
    initProc();

    s1 = getSema(0); initSemD(s1, 1);
    s2 = getSema(1); initSemD(s2, 2);

    p1 = allocPcb();
    p2 = allocPcb();
    p3 = allocPcb();


    success &= headBlocked(NULL) == NULL;
    success &= headBlocked(s1) == NULL;
    success &= headBlocked(s2) == NULL;

    insertBlocked(s1, p1);
    insertBlocked(s2, p2);
    insertBlocked(s1, p3);

    success &= headBlocked(s1) == p1;
    success &= headBlocked(s2) == p2;
    outBlocked(p1);
    success &= headBlocked(s1) == p3;

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
    test("test_outChild", test_outChild);

    test("test_initASL", test_initASL);
    test("test_initSemD", test_initSemD);
    test("test_initSemDExhaustion", test_initSemDExhaustion);
    test("test_insertBlocked", test_insertBlocked);
    test("test_removeBlocked", test_removeBlocked);
    test("test_outBlocked", test_removeBlocked);
    test("test_headBlocked", test_removeBlocked);

    return 0;
}
