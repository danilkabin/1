// TEST GENERATED BY GROK!!!

#include "../src/misc/uidq_bitmask.h"
#include "../src/misc/uidq_utils.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "uidq_bitmask.h"

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            return -1; \
        } else { \
            printf("PASS: %s\n", message); \
        } \
    } while (0)

int test_create(void) {
    uidq_bitmask_t *bitmask = uidq_bitmask_create();
    TEST_ASSERT(bitmask != NULL, "Create bitmask");
    uidq_bitmask_free(bitmask);
    return 0;
}

int test_init(void) {
    uidq_bitmask_t bitmask = {0};
    int ret = uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    TEST_ASSERT(ret == 0 && bitmask.initialized && bitmask.bit_capacity == 64 && bitmask.word_bits == 64, "Init bitmask");
    TEST_ASSERT(bitmask.bit_count == 0, "Init bit count is 0");
    TEST_ASSERT(uidq_bitmask_init(NULL, 64, sizeof(uint64_t)) == -1, "Init with NULL fails");
    TEST_ASSERT(uidq_bitmask_init(&bitmask, 0, sizeof(uint64_t)) == -1, "Init with zero capacity fails");
    TEST_ASSERT(uidq_bitmask_init(&bitmask, 64, 0) == -1, "Init with zero word size fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_create_and_init(void) {
    uidq_bitmask_t *bitmask = uidq_bitmask_create_and_init(64, sizeof(uint64_t));
    TEST_ASSERT(bitmask != NULL && bitmask->initialized && bitmask->bit_capacity == 64, "Create and init bitmask");
    TEST_ASSERT(uidq_bitmask_create_and_init(0, sizeof(uint64_t)) == NULL, "Create and init with zero capacity fails");
    uidq_bitmask_free(bitmask);
    return 0;
}

int test_free(void) {
    uidq_bitmask_t *bitmask = uidq_bitmask_create_and_init(64, sizeof(uint64_t));
    uidq_bitmask_free(bitmask);
    TEST_ASSERT(1, "Free bitmask (no crash)");
    uidq_bitmask_free(NULL);
    TEST_ASSERT(1, "Free NULL bitmask (no crash)");
    return 0;
}

int test_exit(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_exit(&bitmask);
    TEST_ASSERT(!bitmask.initialized && bitmask.mask == NULL, "Exit clears bitmask");
    uidq_bitmask_exit(NULL);
    TEST_ASSERT(1, "Exit NULL bitmask (no crash)");
    return 0;
}

int test_set(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    TEST_ASSERT(uidq_bitmask_set(&bitmask, 5) == true, "Set bit 5");
    TEST_ASSERT(bitmask.bit_count == 1 && uidq_bitmask_bit_test(&bitmask, 5) == 1, "Bit 5 is set");
    TEST_ASSERT(uidq_bitmask_set(&bitmask, 5) == true, "Set bit 5 again (no change)");
    TEST_ASSERT(bitmask.bit_count == 1, "Bit count unchanged after re-set");
    TEST_ASSERT(uidq_bitmask_set(&bitmask, 64) == false, "Set out-of-bounds bit fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_clear(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    TEST_ASSERT(uidq_bitmask_clear(&bitmask, 5) == true, "Clear bit 5");
    TEST_ASSERT(bitmask.bit_count == 0 && uidq_bitmask_bit_test(&bitmask, 5) == 0, "Bit 5 is cleared");
    TEST_ASSERT(uidq_bitmask_clear(&bitmask, 5) == true, "Clear bit 5 again (no change)");
    TEST_ASSERT(bitmask.bit_count == 0, "Bit count unchanged after re-clear");
    TEST_ASSERT(uidq_bitmask_clear(&bitmask, 64) == false, "Clear out-of-bounds bit fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_bit_test(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    TEST_ASSERT(uidq_bitmask_bit_test(&bitmask, 5) == 1, "Test bit 5 is set");
    TEST_ASSERT(uidq_bitmask_bit_test(&bitmask, 6) == 0, "Test bit 6 is clear");
    TEST_ASSERT(uidq_bitmask_bit_test(&bitmask, 64) == -1, "Test out-of-bounds bit fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_test_sequence(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    uidq_bitmask_set(&bitmask, 6);
    TEST_ASSERT(uidq_bitmask_test_sequence(&bitmask, 1, 5, 2) == 5, "Sequence of 2 set bits at 5");
    TEST_ASSERT(uidq_bitmask_test_sequence(&bitmask, 0, 7, 2) == 7, "Sequence of 2 clear bits at 7");
    TEST_ASSERT(uidq_bitmask_test_sequence(&bitmask, 1, 5, 3) == -1, "No sequence of 3 set bits at 5");
    TEST_ASSERT(uidq_bitmask_test_sequence(&bitmask, 0, 64, 1) == -1, "Sequence out-of-bounds fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_find_first_bit(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    TEST_ASSERT(uidq_bitmask_find_first_bit(&bitmask, 1) == 5, "Find first set bit");
    TEST_ASSERT(uidq_bitmask_find_first_bit(&bitmask, 0) == 0, "Find first clear bit");
    uidq_bitmask_bits_clear(&bitmask);
    TEST_ASSERT(uidq_bitmask_find_first_bit(&bitmask, 1) == -1, "No set bits found");
    TEST_ASSERT(uidq_bitmask_find_first_bit(&bitmask, 2) == -1, "Invalid type fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_find_grab_bit(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    uidq_bitmask_set(&bitmask, 6);
    TEST_ASSERT(uidq_bitmask_find_grab_bit(&bitmask, 0, 2, 1) == 5, "Find 2 set bits at 5");
    TEST_ASSERT(uidq_bitmask_find_grab_bit(&bitmask, 0, 3, 0) == 0, "Find 3 clear bits at 0");
    TEST_ASSERT(uidq_bitmask_find_grab_bit(&bitmask, 0, 3, 1) == -1, "No 3 set bits found");
    TEST_ASSERT(uidq_bitmask_find_grab_bit(&bitmask, 63, 2, 0) == -1, "Out-of-bounds sequence fails");
    TEST_ASSERT(uidq_bitmask_find_grab_bit(&bitmask, 0, 0, 1) == -1, "Zero grab fails");
    TEST_ASSERT(uidq_bitmask_find_grab_bit(&bitmask, 0, 2, 2) == -1, "Invalid type fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_add(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    int start = uidq_bitmask_add(&bitmask, 0, 3);
    TEST_ASSERT(start == 0 && uidq_bitmask_bit_test(&bitmask, 0) == 1 &&
                uidq_bitmask_bit_test(&bitmask, 1) == 1 &&
                uidq_bitmask_bit_test(&bitmask, 2) == 1, "Add 3 bits at 0");
    TEST_ASSERT(bitmask.bit_count == 3, "Bit count after add is 3");
    uidq_bitmask_bits_clear(&bitmask);
    size_t index;
    for (index = 0; index < 64; index = index + 1) {
        uidq_bitmask_set(&bitmask, index);
    }
    TEST_ASSERT(uidq_bitmask_add(&bitmask, 0, 1) == -1, "Add with no free bits fails");
    TEST_ASSERT(uidq_bitmask_add(&bitmask, 0, 65) == -1, "Add too many bits fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_del(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_add(&bitmask, 0, 3);
    TEST_ASSERT(uidq_bitmask_del(&bitmask, 0, 2) == true, "Delete 2 bits at 0");
    TEST_ASSERT(bitmask.bit_count == 1 && uidq_bitmask_bit_test(&bitmask, 0) == 0 &&
                uidq_bitmask_bit_test(&bitmask, 1) == 0, "Bits 0-1 cleared");
    TEST_ASSERT(uidq_bitmask_del(&bitmask, 64, 1) == false, "Delete out-of-bounds fails");
    TEST_ASSERT(uidq_bitmask_del(&bitmask, 0, 0) == false, "Delete zero count fails");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_save_load(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    int fd = open("test_bitmask.bin", O_CREAT | O_RDWR | O_TRUNC, 0644);
    TEST_ASSERT(fd >= 0, "Open file for save");
    TEST_ASSERT(uidq_bitmask_save(&bitmask, fd) == 0, "Save bitmask");
    close(fd);
    
    uidq_bitmask_t bitmask2 = {0};
    fd = open("test_bitmask.bin", O_RDONLY);
    TEST_ASSERT(fd >= 0, "Open file for load");
    TEST_ASSERT(uidq_bitmask_load(&bitmask2, fd) == 0, "Load bitmask");
    TEST_ASSERT(bitmask2.bit_count == 1 && uidq_bitmask_bit_test(&bitmask2, 5) == 1, "Loaded bitmask matches");
    close(fd);
    unlink("test_bitmask.bin");
    uidq_bitmask_exit(&bitmask);
    uidq_bitmask_exit(&bitmask2);
    return 0;
}

int test_invert(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    uidq_bitmask_invert(&bitmask);
    TEST_ASSERT(bitmask.bit_count == 63 && uidq_bitmask_bit_test(&bitmask, 5) == 0, "Invert bitmask");
    TEST_ASSERT(uidq_bitmask_bit_test(&bitmask, 0) == 1, "Bit 0 is set after invert");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_op(void) {
    uidq_bitmask_t b1 = {0}, b2 = {0}, dest = {0};
    uidq_bitmask_init(&b1, 64, sizeof(uint64_t));
    uidq_bitmask_init(&b2, 64, sizeof(uint64_t));
    uidq_bitmask_init(&dest, 64, sizeof(uint64_t));
    uidq_bitmask_set(&b1, 5);
    uidq_bitmask_set(&b2, 5);
    TEST_ASSERT(uidq_bitmask_op(&dest, &b1, &b2, UIDQ_BITMASK_OP_AND) == 0, "AND operation");
    TEST_ASSERT(dest.bit_count == 1 && uidq_bitmask_bit_test(&dest, 5) == 1, "AND result correct");
    uidq_bitmask_set(&b2, 6);
    TEST_ASSERT(uidq_bitmask_op(&dest, &b1, &b2, UIDQ_BITMASK_OP_OR) == 0, "OR operation");
    TEST_ASSERT(dest.bit_count == 2, "OR result has 2 bits");
    TEST_ASSERT(uidq_bitmask_op(&dest, &b1, &b2, 999) == -1, "Invalid operation fails");
    uidq_bitmask_exit(&b1);
    uidq_bitmask_exit(&b2);
    uidq_bitmask_exit(&dest);
    return 0;
}

int test_bits_clear(void) {
    uidq_bitmask_t bitmask = {0};
    uidq_bitmask_init(&bitmask, 64, sizeof(uint64_t));
    uidq_bitmask_set(&bitmask, 5);
    uidq_bitmask_set(&bitmask, 6);
    uidq_bitmask_bits_clear(&bitmask);
    TEST_ASSERT(bitmask.bit_count == 0 && uidq_bitmask_bit_test(&bitmask, 5) == 0 &&
                uidq_bitmask_bit_test(&bitmask, 6) == 0, "Clear all bits");
    uidq_bitmask_exit(&bitmask);
    return 0;
}

int test_copy(void) {
    uidq_bitmask_t src = {0}, dest = {0};
    uidq_bitmask_init(&src, 64, sizeof(uint64_t));
    uidq_bitmask_set(&src, 5);
    TEST_ASSERT(uidq_bitmask_copy(&dest, &src) == 0, "Copy bitmask");
    TEST_ASSERT(dest.bit_count == 1 && uidq_bitmask_bit_test(&dest, 5) == 1, "Copied bitmask matches");
    uidq_bitmask_exit(&src);
    uidq_bitmask_exit(&dest);
    return 0;
}

int main(void) {
    printf("Running tests...\n");
    int failures = 0;
    failures = failures + (test_create() != 0);
    failures = failures + (test_init() != 0);
    failures = failures + (test_create_and_init() != 0);
    failures = failures + (test_free() != 0);
    failures = failures + (test_exit() != 0);
    failures = failures + (test_set() != 0);
    failures = failures + (test_clear() != 0);
    failures = failures + (test_bit_test() != 0);
    failures = failures + (test_test_sequence() != 0);
    failures = failures + (test_find_first_bit() != 0);
    failures = failures + (test_find_grab_bit() != 0);
    failures = failures + (test_add() != 0);
    failures = failures + (test_del() != 0);
    failures = failures + (test_save_load() != 0);
    failures = failures + (test_invert() != 0);
    failures = failures + (test_op() != 0);
    failures = failures + (test_bits_clear() != 0);
    failures = failures + (test_copy() != 0);
    printf("\nTests completed. Failures: %d\n", failures);
    return failures == 0 ? 0 : 1;
}
