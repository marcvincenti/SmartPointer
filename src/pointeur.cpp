/*
	The MIT License (MIT)

	Copyright (c) 2016 Marc Vincenti

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "../include/pointeur.hpp"
#include <new> // bad_alloc

//définition des variables globales
mapZone zones = {};
bool sp_contexte_new = false;
bool sp_contexte_array = false;
void* sp_contexte_addr = NULL;
std::size_t sp_contexte_size = 0;


void* operator new(std::size_t size) throw(std::bad_alloc)
{
    void *storage = malloc(size);
    if(NULL == storage)
    {
        throw std::bad_alloc();
    }
    sp_contexte_new = true;
    sp_contexte_addr = storage;
    sp_contexte_size = size;
    return storage;
}

void* operator new[](std::size_t size) throw(std::bad_alloc)
{
    void *storage = malloc(size);
    if(NULL == storage)
    {
        throw std::bad_alloc();
    }
    sp_contexte_new = true;
    sp_contexte_array = true;
    sp_contexte_addr = storage;
    sp_contexte_size = size;
    return storage;
}

namespace std
{
template <typename T>
struct hash<SmartPointer<T>>
{
    size_t operator()(const SmartPointer<T> & x) const
    {
        return hash<T *>()(x.get());
    }
};

template<typename T>
void swap(SmartPointer<T>& lhs, SmartPointer<T>& rhs)
{
    if(rhs.refer)
    {
        if(lhs.tas)
        {
            if(lhs.refer)
                (lhs.refer.ref_cnt_tas)--;
            (rhs.refer.ref_cnt_tas)++;
        }
        else
        {
            if(lhs.refer)
                (lhs.refer.ref_cnt_pile)--;
            (rhs.refer.ref_cnt_pile)++;
        }
    }
    if(lhs.refer)
    {
        if(rhs.tas)
        {
            if(rhs.refer)
                (rhs.refer.ref_cnt_tas)--;
            (lhs.refer.ref_cnt_tas)++;
        }
        else
        {
            if(rhs.refer)
                (rhs.refer.ref_cnt_pile)--;
            (lhs.refer.ref_cnt_pile)++;
        }
    }
    std::swap(lhs.is_array, rhs.is_array);
    std::swap(lhs.refer, rhs.refer);
}
}
