/*
Copyright (C) 2020  Léo Germond

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdbool.h>

#include <log.h>

int success = true;

int a(int a1, char *a2) {
	log_enter("%d, \"%s\"", a1, a2);
	log_exit("success");
	return 0;
}

void verbose(void) {
	int one = 1;
	log_int(one);
	int ten = 10;
	log_hex(ten);

	char *toto = "tata";
	log_str(toto);

	a(1, "hello");
}

void user(void) {
#define THIS_IS(sparta) sparta("this is "#sparta)
	THIS_IS(err);
	THIS_IS(warn);
	THIS_IS(info);
	THIS_IS(dbg);
	THIS_IS(trace);

	THIS_IS(err_loc);
	THIS_IS(warn_loc);
	THIS_IS(info_loc);
	THIS_IS(dbg_loc);
	THIS_IS(trace_loc);
}

void asserts(void) {
	ASSERT_MSG(false, "ASSERT_MSG");

	ASSERT_OR_DIE(false);
	err("SHOULD NOT BE REACHED");
	success = false;
die:
	return;
}

void all(void) {
	user();
	verbose();
	asserts();
}

int main(void) {
	printf("\n**** without name, not verbose\n");
	all();

	printf("\n**** without name, verbose:\n");
	log_verbose = true;
	all();

	printf("\n**** with name, verbose:\n");
	log_name = "test_log";
	all();
	return success?EXIT_SUCCESS:EXIT_FAILURE;
}
