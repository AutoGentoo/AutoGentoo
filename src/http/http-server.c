/*
 * http-server.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <http/server.h>
#include <signal.h>

int main(int argc, char* argv[])
{
  struct sockaddr_in clientaddr;
  socklen_t addrlen;
  char c;  
  
  //Default Values PATH = ~/ and PORT=10000
  char PORT[6];
  ROOT = getenv("PWD");
  strcpy(PORT,"10000");
  
  int usephp = 0;
  char * ip = malloc (sizeof(char) * 40);
  int slot=0;

  //Parsing the command line arguments
  while ((c = getopt (argc, argv, "i:p:r:h")) != -1)
  {
    switch (c)
    {
      case 'r':
        ROOT = malloc(strlen(optarg));
        strcpy(ROOT,optarg);
        break;
      case 'p':
        if (!strcmp (optarg, "10001"))
        {
          fprintf (stderr, "Can't bind to port 10001 (used by php server)\n");
          exit(1);
        }
        strcpy(PORT,optarg);
        break;
      case 'h':
        usephp = 1;
        break;
      case 'i':
        strcpy(ip,optarg);
        break;
      case '?':
        fprintf(stderr,"Wrong arguments given!!!\n");
        exit(1);
      default:
        exit(1);
    }
  }
  
  printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
  // Setting all elements to -1: signifies there is no client connected
  
  pid_t pid = fork();
  
  if (pid == 0)
  {
    if (usephp)
    {
      if (ip == NULL)
      {
        ip = "localhost";
      }
      printf ("Started a php server at root directory on port 10001\n");
      printf ("\n%s\n", ip);
      char * phpcmd = malloc (sizeof (char) * strlen("cd   && php -S :10001 ") + strlen (ROOT) + strlen(ip));
      sprintf (phpcmd, "cd %s && php -S %s:10001", ROOT, ip);
      system (phpcmd);
      free(phpcmd);
    }
  }
  else if (pid > 0)
  {
    int i;
    for (i=0; i<CONNMAX; i++)
      clients[i]=-1;
    startServer(PORT);
  
    // ACCEPT connections
    while (1)
    {
      addrlen = sizeof(clientaddr);
      clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
  
      if (clients[slot]<0)
        error ("accept() error");
      else
      {
        if ( fork()==0 )
        {
          respond(slot);
          exit(0);
        }
      }
  
      while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
  }
  else
  {
    printf("fork() failed!\n");
    return 1;
  }
  
  
  kill(pid, SIGTERM);
  
  return 0;
}
