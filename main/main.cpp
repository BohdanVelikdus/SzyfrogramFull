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

#include "MemoryUtilities.h"
#include "BigInteger.h"
#include "Matrix.h"

void uncipherData(BigInteger C,
                  BigInteger M,
                  const std::vector<BigInteger>& X)
{
    Matrix StartMatrix({
                        {BigInteger("1")},
                        {BigInteger("1")},
                        {BigInteger("1")}});

    Matrix matrixToPower(
        { {C, BigInteger("1"), BigInteger("0")},
          {BigInteger("1"), BigInteger("0"), BigInteger("0")},
          {BigInteger("1"), BigInteger("0"), BigInteger("1")}});    
    for(int i = 0; i < X.size(); ++i)
    {
        Matrix rv = (Matrix::fastPower(matrixToPower, X[i]));
        Matrix result = rv.multiplication(StartMatrix);
        BigInteger resultA = (result[2][0]) % M;
        std::cout << resultA << "\n";
    }
    
}

void readFiles()
{
    std::filesystem::path testPath("../test");
    std::filesystem::directory_iterator it(testPath);
    std::filesystem::directory_iterator endIt;
    std::vector<std::string> files;
    for(; it != endIt; ++it)
    {  
        files.push_back((it->path()).generic_string()); 
    }
    BigInteger C, M;
    short T;
    std::vector<BigInteger> X;   
    X.reserve(1000);     
    for(const auto entity : files)
    {
        X.clear();
        std::ifstream file(entity);
        if(!file)
        {
            std::cout << "Wrong file type\n";
            return;
        }
        std::string CM;
        std::getline(file, CM);
        std::stringstream sstream;
        sstream << CM;
        std::string Cstring;
        sstream >> Cstring;
        C = BigInteger(Cstring.c_str());
        std::string MString;
        sstream >> MString;
        M = BigInteger(MString.c_str());
        CM.clear();
        std::getline(file, CM);
        T = static_cast<short>(std::stoi(CM));
        for(int i = 0; i < T; ++i)
        {
            CM.clear();
            std::getline(file, CM);
            X.push_back(BigInteger(CM.c_str())-BigInteger("1"));
        }
        // Here further we will count the result of every test file
        std::cout << "The test File: " << entity << " Test:\n";
        uncipherData(C, M, X);
    }

}





int main()
{
    HEAPCHECK;
    MEMORYCHECKPOINT;
    readFiles();  
    return 0;
}