#include "matrix.h"

Matrix<Neuron>* ForwardPropagation(const Matrix<Weight>* W,
                                   const Matrix<Neuron>* N,
                                   const Matrix<Weight>* B) {
  Matrix<Neuron>* R = new Matrix<Neuron>(W->m_rows, N->m_cols);

  for (int i = 0, i_end = R->m_rows; i < i_end; ++i) {
    for (int j = 0, j_end = R->m_cols; j < j_end; ++j) {
      R->m_matrix[i][j].data = B->m_matrix[i][j].data;
      for (int k = 0, k_end = W->m_cols; k < k_end; ++k) {
        R->m_matrix[i][j].data +=
            W->m_matrix[i][k].data * N->m_matrix[k][j].data;
      }
      R->m_matrix[i][j].ActivationFunction();
    }
  }

  return R;
}

Matrix<Neuron>* BackPropagation(const Matrix<Weight>* W,
                                const Matrix<Neuron>* M,
                                const Matrix<Neuron>* N) {
  Matrix<Neuron>* R = new Matrix<Neuron>(W->m_cols, N->m_cols);

  for (int i = 0, i_end = R->m_rows; i < i_end; ++i) {
    for (int j = 0, j_end = R->m_cols; j < j_end; ++j) {
      for (int k = 0, k_end = W->m_rows; k < k_end; ++k) {
        R->m_matrix[i][j].data +=
            W->m_matrix[k][i].data * N->m_matrix[k][j].data;
      }
      R->m_matrix[i][j].data *= M->m_matrix[i][j].DerivativeActivatioFunction();
    }
  }

  return R;
}
