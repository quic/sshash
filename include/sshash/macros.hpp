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

#ifndef SSHASH_MACROS_HPP
#define SSHASH_MACROS_HPP

// Preprocessor tricks for creating unique identifiers
#define __sshash_pp_cat(x,y) x##y
#define sshash_pp_cat(x,y) __sshash_pp_cat(x,y)
#define  __sshash_pp_str(x) #x
#define sshash_pp_str(x) __sshash_pp_str(x)

// Macro for ELF section placement attribute.
#if defined(__clang__)

// Simple section placement works with clang
#define __sshash_str_section() __attribute__((section(".sshash.str")))

#elif defined(__GNUC__)

// This version generates unique .sshash.str.N section for each invocation
// to avoid conflicts with GCC section attribute restrictions between different
// type of inline,non-inline,etc functions.
#define __sshash_str_section() __attribute__((section(sshash_pp_str(sshash_pp_cat(.sshash.str., __COUNTER__)))))

#else // GNUC
#error "sshash unsupported compiler"
#endif

#define __sshash_str(str, cnt) ({ static constexpr char __sshash_str_section() sshash_pp_cat(__hstr, cnt)[] = str"\0~~~~~~"; sshash_pp_cat(__hstr, cnt); })

// String literal for placing into sshash ELF section.
// The strings are padded by 7 characters to ensure enough room for 8 character SHA digest
#define sshash_str(str) __sshash_str(str, __COUNTER__)

#endif
