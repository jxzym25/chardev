#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>  
//needed for IO things. Attention that this is different from kernel mode 
int lcd; 
#define SCULL_IOC_MAGIC  'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1) 
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, unsigned long)
#define SCULL_READ  _IOR(SCULL_IOC_MAGIC, 3, unsigned long)
#define SCULL_WRITE_READ _IOWR(SCULL_IOC_MAGIC, 4, unsigned long)
void test() 
{ 
  int k, i, sum; 
  char* s = "JERRYCHOU"; 
 
//  memset(s, '2', sizeof(s)); 
  printf("test begin!\n"); 
 
  //printf("write %s\n", s); 
  //k = ioctl(lcd, SCULL_WRITE, s); 
  //char user_msg[4096];
  //k = ioctl(lcd, SCULL_READ, user_msg);
  //printf("read %s\n", user_msg);

  char user_msg[4096] = "JERRYCHOU_READ_WRITE\0";
  k = ioctl(lcd, SCULL_WRITE_READ, user_msg);
  printf("read %s\n", user_msg);
} 
int main(int argc, char **argv) 
{ 
  lcd = open("/dev/zhouyiming", O_RDWR); 
  if (lcd == -1){
      perror("unable to open lcd"); 
      exit(EXIT_FAILURE); 
  } 
 
 
test(); 
 
  close(lcd); 
  return 0; 
} 
