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

#include <stdlib.h>

#include "hogl/format-basic.hpp"
#include "hogl/plugin/format.hpp"

#include "sshash/map.hpp"

// Loadable format plugin with sshash support
namespace sshash {

// Custom format handler
class ssformat : public hogl::format_basic {
private:
	sshash::map _map;

public:
	ssformat(const std::string& hashmap, const std::string& spec) :
		hogl::format_basic(spec.c_str())
	{
		// Load map
		if (!_map.load(hashmap)) {
			fprintf(stderr, "Failed to load hashmap file\n");
			fflush(stderr);
			abort();
		}
	}

	// Process log record (called from hogl::engine -> hogl::output)
	virtual void process(hogl::ostrbuf &sb, const hogl::format::data &d);

	const char* unhash(const char *str)
	{
		// Lookup the string in hashmap.
		// Return as is if not found, otherwise return the original string.
		try {
			auto& pt = _map.get_child(std::string(str) + ".str");
			return pt.data().c_str();
		} catch (std::exception &e) {
			return str;
		}
	}

	const char* get_arg_str(const hogl::record& r, unsigned int type, unsigned int i)
	{
		if (type == hogl::arg::CSTR) {
			unsigned int len;
			const char *str = (const char*) r.get_arg_data(i, len);
			if (!len)
				return "null";
			return str;
		}

		uint64_t ptr;
		if (hogl::arg::is_32bit(type))
			ptr = r.get_arg_val32(i);
		else
			ptr = r.get_arg_val64(i);
		return (const char *) ptr;
	}
};

void ssformat::process(hogl::ostrbuf &sb, const hogl::format::data &d)
{
	const hogl::record &r = *d.record;

	record_data rd = {};
	rd.record    = d.record;
	rd.ring_name = d.ring_name;
	rd.next_arg  = 0;

	// Preprocess names
	const hogl::area *area = r.area;
	if (area) {
		rd.area_name = unhash(area->name());
		rd.sect_name = unhash(area->section_name(r.section));
	} else {
		rd.area_name = "INVALID";
		rd.sect_name = "INVALID";
	}

	// Preprocess strings
	for (unsigned int i = 0; i < hogl::record::NARGS; i++) {
		unsigned int type = r.get_arg_type(i);
		if (type == hogl::arg::NONE)
			break;
		if (type == hogl::arg::CSTR || type == hogl::arg::GSTR)
			rd.arg_str[i] = unhash(get_arg_str(r, type, i));
	}

	if (_fields == DEFAULT)
		format_basic::default_header(sb, rd);
	else if (_fields == FAST0)
		format_basic::fast0_header(sb, rd);
	else if (_fields == FAST1)
		format_basic::fast1_header(sb, rd);
	else
		format_basic::flexi_header(sb, rd);

	unsigned int t0 = r.get_arg_type(0);
	unsigned int t1 = r.get_arg_type(1);

	if ((t0 == hogl::arg::CSTR || t0 == hogl::arg::GSTR) && t1 != hogl::arg::NONE)
		format_basic::output_fmt(sb, rd);
	else if (t0 == hogl::arg::RAW)
		format_basic::output_raw(sb, rd);
	else
		format_basic::output_plain(sb, rd);
}

} // namespace sshash

// Allocate and initialize format plugin.
// Returns a pointer to initialized hogl::format instance
static hogl::format* create(const char *)
{
	const char *hashmap = getenv("SSHASH_FMT_HASHMAP");
	if (!hashmap) {
		fprintf(stderr, "SSHASH_FMT_HASHMAP env variable is not defined\n");
		fflush(stderr);
		abort();
	}
	const char *spec = getenv("SSHASH_FMT_SPEC");
	if (!spec)
		spec = "fast1";

	return new sshash::ssformat(hashmap, spec);
}

// Release all memmory allocated by format plugin.
static void release(hogl::format *f)
{
	auto *fmt = reinterpret_cast<sshash::ssformat *>(f);
	delete fmt;
}

// Each plugin must export this symbol
hogl::plugin::format __hogl_plugin_format = { create, release };
