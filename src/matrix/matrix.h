#ifndef MLP_SRC_MATRIX_H_
#define MLP_SRC_MATRIX_H_

#include <cstring>
#include <stdexcept>

#include "primitive.h"

template <class T>
class Matrix {
 public:
  Matrix() = delete;
  Matrix(const unsigned rows, const unsigned cols);
  Matrix(const Matrix<T>& other);
  Matrix(Matrix<T>&& other);

  ~Matrix();

  Matrix<T>& operator=(const Matrix<T>& other);
  Matrix<T>& operator=(Matrix<T>&& other);

  friend Matrix<Neuron>* ForwardPropagation(const Matrix<Weight>* W,
                                            const Matrix<Neuron>* N,
                                            const Matrix<Weight>* B);
  friend Matrix<Neuron>* BackPropagation(const Matrix<Weight>* W,
                                         const Matrix<Neuron>* M,
                                         const Matrix<Neuron>* N);

  Matrix<T>& operator+=(Matrix<T> other);

  T& operator()(const unsigned row, const unsigned col);
  const T& operator()(const unsigned row, const unsigned col) const;

  const unsigned GetRows() const;
  const unsigned GetCols() const;

 private:
  T** m_matrix = nullptr;
  unsigned m_rows = 0U;
  unsigned m_cols = 0U;

  void Init(const unsigned rows, const unsigned cols);
  void Destroy();
};

template <class T>
Matrix<T>::Matrix(const unsigned rows, const unsigned cols)
    : m_rows{rows}, m_cols{cols} {
  Init(m_rows, m_cols);
}

template <class T>
Matrix<T>::Matrix(const Matrix<T>& other) {
  *this = other;
}

template <class T>
Matrix<T>::Matrix(Matrix<T>&& other) {
  *this = other;
}

template <class T>
Matrix<T>::~Matrix() {
  Destroy();
}

template <class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
  if (this != &other) {
    Destroy();

    m_rows = other.m_rows;
    m_cols = other.m_cols;

    Init(m_rows, m_cols);

    for (unsigned i = 0; i < m_rows; ++i) {
      for (unsigned j = 0; j < m_cols; ++j) {
        m_matrix[i][j] = other.m_matrix[i][j];
      }
    }
  }
  return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator+=(Matrix<T> other) {
  for (unsigned i = 0; i < m_rows; ++i) {
    for (unsigned j = 0; j < m_cols; ++j) {
      m_matrix[i][j] += other.m_matrix[i][j];
    }
  }
  return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator=(Matrix<T>&& other) {
  if (this != &other) {
    Destroy();

    m_rows = other.m_rows;
    m_cols = other.m_cols;
    m_matrix = other.m_matrix;

    other.m_matrix = nullptr;
    other.m_rows = other.m_cols = 0U;
  }
  return *this;
}

template <class T>
T& Matrix<T>::operator()(const unsigned row, const unsigned col) {
  return m_matrix[row][col];
}

template <class T>
const T& Matrix<T>::operator()(const unsigned row, const unsigned col) const {
  return m_matrix[row][col];
}

template <class T>
const unsigned Matrix<T>::GetRows() const {
  return m_rows;
}

template <class T>
const unsigned Matrix<T>::GetCols() const {
  return m_cols;
}

template <class T>
void Matrix<T>::Init(const unsigned rows, const unsigned cols) {
  m_matrix = new T*[rows];
  for (unsigned i = 0; i < rows; ++i) {
    m_matrix[i] = new T[cols]{};
  }
}

template <class T>
inline void Matrix<T>::Destroy() {
  if (m_matrix != nullptr) {
    for (unsigned i = 0; i < m_rows; ++i) {
      delete[] m_matrix[i];
    }
    delete[] m_matrix;
  }

  m_rows = m_cols = {};
}

Matrix<Neuron>* ForwardPropagation(const Matrix<Weight>* W,
                                   const Matrix<Neuron>* N,
                                   const Matrix<Weight>* B);
Matrix<Neuron>* BackPropagation(const Matrix<Weight>* W,
                                const Matrix<Neuron>* M,
                                const Matrix<Neuron>* N);

#endif  // MLP_SRC_MATRIX_H_
