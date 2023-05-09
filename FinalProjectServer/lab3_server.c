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
#include <math.h>


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

#define KPX 0.0003
#define KDX 0.00008
#define KIX 0.0000

#define KPY 0.0003
#define KDY 0.00008
#define KIY 0.00000

#define BUTTERFILT_ORD 4
double b[BUTTERFILT_ORD + 1] = {0.2346995, 0.93879798, 1.40819697, 0.93879798, 0.2346995};
double a[BUTTERFILT_ORD + 1] = {1.0, 1.30660514, 1.03045384, 0.36236904, 0.0557639};

void printBits(uint16_t num) {
  int i = 0;
  for (; i < (2 * 8); i++) {
    printf("%i ", num & 0x01);
    num = num >> 1;
  }
}

int getx_butter(int cur, double * a, double * b)
{
	double retval;
	int i;

	static double inx[BUTTERFILT_ORD+1];
	static double outx[BUTTERFILT_ORD+1];
	
	// Perform sample shift
	for (i = BUTTERFILT_ORD; i > 0; --i) {
		inx[i] = inx[i-1];
		outx[i] = outx[i-1];
	}
	inx[0] = cur;

	// Compute filtered value
	retval = 0;
	for (i = 0; i < BUTTERFILT_ORD+1; ++i) {
		retval += inx[i] * b[i];
		if (i > 0)
			retval -= outx[i] * a[i];
	}
	outx[0] = retval;

	return retval;
}

int gety_butter(int cur, double * a, double * b)
{
	double retval;
	int i;

	static double iny[BUTTERFILT_ORD+1];
	static double outy[BUTTERFILT_ORD+1];
	
	// Perform sample shift
	for (i = BUTTERFILT_ORD; i > 0; --i) {
		iny[i] = iny[i-1];
		outy[i] = outy[i-1];
	}
	iny[0] = cur;

	// Compute filtered value
	retval = 0;
	for (i = 0; i < BUTTERFILT_ORD+1; ++i) {
		retval += iny[i] * b[i];
		if (i > 0)
			retval -= outy[i] * a[i];
	}
	outy[0] = retval;

	return retval;
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
    
    int eXp = 0;
    int eYp = 0;
    
    uint16_t errSumX = 0;
    uint16_t errSumY = 0;
    
    double pm1 = 0.9;
    double pm2 = 0.9;
    
    double ppm1 = 0.9;
    double ppm2 = 0.9;
    
	while (1)
	{

        
		int ack = 0;

		int byte_read;
		
		uint16_t ball_x;
		uint16_t ball_y;

		while (!ack)
		{

			byte_read = 0;


            
			unsigned char new_pr[4];
            
			while (byte_read < 4)
			{
				byte_read += read(ifd, new_pr + byte_read, 4 - byte_read);
			}
			// receive and print the x position of ball in terminal

            ball_x = (new_pr[1] << 8) | new_pr[0];
            ball_y = (new_pr[3] << 8) | new_pr[2];
            printf("ball_x: %d and ball_y: %d\n", ball_x, ball_y);
            
//            ball_x = (uint16_t)getx_butter(ball_x, b, a);
//            ball_y = (uint16_t)gety_butter(ball_y, b, a);
//            printf("FILTERED!!! ball_x: %d and ball_y: %d\n", ball_x, ball_y);
            
            //int errX = getx_butter(ball_x, b, a) - 1635;
            //int errY = gety_butter(ball_y, b, a) - 1500;
		int errX = ball_x - 1635;
		int errY = ball_y - 1500;
            errSumX += errX;
            if (errSumX > 60000 || errSumX < -60000) errSumX = 0;
            errSumY += errY;
            if (errSumY > 60000 || errSumY < -60000) errSumY = 0;

            double angleX = 0.0 - KPX * (double)errX * (3.1415926 / 2.0) - KDX * (double)(errX - eXp) / 0.05 * (3.1415926 / 2.0) - KIX * (double)(errSumX) * 0.05 * (3.1415926 / 2.0);
            double angleY = 0.0 - KPY * (double)errY * (3.1415926 / 2.0) - KDY * (double)(errY - eYp) / 0.05 * (3.1415926 / 2.0) - KIY * (double)(errSumY) * 0.05 * (3.1415926 / 2.0);
            printf("angleX: %f and angleY: %f\n", angleX, angleY);
            
            eXp = errX;
            eYp = errY;
            
            double m1 = ((2.1 - 0.9) / 3.1415926 * (angleX + 3.1415926 / 2.0) + 0.9);
            double m2 = ((2.1 - 0.9) / 3.1415926 * (angleY + 3.1415926 / 2.0) + 0.9) * 0.3 + pm2 * 0.7;
            printf("m1: %f and m2: %f\n", m1, m2);
            ppm1 = pm1;
            ppm2 = pm2;
            pm1 = m1;
            pm2 = m2;
            
            if (m1 > 2.1) m1 = 2.1;
            if (m1 < 0.9) m1 = 0.9;
            if (m2 > 2.1) m2 = 2.1;
            if (m2 < 0.9) m2 = 0.9;
            
            uint16_t dutyX = (uint16_t)(4000.0 * (20.0 - m1) / 20.0);
            uint16_t dutyY = (uint16_t)(4000.0 * (20.0 - m2) / 20.0);
            
            char msg[4];
            
            msg[0] = dutyX & 0xff;
            msg[1] = (dutyX >> 8) & 0xff;
            msg[2] = dutyY & 0xff;
            msg[3] = (dutyY >> 8) & 0xff;
            
	printf("dutyX: %u and dutyY: %u\n", dutyX, dutyY);
	write(ofd, msg, 4);
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
