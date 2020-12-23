#include <execinfo.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

const int a = 10;  // text
static int c = 2;  // data
static int b;
int d = 1;
int e;  // bss

void print_backtrace(void) {
  static const char start[] = "BACKTRACE ------------\n";
  static const char end[] = "----------------------\n";

  void *bt[1024];
  int bt_size;
  char **bt_syms;
  int i;

  bt_size = backtrace(bt, 1024);
  bt_syms = backtrace_symbols(bt, bt_size);
//  full_write(STDERR_FILENO, start, strlen(start));
  for (i = 1; i < bt_size; i++) {
    printf("%s\n", bt_syms[i]);

//    size_t len = strlen(bt_syms[i]);
//    full_write(STDERR_FILENO, bt_syms[i], len);
//    full_write(STDERR_FILENO, "\n", 1);
  }
//  full_write(STDERR_FILENO, end, strlen(end));
  free(bt_syms);
}

void test2(){
  std::string test2 = "test2";
  print_backtrace();
}

void test1(){
  test2();
}

int main(int argc, char *argv[]) {
  int len = 0;
  char *string = NULL;
  char *ptr1;
  char *ptr2;
  char *ptr3;
  char *ptr4;
  char *p_malloc = (char *)malloc(1024);  // heap
  memcpy(p_malloc, "hello core dump!", 16);

  char *shem = NULL;
  int shmid = 0;

  const int a1 = 10;
  static int c1 = 2;
  static int b1;
  int d1 = 1;
  int e1;

  int fd = open("111.txt", O_RDWR);
  char cmd[1024];
  sprintf(cmd, "echo '%s' > /proc/%d/coredump_filter", argv[1],
          getpid());  //设定coredump范围

  system(cmd);

  // filebackend 共享内存映射
  ptr1 =
      (char *)mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  // filebackend 私有内存映射
  ptr2 =
      (char *)mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  //匿名私有内存 映射
  ptr3 = (char *)mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  //匿名共享内存 映射
  ptr4 = (char *)mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_SHARED, -1, 0);

  memcpy(ptr3, "hello core dump again!", 22);

  // systemV share 内存
  shmid = shmget(IPC_PRIVATE, 4 * 1024, IPC_CREAT | 0600);
  shem = (char *)shmat(shmid, NULL, 0);

  printf("p_malloc = 0x%x\n", p_malloc);

  printf("globle constant = 0x%x\n", &a);
  printf("globle static init address = 0x%x\n", &c);
  printf("globle static address = 0x%x\n", &b);
  printf("globle init address = 0x%x\n", &d);
  printf("globle address = 0x%x\n", &e);

  printf("local constant = 0x%x\n", &a1);
  printf("local static init address = 0x%x\n", &c1);
  printf("local static address = 0x%x\n", &b1);

  printf("stack address = 0x%x\n", &len);

  printf("fd shared ptr= 0x%x\n", ptr1);
  printf("fd private ptr = 0x%x\n", ptr2);
  printf("ANONYMOUS private = 0x%x\n", ptr3);
  printf("ANONYMOUS shared = 0x%x\n", ptr4);

  printf("ANONYMOUS private content = %s\n", ptr3);

  printf("shm id = 0x%x\n", shmid);
  printf("shm pointer = 0x%x\n", shem);

  // 使用pmap查看本进程的内存
  memset(cmd, 0, 1024);
  sprintf(cmd, "pmap -d %d", getpid());
  system(cmd);

  test1();
  return 0;

  // 自身core dump
  int *p = 0;
  printf("p is: %d\n", *p);

  return 0;

  // gcore dump 出core
  memset(cmd, 0, 1024);
  sprintf(cmd, "gcore  %d", getpid());
  system(cmd);

  free(p_malloc);

  shmctl(shmid, IPC_RMID, NULL);

  munmap(ptr1, 4 * 1024);
  munmap(ptr2, 4 * 1024);
  munmap(ptr3, 4 * 1024);
  munmap(ptr4, 4 * 1024);
  close(fd);

  return 0;
}

// g++ -g ./main.cpp
