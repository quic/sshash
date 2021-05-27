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

#ifndef SSHASH_MAP_HPP
#define SSHASH_MAP_HPP

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include <string>
#include <iostream>

#include <boost/property_tree/ptree.hpp>

namespace sshash {

class map : public boost::property_tree::ptree {
public:
	typedef boost::property_tree::ptree parent;
	using parent::put;
	using parent::get;

	/**
         * Populate hash map from a file
         * @param filename name of the JSON file to load
         * @param optional don't fail if the file does not exists
         * @return true on success, false on failure
         */
	bool load(const std::string& filename, bool optional = false);

	/**
         * Populate hash map from input stream
         * @param is input stream that contains JSON to load
         * @return true on success, false on failure
         */
	bool load(std::istream &is);

	/**
         * Save hash map into a file
         * @param filename name of the JSON file to save to
         * @return true on success, false on failure
         */
	bool save(const std::string& filename);

	/**
 	 * Update hash map
	 * Returns false if hash already exists, and true otherwise.
 	 */
	bool update(const std::string& hash, const std::string& str, const std::string& elf);

private:
	void dump_subtree(const std::string& path, const parent &p);
};

} // namespace sshash

#endif // SSHASH_MAP_HPP
