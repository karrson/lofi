import os
import re

# A regular expression to match syscall definitions in C code
SYSCALL_REGEX = r"SYS_([a-zA-Z0-9]+)"

def list_syscalls(dir_path):
    # A dictionary to store the list of syscalls and their definitions
    syscalls = []
    for root, dirs, files in os.walk(dir_path):
        for file_name in files:
            if file_name.endswith(".c") or file_name.endswith(".h") or file_name.endswith(".asm") or file_name.endswith(".s"):
                file_path = os.path.join(root, file_name)
                with open(file_path, "r") as f:
                    code = f.read()
                    # Find all syscall definitions in the file using regex
                    for match in re.finditer(SYSCALL_REGEX, code, re.MULTILINE):
                        syscall_name = match.group(1)
                        syscalls.append(syscall_name)
    syscalls = set(syscalls)
    syscalls = sorted(syscalls)
    syscalls = sorted(set([x.lower() for x in syscalls]), key=lambda x: (x.lower(), -len(x)))
    syscalls = [s.lower() for s in syscalls]
    return syscalls

KnownSystemCallsBasicToAdvanced = ['write', 'exit', 'read', 'open', 'close', 'stat', 'fstat', 'lseek', 'mmap', 'mprotect', 'munmap', 'brk', 'ioctl', 'pread64', 'pwrite64', 'readv', 'writev', 'access', 'pipe', 'select', 'mremap', 'msync', 'mincore', 'madvise', 'shmget', 'shmat', 'shmctl', 'dup', 'dup2', 'pause', 'nanosleep', 'getitimer', 'alarm', 'setitimer', 'getpid', 'sendfile', 'socket', 'connect', 'accept', 'sendto', 'recvfrom', 'sendmsg', 'recvmsg', 'shutdown', 'bind', 'listen', 'getsockname', 'getpeername', 'socketpair', 'setsockopt', 'getsockopt', 'clone', 'fork', 'vfork', 'execve', 'wait4', 'kill', 'uname', 'semget', 'semop', 'semctl', 'shmdt', 'msgget', 'msgsnd', 'msgrcv', 'msgctl', 'fcntl', 'flock', 'fsync', 'fdatasync', 'truncate', 'ftruncate', 'getdents', 'getcwd', 'chdir', 'fchdir', 'rename', 'mkdir', 'rmdir', 'creat', 'link', 'unlink', 'symlink', 'readlink', 'chmod', 'fchmod', 'chown', 'fchown', 'lchown', 'umask', 'gettimeofday', 'getrlimit', 'getrusage', 'sysinfo', 'times', 'ptrace', 'getuid', 'syslog', 'getgid', 'setuid', 'setgid', 'geteuid', 'getegid', 'setpgid', 'getppid', 'getpgrp', 'setsid', 'setreuid', 'setregid', 'getgroups', 'setgroups', 'setresuid', 'getresuid', 'setresgid', 'getresgid', 'getpgid', 'setfsuid', 'setfsgid', 'getsid', 'capget', 'capset', 'sigaltstack', 'utime', 'mknod', 'uselib', 'personality', 'ustat', 'statfs', 'fstatfs', 'sysfs', 'getpriority', 'setpriority', 'settimeofday', 'mount', 'swapon', 'swapoff', 'reboot', 'sethostname', 'setdomainname', 'ioperm', 'iopl', 'accept4', 'acct', 'add', 'adjtimex', 'afs', 'arch', 'auxv', 'bpf', 'cachectl', 'cacheflush', 'chown32', 'chroot', 'clock', 'clone3', 'copy', 'create', 'delete', 'dispatch', 'dup3', 'epoll', 'eventfd', 'eventfd2', 'execveat', 'faccessat', 'faccessat2', 'fadvise', 'fadvise64', 'fallocate', 'fanotify', 'fchmodat', 'fchown32', 'fchownat', 'fcntl64', 'fgetxattr', 'file', 'finit', 'flistxattr', 'fremovexattr', 'fsconfig', 'fsetxattr', 'fsmount', 'fsopen', 'fspick', 'fstat64', 'fstatat', 'fstatat64', 'fstatfs64', 'fsuid', 'ftruncate64', 'futex', 'futimesat', 'get', 'getcpu', 'getdents64', 'getegid32', 'geteuid32', 'getgid32', 'getgroups32', 'getpmsg', 'getrandom', 'getresgid32', 'getresuid32', 'gettid', 'getuid32', 'getxattr', 'init', 'inotify', 'io', 'ioprio', 'ipc', 'kcmp', 'kexec', 'keyctl', 'klog', 'landlock', 'lchown32', 'lgetxattr', 'linkat', 'listxattr', 'llistxattr', 'lookup', 'lremovexattr', 'lsetxattr', 'lstat', 'lstat64', 'max', 'mbind', 'membarrier', 'memfd', 'migrate', 'mkdirat', 'mknodat', 'mlock', 'mlock2', 'mlockall', 'mman', 'mmap2', 'modify', 'move', 'mq', 'msg', 'mtio', 'munlock', 'munlockall', 'name', 'newfstatat', 'nfsservctl', 'openat', 'openat2', 'param', 'perf', 'pidfd', 'pipe2', 'pivot', 'pkey', 'poll', 'ppoll', 'prctl', 'pread', 'preadv', 'preadv2', 'prlimit64', 'process', 'procfs', 'pselect6', 'putpmsg', 'pwrite', 'pwritev', 'pwritev2', 'query', 'quota', 'quotactl', 'random', 'readahead', 'readlinkat', 'recvmmsg', 'reg', 'remap', 'removexattr', 'renameat', 'renameat2', 'request', 'resource', 'restart', 'riscv', 'rseq', 'rt', 'sched', 'seccomp', 'security', 'sem', 'semtimedop', 'sendfile64', 'sendmmsg', 'set', 'setfsgid32', 'setfsuid32', 'setgid32', 'setgroups32', 'setns', 'setregid32', 'setresgid32', 'setresuid32', 'setreuid32', 'setrlimit', 'setuid32', 'setxattr', 'shm', 'signalfd', 'signalfd4', 'socketcall', 'splice', 'stat64', 'statfs64', 'statvfs', 'statx', 'swap', 'symlinkat', 'sync', 'syncfs', 'syscall', 'sysmacros', 'tee', 'tgkill', 'time', 'timeb', 'timer', 'timerfd', 'timex', 'tkill', 'truncate64', 'ttydefaults', 'tuxcall', 'types', 'ugetrlimit', 'uio', 'umount2', 'un', 'unlinkat', 'unshare', 'user', 'userfaultfd', 'utimensat', 'utimes', 'utsname', 'vhangup', 'vmsplice', 'vserver', 'wait', 'waitid', 'xattr']
FoundSystemCallsRandom = list_syscalls(os.getcwd())
syscalls = [i for i in KnownSystemCallsBasicToAdvanced if i in FoundSystemCallsRandom]
syscalls += [i for i in FoundSystemCallsRandom if not i in syscalls]
print(syscalls)
print(len(syscalls))
