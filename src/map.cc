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
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "sshash/map.hpp"

namespace sshash {

bool map::update(const std::string& hash, const std::string& str, const std::string& elf)
{
	std::string __hs = hash + ".str";
	std::string __he = hash + ".elf";

	std::string ex = this->get<std::string>(__hs, std::string());
	if (!ex.empty())
		return false;

	this->put(__hs, str);
	this->put(__he, elf);

	return true;
}

bool map::load(const std::string& filename, bool optional)
{
	namespace jsp = boost::property_tree::json_parser;
	try
	{
		parent &p = *this;
		jsp::read_json(filename, p);
	}
	catch (std::exception &e)
	{
		if (optional)
			return true;
		std::cerr << "failed to load map: " << e.what() << "\n";
		return false;
	}
	return true;
}

bool map::load(std::istream &is)
{
	namespace jsp = boost::property_tree::json_parser;
	try
	{
		parent &p = *this;
		jsp::read_json(is, p);
	}
	catch (std::exception &e)
	{
		std::cerr << "failed to load map: " << e.what() << "\n";
		return false;
	}
	return true;
}

bool map::save(const std::string& filename)
{
	namespace jsp = boost::property_tree::json_parser;
	try
	{
		parent &p = *this;
		jsp::write_json(filename, p);
	}
	catch (std::exception &e)
	{
		std::cerr << "Failed to write config: " << e.what() << "\n";
		return false;
	}
	return true;
}

} // namespace sshash
