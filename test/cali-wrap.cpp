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

// A minimal Caliper instrumentation demo 

#include <caliper/Annotation.h>
#include <math.h>
#include <caliper/CaliFunctional.h>

int doWork(int* inArray, int size){
    int reduce_var = 0;
    for(int i=0; i<size; i++){
        inArray[i] = i*i;
        reduce_var += i*i;
    }
    return  reduce_var;
}

template<typename T>
T* initialize(size_t data_size, T initial_value){
    T* array = (T*)malloc(sizeof(T)*data_size);
    for(size_t i=0;i<data_size;i++){
        array[i] = initial_value;
    }
    return array;
}

auto doWorkWrapped = cali::wrap_function_and_args("doWork", doWork);



int main(int argc, char* argv[])
{
    constexpr int data_size = 1000000;
    constexpr int iterations = 10;
    int data_size_increment = data_size/iterations;
    cali::wrap("Program",[&](){
        int * inArray;
        cali::wrap("Initialization",[&](){
            inArray = cali::wrap_with_args("initializer",initialize<int>,data_size, 0);
        });
        for(int size = data_size_increment; size<=data_size; size+=data_size_increment){
            doWorkWrapped(inArray,size);
        }
    });
}
