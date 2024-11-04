#include "Matrix.h"

Matrix::Matrix(const Matrix &obj)
{
    this->m_Matrix = obj.m_Matrix;
    this->dimX = obj.dimX;
    this->dimY = obj.dimY;
}

Matrix Matrix::fastPower(const Matrix obj, BigInteger power)
{
    int size = obj.m_Matrix.size();
    std::vector<std::vector<BigInteger>> newMatrix(obj.dimX);
    for(int j = 0; j < obj.dimX; ++j)
    {
        newMatrix[j] = std::vector<BigInteger>(obj.dimY);
        for(int f = 0; f < obj.dimY; ++f)
        {
            newMatrix[j][f] = BigInteger("0");
        }
    }
    for(int i = 0; i < obj.dimX; ++i)
    {
        newMatrix[i][i] = BigInteger("1");
    }
    if(power == BigInteger("0"))
    {
        return Matrix(newMatrix);
    }
    Matrix result(newMatrix);
    std::stack<std::pair<Matrix, BigInteger>> stack;
    stack.push({obj, power});
    while (!stack.empty()) {
        auto [currentMatrix, currentExp] = stack.top();
        stack.pop();
        if (currentExp == BigInteger("1")) {
            result = result.multiplication(currentMatrix);
        } else if (currentExp % BigInteger("2") == BigInteger("0")) {
            Matrix squaredMatrix = currentMatrix.multiplication(currentMatrix);
            stack.push({squaredMatrix, currentExp / BigInteger("2")});
        } else {
            result = result.multiplication(currentMatrix);
            stack.push({currentMatrix, currentExp - BigInteger("1")});
        }
    } 
    return result;
}

Matrix Matrix::multiplication(const Matrix &obj) const
{
    std::vector<std::vector<BigInteger>> newMatrix(this->dimX);
    for(auto& vec : newMatrix)
    {
        for(int i = 0; i < obj.dimX; ++i)
        {
            vec.push_back(BigInteger("0"));
        }
    }
    for (int i = 0; i < this->dimY; ++i) {
        for (int j = 0; j < obj.dimX; ++j) {
            for (int k = 0; k < this->dimY; ++k) {
                newMatrix[i][j] += this->m_Matrix[i][k] * obj.m_Matrix[k][j];
            }
        }
    }
    return Matrix(newMatrix);
}

void Matrix::printMatrix() const
{
    for(int i = 0; i < m_Matrix.size(); ++i)
    {
        for(int j = 0; j < m_Matrix[i].size(); ++j)
        {
            std::cout << m_Matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

std::vector<BigInteger> &Matrix::operator[](int index)
{
    return m_Matrix[index];
}
