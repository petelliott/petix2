#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {

    //TODO: read /etc/ttys
    pid_t getty_pid = fork();
    if (getty_pid == 0) {
        char *const gettyargv[] = {"/bin/getty", "/dev/vgatty", NULL};
        execve("/bin/getty", gettyargv, NULL);
    } else {
        int wstatus;
        waitpid(getty_pid, &wstatus, 0);
    }

    // we probably wont return
    return 0;
}
