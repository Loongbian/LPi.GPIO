#ifndef _CPUINFO_H
#define _CPUINFO_H
typedef struct
{
   int p1_revision;
   char *ram;
   char *manufacturer;
   char *processor;
   char *type;
   char revision[1024];
} lpi_info;

int get_lpi_info(lpi_info *info);
#endif
