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

#include <charconv>
#include <cstdint>

enum Sign
{
    Positive, Negative
};

class BigInteger
{
public:
    BigInteger() = default;

    BigInteger( const char *pNumber);
    
    BigInteger( const BigInteger& obj);
    
    BigInteger& operator=( const BigInteger& obj);
    
    BigInteger(BigInteger&& obj) noexcept : m_pNumber(std::move(obj.m_pNumber)), mSign(obj.mSign), mSize(obj.mSize){}
    
    BigInteger& operator=(BigInteger&& obj) noexcept;
    
    BigInteger& operator+=(const BigInteger& obj);
     
    BigInteger operator+(const BigInteger& obj);
    
    BigInteger operator-() const;

    BigInteger& operator-=(const BigInteger& obj);
    
    BigInteger operator-(const BigInteger& obj) const;
    
    friend std::ostream& operator<<(std::ostream& os, const BigInteger& obj);

    std::strong_ordering operator<=>(const BigInteger& obj) const;

    bool operator==(const BigInteger& obj) const;
    
    bool operator!=(const BigInteger& obj) const;
    
    BigInteger& operator++();
    
    BigInteger operator++(int);
    
    BigInteger& operator--();
    
    BigInteger operator--(int);

    BigInteger operator*(const BigInteger& obj) const;

    BigInteger operator/(const BigInteger& obj) const;
    
    BigInteger& operator/=(const BigInteger& obj);

    BigInteger& operator%=(const BigInteger& obj);
    
    BigInteger operator%(const BigInteger& obj) const;
    
private:
    std::unique_ptr<char[]> m_pNumber;
    int mSize;
    Sign mSign; 
    
    char *substract(const std::unique_ptr<char[]> &greater, const std::unique_ptr<char[]> &less, int greaterSize, int lessSize);
    
    char *add(const std::unique_ptr<char[]> &a1, const std::unique_ptr<char[]> &a2, int a1Size, int a2Size);
    
    std::strong_ordering absCompare(const BigInteger& obj) const;
    
    std::strong_ordering partialCompare(const BigInteger& obj) const;
    
    std::strong_ordering partialComapare(const char* firstPart, const char* secondPart) const;
    
    std::strong_ordering partialCompareMultiplication(const char* mem1, int sizeMem1, const char* mem2, int sizeMem2) const;
    
    std::tuple<char*, int> multiplicationChars(const char* first, int sizeFirst, const char* second, int sizeSecond) const;
    
    std::tuple<char*, int> minusChars(const char* first, int sizeFirst, const char* second, int sizeSecond) const;
    
    std::strong_ordering memCompare(const char* first, int sizeFirst, const char* second, int sizeSecond) const;
    
    std::pair<BigInteger, BigInteger> division(const BigInteger& obj) const;
    
};