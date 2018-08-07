#pragma once


#include <iostream>
#include <algorithm>
#include <stdexcept>

#define MAGIC_CANARY_NUMBER 666
#define REALLOCATION_FACTOR 2
#define START_VECTOR_SZ 5


enum
{
    POP_ERROR = 1,
    REALLOCATION_PROBLEM,
    REALLOCATION_WARNING
};


template<typename Data_T>
class vector
{
private:

    int _canary1;

    size_t _capacity;
    size_t _size;
    Data_T *_data;

    int *_canaryPtr3;
    int *_canaryPtr4;

    int _canary3;
    int _canary4;
    int _canary2;

    bool _vectorCondition;

    int calcHash();

    bool printElem(int elemNum, std::ostream &out);

public:

    explicit vector(size_t size = START_VECTOR_SZ);

    vector(size_t _size, const Data_T &extender);

    vector(vector<Data_T> &&that) noexcept;

    vector(const vector<Data_T> &that);

    vector<Data_T> &operator=(vector &&that) noexcept;

    vector<Data_T> &operator=(const vector &that);

    void swap(vector<Data_T> &that);

    Data_T &operator[](unsigned int pos);

    int push_back(const Data_T &value);

    int pop_back(Data_T *var = NULL);

    size_t getSize();

    size_t getCapacity();

    void dump(std::ostream &os = std::cout);

    int alloc();

    bool vectorOK();

    ~vector();
};


template<typename Data_T>
vector<Data_T>::vector(vector &&that) noexcept :
        _canary1(that._canary1),
        _canary2(that._canary2),
        _canaryPtr3((int *) (_data - 1)),
        _canaryPtr4((int *) ((char *) (_data - 1) + (that._capacity + 1) * sizeof(Data_T))),
        _canary3(*_canaryPtr3),
        _canary4(*_canaryPtr4),

        _size(that._size),
        _capacity(that._capacity),
        _vectorCondition(true),

        _data(that._data)
{
    that._data = nullptr;
}


template<typename T>
vector<T>::vector(size_t size) try :
        _data(new T[size + 2]),

        _canary1(MAGIC_CANARY_NUMBER),
        _canary2(MAGIC_CANARY_NUMBER),
        _canaryPtr3((int *) _data),
        _canaryPtr4((int *) ((char *) _data + (size + 1) * sizeof(T))),
        _canary3(*_canaryPtr3),
        _canary4(*_canaryPtr4),
        _size(0),
        _capacity(size),
        _vectorCondition(true)
{
    _data++;
}
catch (std::exception &e)
{
    throw;
}


template<typename Data_T>
vector<Data_T>::vector(const vector<Data_T> &that) try :
        _data(new Data_T[that._capacity + 2]),
        _canary1(that._canary1),
        _canary2(that._canary2),
        _canaryPtr3((int *) (_data - 1)),
        _canaryPtr4((int *) ((char *) (_data - 1) + (that._capacity + 1) * sizeof(Data_T))),
        _canary3(*_canaryPtr3),
        _canary4(*_canaryPtr4),

        _size(that._size),
        _capacity(that._capacity),
        _vectorCondition(true)
{

    _data++;

    for (int i = 0; i < _capacity; i++)
        this->_data[i] = that._data[i];
}
catch (std::exception &e)
{
    throw;
}


template<typename Data_T>
vector<Data_T>::vector(size_t _size, const Data_T &extender) try :
        _data(new Data_T[_size + 2]),
        _canary1(MAGIC_CANARY_NUMBER),
        _canary2(MAGIC_CANARY_NUMBER),
        _canaryPtr3((int *) _data),
        _canaryPtr4((int *) ((char *) _data + (_size + 1) * sizeof(Data_T))),
        _canary3(*_canaryPtr3),
        _canary4(*_canaryPtr4),

        _size(0),
        _capacity(_size),
        _vectorCondition(true)
{
    _data++;

    for (int i = 0; i < _capacity; i++)
        this->_data[i] = extender;

    _size = _capacity;
}

catch (std::exception &e)
{
    throw;
}


template<typename Data_T>
void vector<Data_T>::swap(vector<Data_T> &that)
{
    std::swap(_size, that._size);
    std::swap(_capacity, that._capacity);
    std::swap(_canary1, that._canary1);
    std::swap(_canary2, that._canary2);
    std::swap(_canary3, that._canary3);
    std::swap(_canary4, that._canary4);
    std::swap(_canaryPtr4, that._canaryPtr4);
    std::swap(_canaryPtr3, that._canaryPtr3);
    std::swap(_data, that._data);
}


template<typename Data_T>
vector<Data_T> &vector<Data_T>::operator=(const vector<Data_T> &that)
{
    vector<Data_T> temp(that);
    temp.swap(*this);
}


template<typename Data_T>
vector<Data_T> &vector<Data_T>::operator=(vector<Data_T> &&that) noexcept
{
    _canary1 = that._canary1;
    _canary2 = that._canary2;
    _canary3 = that._canary3;
    _canary4 = that._canary4;
    _canaryPtr3 = that._canaryPtr3;
    _canaryPtr4 = that._canaryPtr4;
    _size = that._size;
    _capacity = that._capacity;
    _vectorCondition = true;

    Data_T *l_Data = that._data;
    that._data = _data;
    _data = l_Data;
}

template<typename Data_T>
Data_T &vector<Data_T>::operator[](unsigned int pos)
{
    if (pos >= _capacity)
        throw std::out_of_range("vector::_M_range_check: __n (which is " +
                                std::to_string(pos) +
                                ") >= this->capacity() (which is " +
                                std::to_string(getSize()) + ")");
    if (pos > _size)
        _size = (size_t) pos;

    return _data[pos];
}


template<typename Data_T>
bool vector<Data_T>::printElem(int elemNum, std::ostream &out)
{
    return false;
}


#define printDef(elem_t)                                         \
template<>                                                       \
bool vector<elem_t>::printElem(int elemNum, std::ostream &out)   \
{                                                                \
    if (elemNum < _capacity) {                                   \
        out << _data[elemNum];                                   \
        return true;                                             \
    }                                                            \
    return false;                                                \
}                                                                \


printDef(bool)

printDef(short)

printDef(long long)

printDef(int)

printDef(size_t)

printDef(double)

printDef(char)

printDef(void*)

printDef(long double)

printDef(float)

printDef(unsigned int)

#undef printDef


template<typename T>
int vector<T>::alloc()
{
    try
    {
        auto *Alloc_Data = new T[_capacity * REALLOCATION_FACTOR + 2];
        for (int i = 0; i < _capacity; i++)
            Alloc_Data[i + 1] = _data[i];

        --_data;
        delete[] _data;

        _data = Alloc_Data;
        _capacity = _capacity * REALLOCATION_FACTOR;
        _canaryPtr3 = ((int *) _data);
        _canaryPtr4 = ((int *) ((char *) _data + (_capacity + 1) * sizeof(T)));
        _data++;
    }
    catch (std::bad_alloc &c)
    {
        return REALLOCATION_PROBLEM;
    }
    return true;

}


template<typename T>
int vector<T>::pop_back(T *var)
{
    if (_size > 0)
    {
        if (var)
            *var = _data[--_size];
        else
            _data[--_size] = {0};
    }
    else
    {
        _vectorCondition = false;
        return POP_ERROR;
    }
    return 0;
}


template<typename T>
int vector<T>::push_back(const T &value)
{
    if (_size == _capacity)
    {
        _vectorCondition = false;
        alloc();
        /* if (!alloc())
             return REALLOCATION_PROBLEM;*/

        _data[_size++] = value;
        return REALLOCATION_WARNING;
    }
        _data[_size++] = value;

    return 0;
}


#define CUR_BYTE (((unsigned char *) _data - sizeof(T))[i])

template<typename T>
int vector<T>::calcHash()
{
    int l_hash = 0;

    for (unsigned int i = 0; i < sizeof(T) * (_size + 1); i++)
    {
        if (i & 1u)
            l_hash += (int) CUR_BYTE;
        else l_hash += 2 * (int) CUR_BYTE;
    }
    l_hash += _size;
    return l_hash;
}

#undef CUR_BYTE


template<typename T>
bool vector<T>::vectorOK()
{
    if (_canary1 != MAGIC_CANARY_NUMBER ||
        _canary2 != MAGIC_CANARY_NUMBER ||
        _canary3 != *_canaryPtr3 ||
        _canary4 != *_canaryPtr4)
        _vectorCondition = false;

    return _vectorCondition;
}

#undef MAGIC_CANARY_NUMBER


template<typename T>
void vector<T>::dump(std::ostream &os)
{
    int counter = 0;
    while (printElem(counter++, os))
        os << std::endl;
}


template<typename T>
size_t vector<T>::getSize()
{
    return _size;
}


template<typename T>
size_t vector<T>::getCapacity()
{
    return _capacity;
}


template<typename T>
vector<T>::~vector()
{
    if (_data)
    {
        --_data;
        delete[] _data;
        _data = nullptr;
    }
}
