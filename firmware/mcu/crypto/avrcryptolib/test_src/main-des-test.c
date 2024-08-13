/* main-des-test.c */
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
/*
 * des test-suit
 * 
*/

#include "main-test-common.h"

#include "des.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_des.h"
#include "bcal_tdes.h"
#include "bcal_tdes2.h"

char* algo_name = "DES";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&des_desc,
	(bcdesc_t*)&tdes2_desc,
	(bcdesc_t*)&tdes_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_des(const char* param){
	if(!param){
		bcal_nessie_multiple(algolist);
	}else{
		uint8_t i=0;
		bcdesc_t* ptr;
		for(;;){
			ptr = (bcdesc_t*)pgm_read_word(&algolist[i++]);
			if(ptr == NULL){
				cli_putstr_P(PSTR("\r\nunknown algorithm: "));
				cli_putstr(param);
				cli_putstr_P(PSTR("\r\navailable algorithms are:"));
				i = 0;
				while((ptr = (bcdesc_t*)pgm_read_word(&algolist[i++]))){
					cli_putstr_P(PSTR("\r\n\t"));
					cli_putstr_P((const char*)pgm_read_word(&ptr->name));
				}
				return;
			}
			if(!strcmp_P(param, (const char*)pgm_read_word(&ptr->name))){
				bcal_nessie(ptr);
				return;
			}
		}
	}
}

void testrun_performance_des(void){
	bcal_performance_multiple(algolist);
}
/*****************************************************************************
 * main						
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,  (void*)1, (void_fpt)testrun_nessie_des },
	{ test_str,    (void*)1, (void_fpt)testrun_nessie_des },
	{ performance_str, NULL, testrun_performance_des},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main(void) {
	main_setup();

	for(;;){
		welcome_msg(algo_name);
		cmd_interface(cmdlist);
    }

}

