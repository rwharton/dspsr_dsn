#include <stdio.h>
#include <stdlib.h>

void error_message(char *message) /*includefile */
{
  fprintf(stderr,"ERROR: %s\n",message);
  exit(1);
}
