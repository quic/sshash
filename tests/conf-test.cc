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

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "sshash/macros.hpp"

typedef boost::property_tree::ptree ptree;

void load_json(const std::string f, ptree& pt)
{
	namespace jsp = boost::property_tree::json_parser;
	try
	{
		jsp::read_json(f, pt);
	}
	catch (std::exception &e)
	{
		std::cerr << "failed to load json: " << e.what() << "\n";
		exit(1);
	}
}

void load_xml(const std::string f, ptree& pt)
{
	namespace xsp = boost::property_tree::xml_parser;
	try
	{
		xsp::read_xml(f, pt);
	}
	catch (std::exception &e)
	{
		std::cerr << "failed to load json: " << e.what() << "\n";
		exit(1);
	}

	// Strip off the top 'xml' element to match json layout
	pt = pt.get_child("xml");
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cerr << "usage: conf-test <xml|json> input-file\n";
		exit(1);
	}

	std::string fmt   = argv[1];
	std::string input = argv[2];

	ptree pt;

	if (fmt == "json")
		load_json(input, pt);
	else if (fmt == "xml")
		load_xml(input, pt);
	else {
		std::cerr << "unsupported format\n";
		exit(1);
	}

	std::string l2 = pt.get<std::string>(std::string() + sshash_str("top-secret") + "." + sshash_str("level2"), std::string());
	if (l2 != "yes level2 looks good") {
		std::cerr << "level2 string doesn't match: [" << l2 << "]\n";
		exit(1);
	}

	std::string ev = pt.get<std::string>(std::string("public") + "." + "everyone", std::string());
	if (ev != "value is good") {
		std::cerr << "public string doesn't match\n";
		exit(1);
	}

	std::cout << "all strings match\n";

	return 0;
}
