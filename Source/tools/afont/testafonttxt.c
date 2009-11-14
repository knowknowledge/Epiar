/* testafonttxt.c */

#include <stdio.h>
#include "afont.h"

#define FONTFILE "font.af"

int main( int argc, char **argv )
{
  afont *a;

  a = afont_load(FONTFILE);
  if(!a) {
    fprintf(stderr, "Couldn't load font %s\n", FONTFILE);
    return 0;
  }

  afont_dump_char(a, 'A');
  afont_dump_char(a, 'S');
  afont_dump_char(a, 'D');
  afont_dump_char(a, 'F');
  afont_dump_char(a, '7');

  return 0;
}
