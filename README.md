# SmartPointer

Smart Pointer implementation without any pool or any previous memory allocated.

## How it works ?

In this case, we choose to work with contexts. We actually have just one context struct (this is why it's not thread safe today).

```cpp
extern bool sp_contexte_new;
extern bool sp_contexte_array;
extern void* sp_contexte_addr;
extern std::size_t sp_contexte_size;
```

+ The **new** indicate the object is in the heap (*i.e.* we called the instruction).
+ The **array** indicate if it's an array or not.
+ And **addr**, **size** are the address and size of the new object.

Now, we now for each SmartPointer if the pointed object is in the stack or in the heap.
Each SmartPointer also contains the number of references from the stack **and** from the heap to the pointed object, 
the address of the pointed object, if the SmartPointer itself is in an array and is in the heap (because of the contexts uses).

```cpp
template <typename T>
struct compteur
{
    T * ptr;
    int ref_cnt_tas;
    int ref_cnt_pile;
    bool heap = true; //l'objet pointé est-il dans le tas?
};
```

And later in class SmartPointer :

```cpp
private:

    //Attributs
    bool is_array;//le smartPointeur contient-il un tableau ?
    bool tas;//le smartPointeur est-il dans le tas ?
    compteur<T> * refer;//la structure reste à la fin de l'objet
```

And finally, we have :

```cpp
typedef std::pair<void*, std::pair<size_t, std::list<void*>>> paireZone;
typedef std::map<void*, std::pair<size_t, std::list<void*>>> mapZone;
```

Where **mapZone** is a map with the address of the pointed object as key and a pair of the size of the object 
and a list of pointer which are pointing this object.
