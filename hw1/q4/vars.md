# Sample Output

Please refer to `q4.c`.

```bash
$ ./a.out
Parent: = 29090         # pid1, line 35
Root: = 25738           # pid0, line 38
Child PID = 29091       # pid2, line 20
Parent PID = 29090      # pid1, line 21
Grandchild PID = 29092  # pid4, line 30
Child PID = 29091       # pid5, line 31
```

# Analysis

| VAR  | VALUE | DESC                                                                   |
|------|-------|------------------------------------------------------------------------|
| pid  | 29091 | pid of child process; same as pid2                                     |
| pid0 | 25738 | pid of parent of initial process; i.e. "root"                          |
| pid1 | 29090 | pid of parent of child process; i.e. pid of initial process            |
| pid2 | 29091 | pid of child process                                                   |
| pid3 | 29092 | pid of grandchild process; same as pid4                                |
| pid4 | 29092 | pid of grandchild process; same as pid3                                |
| pid5 | 29091 | pid of parent process of grandchild process; i.e. pid of child process |

## Synopsis

The initial program forks, creating a child process.  The initial process
continues to the `wait` statement on line 39 (_Initial process_, below).

The child process forks, creating a grandchild process.  The child process
continues (_Child process_, below).

The grandchild process continues (_Grandchild process_, below).

After all child and grandchild processes terminate, the initial process resumes
at the line immediately following the call to `wait`.

### Initial process

After the call to `fork()` on line 11, the initial process proceeds to the
`else` statement on line 34, having failed to meet the other conditions of the
_if-else_ control structure.

The PID of the initial process is assigned to `pid1`. The PID of the initial
process's parent - i.e. "root" - is assigned to `pid0`.  The values of `pid1` 
and `pid0` are printed as "Parent" and "Root", respectively.  The parent process 
then `wait`s until all child processes terminate, then terminates after the
`return` statement on line 42.

### Child process

In the child process, the PID of the parent process (that is, the initial
process) is assigned to `pid1`.  Note that this variable receives the value of
two separate calls - a call to `getppid()` on line 17, and a call to `getpid()`
on line 35, creating a race condition. However, since each of these calls refers 
to the same process - that is, the initial process - this race condition is
essentially mitigated.

The values of `pid2` and `pid1` are printed as "Child PID" and "Parent PID",
respectively.

On line 22 there is another call to `fork()`, creating a grandchild process
whose PID is assigned to `pid3`.

### Grandchild process

In the grandchild process, the PID of the grandchild process is assigned to 
`pid4`.  This is the same as the value assigned to `pid3` in the child process,
above.  The PID of of the parent process of the grandchild process (that is, the 
child process), is assigned to `pid5`. 

The values of `pid4` and `pid5` are printed as "Grandchild PID" and "Child
PID", respectively.
