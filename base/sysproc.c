#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
  /* Either of the following will work. Does not harm to put them together. */
  outw(0xB004, 0x0|0x2000); // working for old qemu
  outw(0x604, 0x0|0x2000); // working for newer qemu
  
  return 0;
}

extern int sched_trace_enabled;
extern int sched_trace_counter;
int sys_enable_sched_trace(void)
{
  if (argint(0, &sched_trace_enabled) < 0)
  {
    cprintf("enable_sched_trace() failed!\n");
  }
  
  sched_trace_counter = 0;

  return 0;
}

int child_first;

int sys_fork_winner(void)
{
  if (argint(0, &child_first) < 0) {
    return -1;
  }

  return 0;
}

extern int schedPolicy;

int sys_set_sched(void) {
  int policyNum;
  // get arguments from user
  if (argint(0, &policyNum) < 0) {
    return -1;
  }
  if (policyNum == 0 || policyNum == 1) {
    // 0 for round robin and 1 for stride
    schedPolicy = policyNum;
    cprintf("Scheduling policy is currently %d\n", schedPolicy);
    return 0;
  }
  return -1;
}

int sys_tickets_owned(void)
{
  int pid;
  if(argint(0, &pid) < 0)
    return -1;

  return tickets_owned(pid);
}

int sys_transfer_tickets(void)
{
  int pid, tickets;
  
  if(argint(0, &pid) < 0 || argint(1, &tickets) < 0)
    return -1;
  
  return transfer_tickets(pid, tickets);
}
