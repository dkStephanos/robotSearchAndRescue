#include "Log.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;

// Message record
struct Message {
    int from;
    char payload[32];
};

int checkForPortNumber(char** argv, int argc) {
   char* p;
   if(argv[1] && argv[2] && strcmp(argv[1], "-p") == 0) {
      if(strtol(argv[2], &p, 10) > 1024) {
         return 1;
      } else {
         //If the port number given isn't an int above 1024, return 0 so we know we have a bad port number
         return 0;
      }
   } else {
      //Return -1 so we know the -p command wasn't passed correctly
      return -1;
   }
}

int main(int argc, char** argv )
{
   int sockdesc;            // Socket descriptor
   struct addrinfo *myinfo; // Address record
   char portnum[81];
   Message mymessage;
   int connection;
   int value;
   Log log1;

    // Create the socket with domain, type, protocol as
   //    internet, stream, default
   sockdesc = socket(AF_INET, SOCK_STREAM, 0);
   if ( sockdesc < 0 )
   {
      cout << "Error creating socket" << endl;
      exit(0);
   }

   int status = checkForPortNumber(argv, argc);

   if(status == 0) {
      cout << "Invalid port number. Must be an int above 1024.\n";
      exit(0);
   } else if(status == -1) {
      cout << "First argument must be '-p', second argument must specify port number.\n";
      exit(0);
   } else {
      strcpy(portnum, argv[2]);
   }

   // Set up the address record
   if ( getaddrinfo("127.0.0.1", portnum, NULL, &myinfo) != 0 )
   {
      cout << "Error getting address" << endl;
      exit(0);
   }

   // Bind the socket to an address
   if (bind(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen) < 0 )
   {
      cout << "Error binding to socket" << endl;
      exit(0);
   }

   // Now listen to the socket
   if ( listen(sockdesc, 1) < 0 )
   {
      cout << "Error in listen" << endl;
      exit(0);
   }

   cout << "Calling accept" << endl;
   // Accept a connect, check the returned descriptor
   connection = accept(sockdesc, NULL, NULL);
   if ( connection < 0 )
   {
      cout << "Error in accept" << endl;
      exit(0);
   }

   value = read(connection, (char*)&mymessage, sizeof(Message));

   log1.setLogfileName(mymessage.payload);

   // Open the log file
   if(log1.open() < 0) {
      cout << "Error opening log file" << endl;
      exit(0);
   }

   // Note: this loop is typical of servers that accept multiple
   // connections on the same port - normally, after accept( )
   // returns, you'd fork off a process to handle that request, or
   // create a thread to do the same, passing the returned value as
   // a parameter for read( ) and write( ) to use -
   // that is, accept( ) returns a new descriptor - 
   // you actually talk on a different socket in the child.
   // The main program would then loop around, and wait at accept( )
   // for another request for a connection, then hand that new
   // connection off to a child or thread, etc.
   // So the server runs forever - see the for loop parameters -
   // well, until you kill it manually.  Here, we break from
   // the for loop after one message.
   for(;;)
   {
	 // Here's where the fork( ) or pthread_create( ) call would
	 // normally go, passing connection (returned by accept( )
	 // above) as a parameter.  connection is a file descriptor
	 // attached to a different port, so that the server can
	 // continue to accept connections on the original port.
	 //
	 // Instead of all that, this program just does the
	 // following:
         // Read exactly one message
	 // Note that the first parameter of read is the returned
	 // value from accept( ) above.
	 value = read(connection, (char*)&mymessage, sizeof(Message));
	 // Log the message
	 log1.writeLogRecord(mymessage.payload);

	 // Display the message
	 cout << "Server received: " << endl;
	 cout << "  cvalue: " << mymessage.payload << endl;

   if(strcmp(mymessage.payload, "Q") == 0) {
      break;
   }

   } // for

   // Close the connection
   close(connection);
   log1.close();
   return 0;

} // main( )

