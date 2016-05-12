/*
	The MIT License (MIT)

	Copyright (c) 2016 Marc 

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

#ifndef POINTEUR_HPP
#define POINTEUR_HPP

#include <map>
#include <list>
#include <assert.h>
#include <algorithm>


//======================================================================
//  Declarations classe SmartPointer, struct compteur et fonctions amies
//======================================================================


template<typename T> class SmartPointer;
typedef std::pair<void*, std::pair<size_t, std::list<void*>>> paireZone;
// <addresse pointée, <taille objet pointé, liste adresses pointeur pointant> >
typedef std::map<void*, std::pair<size_t, std::list<void*>>> mapZone;


//déclaration des variables globales --> pas thread-safe
extern mapZone zones;
extern bool sp_contexte_new;
extern bool sp_contexte_array;
extern void* sp_contexte_addr;
extern std::size_t sp_contexte_size;

template <typename T>
struct compteur
{
    T * ptr;
    int ref_cnt_tas;
    int ref_cnt_pile;
    bool heap = true; //l'objet pointé est-il dans le tas?
};

template <typename U>
std::ostream & operator << (std::ostream & os, SmartPointer<U> const& smrt_ptr);

template <typename U>
std::istream & operator << (std::istream & is, SmartPointer<U> & smrt_ptr);


//======================================================================
//  Définition classe SmartPointer
//======================================================================

template<typename T>
class SmartPointer
{
public:

    SmartPointer();//Constructeur par défaut
    SmartPointer(T&);//Constructeur par valeur
    SmartPointer(T*);//Constructeur depuis une adresse
    SmartPointer(const SmartPointer<T>&);//Constructeur par copie

    SmartPointer& operator=(const SmartPointer&);//Surcharge de l'affectation

    operator bool() const;

    T& operator*();//Surcharge de l'opérateur *
    T* operator->();//Surcharge du déréferencement
    T& operator[](unsigned int);//Opérateur []

    SmartPointer& operator++();// Opérateur ++ préfixé
    SmartPointer operator++(int);// Opérateur ++ postfixé
    SmartPointer& operator--();// Opérateur -- préfixé
    SmartPointer operator--(int);// Opérateur -- postfixé

    void* get() const;//Accès lecture adresse
    bool getTas() const;
    unsigned getRefHeap() const;
    unsigned getRefSatck() const;

    ~SmartPointer();//Destructeur

    template <typename U> friend void std::swap(SmartPointer<U>& , SmartPointer<U>&);
    template <typename U> friend size_t hash(SmartPointer<U>&);

    template <typename U> friend std::ostream& operator << (std::ostream &, SmartPointer<U> const&);
    template <typename U> friend std::istream& operator >> (std::istream &, SmartPointer<U> &);

private:

    //Fonctions
    void flush();
    void cycle_search(void *);
    bool was_born_in_heap(void *) const;

    //Attributs
    bool is_array;//le smartPointeur contient-il un tableau ?
    bool tas;//le smartPointeur est-il dans le tas ?
    compteur<T> * refer;//la structure reste à la fin de l'objet

};


//======================================================================
// Implantation des fonctions membres de SmartPointer
//======================================================================

template <typename T>
unsigned SmartPointer<T>::getRefHeap() const
{
    if(refer)
        return refer->ref_cnt_tas;
    return 0;
}

template <typename T>
unsigned SmartPointer<T>::getRefSatck() const
{
    if(refer)
        return refer->ref_cnt_pile;
    return 0;
}


template <typename T>
bool SmartPointer<T>::was_born_in_heap(void* addr) const
{
    return (addr && addr >= sp_contexte_addr && (char*)(addr) <= ((char*)sp_contexte_addr + sp_contexte_size));
}

template <typename T>
SmartPointer<T>::SmartPointer() : refer(NULL), tas(was_born_in_heap(this)), is_array(false)
{
    mapZone::iterator it = zones.find(NULL);
    if(it != zones.end())
        it->second.second.push_back(this);
    else
        zones.insert(paireZone(NULL, std::pair<size_t, std::list<void*>>(0, {this})));
}

template <typename T>
SmartPointer<T>::SmartPointer(T& obj) : tas(was_born_in_heap(this)), is_array(false)
{
    refer = new compteur<T>;
    refer->ptr = &obj;
    if(tas)
    {
        refer->ref_cnt_tas = 1;
        refer->ref_cnt_pile = 0;
    }
    else
    {
        refer->ref_cnt_tas = 0;
        refer->ref_cnt_pile = 1;
    }
    refer->heap = false;
    mapZone::iterator it = zones.find((void*)&obj);
    if(it != zones.end())
        it->second.second.push_back(this);
    else
        zones.insert(paireZone((void*)&obj, std::pair<size_t, std::list<void*>>(sizeof(T), {this})));
}

template <typename T>
SmartPointer<T>::SmartPointer(T* ptr) : tas(was_born_in_heap(this)), is_array(/*was_born_in_heap(ptr) && */sp_contexte_array)
{
    if(ptr)
    {
        refer = new compteur<T>;
        refer->ptr = ptr;
        if(tas)
        {
            refer->ref_cnt_tas = 1;
            refer->ref_cnt_pile = 0;
        }
        else
        {
            refer->ref_cnt_tas = 0;
            refer->ref_cnt_pile = 1;
        }

        sp_contexte_array = false;
        mapZone::iterator it = zones.find((void*)ptr);
        assert(it == zones.end());//ne pas fournir une adresse extraite d'un autre SmartPointer
        zones.insert(paireZone((void*)ptr, std::pair<size_t, std::list<void*>>(sizeof(T), {this})));
    }
    else
    {
        refer = NULL;
        mapZone::iterator it = zones.find(NULL);
        if(it != zones.end())
            it->second.second.push_back(this);
        else
            zones.insert(paireZone(NULL, std::pair<size_t, std::list<void*>>(0, {this})));

    }
}

template <typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T>& rhs) : refer(rhs.refer), tas(was_born_in_heap(this)), is_array(rhs.is_array)
{
    if(refer)
    {
        if(tas)
        {
            (refer->ref_cnt_tas)++;
        }
        else
        {
            (refer->ref_cnt_pile)++;
        }
        mapZone::iterator it = zones.find((void*)rhs.refer->ptr);
        it->second.second.push_back(this);
    }
    else
    {
        mapZone::iterator it = zones.find(NULL);
        if(it != zones.end())
            it->second.second.push_back(this);
        else
            zones.insert(paireZone(NULL, std::pair<size_t, std::list<void*>>(0, {this})));
    }
}

template <typename T>
SmartPointer<T>& SmartPointer<T>::operator=(const SmartPointer<T>& rhs)
{
    sp_contexte_addr = NULL;
    sp_contexte_size = 0;
    if(*this != rhs)  // auto-assignement
    {
        flush();
        refer = rhs.refer;
        is_array=rhs.is_array;
        if(rhs.refer != NULL)
        {
            if(tas)
            {
                (refer->ref_cnt_tas)++;
            }
            else
            {
                (refer->ref_cnt_pile)++;
            }
            mapZone::iterator it = zones.find(rhs.get());
            assert(it != zones.end());
            it->second.second.push_back(this);
        }
        else
        {
            mapZone::iterator it = zones.find(NULL);
            if(it != zones.end())
                it->second.second.push_back(this);
            else
                zones.insert(paireZone(NULL, std::pair<size_t, std::list<void*>>(0, {this})));
        }
    }
    return *this;
}

template <typename T>
SmartPointer<T>::operator bool ()const
{
    return refer !=0 ;
}

template <typename T>
T& SmartPointer<T>::operator*()
{
    return *(refer->ptr);
}

template <typename T>
T* SmartPointer<T>::operator->()
{
    return refer->ptr;
}

template <typename T>
T& SmartPointer<T>::operator[](unsigned int index)
{
    return refer->ptr[index];
}

template <typename T>
void* SmartPointer<T>::get() const
{
    if(refer != 0)
        return refer->ptr;
    return refer;
}

template <typename T>
bool SmartPointer<T>::getTas() const
{
    return this->tas;
}

template <typename T>
SmartPointer<T>::~SmartPointer()
{
    flush();
}


template <typename T>
void SmartPointer<T>::flush()
{
    if(get())
    {
        mapZone::iterator it = zones.find((void*)refer->ptr);
        assert(it != zones.end());
        it->second.second.remove(this);
        if(tas)
        {
            (refer->ref_cnt_tas)--;
        }
        else
        {
            (refer->ref_cnt_pile)--;
        }

        if(refer->ref_cnt_pile < 1 && refer->ref_cnt_tas < 1)
        {
            if(refer->heap)
            {
                if(is_array)
                {
                    delete [] refer->ptr;
                }
                else
                {
                    delete refer->ptr;
                }
            }
            zones.erase(refer->ptr);
            delete refer;
            refer = NULL;
        }
        else if(refer->ref_cnt_pile < 1)
        {
            cycle_search(refer->ptr);
        }
    }
}

template <typename T>
void SmartPointer<T>::cycle_search(void * objAddr)
{
    std::list<void*> deja_vu = {}; //liste des SmartPointer déjà traités
    std::list<void*> temp = {}; //liste d'adresses de SmartPointer
    void * addr;
    std::list<void*>::iterator it2;

    mapZone::iterator itemp = zones.find((void*)refer->ptr);
    assert(itemp != zones.end());
    temp = itemp->second.second;


    while(! temp.empty())
    {
        addr = temp.front();
        temp.pop_front();

        mapZone::iterator it = zones.upper_bound(addr);
        it--;
        if(addr >= it->first && addr <= ((char*)(it->first)+(it->second.first)))
        {
            for(it2 = it->second.second.begin(); it2!= it->second.second.end(); ++it2)
            {

                std::list<void*>::iterator findDeja = std::find(deja_vu.begin(), deja_vu.end(), *it2);
                std::list<void*>::iterator findTemp = std::find(temp.begin(), temp.end(), *it2);

                if(findDeja==deja_vu.end() && findTemp==temp.end() && *it2 != objAddr && *it2!= this)
                {
                    if(((SmartPointer<void*>*)(*it2))->getTas() == false)//s'il existe un lien vers la pile le cycle est accesible
                        return;
                    temp.push_back(*it2);
                }
                else
                {
                    if(*it2!= objAddr && ((SmartPointer<void*>*)(*it2))->getRefHeap() > 0 )
                        return;
                }
            }
        }
        deja_vu.push_back(addr);
    }
    refer->heap = false;//un cycle non accessible depuis la pile a été détecté,
    // on le brise en se faisant passer pour un élément de la pile puis on force la suppression
    refer->ref_cnt_tas = 0;
    refer->ref_cnt_pile = 0;
    if(is_array)
    {
        delete [] refer->ptr;
    }
    else
    {
        delete refer->ptr;
    }
    zones.erase(refer->ptr);
}

template<typename T, typename U>
bool operator==(SmartPointer<T> const & a, SmartPointer<U> const & b)
{
    return a.get() == b.get();
}

template<typename T>
bool operator!=(SmartPointer<T> const & a, SmartPointer<T> const & b)
{
    return a.get() != b.get();
}

template<typename T, typename U>
bool operator<(SmartPointer<T> const & a, SmartPointer<U> const & b)
{
    return a.get() < b.get();
}

template<typename T>
SmartPointer<T>& SmartPointer<T>::operator++()
{
    ++(*this);
    return *this;
}

template<typename T>
SmartPointer<T> SmartPointer<T>::operator++(int i)
{
    SmartPointer<T> res = *this;
    ++(*this);
    return res;
}

template<typename T>
SmartPointer<T>& SmartPointer<T>::operator--()
{
    (*this)--;
    return *this;
}

template<typename T>
SmartPointer<T> SmartPointer<T>::operator--(int i)
{
    SmartPointer<T> res = *this;
    --(*this);
    return res;
}

template <typename U>
std::ostream & operator << (std::ostream & os, SmartPointer<U> const& smrt_ptr)
{
    os << smrt_ptr.get();
    return os;
}

template <typename U>
std::istream & operator << (std::istream & is, SmartPointer<U> & smrt_ptr)
{
    is >> (smrt_ptr.get());
    return is;
}

#endif
