#include "OptimisationUtils.h"
#include <complex>
#include <vector>
#include <string> 
#include <fstream>

using std::vector;
using std::complex;
using std::string;

vector<string> OptimisationUtils::tokenise(const string &line, char delim)
{
    vector<string> tokens;
    size_t pos = 0;
    size_t next = 0;
    do
    {
        next = line.find(delim, pos);
        tokens.push_back(line.substr(pos, (next - pos)));
        pos = next + 1;
    } while (next != string::npos);

    return tokens;
}

template <> double OptimisationUtils::getDataFromTokens(const vector<string> &tokens)
{
    return std::stod(tokens[0]);
}

template<> complex<double> OptimisationUtils::getDataFromTokens(const vector<string> &tokens)
{
    return complex<double>(std::stod(tokens[0]), std::stod(tokens[1]));
}