

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

static int g_fail = 0;
#define CHECK(c,m) do{ if(c){printf("  PASS: %s\n",m);} else {printf("  FAIL: %s\n",m); g_fail++;} }while(0)

static int claim_oneshot()
{
    char path[64];
    std::snprintf(path, sizeof(path), "/tmp/.ember.load.%d", getpid());
    int fd = open(path, O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (fd < 0) return -2;
    if (flock(fd, LOCK_EX | LOCK_NB) != 0) { close(fd); return -1; }
    return fd;
}

int main()
{
    char path[64];
    std::snprintf(path, sizeof(path), "/tmp/.ember.load.%d", getpid());
    unlink(path);

    printf("[1] two instances in one process: only the first hooks\n");
    int a = claim_oneshot();
    int b = claim_oneshot();
    CHECK(a >= 0,  "instance A claims the one-shot (would run hooking)");
    CHECK(b == -1, "instance B is blocked (would skip -> no re-scan of hooked code)");

    printf("[2] lock auto-releases (no stale-lock / pid-reuse hazard)\n");
    if (a >= 0) close(a);
    int c = claim_oneshot();
    CHECK(c >= 0, "after release, a fresh process can claim again");
    if (c >= 0) close(c);

    unlink(path);
    printf(g_fail ? "\nFAILED (%d)\n" : "\nOK — cross-instance one-shot holds\n", g_fail);
    return g_fail ? 1 : 0;
}
