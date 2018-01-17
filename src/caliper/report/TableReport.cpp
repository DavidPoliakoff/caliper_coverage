// Copyright (c) 2016, Lawrence Livermore National Security, LLC.  
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

/// \file  TableReport.cpp
/// \brief Generates text reports from Caliper snapshots on flush() events 

#include "caliper/report/TableReport.h"

#include "caliper/Caliper.h"
#include "caliper/SnapshotRecord.h"

using namespace cali;
using namespace cali::report;


void TableReport::report()
{
    Caliper c;

    c.flush(nullptr, 
            [this,&c](const SnapshotRecord* snapshot){
                using namespace std::placeholders;
                m_selector(c, snapshot->to_entrylist(), std::bind(&TableFormatter::process_record, &m_table_formatter, _1, _2));
                return true;
            });

    m_table_formatter.flush(c, *m_output_stream);
}

TableReport::TableReport(output_stream_type out, 
                         const std::string& attributes, 
                         const std::string& sort, 
                         const std::string& filter)
    : m_output_stream(out),
      m_table_formatter(attributes, sort),
      m_selector(filter)
{ }

TableReport::TableReport(std::ostream&      out, 
                         const char*        attributes, 
                         const char*        sort, 
                         const char*        filter)
    : m_output_stream(&out),
      m_table_formatter(attributes, sort),
      m_selector(filter)
{ }

TableReport::TableReport(FILE*              fp, 
                         const char*        attributes, 
                         const char*        sort, 
                         const char*        filter)
    : m_output_stream(new std::ostream(new FileBufferStream(fp))),
      m_table_formatter(attributes, sort),
      m_selector(filter)
{ }

TableReport::~TableReport()
{ }

