

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <time.h>

#include <boost/program_options.hpp>


namespace po = boost::program_options;

size_t randNumber()
{
  size_t num;
  char *_num = (char*) &num;
  for (int i = 0 ; i < 8 ; i++ )
    _num[i] = rand()%255;

  return num;

}

int main( int argC, char *argV[] )
{



    int line_length;

    size_t max_size;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("length,l", po::value<int>(&line_length)->default_value(6), "set line length")
        ("size,s", po::value<size_t>(&max_size)->default_value(10000000), "set the size of generated data")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argC, argV, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
    	fprintf(stderr, "Usage: %s [-l len] [-s size]\n", argV[0] );
        exit(1);
    }


  srand( time(NULL) );

  size_t total_size = 0;

  while( total_size < max_size )
  {

    char line[1024];

    for (int i = 0 ; i < line_length ; i++ )
      line[i] = 97 + rand()%20;
    line[line_length]='\0';

    total_size += fprintf(stdout, "%s\n", line );   
  }

  fprintf( stderr , "Total generated %lu bytes\n", total_size );

  return 0;
}
