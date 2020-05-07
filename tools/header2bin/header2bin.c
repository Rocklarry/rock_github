/*

Convert Moxa firmware images from header files into binary blobs
usable by the kernel firmware loader.

Copyright Andrew Lunn <andrew@lunn.ch> 2013

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "UPort1250FW.h"
#include "UPort1250IFW.h"
#include "UPort1410FW.h"
#include "UPort1450FW.h"
#include "UPort1450IFW.h"
#include "UPort1610_16FW.h"
#include "UPort1610_8FW.h"
#include "UPort1650_16FW.h"
#include "UPort1650_8FW.h"
#include "dbmdx_UseCases_Common.h"
#include "dbmdx_UseCases.h"
#include "untitdd.h"


static void
header2bin(const unsigned char *data, size_t len, const int productid)
{
	char buf[32];
	int fd;
	int ret;
	
	snprintf(buf, sizeof(buf) - 1, "moxa-%04x.fw", productid);
	
	fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		fprintf(stderr, "Creating file %s: %s\n", buf,
			strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	ret = write(fd, data, len);
	if (ret != len) {
		fprintf(stderr, "Write truncated!\n");
		exit (EXIT_FAILURE);
	}
	if (ret < 0) {
		fprintf(stderr, "Writing data to file: %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}

	close(fd);
}


int
main(int argc, char * argv[])
{
/*
	header2bin(UPort1250FW, sizeof(UPort1250FW), 1250);
	header2bin(UPort1250IFW, sizeof(UPort1250IFW), 1251);
	header2bin(UPort1410FW, sizeof(UPort1410FW), 1410);
	header2bin(UPort1450FW, sizeof(UPort1450FW), 1450);
	header2bin(UPort1450IFW, sizeof(UPort1450IFW), 1451);
	header2bin(UPort1610_8FW, sizeof(UPort1610_8FW), 1618);
	header2bin(UPort1610_16FW, sizeof(UPort1610_16FW), 1613);
	header2bin(UPort1650_8FW, sizeof(UPort1650_8FW), 1658);
	header2bin(UPort1650_16FW, sizeof(UPort1650_16FW), 1653);
*/

	header2bin(Addon_ASRP, sizeof(Addon_ASRP), 000);
	header2bin(Addon_Pomelo, sizeof(Addon_Pomelo), 001);
	header2bin(Addon_Sensory, sizeof(Addon_Sensory), 002);
	header2bin(UPort1610_16FW, sizeof(UPort1610_16FW), 003);
	header2bin(Alexa_Model, sizeof(Alexa_Model), 004);
	header2bin(ASRP_Pomelo, sizeof(ASRP_Pomelo), 005);
	header2bin(UPort2222FW, sizeof(UPort2222FW), 006);


	return EXIT_SUCCESS;
}
