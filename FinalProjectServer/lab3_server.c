/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: template file for serial          */
/*                communication server              */
/*                                                  */
/****************************************************/
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "pc_crc16.h"
#include "lab3.h"


#define GREETING_STR                \
	"CS454/654 - Lab 3 Server\n"    \
	"Author: Renato Mancuso (BU)\n" \
	"(Enter a message to send.  Type \"quit\" to exit)\n"

#define USAGE_STR                                                      \
	"\nUSAGE: %s [-v] [-t percentage] <dev>\n"                         \
	"   -v \t\t Verbose output\n"                                      \
	"   -t \t\t Invoke troll with specified bit flipping percentage\n" \
	"   <dev> \t Path to serial terminal device to use, e.g. /dev/ttyUSB0\n\n"

#define TROLL_PATH "./lab3_troll"

void printBits(uint16_t num) {
  int i = 0;
  for (; i < (2 * 8); i++) {
    printf("%i ", num & 0x01);
    num = num >> 1;
  }
}

int main(int argc, char *argv[])
{
	double troll_pct = 0.3;		   // Perturbation % for the troll (if needed)
	int ifd, ofd, i, N, troll = 0; // Input and Output file descriptors (serial/troll)
	char str[MSG_BYTES_MSG], opt;  // String input
	struct termios oldtio, tio;	   // Serial configuration parameters
	int VERBOSE = 0;			   // Verbose output - can be overriden with -v
	int dev_name_len;
	char *dev_name = NULL;

	/* Parse command line options */
	while ((opt = getopt(argc, argv, "-t:v")) != -1)
	{
		switch (opt)
		{
		case 1:
			dev_name_len = strlen(optarg);
			dev_name = (char *)malloc(dev_name_len);
			strncpy(dev_name, optarg, dev_name_len);
			break;
		case 't':
			troll = 1;
			troll_pct = atof(optarg);
			break;
		case 'v':
			VERBOSE = 1;
			break;
		default:
			break;
		}
	}

	/* Check if a device name has been passed */
	if (!dev_name)
	{
		fprintf(stderr, USAGE_STR, argv[0]);
		exit(EXIT_FAILURE);
	}

	// Open the serial port (/dev/ttyS1) read-write
	ifd = open(dev_name, O_RDWR | O_NOCTTY);
	if (ifd < 0)
	{
		perror(dev_name);
		exit(EXIT_FAILURE);
	}
	printf(GREETING_STR);

	// Start the troll if necessary
	if (troll)
	{
		// Open troll process (lab5_troll) for output only
		FILE *pfile;   // Process FILE for troll (used locally only)
		char cmd[128]; // Shell command

		snprintf(cmd, 128, TROLL_PATH " -p%f %s %s", troll_pct,
				 (VERBOSE) ? "-v" : "", dev_name);

		pfile = popen(cmd, "w");
		if (!pfile)
		{
			perror(TROLL_PATH);
			exit(-1);
		}
		ofd = fileno(pfile);
	}
	else
		ofd = ifd; // Use the serial port for both input and output

	//
	// WRITE ME: Set up the serial port parameters and data format
	//
	/* Get old settings to restore at exit */
	tcgetattr(ifd, &oldtio);
	/* Setup desired settings */
	// B9600: baud rate
	// CS8: 8 data bits
	// CLOCAL: ignore modem control lines
	// CREAD: if read
	tio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	/* Flush any pending request on the port */
	tcflush(ifd, TCIFLUSH);
	/* Set new attributes for serial port */
	tcsetattr(ifd, TCSANOW, &tio);

	// pointer for str, current character that we're writing to
	char *sptr;
	// length of str/message body
	unsigned char n_read = 0;
	while (1)
	{
	  	//
		// WRITE ME: Read a line of input (Hint: use fgetc(stdin) to read each character)
		//
	  //		sptr = str;
	  //	printf("> ");
	  //
		// Read a line of input
	  //	while ((sptr < str + MSG_BYTES_MSG - 1) &&
	  //		((*sptr++ = fgetc(stdin)) != '\n'));
	  //	if (sptr >= str + MSG_BYTES_MSG - 1)
	  //		printf("Warning: %d characters exceeded\n", MSG_BYTES_MSG);
	  //	*(--sptr) = 0; // terminate string, removing newline
	  //
	  //	if (strcmp(str, "quit") == 0)
	  //		break;

		// get length of message body
	  //	for (n_read = 0; str[n_read] != '\0'; ++n_read);

		//
		// WRITE ME: Compute crc (only lowest 16 bits are returned)
		//
		// get crc of msg
	  //	int crc = pc_crc16(str, n_read);
        // swap the bytes for crc using bit operation
        //crc = ((crc << 8) & 0xff00) | ((crc >> 8) & 0x00ff);
		int ack = 0;
	//	int attempts = 0;
	//	unsigned char s;
		int byte_read;
		
		uint16_t ball_x;
		uint16_t ball_y;

		while (!ack)
		{
		  //printf("Sending (attempt %d)...\n", ++attempts);

			//
			// WRITE ME: Send message
			//
			//s = 0x0;
			//write(ofd, &s, 1);
			//write(ofd, &crc, 2);
			//write(ofd, &n_read, 1);
			//write(ofd, str, n_read);

			//printf("Message sent, waiting for ack...\n ");

			//
			// WRITE ME: Wait for MSG_ACK or MSG_NACK
			//
            // a buffer to read the ack/nack msg
			byte_read = 0;

			// a new pointer only used to read ack/nack msg
			//char new_pr[2];
			// read the msg
			//while (byte_read < 2)
			  //{
			  //byte_read += read(ifd, new_pr, 2);
				//}
			// receive and print the x position of ball in terminal
			//ball_x = (new_pr[1] << 8) | new_pr[0];
			//printf("ball_x: %d and ball_y: %d\n", ball_x, 0);

			// a new pointer only used to read ack/nack msg
			char new_pr[4];
			// read the msg
			while (byte_read < 4)
			{
				byte_read += read(ifd, new_pr, 4);
			}
			// receive and print the x position of ball in terminal
			ball_x = (new_pr[1] << 8) | new_pr[2];
			ball_y = (new_pr[0] << 8) | new_pr[3];
			printf("ball_x: %d and ball_y: %d\n", ball_x, ball_y);
		}
		
		printf("\n");
	}

	//
	// WRITE ME: Reset the serial port parameters
	//
	tcflush(ifd, TCIFLUSH);
	tcsetattr(ifd, TCSANOW, &oldtio);

	// Close the serial port
	close(ifd);

	return EXIT_SUCCESS;
}
