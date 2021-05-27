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
#include <unordered_map>

#include <openssl/sha.h>

#include "sha.hpp"

int main(int argc, char *argv[])
{
	unsigned int abbrev = 7;

	if (argc > 1)
	       abbrev = atoi(argv[1]);	

	std::unordered_map<std::string, std::string> mymap;

	sshash::sha myhash(abbrev);

	for (uint64_t i=0; i<99999999; i++) {
		std::stringstream ss;
		ss << "this is a test " << i;

		std::string out;
		myhash.digest(out, ss.str());

		//printf("hash: [%s] --> %s\n", ss.str().c_str(), out.c_str());

		auto it = mymap.find(out);
		if (it != mymap.end()) {
			printf("conflict: %s [%s] [%s]\n", out.c_str(), it->second.c_str(), ss.str().c_str());
			continue;
		}

		mymap[out] = ss.str();
	}

	return 0;
}
