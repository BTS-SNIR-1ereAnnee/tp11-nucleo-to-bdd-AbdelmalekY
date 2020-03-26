#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>         // descripteurs de fichiers ( open()...)
#include <termios.h>    //
#include <errno.h>
#include <iostream>
#include <mariadb/mysql.h>
#include <stdlib.h>

#define HOST "localhost"
#define USER "bts"
#define PWD  "snir"
#define BASE "capteurs"

/* baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h> */
#define BAUDRATE B115200            
/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyS2"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;
int main()
{
      int sfd, c, res;
    struct termios newtio;
    char buf[255];
    ssize_t size;
    
    sfd = open(MODEMDEVICE, O_RDONLY | O_NOCTTY );
    if (sfd == -1)
    {
      printf ("Error  no is : %d\n", errno);
      printf("Error description is : %s\n",strerror(errno));
      return(-1);
    }
    bzero(&newtio, sizeof(newtio));
       
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
         
        
    newtio.c_iflag = IGNPAR | ICRNL;
         
       
    newtio.c_lflag = ICANON;
         

    
     tcflush(sfd, TCIFLUSH);
     tcsetattr(sfd,TCSANOW,&newtio);
    
    for (int i =0 ; i<50 ; i++)
    {
        
    /*
      ** On lit :
      ** on passe a read :
      ** - le fd,
      ** - le buffer
      ** - la taille du buffer
      ** Attention si tu passe une taille de buffer plus grande que la taille de ton buffer,
      ** ton programme deviens sensible aux Buffer Overflow
      */
      size = read (sfd, buf, 127);
   
      /*
      ** On raoute un '\0' à la fin de la chaine lut, pour être sur d'avoir une chaine de caractères valide.
      ** size correspondant a l'index du dernier caractere du buffer + 1.
      ** Ceci est utile si tu veux utiliser ta chaine dans une fonction comme strcmp() ou printf()
      */
 
      buf[size] = 0;
   
      /*
      ** On affiche ce que l'on viens de lire dans la console :
      ** NOTE :
      ** il existe des FD speciaux :
      ** Le fd 1 est la sortie standart ( console )
      */
 
      //write (1, buf, size);
      std::cout << i << " " << buf << "\n";
      }
 
  /* Ne pas oublier de libérer ton file descriptor */
  close(sfd);
 
MYSQL *conn;
if ((conn = mysql_init(NULL)) == NULL)
{
fprintf(stderr, "Could not init DB\n");
return EXIT_FAILURE;
}
if (mysql_real_connect(conn, "localhost", "bts", "snir", "capteurs", 0, NULL,
0) == NULL)
{
fprintf(stderr, "DB Connection Error\n");
return EXIT_FAILURE;
}
if (mysql_query(conn, "INSERT INTO mesures (temperature,humidite,pression) VALUES ('10','8','9')") != 0)
{
fprintf(stderr, "Query Failure\n");
return EXIT_FAILURE;
}
mysql_close(conn);
return EXIT_SUCCESS;
  return 0;
}
