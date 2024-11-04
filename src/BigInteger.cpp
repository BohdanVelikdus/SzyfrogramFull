#include "BigInteger.h"

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

#include "MemoryUtilities.h"


BigInteger::BigInteger(const char *pNumber)
{
    if('0' == *pNumber)
    {
        mSign = Sign::Positive;
        mSize = 1;
        m_pNumber = std::make_unique<char[]>(2);
        m_pNumber[0] = '0';
    }
    else if('-' == *pNumber)
    {
        mSign = Sign::Negative;
        mSize = std::strlen(pNumber) - 1;
        // -1 for a minus sign, +1 for nullterminator
        m_pNumber = std::make_unique<char[]>(mSize + 1);
        for(std::size_t i = 1; i <= mSize; ++i)
        {
            if(std::isdigit(pNumber[i]))
            {
                m_pNumber[i - 1] = pNumber[i];
            }
            else
                throw std::exception("Not all chars in string are digits!");
        }     
    }
    else
    {
        mSign = Sign::Positive;
        mSize = std::strlen(pNumber);
        // all numbers and null terminator
        m_pNumber = std::make_unique<char[]>(mSize + 1);
        for(std::size_t i = 0; i < mSize; ++i)
        {
            if(std::isdigit(pNumber[i]))
            {
                m_pNumber[i] = pNumber[i];
            }
            else
                throw std::exception("Not all chars in string are digits!");
        }
    }
    m_pNumber[mSize] = '\0';
}

BigInteger::BigInteger(const BigInteger &obj)
{
    this->mSize = obj.mSize;
    this->mSign = obj.mSign;
    m_pNumber = std::make_unique<char[]>(mSize + 1);
    memcpy_s(this->m_pNumber.get(), mSize + 1, obj.m_pNumber.get(), obj.mSize + 1);
}

BigInteger &BigInteger::operator=(const BigInteger &obj)
{
    if(this == &obj)
        return *this;
    
    m_pNumber.reset();
    this->mSign = obj.mSign;
    this->mSize = obj.mSize;
    m_pNumber = std::make_unique<char[]>(this->mSize + 1);
    memcpy_s(this->m_pNumber.get(), mSize + 1, obj.m_pNumber.get(), obj.mSize + 1);
    return *this;
}

BigInteger &BigInteger::operator=(BigInteger &&obj) noexcept
{
    if(this == &obj)
        return *this;
    this->mSize = obj.mSize;
    this->mSign = obj.mSign;
    this->m_pNumber.reset();
    this->m_pNumber = std::move(obj.m_pNumber);
    return *this;
}

BigInteger &BigInteger::operator+=(const BigInteger &obj)
{
    if(this->mSign == obj.mSign)
    {
        std::unique_ptr<char[]> result(add(this->m_pNumber, obj.m_pNumber, this->mSize, obj.mSize));
        this->mSize = std::strlen(result.get());
        m_pNumber.reset();
        m_pNumber = std::move(result);
    }
    else
    {
        auto compare = absCompare(obj);
        if(compare == std::strong_ordering::less)
        {
            if(this->mSign == Sign::Positive && obj.mSign == Sign::Negative)
            {
                std::unique_ptr<char[]> update(substract(obj.m_pNumber, this->m_pNumber, obj.mSize, this->mSize));
                this->mSize = std::strlen(update.get());
                this->mSign = Sign::Negative;
                m_pNumber.reset();
                m_pNumber = std::move(update);
            }else
            {
                std::unique_ptr<char[]> update(substract(obj.m_pNumber, this->m_pNumber, obj.mSize, this->mSize));
                this->mSize = std::strlen(update.get());
                this->mSign = Sign::Positive;
                m_pNumber.reset();
                m_pNumber = std::move(update);
            }
        }
        else if(compare == std::strong_ordering::greater)
        {
            if(this->mSign == Sign::Positive && obj.mSign == Sign::Negative)
            {
                std::unique_ptr<char[]> update(substract(this->m_pNumber, obj.m_pNumber, this->mSize, obj.mSize));
                this->mSize = std::strlen(update.get());
                this->mSign = Sign::Positive;
                m_pNumber.reset();
                m_pNumber = std::move(update);
            }else
            {
                std::unique_ptr<char[]> update(substract(this->m_pNumber, obj.m_pNumber, this->mSize, obj.mSize));
                this->mSize = std::strlen(update.get());
                this->mSign = Sign::Negative;
                m_pNumber.reset();
                m_pNumber = std::move(update);
            }
        }
        else
        {
            std::unique_ptr<char[]> update(new char[2]);
            update[0] = '0';
            update[1] = '\0';
            m_pNumber.reset();
            m_pNumber = std::move(update);
            this->mSign = Sign::Positive;
            this->mSize = 1;
        }
    }
    return *this;
}

BigInteger BigInteger::operator+(const BigInteger &obj)
{
    BigInteger result = *this;
    result += obj;
    return result;
}

BigInteger BigInteger::operator-() const
{
    BigInteger result = *this;
    result.mSign = (result.mSign == Sign::Negative) ? Sign::Positive : Sign::Negative; 
    return result;
}

BigInteger &BigInteger::operator-=(const BigInteger &obj)
{
    *this += -obj;
    return *this;
}

BigInteger BigInteger::operator-(const BigInteger &obj) const
{
    BigInteger result = *this;
    result -= obj;
    return result;
}

bool BigInteger::operator==(const BigInteger &obj) const
{
    if(this->mSign == obj.mSign && this->mSize == obj.mSize)
    {
        if(memcmp(this->m_pNumber.get(), obj.m_pNumber.get(), obj.mSize) == 0)
            return true;
    }
    return false;
}

bool BigInteger::operator!=(const BigInteger &obj) const
{
    return !(*this == obj);
}

BigInteger &BigInteger::operator++()
{
    *this += BigInteger("1");
    return *this;
}

BigInteger BigInteger::operator++(int)
{
    BigInteger res = *this;
    ++(*this);
    return res;
}

BigInteger &BigInteger::operator--()
{
    *this += BigInteger("-1");
    return *this;
}

BigInteger BigInteger::operator--(int)
{
    BigInteger res = *this;
    --(*this);
    return res;
}

BigInteger BigInteger::operator*(const BigInteger &obj) const
{
    // for null terminator
    int size = this->mSize + obj.mSize + 1; 
    std::unique_ptr<char[]> resArray(new char[size]);
    memset(resArray.get(), '0', this->mSize + obj.mSize);
    resArray[this->mSize + obj.mSize] = '\0';
    int carry = 0;
    int startPos = this->mSize + obj.mSize - 1;
    for(int i = obj.mSize-1; i >= 0 ; --i)
    {
        int numI = obj.m_pNumber[i] - '0';
        for(int j = this->mSize - 1; j>= 0; --j)
        {
            int numJ = this->m_pNumber[j] - '0';
            int alreadyNumber = resArray[startPos - (this->mSize - 1 - j)] - '0';
            int res = numI * numJ + carry + alreadyNumber;
            carry = 0;
            if(res >= 10)
            {
                carry = res / 10;
                res %= 10;
            }
            resArray[startPos - (this->mSize - 1 - j)] = res + '0';
        }
        if(carry > 0)
        {
            int iter = 0;
            int alreadyNumber;// = resArray[startPos - this->mSize] - '0';
            int res;// = alreadyNumber + carry;
            do 
            {
                alreadyNumber = resArray[startPos - this->mSize - iter] - '0';
                res = alreadyNumber + carry;
                carry = 0;
                if(res >= 10)
                {
                    carry = res / 10; 
                    res %= 10;                     
                }
                resArray[startPos - this->mSize - iter] = res + '0';
                ++iter;
            }while(carry != 0);
        }
        --startPos;
    }
    char* charsResult = resArray.release();
    std::string strResult = charsResult;
    delete[] charsResult;
    std::size_t pos = strResult.find_first_not_of('0');
    if(pos != std::string::npos)
    {
        strResult = strResult.substr(pos);
    }else
    {
        strResult = "0";
    }
    if(this->mSign != obj.mSign)
    {
        strResult.insert(strResult.begin(), '-');
    }
    return BigInteger(strResult.c_str());
}

BigInteger BigInteger::operator/(const BigInteger &obj) const
{
    auto pair = division(obj);
    return std::get<0>(pair);
}

BigInteger &BigInteger::operator/=(const BigInteger &obj)
{
    auto pair = division(obj);
    *this = std::get<0>(pair);
    return *this;
}

BigInteger &BigInteger::operator%=(const BigInteger &obj)
{
    *this = std::get<1>(division(obj));
    return *this;
}

BigInteger BigInteger::operator%(const BigInteger &obj) const
{
    BigInteger copy = *this;
    auto tup = copy.division(obj);
    BigInteger remainder = std::get<1>(tup);
    return remainder;
}

char *BigInteger::substract(const std::unique_ptr<char[]> &greater, const std::unique_ptr<char[]> &less, int greaterSize, int lessSize)
{
    int borrow = 0;
    std::stringstream ss;
    
    for(int i = std::strlen(greater.get()) - 1, j = std::strlen(less.get()) - 1; i >= 0 && j >= 0; --i,--j)
    {
        int numGreater = greater[i] - '0';
        numGreater -= borrow;
        borrow = 0;
        int numLess = less[j] - '0';
        if(numGreater < numLess)
        {
            numGreater += 10;
            borrow = 1;
        }    
        int res = numGreater - numLess;
        ss << res;
    }
    for(int j = greaterSize - lessSize - 1; j >= 0; --j)
    {
        int num = greater[j] - '0';
        num -= borrow;
        borrow = 0;
        if(num < 0)
        {
            num += 10;
            borrow = 1;
        }
        if( (j == 0 && num != 0) || j != 0)
        {
            ss << num;
        }
    }
    std::string toR = ss.str();
    std::reverse(toR.begin(), toR.end());
    std::size_t posNotLeadingZero = toR.find_first_not_of('0');
    if (posNotLeadingZero != std::string::npos) {
        toR = toR.substr(posNotLeadingZero);
    } else {
        toR = "0";
    }
    char *newArray = new char[toR.size() + 1];
    memcpy_s(newArray, toR.size() + 1, toR.c_str(), toR.size() + 1);
    return newArray;
}

char *BigInteger::add(const std::unique_ptr<char[]> &a1, const std::unique_ptr<char[]> &a2, int a1Size, int a2Size)
{
    int carry = 0;
    std::stringstream ss;
    for(int i = std::strlen(a1.get()) - 1, j = std::strlen(a2.get()) - 1; i >=0 && j >=0; --i, --j)
    {
        int res = (a1[i] - '0') + (a2[j] - '0') + carry;
        carry = 0;
        if(res >= 10)
        {
            res -=10;
            carry++;
        }  
        ss << res;
    }
    if(a1Size > a2Size)
    {
        for(int i = a1Size - a2Size - 1; i >= 0; --i)
        {
            int res = ((a1[i]) - '0') + carry;
            carry = 0;
            if(res >= 10)
            {
                res -=10;
                carry++;
            }  
            ss << res;
        }
    }else
    {
        for(int i = a2Size - a1Size - 1; i >= 0; --i)
        {
            int res = ((a2[i]) - '0') + carry;
            carry = 0;
            if(res >= 10)
            {
                res -=10;
                carry++;
            }  
            ss << res;
        }
    }
    if (carry != 0)
        ss << carry;
    std::string res = ss.str();
    std::reverse(res.begin(), res.end());
    std::size_t posNotLeadingZero = res.find_first_not_of('0');
    if (posNotLeadingZero != std::string::npos) {
        res = res.substr(posNotLeadingZero);
    } else {
        res = "0";
    }
    char* array = new char[res.length() + 1];
    memcpy_s(array, res.length() + 1, res.c_str(), res.length() + 1);
    return array;
}

std::strong_ordering BigInteger::absCompare(const BigInteger &obj) const
{
    if(this->mSize > obj.mSize)
        return std::strong_ordering::greater;
    else if(this->mSize < obj.mSize)
        return std::strong_ordering::less;
    else
    {
        for(size_t i = 0; i < this->mSize; ++i)
        {
            if(this->m_pNumber[i] < obj.m_pNumber[i])
                return std::strong_ordering::less;
            else if(this->m_pNumber[i] > obj.m_pNumber[i])
                return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }
}

std::strong_ordering BigInteger::partialCompare(const BigInteger &obj) const
{
    for(int i = 0; i < obj.mSize; ++i)
    {
        if(this->m_pNumber[i] < obj.m_pNumber[i])
            return std::strong_ordering::less;
        else if(this->m_pNumber[i] > obj.m_pNumber[i])
            return std::strong_ordering::greater; 
    }
    return std::strong_ordering::equal;
}

std::strong_ordering BigInteger::partialComapare(const char *firstPart, const char *secondPart) const
{
    return std::strong_ordering::greater;
}

std::strong_ordering BigInteger::partialCompareMultiplication(const char *mem1, int sizeMem1, const char *mem2, int sizeMem2) const
{
    if(sizeMem2 > sizeMem1)
        return std::strong_ordering::less;
    // need greater 
    else if(sizeMem2 < sizeMem1)
        return std::strong_ordering::greater;
    else
    {
        for(int i = 0; i < sizeMem1; ++i)
        {
            if(*(mem1+i) > *(mem2+i))
            {
                return std::strong_ordering::greater;
            }
            else if(*(mem1+i) < *(mem2+i))
            {
                return std::strong_ordering::less;
            }
        }
    }
    return std::strong_ordering::equal;
}

std::tuple<char *, int> BigInteger::multiplicationChars(const char *first, int sizeFirst, const char *second, int sizeSecond) const
{
    // for null terminator
    int size = sizeFirst + sizeSecond + 1; 
    std::unique_ptr<char[]> resArray(new char[size]);
    memset(resArray.get(), '0', sizeFirst + sizeSecond);
    resArray[sizeFirst + sizeSecond] = '\0';
    int carry = 0;
    int startPos = sizeFirst + sizeSecond - 1;
    for(int i = sizeSecond - 1; i >= 0 ; --i)
    {
        int numI = second[i] - '0';
        for(int j = sizeFirst - 1; j>= 0; --j)
        {
            int numJ = first[j] - '0';
            int alreadyNumber = resArray[startPos - (sizeFirst - 1 - j)] - '0';
            int res = numI * numJ + carry + alreadyNumber;
            carry = 0;
            if(res >= 10)
            {
                carry = res / 10;
                res %= 10;
            }
            resArray[startPos - (sizeFirst - 1 - j)] = res + '0';
        }
        if(carry > 0)
        {
            int iter = 0;
            int alreadyNumber;// = resArray[startPos - this->mSize] - '0';
            int res;// = alreadyNumber + carry;
            do 
            {
                alreadyNumber = resArray[startPos - sizeFirst - iter] - '0';
                res = alreadyNumber + carry;
                carry = 0;
                if(res >= 10)
                {
                    carry = res / 10; 
                    res %= 10;                     
                }
                resArray[startPos - sizeFirst - iter] = res + '0';
                ++iter;
            }while(carry != 0);
        }
        --startPos;
    }
    char* charsResult = resArray.release();
    std::string strResult = charsResult;
    delete[] charsResult;
    std::size_t pos = strResult.find_first_not_of('0');
    if(pos != std::string::npos)
    {
        strResult = strResult.substr(pos);
    }else
    {
        strResult = "0";
    }
    char* result = new char[strResult.size()+1];
    memcpy_s(result, strResult.size() + 1, strResult.c_str(), strResult.size() + 1);
    std::tuple<char*, int> tup = std::make_tuple(result, strResult.size());
    return tup;
}

std::tuple<char *, int> BigInteger::minusChars(const char *first, int sizeFirst, const char *second, int sizeSecond) const
{
    int carry = 0;
    char* res;
    // substraction algo here 
    res = new char[sizeFirst + 1];
    memset(res, 0, sizeFirst);
    res[sizeFirst] = '\0';
    int diff = sizeFirst - sizeSecond;
    for(int j = sizeSecond - 1; j >=0; --j)
    {
        int numFirst = first[j + diff] - '0';
        int numSecond = second[j] - '0';
        int result = numFirst - numSecond - carry;
        carry = 0;
        if(result < 0)
        {
            result += 10;
            carry = 1;
        }
        res[j + diff] = result + '0';
    }
    if(carry != 0)
    {
        int num = first[0] - '0';
        num -= carry;
        res[0] = num + '0';
    }
    
    std::string resStr = res;
    delete[] res;
    auto sizeT = resStr.find_first_not_of('0');
    if(sizeT == std::string::npos)
        resStr = "0";
    else 
        resStr = resStr.substr(sizeT);
    char* resMemory = new char[resStr.size()+1];
    memcpy_s(resMemory, resStr.size()+1, resStr.c_str(), resStr.size()+1);
    return std::make_tuple(resMemory, std::strlen(resMemory));
}

std::strong_ordering BigInteger::memCompare(const char *first, int sizeFirst, const char *second, int sizeSecond) const
{
    if(sizeFirst < sizeSecond)
        return std::strong_ordering::less;
    for(int i = 0; i < sizeSecond; ++i)
    {
        if(*(first+i) > *(second+i))
            return std::strong_ordering::greater;
        else if(*(first+i) < *(second+i))
            return std::strong_ordering::less;
    }
    return std::strong_ordering::equal;
}

std::pair<BigInteger, BigInteger> BigInteger::division(const BigInteger &obj) const
{
    auto absC = absCompare(obj);
    if(std::strong_ordering::equal == absC)
    {
        if(this->mSign == obj.mSign)
        {
            return std::make_pair(BigInteger("1"), BigInteger("0"));
        }
        else 
        {
           return std::make_pair(BigInteger("-1"), BigInteger("0"));
        }
    }
    if(std::strong_ordering::less == absC)
    {
        if(obj.mSign == Sign::Positive)
        {
            if(this->mSign == Sign::Positive)
                return std::make_pair(BigInteger("0"), *this);
            else // this->mSign == Sign::Negative 
                return std::make_pair(BigInteger("0"), (obj - *this));
        }
        else if(obj.mSign == Sign::Negative)
        {
            if(this->mSign == Sign::Positive)
            {
                BigInteger copyOfThis = *this;
                BigInteger res = copyOfThis + obj;
                return std::make_pair(BigInteger("0"), res);
            }
            else // this->mSign == Sign::Negative 
            {
                 return std::make_pair(BigInteger("0"), *this);
            }   
        }
    }
    if(std::strong_ordering::greater == absC)
    {
        auto partCompare = partialCompare(obj);
        // for greater 
        int sizeDivided = this->mSize;
        char* divided = new char[sizeDivided];
        memcpy_s(divided, sizeDivided, this->m_pNumber.get(), sizeDivided);
        int sizeDivisor = obj.mSize;
        char* divisor = new char[sizeDivisor];
        memcpy_s(divisor, sizeDivisor, obj.m_pNumber.get(), sizeDivisor);
        //means we get only the 4 and 4 fro all, do not afford from additional 
        std::stringstream ss;
        do
        {
            if(partCompare == std::strong_ordering::less && sizeDivided <= sizeDivisor)
            {
                // now returning the result
                
                std::string remainderOFDivision;
                char* rem = new char[sizeDivided + 1];
                memcpy_s(rem, sizeDivided, divided, sizeDivided);
                rem[sizeDivided] = '\0';
                delete[] divided;
                delete[] divisor;
                std::string resultOfDivision = ss.str();
                remainderOFDivision = rem;
                delete[] rem;
                if(this->mSign == Sign::Positive && obj.mSign == Sign::Positive)
                {
                    return std::make_pair(BigInteger(resultOfDivision.c_str()), BigInteger(remainderOFDivision.c_str()));
                }
                else if(this->mSign == Sign::Positive && obj.mSign == Sign::Negative)
                {
                    BigInteger copyOfOther = obj;
                    return std::make_pair(BigInteger(resultOfDivision.c_str()), copyOfOther + BigInteger(remainderOFDivision.c_str()));
                }
                else if(this->mSign == Sign::Negative && obj.mSign == Sign::Positive)
                {
                    resultOfDivision.insert(resultOfDivision.begin(), '-');
                    return std::make_pair(BigInteger(resultOfDivision.c_str()), BigInteger(remainderOFDivision.c_str()));
                }
                else if(this->mSign == Sign::Negative && obj.mSign == Sign::Negative)
                {
                    remainderOFDivision.insert(remainderOFDivision.begin(), '-');
                    return std::make_pair(BigInteger(resultOfDivision.c_str()), BigInteger(remainderOFDivision.c_str()));
                }
            }   
            int sizeForFirst = (partCompare == std::strong_ordering::greater || partCompare == std::strong_ordering::equal) ? sizeDivisor : sizeDivisor + 1;
            int sizeForSecond = sizeDivisor;
            bool endFlag = false;
            for(int i = 9; i >= 0; --i)
            {
                char* num = new char[2];
                *num = i + '0';
                *(num+1) = '\0';
                std::tuple<char*, int> res = multiplicationChars(divisor, sizeForSecond, num, 1);
                auto loopCompare = partialCompareMultiplication(divided, sizeForFirst, std::get<0>(res), std::get<1>(res));
                if(loopCompare != std::strong_ordering::less){
                    auto tup = minusChars(divided, sizeForFirst, std::get<0>(res), std::get<1>(res));
                    if(std::get<1>(tup) == 1 && std::stoi(std::get<0>(tup)) == 0)
                    {
                        char* newNumber = new char[sizeDivided - sizeForSecond];
                        memcpy_s(newNumber, sizeDivided - sizeForSecond, divided+sizeForFirst, sizeDivided - sizeForSecond);
                        delete[] divided;
                        divided = newNumber;
                        sizeDivided = sizeDivided - sizeForSecond;
                        delete[] std::get<0>(tup);
                        ss << i;
                        for(int j = 0; j < sizeDivisor; ++j)
                            ss << 0;
                        int f = 0;
                    }
                    else
                    {
                        // error here
                        char* newNumber = new char[std::get<1>(tup) +  sizeDivided - sizeForFirst];
                        memcpy_s(newNumber, std::get<1>(tup), std::get<0>(tup), std::get<1>(tup));
                        memcpy_s(newNumber+std::get<1>(tup), sizeDivided - sizeForFirst, divided+sizeForFirst, sizeDivided - sizeForFirst);
                        delete[] divided;
                        divided = newNumber;
                        sizeDivided = std::get<1>(tup) +  sizeDivided - sizeForFirst;
                        delete[] std::get<0>(tup);
                        ss << i;
                        // checking for 0
                        if(sizeDivisor - std::get<1>(tup) > 1)
                        {
                            int counter = (sizeDivided > sizeDivisor) ? sizeDivisor - std::get<1>(tup)-1 : sizeDivided - 1;
                            for(int j = 0; j < counter; ++j)
                                ss << 0;
                        }
                        int f = 0;
                    }
                    endFlag = true;
                }
                if(endFlag)
                {
                    delete[] std::get<0>(res);
                    delete[] num;
                    break;
                }
                else
                {
                    delete[] std::get<0>(res);
                    delete[] num;
                }
            }
            partCompare = memCompare(divided, sizeDivided, divisor, sizeDivisor);
        }while(true);
    }
}

std::strong_ordering BigInteger::operator<=>(const BigInteger &obj) const
{
    if(this->mSign == obj.mSign)
    {
        std::strong_ordering compareResult = absCompare(obj);
        if(compareResult == std::strong_ordering::equal)
            return std::strong_ordering::equal;
        if(this->mSign == Sign::Negative)
        {
            if(compareResult == std::strong_ordering::greater)
                return std::strong_ordering::less;
            else if (compareResult == std::strong_ordering::less)
                return std::strong_ordering::greater;
        }
        else
        {
            return compareResult;
        }
    }
    if(this->mSign == Sign::Negative && obj.mSign == Sign::Positive)
        return std::strong_ordering::less;
    else 
        return std::strong_ordering::greater;
}

std::ostream& operator<<(std::ostream& os, const BigInteger& obj)
{
    std::string sign = (obj.mSign == Sign::Negative)?"-":"";
    os << sign << obj.m_pNumber;
    return os;
}
