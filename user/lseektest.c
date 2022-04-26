#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define TESTFILE_CONTENTS "0123456789"

void clearBuffer(char* buf, int size) {
    int i;
    for (i = 0; i < size; i++) {
        buf[i] = 0;
    }
}

int lseektest(void) {

    char buffer[10];
    char refbuffer[10] = TESTFILE_CONTENTS;

    int failed = 0;

    int fd = open("/TESTFILE", O_RDONLY);
    if (fd < 0) {
        printf("open failed\n");
        return -1;
    }

    /**
     * ==========================================
     * Test: SEEK_SET
     * ==========================================
     */

    int ret = lseek(fd, 3, SEEK_SET);
    if (ret < 0) {
        printf("lseek failed with %d: SEEK_SET\n", ret);
        return -1;
    }

    read(fd, buffer, 10);
    if(strcmp(buffer, &refbuffer[3]) != 0) {
        printf("Test failed at: SEEK_SET\n");
        printf("--> \"%s\" != \"%s\"\n\n", buffer, &refbuffer[3]);
        failed = 1;
    } else {
        printf("Test passed at: SEEK_SET\n");
        printf("--> \"%s\" == \"%s\"\n\n", buffer, &refbuffer[3]);
    }

    /**
     * ==========================================
     * Test: SEEK_CUR
     * ==========================================
     */

    lseek(fd, 3, SEEK_SET);
    clearBuffer(buffer, 10);

    ret = lseek(fd, 2, SEEK_CUR);
    if (ret < 0) {
        printf("lseek failed with %d: SEEK_CUR\n", ret);
        return -1;
    }

    read(fd, buffer, 10);
    if(strcmp(buffer, &refbuffer[5]) != 0) {
        printf("Test failed at: SEEK_CUR\n");
        printf("--> \"%s\" != \"%s\"\n", buffer, &refbuffer[5]);
        failed = 1;
    } else {
        printf("Test passed at: SEEK_CUR\n");
        printf("--> \"%s\" == \"%s\"\n\n", buffer, &refbuffer[5]);
    }

    /**
     * ==========================================
     * Test: SEEK_END
     * ==========================================
     */

    lseek(fd, 0, SEEK_SET);
    clearBuffer(buffer, 10);

    //Test lseek: SEEK_END
    ret = lseek(fd, -2, SEEK_END);
    if (ret < 0) {
        printf("lseek failed with %d: SEEK_END\n", ret);
        return -1;
    }

    read(fd, buffer, 10);
    if(strcmp(buffer, &refbuffer[8]) != 0) {
        printf("Test failed at: SEEK_END\n");
        printf("--> \"%s\" != \"%s\"\n\n", buffer, &refbuffer[8]);
        failed = 1;
    } else {
        printf("Test passed at: SEEK_END\n");
        printf("--> \"%s\" == \"%s\"\n\n", buffer, &refbuffer[8]);
    }

    /**
     * ==========================================
     * Test: SEEK_END - Fail
     * ==========================================
     */

    lseek(fd, 0, SEEK_SET);
    clearBuffer(buffer, 10);

    ret = lseek(fd, 4, SEEK_END);
    if (ret < 0) {
        printf("Test passed at: SEEK_END - Fail\n");
        printf("--> lseek(fd, 4, SEEK_END) < 0\n\n", ret);
    } else {
        printf("Test failed at: SEEK_END - Fail\n");
        printf("--> lseek(fd, 4, SEEK_END) > 0\n\n", ret);
    }

    /**
     * ==========================================
     * Test: SEEK_SET - Fail
     * ==========================================
     */

    lseek(fd, 0, SEEK_SET);
    clearBuffer(buffer, 10);

    ret = lseek(fd, 12, SEEK_SET);
    if (ret < 0) {
        printf("Test passed at: SEEK_SET - Fail\n");
        printf("--> lseek(fd, 12, SEEK_SET) < 0\n\n", ret);
    } else {
        printf("Test failed at: SEEK_SET - Fail\n");
        printf("--> lseek(fd, 12, SEEK_SET) > 0\n\n", ret);
    }

    return failed ? -1 : 0;
}


int main(int argc, char *argv[]) {
    if(lseektest() == 0) {
        printf("\nAll tests PASSED\n");
    } else {
        printf("\nTest FAILED\n");
    }
    exit(0);
}