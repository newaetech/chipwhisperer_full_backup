/* gf256_table_gen.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \file     gf256_table_gen.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-13
 * \license  GPLv3 or later
 * 
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "gf256mul.h"

int print_header =  1,
    print_braces =  1,
    print_base   =  1,
    print_align  =  1,
    print_prefix =  1;

char* fmt_strings[]={"%d", "%3d",
                     "%X", "%2.2X",
                     "%o", "%3o",
                     "%d", "%3d",
                     "0x%X", "0x%2.2X",
                     "0%o", "0%3o"};
                     
void print_help(void){
	puts(
"gf256_table_gen [-cCbBdhoaApP] [-l min] [-u max]\n"
"                [-f formatstring] [-o file] -r reducer value {values} \n"
"\n"
" -c/-C turn C-header on/off (default: on)\n"
" -b/-B turn braces on/off (defaul: on)\n"
" -d/-h/-8 print values in decimal/hexadecimal/octal (default: hexadecimal)\n"
" -a/-a  turn alignment on/off (default: on)\n"
" -p/-P  turn base prefix on/off (default: on)\n"
" -r reducer reducer is the reduction polynomial represented as numeric value\n"
" -l min starting value for multiplication table (default: 0)\n"
" -u max upper limit value for multiplication table (default: 255)\n"		
" -f formatstring string for formating the output of a singel multiplicxation\n"
" -o file outputfile\n"
);
}
                     
int main(int argc, char** argv){
	int i,j;
	int c;
	int option_index=0;
	char* fmt=NULL;
	static struct option long_options[] =
             {
               /* These options set a flag. */
               {"print-header",    no_argument, &print_header, 1},
               {"no-print-header", no_argument, &print_header, 0},
               {"print-braces",    no_argument, &print_braces, 1},
               {"no-print-braces", no_argument, &print_braces, 0},
               {"print-dec",       no_argument, &print_base, 0},
               {"print-hex",       no_argument, &print_base, 1},
               {"print-octal",     no_argument, &print_base, 2},
               {"align",           no_argument, &print_align, 1},
               {"no-align",        no_argument, &print_align, 0},
               {"print-prefix",    no_argument, &print_prefix, 1},
               {"no-print-prefix", no_argument, &print_prefix, 0},
               
               /* These options don't set a flag.
                  We distinguish them by their indices. */
               {"help",    no_argument      , 0, '?'},     
               {"reducer", required_argument, 0, 'r'},
               {"min",     required_argument, 0, 'l'},
               {"max",     required_argument, 0, 'u'},
               {"format",  required_argument, 0, 'f'},
               {0, 0, 0, 0}
             };

	
	
	unsigned long ul_a;
	int columns=8;
	unsigned long ul_reducer=0x00, max=0xff, min=0x00;
	uint8_t reducer, a;
	char** eptr;
	FILE* of = stdout;
	while(c!=-1){
		c = getopt_long(argc, argv,"cCbBdh8aApPr:l:u:f:", 
		                long_options, &option_index);
		switch(c){
			case 'c': print_header=1; break;
			case 'C': print_header=0; break;
			case 'b': print_braces=1; break;
			case 'B': print_braces=0; break;
			case 'h': print_base = 1; break;
			case 'd': print_base = 0; break;
			case '8': print_base = 2; break;
			case 'a': print_align =1; break;
			case 'A': print_align =0; break;
			case 'p': print_prefix=1; break;
			case 'P': print_prefix=0; break;
			case 'r': ul_reducer = strtoul(optarg, eptr, 0);
			          if((**eptr)!='\0'){
					     fprintf(stderr, "Error, invalid reducer value \"%s\"!\n",
					             optarg);
					     return -1;
					  }
					  break;
			case 'l': min = strtoul(optarg, eptr, 0);
			          if(((**eptr)!='\0')||min>0xff){
					     fprintf(stderr, "Error, invalid minimum value \"%s\"!\n",
					             optarg);
					     return -1;
					  }
					  break;
			case 'u': max = strtoul(optarg, eptr, 0);
			          if(((**eptr)!='\0')||max>0xff){
					     fprintf(stderr, "Error, invalid maximum value \"%s\"!\n",
					             optarg);
					     return -1;
					  }
					  break;
			case 'f': fmt = optarg; break;
			case  -1: break;
			case '?': 
			default: 
				printf ("unknown option '%c' (%2.2X) !\n\t", c, c);
				print_help();
				return 0;
				break;
		}
	}
	if(!(reducer = ul_reducer&0xff)){
			fprintf(stderr, "You must specify a reductionpolynomial!\n", argv[i]);
			return -1;
	}
	if(!fmt)
		fmt = fmt_strings[print_prefix*6+print_base*2+print_align];
	
	for(i=optind; i<argc; ++i){
		ul_a=strtoul(argv[i], eptr, 0)&0xff;
		if(((**eptr)!='\0')|| ul_a>0xff){
			fprintf(stderr, "Error, invalid number \"%s\"!\n", argv[i]);
			return -1;
		}
		a = ul_a;
		if(print_header)
			fprintf(of,"\nuint8_t lut_gfmul_%s_%2.2x",argv[i],reducer);
		if(print_braces)
			fprintf(of," = {\n\t");
		
		for(j=min; j<=max-1; ++j){
			fprintf(of,fmt,gf256mul(a, j, reducer));
			if(j%columns==columns-1){
				fprintf(of, ",\n\t");
			}else{
				fprintf(of, ", ");
			}
		}
		fprintf(of, fmt, gf256mul(a,max, reducer));
		if(print_braces)
			fprintf(of, " }");
		fprintf(of, "\n\n");	
	}
	return 0;
}
