#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <cctype>
#include <cstring>
#include <memory>
#include <sstream>
#include <chrono>
#include <string_view>
#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <stack>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <charconv>
#include <cstdint>

#include "BigInteger.h"

class Matrix
{
public:
    Matrix(std::vector<std::vector<BigInteger>> vec) : m_Matrix(vec)
    {
        dimX = vec[0].size();
        dimY = vec.size();
    }

    Matrix(const Matrix& obj);

    static Matrix fastPower(const Matrix obj, BigInteger power);

    Matrix multiplication(const Matrix& obj) const;

    void printMatrix() const;

    std::vector<BigInteger>& operator[](int index);
private:
    std::vector<std::vector<BigInteger>> m_Matrix; 
    int dimX;
    int dimY;
};