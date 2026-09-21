#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <abi/syscalls.h>

typedef long scw;
extern long __do_syscall_ret(unsigned long);
extern scw __do_syscall3(long, scw, scw, scw);

#define EV_SYN     0x00
#define EV_KEY     0x01
#define EV_MSC     0x04
#define SYN_REPORT 0x00
#define MSC_SCAN   0x04

#define EVIOCGVERSION 0x80044501UL
#define EVIOCGID      0x80084502UL
#define EVIOCGNAME_L256 0x81004506UL

// Matches the kernel's 24-byte struct input_event.
struct input_event {
    long long      tv_sec;
    long long      tv_usec;
    unsigned short type;
    unsigned short code;
    int            value;
};

int main(int argc, char **argv) {
    printf("evdev_test: start\n");

    const char *path = "/dev/event0";
    if (argc > 1)
        path = argv[1];

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("evdev_test: FAIL open %s (errno=%d)\n", path, errno);
        return 1;
    }

    long ver = 0;
    if (__do_syscall_ret(__do_syscall3(SYS_IOCTL, fd, (scw)EVIOCGVERSION, (scw)(&ver))) == 0)
        printf("evdev_test: EVIOCGVERSION=0x%lx\n", ver);
    else
        printf("evdev_test: EVIOCGVERSION failed\n");

    char name[256];
    long n = __do_syscall_ret(__do_syscall3(SYS_IOCTL, fd, (scw)EVIOCGNAME_L256, (scw)(&name[0])));
    name[255] = 0;
    printf("evdev_test: EVIOCGNAME len=%ld name=%s\n", n, name);

    struct input_event ev;
    for (int i = 0; i < 80; i++) {
        ssize_t r = read(fd, &ev, sizeof(ev));
        if (r != (ssize_t)sizeof(ev)) {
            printf("evdev_test: read=%ld errno=%d (stopping)\n", (long)r, errno);
            break;
        }
        printf("evdev_test: type=%u code=%u value=%d\n", ev.type, ev.code, ev.value);
        fflush(stdout);
    }

    close(fd);
    printf("evdev_test: done\n");
    return 0;
}