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
#include <cstring>
#include <string>

#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>

#include "sha.hpp"

namespace sshash {


sha::sha(unsigned int abbrev) : _abbrev(abbrev)
{
	// Setup alpha-numeric map
	// This give us 2,478,652,606,080 permutations (about 42 bits)
	char *ptr = _map;
	for (char i='A'; i<='Z'; i++) *ptr++ = i; // base map
	for (char i='a'; i<='z'; i++) *ptr++ = i; // base map
	for (char i='0'; i<='9'; i++) *ptr++ = i; // full map
	*ptr = '\0';
}

bool sha::digest(std::string &out, const char *str, unsigned int n)
{
	if (!str)
		return false;

	unsigned char md_out[EVP_MAX_MD_SIZE];
	const int md_len = 8;

#ifdef SN_shake128
	// Use SHAKE128 with 8 byte digest length
	const EVP_MD *md = EVP_shake128();
	if (!md)
		return false;

	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, str, n);

	EVP_DigestFinalXOF(mdctx, md_out, md_len);

	EVP_MD_CTX_free(mdctx);
#else
	SHA512((const unsigned char *)str, n, md_out);
#endif

	// Convert digest into uint64
	uint64_t d = 0;
	for (unsigned int i = 0; i < md_len; i++)
		d = (d << 8) | md_out[i];

	// Convert into alpha-numeric string
	out.clear();

	// First char is generated using only the basemap to ensure the hash str
	// never starts with a digit.
	out += _map[d % BASE_MAPSIZE];
	d /= BASE_MAPSIZE;

	for (; out.size() < _abbrev; d /= FULL_MAPSIZE)
		out += _map[d % FULL_MAPSIZE];

	return true;
}

} // namespace sshash
