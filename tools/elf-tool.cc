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

#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <exception>

#include "sshash/map.hpp"
#include "elf-parser.hpp"
#include "sha.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;
static po::variables_map optmap;

static bool opt_verbose = false;
static bool opt_dryrun  = false;

// Parse sshash padded strings from an ELF sections
class string_parser {
public:
	string_parser(int fd, unsigned int sect_offset, unsigned int sect_size)
		: _fd(fd), _start(sect_offset), _size(sect_size)
	{
		_end = _start + _size;
		_offset = _start;
	}

	// Get the next string
	// Returns content, offset and room till next string
	bool next(std::string& str, uint64_t& offset, size_t& room)
	{
		offset = _offset; // current offset
		_str.clear();

		try {
			do_read(true,  false); // main string
			do_read(false, true);  // zeros
			do_read(false, false); // sshash pad
			do_read(false, true);  // zeros
		} catch (std::exception& e) {
			std::cerr << "failed to read next string: " << e.what() << '\n';
			return false;
		}

		room = _offset - offset;
		str  = _str;
		return !str.empty();
	}

private:

	void do_read(bool append, bool null_term)
	{
		// std::cout << "READ: @" << std::hex << _offset << " append " << append << " null_term " << null_term << "\n";
		for (;_offset < _end; _offset++) {
			char c;
			int r = pread(_fd, &c, 1, _offset);
			if (r != 1)
				throw std::string("read failed: ") + strerror(errno);

			// std::cout << "READ: " << (isprint(c) ? c : '.') << " " << int(c) << " @" << std::hex << _offset <<"\n";

			if ((c == '\0') ^ null_term)
				return;

			if (append)
				_str += c;
		}
	}

	int _fd;
	uint64_t _start;
	uint64_t _end;
	uint64_t _size;
	uint64_t _offset;

	std::string _str;
};

static bool elf_process_section(sshash::map& map, sshash::sha& sha, const std::string& infile, int fd, const elf_parser::section_t& s)
{
	std::cout << "processing section: " << s.section_name << "\n";

	// For each string in section, generate hash, store hash to string mapping,
	// and replace the string with hash value.

	// Init string parser
	string_parser sp(fd, s.section_offset, s.section_size);

	std::string str;
	uint64_t str_offset, str_room;
	while (sp.next(str, str_offset, str_room)) {
		if (opt_verbose) {
			std::cout << "string:" 
				<< std::hex << " offset: " << str_offset
				<< std::dec << " room: "   << str_room
				<< " [" << str << "]\n";
		}

		if (str_room < sha.size()) {
			std::cerr << " not enough room for digest. missing pad???\n";
			return false;
		}

		// Hash the string
		std::string hash;
		sha.digest(hash, str);

		if (opt_verbose) {
			std::cout << "digest: " << hash << " [" << str << "]\n";
		}

		// Update the map
		if (!map.update(hash, str, infile)) {
			std::string s = map.get<std::string>(hash + ".str");
			if (s.compare(str) != 0) {
				std::cerr << infile << ": hash collision: " << hash << " [" << str << "] [" << s << "]\n";
				return false;
			}
		}

		// Write replacement string
		if (!opt_dryrun) {
			hash.resize(str_room, 0);
			if (pwrite(fd, hash.data(), str_room, str_offset) != (ssize_t) str_room) {
				std::cerr << infile << " write failed: " << strerror(errno) << "\n";
				return false;
			}
		}
	}

	return true;
}


static bool elf_process(sshash::map& map, sshash::sha& sha, const std::string& infile)
{
	std::cout << "processing " << infile << "\n";

	elf_parser::Elf_parser elf_parser(infile);
	if (elf_parser.failed()) {
		std::cerr << infile << ": readelf failed: " << elf_parser.last_error() << "\n";
		return false;
	}

	// Open ELF file for reading and writing
	int fd = open(infile.c_str(), O_RDWR);
	if (fd < 0) {
		std::cerr << infile << " open failed: " << strerror(errno) << "\n";
		return false;
	}

	// Find all .sshash.str sections
	unsigned int found = 0;
	std::vector<elf_parser::section_t> sections = elf_parser.get_sections();
	for (auto& s : sections) {
		if (s.section_name.find(".sshash.str") != std::string::npos) {
			if (!elf_process_section(map, sha, infile, fd, s))
				return false;
			found++;
		}
	}

	close(fd);

	if (!found)
		std::cout << "warn: " << infile << " : does not contain .sshash.str sections\n";

	return true;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> input;

	// **** Parse command line arguments ****
	po::options_description optdesc("sshash-elf -- tool for processing sshash stings in ELF files\n"
				"Usage: sshash-elf <--hashmap map> [options] [elf-input-files]\n"
				"Options");
	optdesc.add_options()
		("help", "Print this message")
		("input",     po::value<std::vector<std::string> >(&input)->composing(), "Input ELF file. Multiple files can be specified.")
		("hashmap,m", po::value<std::string>(), "Output hasmap file.")
		("minlen,L",  po::value<unsigned int>()->default_value(8), "Length of the hash value (aka min string length)")
		("dryrun",    "Generate hashmap file but do not modify input files")
		("verbose",   "Show verbose info (digest values, etc)");

	po::positional_options_description popt;
	popt.add("input", -1);

	po::store(po::command_line_parser(argc, argv).
	          options(optdesc).positional(popt).run(), optmap);

	po::notify(optmap);

	// ** All argument values (including the defaults) are now storred in 'optmap' **

	if (optmap.count("help") || input.empty()) {
		std::cout << optdesc << std::endl;
		return 1;
	}

	opt_verbose = optmap.count("verbose");
	opt_dryrun  = optmap.count("dryrun");

	char usage_banner[] = "usage: sshash-tool [<elf_file>] [<ssiMapFilename>]\n";
	if(argc < 3) {
		std::cerr << usage_banner;
		return -1;
	}

	sshash::map map;

	// Load map.
	// This may fail if the map doesn't exist yet.
	map.load(optmap["hashmap"].as<std::string>(), true /* optional */);

	// Init hash generator
	const unsigned int minlen = optmap["minlen"].as<unsigned int>();
	sshash::sha sha(minlen);

	// Process all inputs
	for (auto &i : input) {
		if (!elf_process(map, sha, i))
			break;
	}

	// Save updated map
	map.save(optmap["hashmap"].as<std::string>());
	return 0;
}
