//  Copyright (c) 2021, Qualcomm Innovation Center, Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//  SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include <hogl/format-basic.hpp>
#include <hogl/format-raw.hpp>
#include <hogl/output-stdout.hpp>
#include <hogl/output-stderr.hpp>
#include <hogl/output-plainfile.hpp>
#include <hogl/output-pipe.hpp>
#include <hogl/engine.hpp>
#include <hogl/area.hpp>
#include <hogl/mask.hpp>
#include <hogl/post.hpp>

#include "sshash/macros.hpp"

static const hogl::area *test_area;

#define __ssi(str) hogl::arg_gstr(sshash_str(str))
#define dbglogss(area, sect, fmt, args...) hogl::post(area, area->sect, __ssi(fmt), ##args)
#define dbglog(area, sect, fmt, args...)   hogl::post(area, area->sect, hogl::arg_gstr(fmt), ##args)

template <typename T>
void test_temp(T a)
{
	dbglog(test_area, DEBUG, __ssi("template function arg: [%s]"), std::to_string(a));
}

void doTest(unsigned int nloops)
{
	for (unsigned int n=0; n < nloops; ++n) {
        	// Basic cases
		dbglog(test_area, INFO, "plain 0-args no-SSI");
		dbglog(test_area, INFO, __ssi("explictly marked SSI 0-args"));
		dbglogss(test_area, INFO, "sensitive info 0-args");
		dbglogss(test_area, DEBUG, "sensitive debug [%d] [%d]", n*100, n*200);
		dbglog(test_area, WARN, "plain warn 2-str-args: [%s] [%s]", "abc", "xyz");
		dbglog(test_area, ERROR, "plain error 2-ssi-args: [%s] [%s]", __ssi("top secret ABC info"), __ssi("top secret EDF info"));
		dbglogss(test_area, DEBUG, "sensitive debug 2-ssi-arg: [%s]", __ssi("top secret XYZ info"));
		dbglogss(test_area, INFO, "short SSI");

		test_temp(25);
		test_temp(124.56);

		// Special chars
		dbglogss(test_area, INFO, "SSI with newline characters \n that will be\n preserved");
		dbglogss(test_area, DEBUG, "!");

		dbglogss(test_area, WARN, "SSI contains special chars\"\"\'\'\t\\n~*?[];()}");
	}
}

// Command line args {
static struct option main_lopts[] = {
   {"help",    0, 0, 'h'},
   {"nloops",  1, 0, 'N'},
   {"log-format",  1, 0, 'f'},
   {"log-output",  1, 0, 'o'},
   {0, 0, 0, 0}
};

static char main_sopts[] = "hN:f:o:";

static char main_help[] =
   "SSHASH basic test 0.1 \n"
   "Usage:\n"
      "\tbasic_test [options]\n"
   "Options:\n"
      "\t--help -h            Display help text\n"
      "\t--log-format -f <name>   Log format (basic, raw)\n"
      "\t--log-output -o <name>   Log output (file name, stderr, pipe)\n"
      "";
// }

int main(int argc, char *argv[])
{
	std::string log_output("stdout");
	std::string log_format("fast1");
	unsigned int nloops = 1;
	int opt;

	// Parse command line args
	while ((opt = getopt_long(argc, argv, main_sopts, main_lopts, NULL)) != -1) {
		switch (opt) {
		case 'f':
			log_format = optarg;
			break;

		case 'o':
			log_output = optarg;
			break;
	
		case 'N':
			nloops = atoi(optarg);
			break;

		case 'h':
		default:
			printf("%s", main_help);
			exit(0);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 0) {
		printf("%s", main_help);
		exit(1);
	}

	hogl::format *lf;
	hogl::output *lo;

	if (log_format == "raw")
		lf = new hogl::format_raw();
	else
		lf = new hogl::format_basic(log_format.c_str());

	if (log_output == "stderr")
		lo = new hogl::output_stderr(*lf);
	else if (log_output == "stdout")
		lo = new hogl::output_stdout(*lf);
	else if (log_output[0] == '|')
		lo = new hogl::output_pipe(log_output.substr(1).c_str(), *lf);
	else
		lo = new hogl::output_plainfile(log_output.c_str(), *lf);

	hogl::mask logmask(".*", 0);

	hogl::activate(*lo);

	test_area = hogl::add_area(sshash_str("TEST-AREA"));

	hogl::apply_mask(logmask);

	doTest(nloops);

	hogl::deactivate();

	delete lo;
	delete lf;

	return 0;
}
