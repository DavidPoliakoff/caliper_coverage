// Copyright (c) 2015, Lawrence Livermore National Security, LLC.  
// Produced at the Lawrence Livermore National Laboratory.
//
// This file is part of Caliper.
// Written by David Boehme, boehme3@llnl.gov.
// LLNL-CODE-678900
// All rights reserved.
//
// For details, see https://github.com/scalability-llnl/Caliper.
// Please also see the LICENSE file for our additional BSD notice.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the disclaimer below.
//  * Redistributions in binary form must reproduce the above copyright notice, this list of
//    conditions and the disclaimer (as noted below) in the documentation and/or other materials
//    provided with the distribution.
//  * Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse
//    or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/// @file Variant.cpp
/// Variant datatype implementation

#include "caliper/common/Variant.h"

#include "caliper/common/StringConverter.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <sstream>

using namespace cali;

cali_id_t
Variant::to_id(bool* okptr) const
{
    bool      ok = false;
    cali_id_t id = to_uint(&ok);

    if (okptr)
        *okptr = ok;

    return ok ? id : CALI_INV_ID;
}

std::string
Variant::to_string() const
{
    std::string ret;

    switch (this->type()) {
    case CALI_TYPE_INV:
        break;
    case CALI_TYPE_USR:
    {
        size_t size = this->size();
        const void* ptr = data();
            
        std::ostringstream os;

        std::copy(static_cast<const unsigned char*>(ptr), static_cast<const unsigned char*>(ptr) + size,
                  std::ostream_iterator<unsigned>(os << std::hex << std::setw(2) << std::setfill('0'), ":"));

        ret = os.str();
    }
        break;
    case CALI_TYPE_INT:
        ret = std::to_string(to_int());
        break;
    case CALI_TYPE_UINT:
        ret = std::to_string(to_uint());
        break;
    case CALI_TYPE_STRING:
    {
        const char* str = static_cast<const char*>(data());
        std::size_t len = size();

        if (len && str[len-1] == 0)
            --len;
        
        ret.assign(str, len);
    }    
        break;
    case CALI_TYPE_ADDR:
    {
        std::ostringstream os;
        os << std::hex << to_uint();
        ret = os.str();
    }
        break;
    case CALI_TYPE_DOUBLE:
        ret = std::to_string(to_double());
        break;
    case CALI_TYPE_BOOL:
        ret = to_bool() ? "true" : "false";
        break;
    case CALI_TYPE_TYPE:
    {
        ret = cali_type2string(to_attr_type());
    }
        break;
    }

    return ret;
}

Variant
Variant::from_string(cali_attr_type type, const char* str, bool* okptr)
{
    Variant ret;
    bool    ok = false;
    
    switch (type) {
    case CALI_TYPE_INV:
    case CALI_TYPE_USR:
        // Can't convert USR or INV types at this point
        break;
    case CALI_TYPE_STRING:
        ret = Variant(CALI_TYPE_STRING, str, strlen(str));
        ok  = true;
        break;
    case CALI_TYPE_INT:
        {
            int i = StringConverter(str).to_int(&ok);

            if (ok)
                ret = Variant(i);
        }
        break;
    case CALI_TYPE_ADDR:
        {
            uint64_t u = StringConverter(str).to_uint(&ok, 16);

            if (ok)
                ret = Variant(CALI_TYPE_ADDR, &u, sizeof(uint64_t));
        }
        break;
    case CALI_TYPE_UINT:
        {
            uint64_t u = StringConverter(str).to_uint(&ok);

            if (ok)
                ret = Variant(CALI_TYPE_UINT, &u, sizeof(uint64_t));
        }
        break;
    case CALI_TYPE_DOUBLE:
        {
            double d = StringConverter(str).to_double(&ok);

            if (ok)
                ret = Variant(d);
        }
        break;
    case CALI_TYPE_BOOL:
        {
            bool b = StringConverter(str).to_bool(&ok);
                
            if (ok)
                ret = Variant(b);
        }
        break;
    case CALI_TYPE_TYPE:
        {
            cali_attr_type type = cali_string2type(str);
            ok = (type != CALI_TYPE_INV);

            if (ok)
                ret = Variant(type);
        }
        break;
    }

    if (okptr)
        *okptr = ok;

    return ret;
}

std::ostream& cali::operator << (std::ostream& os, const Variant& v)
{
    os << v.to_string();
    return os;
}
