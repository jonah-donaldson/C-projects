#pragma once

#include <vector>
#include <ostream>

using std::vector;

template<typename T, typename Real>
vector<T> operator*(const Real &a, const vector<T> &v)
{
    vector<T> v2(v.size());
    for(size_t i = 0; i < v2.size(); i++)
    {
        v2[i] = a * v[i];
    }
    return v2;
}

template<typename T>
vector<T> operator+(const vector<T> &v1, const vector<T> &v2)
{
    vector<T> v3(v1.size());
    for(size_t i = 0; i < v1.size(); i++)
    {
        v3[i] = v1[i] + v2[i];
    }
    return v3;
}

template<typename T>
vector<T> operator-(const vector<T> &v1, const vector<T> &v2)
{
    vector<T> v3(v1.size());
    for(size_t i = 0; i < v1.size(); i++)
    {
        v3[i] = v1[i] - v2[i];
    }
    return v3;
}

template<typename T>
vector<T> operator*(const vector<T> &v1, const vector<T> &v2)
{
    vector<T> v3(v1.size());
    for(size_t i = 0; i < v1.size(); i++)
    {
        v3[i] = v1[i] * v2[i];
    }
    return v3;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const vector<T> &v)
{
    size_t num_elements = v.size() < 10 ? v.size() : 10;
    os << "(";
    for(size_t i = 0; i < num_elements; i++)
    {
        os << v[i] << ", ";
    }
    os << ")";
    return os;
}