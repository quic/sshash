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

#ifndef SSHASH_SHA
#define SSHASH_SHA

namespace sshash {

// SHA string handler
class sha {
public:
	// Init SHA handler
	// @param abbrev length of the output hash string
	sha(unsigned int abbrev = 7);

	// Process str and generate digest (aka hash output)
	// @param out output string
	// @param str input string
	// @param n   input string length
	bool digest(std::string& out, const char *str, unsigned int n);

	// Process str and generate digest (aka hash output)
	// @param out output string
	// @param in  input string
	bool digest(std::string& out, const std::string& in)
	{
		return digest(out, in.c_str(), in.size());
	}

	// Size/length of the hash string
	size_t abbrev() const { return _abbrev; }
	size_t size() const { return _abbrev; }

private:
	unsigned int _abbrev;

	// Map of characters used for mapping binary SHA digest
	enum {
		BASE_MAPSIZE = 52,
		FULL_MAPSIZE = 62
	};
	char _map[FULL_MAPSIZE+1];
};

} // namespace sshash

#endif // SSHASH_SHA
