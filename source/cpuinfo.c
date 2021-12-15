#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "cpuinfo.h"

int get_lpi_info(lpi_info *info)
{
   	FILE *fp;
   	char buffer[1024];
   	char hardware[1024];
   	char revision[1024];
   	int found = 0;
   	int len;

   	printf("get_lpi_info\n");
	return 0;
}
