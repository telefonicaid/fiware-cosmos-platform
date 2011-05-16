

#include <stdio.h>
#include <cstring>
#include <stdlib.h>

int main( int argc, char *argv[] )
{

#define LONG_LINEA 1024
  char linea[LONG_LINEA];
  size_t prev_number = 0;
  size_t act_number = 0;
  FILE *fp;

  if( argc != 2 )
    {
      printf("Usage: %s file_to_check\n", argv[0] );
      exit(0);
    }

    if ((fp = fopen(argv[1], "r")) == NULL)
    {
	fprintf(stderr, "Error opening input file: '%s'\n", argv[1]);
	perror(argv[1]);
	exit(-1);
    }

    size_t rec = 0;
    size_t num_err = 0;
    bool first = true;
    while (fgets(linea, LONG_LINEA, fp) != NULL)
    {
	sscanf(linea, "%lu\n", &act_number);
	if (act_number < prev_number)
	{
		if (first)
		{
			fprintf(stdout, "First sort failed at rec:%lu, prev_number:%lu, act_number:%lu\n", rec, prev_number, act_number);
			first = false;
		}
		num_err++;
	}
	prev_number = act_number;
	rec++;
    }

    if (num_err)
    {
	fprintf(stdout, "ERROR: num_err:%lu in %lu records, input_file:'%s'\n", num_err, rec, argv[1]);
	return(1);
    }
    else
    {
	fprintf(stdout, "SUCCESS: No error in %lu records, input_file:'%s'\n", rec, argv[1]);
	return(0);
    }
}
