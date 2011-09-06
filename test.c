
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


#include <termios.h>
#include <sys/fcntl.h>

int uart_test()
{
  struct termios termOptions;
  //  char        port[1024];
  int         ttyFid;
  int ret = 0;
  int i;
  
  char buf[64];
  for(i =0;i<64;i++)
    buf[i]='\0';

  // Open the tty:
  ttyFid = open( "/dev/ttyACM0", O_RDWR );
  if (ttyFid == -1)
    {
      printf( "Error unable to open port: /dev/ttyACM0\n" );
      return -1;
    }
  
  // Get the current options:
  if(tcgetattr( ttyFid, &termOptions ))
    perror("error on tcgetattr\n");


  // Set the input/output speed to 921.6kbps
  cfsetispeed( &termOptions, B57600 );
  cfsetospeed( &termOptions, B57600 );

  printf("speed: %i \n",cfgetospeed(&termOptions) );
  
  
  // Now set the term options (set immediately)
  tcsetattr( ttyFid, TCSANOW, &termOptions );
  
  
  //get Q1
  buf[0] = 0x1f;
  buf[1] = 0x34;
  buf[2] = 0x07;
  buf[3] = 0x01;
  buf[4] = 0x00;
  buf[5] = 0x00;
  buf[6] = 0x21;

  ret = write(ttyFid,buf,7);
  printf("write returns:%i\n",ret);

  sleep(2);

  ret = read(ttyFid,buf,64);

  printf("read returns:%i : %s\n",ret,buf);
  
  // All done
  close(ttyFid);
  printf( "Done, port speed set on: \n" );
  return 0;
}

union u_test
{
  void (*testfnk1)(int i);
  void (*testfnk2)(void);
};

void mitInt(int i)
{
  printf ("mitInt : %i\n",i);
}

void mitVoid()
{
  printf("mitVoid\n");
}

int main(int argc,char *argv[])
{
  uart_test();
  return 0;
  /*  //  printf("test\n");
  union u_test theTestUnion1;
  union u_test theTestUnion2;
  theTestUnion1.testfnk1 = mitInt;
  theTestUnion2.testfnk2 = mitVoid;

  (*theTestUnion1.testfnk1)(42);
  (*theTestUnion2.testfnk2)();
  
  printf("%c : 0x55",0x55);
  printf("%c : 0x55",0xAA);
  return 0;*/

}



