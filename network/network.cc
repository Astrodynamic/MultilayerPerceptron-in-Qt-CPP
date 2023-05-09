#include "network.h"

Network::Network(QObject *parent) : QObject(parent) {
  m_flags = NetworkFlags::kMatrixLearning;
}

Network::~Network() {
  DestroyLayers();
  DestroyWeight();
  DestroyCSV();
}

void Network::SetLearningType(const NetworkFlags flag) { m_flags = flag; }

std::vector<std::pair<unsigned, Matrix<Neuron> *> > Network::LoadSCV(
    const std::string &filename, const unsigned depth) {
  std::ifstream file(filename);
  std::string line;

  std::vector<std::pair<unsigned, Matrix<Neuron> *> > result;
  for (unsigned layer = 0U; getline(file, line); ++layer) {
    std::size_t position{};
    result.push_back(
        std::make_pair(static_cast<unsigned>(std::stoi(line, &position)),
                       new Matrix<Neuron>(depth, 1U)));

    const unsigned rows = result[layer].second->GetRows();
    const unsigned cols = result[layer].second->GetCols();
    for (unsigned i = 0U; i < rows; ++i) {
      for (unsigned j = 0U; j < cols; ++j) {
        while (!isdigit(line[position]) && line[position] != '\n') {
          ++position;
        }

        if (position < line.size()) {
          line = line.substr(position);
          result[layer].second->operator()(i, j).data =
              std::stoi(line, &position) / m_colorspace;
        }
      }
    }
  }
  file.close();
  return result;
}

QVector<double> Network::NormalizeResult() {
  QVector<double> result;
  if (m_flags == NetworkFlags::kMatrixLearning) {
    for (unsigned int i = 0; i < m_layers_f.back()->GetRows(); ++i) {
      result.push_back(m_layers_f.back()->operator()(i, 0).data);
    }
  } else {
    for (unsigned int i = 0; i < m_graph_layers.back().size(); ++i) {
      result.push_back(m_graph_layers.back().at(i).data);
    }
  }
  return result;
}

void Network::LoadLearnSample(const std::string &filename) {
  if (m_depths.size() && m_depths[m_layers_position[LayerType::kSLayer]]) {
    m_input_csv_learn =
        LoadSCV(filename, m_depths[m_layers_position[LayerType::kSLayer]]);
  }
  learn_step = 99.f / (m_epoch * m_input_csv_learn.size());
  emit IsReady();
}

void Network::LoadTestsSample(const std::string &filename) {
  if (m_depths.size() && m_depths[m_layers_position[LayerType::kSLayer]]) {
    m_input_csv_tests =
        LoadSCV(filename, m_depths[m_layers_position[LayerType::kSLayer]]);
  }
  test_step = 99.f / m_input_csv_tests.size();
  emit IsReady();
}

void Network::GenerateWeights(QVector<unsigned> &depths) {
  for (unsigned i = m_layers_position[LayerType::kSLayer];
       i < depths.size() - 1U; ++i) {
    m_weights_main.push_back(
        new Matrix<Weight>(depths.at(i + 1U), depths.at(i)));
    m_weights_bias.push_back(new Matrix<Weight>(depths.at(i + 1U), 1U));
  }
}

void Network::UpdateLayerPosition(unsigned last) {
  m_layers_position[LayerType::kSLayer] = 0U;
  m_layers_position[LayerType::kALayer] = 1U;
  m_layers_position[LayerType::kRLayer] = last;
}

void Network::LayerFormation(std::vector<GNeuron> &layer, unsigned position) {
  if (position < m_layers_position[LayerType::kRLayer]) {
    for (unsigned depth = 0U; depth < layer.size(); ++depth) {
      for (unsigned weight = 0U; weight < m_weights_main[position]->GetRows();
           ++weight) {
        layer[depth].next.push_back(
            &(m_weights_main[position]->operator()(weight, depth)));
      }
    }
  }

  if (position > m_layers_position[LayerType::kSLayer]) {
    for (unsigned depth = 0U; depth < layer.size(); ++depth) {
      for (unsigned weight = 0U;
           weight < m_weights_main[position - 1U]->GetCols(); ++weight) {
        layer[depth].prev.push_back(
            &(m_weights_main[position - 1U]->operator()(depth, weight)));
      }
      layer[depth].bias = &m_weights_bias[position - 1U]->operator()(depth, 0U);
    }
  }
}

void Network::GenerateGraphLayers(QVector<unsigned> &depths) {
  m_graph_layers.clear();

  m_graph_layers.resize(depths.size());
  for (unsigned layer = 0U; layer < depths.size(); ++layer) {
    m_graph_layers[layer].resize(depths[layer]);
    LayerFormation(m_graph_layers[layer], layer);
  }
}

void Network::Update(QVector<unsigned> depths) {
  DestroyLayers();
  DestroyWeight();

  UpdateLayerPosition(depths.size() - 1U);
  GenerateWeights(depths);
  GenerateGraphLayers(depths);

  copy(depths.begin(), depths.end(), back_inserter(m_depths));
  emit IsReady();
}

void Network::SetEpoch(unsigned epoch) { m_epoch = epoch; }

void Network::BreakProcess() { m_break_flag = true; }

void Network::SetCrossValidGroup(int group_val) {
  m_validation_parts = group_val;
}

template <class T>
void Network::DestroyData(std::vector<T> &src, unsigned idx) {
  for (unsigned i = idx, i_end = src.size(); i < i_end; ++i) {
    delete src[i];
  }
  src.clear();
}

template <class T1, class T2>
void Network::DestroyData(std::vector<std::pair<T1, T2> > &src, unsigned idx) {
  for (unsigned i = idx, i_end = src.size(); i < i_end; ++i) {
    delete src[i].second;
  }
  src.clear();
}

void Network::DestroyLayers() {
  DestroyData(m_layers_f, m_layers_position[LayerType::kALayer]);
  DestroyData(m_layers_b, m_layers_position[LayerType::kSLayer]);
}

void Network::DestroyWeight() {
  DestroyData(m_weights_main, m_layers_position[LayerType::kSLayer]);
  DestroyData(m_weights_bias, m_layers_position[LayerType::kSLayer]);
}

void Network::DestroyCSV() {
  DestroyData(m_input_csv_learn, m_layers_position[LayerType::kSLayer]);
  DestroyData(m_input_csv_tests, m_layers_position[LayerType::kSLayer]);
}

void Network::Learning() {
  tkeeper.StartTimer();
  learn_stage = 0.f;
  if (m_flags == NetworkFlags::kMatrixLearning) {
    if (m_validation_parts) {
      MatrixValidation();
    } else {
      MatrixLearning();
    }
  } else if (m_flags == NetworkFlags::kGraphLearning) {
    if (m_validation_parts) {
      GpaphValidation();
    } else {
      GraphLearning();
    }
  }
  m_break_flag = false;
  emit IsReady();
  emit NetworkLearningStage(100);
  emit LearningTime(tkeeper.EndTimer());
}

void Network::ScanImg(QPixmap input_image) {
  DestroyLayers();
  if (m_flags == NetworkFlags::kMatrixLearning) {
    MatForwardPropagation(SenseData(
        NormalaizeImage::normalize(input_image, std::sqrt(*m_depths.begin()))));
  } else if (m_flags == NetworkFlags::kGraphLearning) {
    GraphForwardPropagation(SenseData(
        NormalaizeImage::normalize(input_image, std::sqrt(*m_depths.begin()))));
  }
  emit IsReady();
  emit SendResult(NormalizeResult());
}

void Network::Testing() {
  tkeeper.StartTimer();
  std::vector<double> metrics;
  test_stage = 0.f;
  if (m_flags == NetworkFlags::kMatrixLearning) {
    metrics = MatrixTesting();
  } else if (m_flags == NetworkFlags::kGraphLearning) {
    metrics = GraphTesting();
  }
  m_average_MLP_metrics.clear();
  for (auto it = metrics.begin(); it != metrics.end(); ++it) {
    m_average_MLP_metrics.push_back(*it);
  }
  m_break_flag = false;
  emit NetworkTestingStage(100);
  emit SendMetrix(m_average_MLP_metrics);
  emit TestingingTime(tkeeper.EndTimer());
  emit IsReady();
}

void Network::SaveBinMLP(QString save_way) {
  QFile file(save_way);
  if (file.open(QIODevice::WriteOnly)) {
    QDataStream d_stream(&file);
    d_stream << m_flags;
    d_stream << static_cast<unsigned>(m_depths.size());
    for (auto &iter_of_depth : m_depths) {
      d_stream << iter_of_depth;
    }

    for (auto &iter_of_metrics_ : m_average_MLP_metrics) {
      d_stream << iter_of_metrics_;
    }

    for (auto &iter_on_matrix : m_weights_main) {
      for (unsigned int i = 0; i < iter_on_matrix->GetRows(); ++i) {
        for (unsigned int j = 0; j < iter_on_matrix->GetCols(); ++j) {
          d_stream << iter_on_matrix->operator()(i, j).data;
        }
      }
    }

    for (auto &iter_on_bias : m_weights_bias) {
      for (unsigned int i = 0; i < iter_on_bias->GetRows(); ++i) {
        for (unsigned int j = 0; j < iter_on_bias->GetCols(); ++j) {
          d_stream << iter_on_bias->operator()(i, j).data;
        }
      }
    }

    file.close();
  }
  emit IsReady();
}

void Network::UpdateLearningRate(double val) { m_learning_rate = val; }

void Network::LoadBinMLP(QString load_way) {
  QFile file(load_way);
  file.open(QIODevice::ReadOnly);
  QDataStream d_stream(&file);
  unsigned layout_count = 0;
  d_stream >> m_flags;
  d_stream >> layout_count;
  QVector<unsigned> new_vec;
  for (; layout_count; --layout_count) {
    unsigned tmp = 0;
    d_stream >> tmp;
    new_vec.push_back(tmp);
  }

  Update(new_vec);
  m_average_MLP_metrics.clear();
  for (int i = 0; i < 4; ++i) {
    double tmp_d;
    d_stream >> tmp_d;
    m_average_MLP_metrics.push_back(tmp_d);
  }
  emit SendMetrix(m_average_MLP_metrics);
  for (auto &iter_on_matrix : m_weights_main) {
    for (unsigned int i = 0; i < iter_on_matrix->GetRows(); ++i) {
      for (unsigned int j = 0; j < iter_on_matrix->GetCols(); ++j) {
        d_stream >> iter_on_matrix->operator()(i, j).data;
      }
    }
  }
  for (auto &iter_on_bias : m_weights_bias) {
    for (unsigned int i = 0; i < iter_on_bias->GetRows(); ++i) {
      for (unsigned int j = 0; j < iter_on_bias->GetCols(); ++j) {
        d_stream >> iter_on_bias->operator()(i, j).data;
      }
    }
  }
  file.close();
  emit IsReady();
  emit LoadingModel(new_vec);
}

void Network::MatForwardPropagation(Matrix<Neuron> *sensor) {
  m_layers_f.push_back(sensor);
  for (unsigned mat = 0; mat < m_weights_main.size(); ++mat) {
    m_layers_f.push_back(ForwardPropagation(
        m_weights_main[mat], m_layers_f[mat], m_weights_bias[mat]));
  }
}

void Network::MatCalculateDelta(unsigned ans) {
  m_layers_b.push_back(
      new Matrix<Neuron>(m_depths[m_layers_position[LayerType::kRLayer]], 1U));
  m_layers_b.back()->operator()(ans - 1U, 0U).data = 1.0;

  for (unsigned row = 0U; row < m_layers_b.back()->GetRows(); ++row) {
    Neuron &next = m_layers_f.back()->operator()(row, 0U);
    Neuron &back = m_layers_b.back()->operator()(row, 0U);
    back.data = next.Delta(back);
  }
}

void Network::MatCalculateDelta(Matrix<Weight> *&weights, Matrix<Neuron> *&out,
                                Matrix<Neuron> *&tgt) {
  m_layers_b.push_back(BackPropagation(weights, out, tgt));
}

void Network::MatCorrectWeights(Matrix<Neuron> *&out, Matrix<Neuron> *&tgt,
                                Matrix<Weight> *&weights,
                                Matrix<Weight> *&biases) {
  for (unsigned row = 0U; row < weights->GetRows(); ++row) {
    double delta = tgt->operator()(row, 0U).data;
    for (unsigned col = 0U; col < weights->GetCols(); ++col) {
      double neuron = out->operator()(col, 0U).data;
      weights->operator()(row, col).data -= m_learning_rate * neuron * delta;
    }
    biases->operator()(row, 0U).data -= m_learning_rate * delta;
  }
}

void Network::MatBackPropagation(unsigned ans) {
  MatCalculateDelta(ans);

  for (int mat = m_weights_main.size() - 1; mat >= 0; --mat) {
    Matrix<Neuron> *&out = m_layers_f[mat];
    Matrix<Neuron> *&tgt = m_layers_b.back();
    Matrix<Weight> *&biases = m_weights_bias[mat];
    Matrix<Weight> *&weights = m_weights_main[mat];

    MatCorrectWeights(out, tgt, weights, biases);
    if (mat) {
      MatCalculateDelta(weights, out, tgt);
    }
  }
}

void Network::MatrixLearning() {
  QList<QPointF> error_values;
  int stage_last{};
  for (unsigned epoch = 0U; epoch < m_epoch && !m_break_flag; ++epoch) {
    for (unsigned position = 0U;
         position < m_input_csv_learn.size() && !m_break_flag; ++position) {
      DestroyLayers();
      MatForwardPropagation(m_input_csv_learn[position].second);
      error_values.push_back(
          MatrixTrainingError(position, m_input_csv_learn[position].first));
      MatBackPropagation(m_input_csv_learn[position].first);
      learn_stage += learn_step;
      if ((int)learn_stage > stage_last) {
        stage_last = (int)learn_stage;
        emit SendNewMistake(error_values);
        emit NetworkLearningStage(stage_last);
        error_values.clear();
      }
    }
  }
}

void Network::GraphInitSLayer(Matrix<Neuron> *&layer) {
  for (unsigned neuron = 0U; neuron < layer->GetRows(); ++neuron) {
    m_graph_layers[0U][neuron].data = layer->operator()(neuron, 0U).data;
  }
}

void Network::GraphForwardPropagation(Matrix<Neuron> *layer) {
  GraphInitSLayer(layer);
  for (unsigned n_layer = m_layers_position[LayerType::kALayer];
       n_layer < m_graph_layers.size(); ++n_layer) {
    for (auto &it : m_graph_layers[n_layer]) {
      it.ForwardPropagation(m_graph_layers[n_layer - 1U]);
    }
  }
}

void Network::GraphCalculateDelta(unsigned ans) {
  unsigned layers_size = m_graph_layers.size() - 1U;
  for (unsigned neuron = 0U; neuron < m_graph_layers[layers_size].size();
       ++neuron) {
    m_graph_layers[layers_size][neuron].Delta(ans != neuron ? 0.0 : 1.0);
  }

  for (unsigned layer = m_layers_position[LayerType::kRLayer] - 1U; layer > 0;
       --layer) {
    for (unsigned neuron = 0U; neuron < m_graph_layers[layer].size();
         ++neuron) {
      m_graph_layers[layer][neuron].Delta(m_graph_layers[layer + 1]);
    }
  }
}

void Network::GraphCorrectWeights() {
  for (unsigned layer = 1U; layer < m_graph_layers.size(); ++layer) {
    for (unsigned neuron = 0U; neuron < m_graph_layers[layer].size();
         ++neuron) {
      m_graph_layers[layer][neuron].CorrectWeights(m_graph_layers[layer - 1U],
                                                   m_learning_rate);
    }
  }
}

void Network::GraphBackPropagation(unsigned ans) {
  GraphCalculateDelta(ans);
  GraphCorrectWeights();
}

void Network::GraphLearning() {
  QList<QPointF> error_values;
  int stage_last{};
  for (unsigned epoch = 0U; epoch < m_epoch && !m_break_flag; ++epoch) {
    for (unsigned position = 0U;
         position < m_input_csv_learn.size() && !m_break_flag; ++position) {
      GraphForwardPropagation(m_input_csv_learn[position].second);
      error_values.push_back(
          GraphTrainingError(position, m_input_csv_learn[position].first));
      GraphBackPropagation(m_input_csv_learn[position].first - 1U);
      learn_stage += learn_step;
      if ((int)learn_stage > stage_last) {
        stage_last = (int)learn_stage;
        emit SendNewMistake(error_values);
        emit NetworkLearningStage(stage_last);
        error_values.clear();
      }
    }
  }
}

void Network::MatFormConfusionMatrix(std::vector<unsigned> &confusion,
                                     unsigned ans) {
  Matrix<Neuron> *&layer = m_layers_f.back();
  for (unsigned i = 0U; i < layer->GetRows(); ++i) {
    if (layer->operator()(i, 0U).data > 0.5) {
      if (ans == i) {
        ++confusion[ConfusionMatrix::kTruePositive];
      } else {
        ++confusion[ConfusionMatrix::kFalsePositive];
      }
    } else {
      if (ans == i) {
        ++confusion[ConfusionMatrix::kFalseNegative];
      } else {
        ++confusion[ConfusionMatrix::kTrueNegative];
      }
    }
  }
}

std::vector<double> Network::FormMetrixNetwork(
    std::vector<unsigned> &confusion) {
  std::vector<double> metrics(Metrics::kMetricsAttribute, 0.0);
  double TP = (double)confusion[ConfusionMatrix::kTruePositive];
  double FP = (double)confusion[ConfusionMatrix::kFalsePositive];
  double FN = (double)confusion[ConfusionMatrix::kFalseNegative];
  double TN = (double)confusion[ConfusionMatrix::kTrueNegative];

  metrics[Metrics::kAccuracy] = (TP + TN) / (TP + TN + FP + FN);
  metrics[Metrics::kPrecision] = TP / (TP + FP);
  metrics[Metrics::kRecall] = TP / (TP + FN);

  const double &precision = metrics[Metrics::kPrecision];
  const double &recall = metrics[Metrics::kRecall];

  metrics[Metrics::kFMeasure] =
      (2.0 * precision * recall) / (precision + recall);
  return metrics;
}

std::vector<double> Network::MatrixTesting() {
  std::vector<unsigned> confusion(ConfusionMatrix::kConfusionMatrixAttribute,
                                  0U);
  for (unsigned position = 0U;
       position < m_input_csv_tests.size() && !m_break_flag; ++position) {
    DestroyLayers();
    MatForwardPropagation(m_input_csv_tests[position].second);
    MatFormConfusionMatrix(confusion, m_input_csv_tests[position].first - 1U);
    test_stage += test_step;
    emit NetworkTestingStage((int)test_stage);
  }
  return FormMetrixNetwork(confusion);
}

void Network::GraphFormConfusionMatrix(std::vector<unsigned> &confusion,
                                       unsigned ans) {
  const std::vector<GNeuron> &layer =
      m_graph_layers[m_layers_position[LayerType::kRLayer]];
  for (unsigned i = 0U; i < layer.size(); ++i) {
    if (layer[i].data > 0.5) {
      if (ans == i) {
        ++confusion[ConfusionMatrix::kTruePositive];
      } else {
        ++confusion[ConfusionMatrix::kFalsePositive];
      }
    } else {
      if (ans == i) {
        ++confusion[ConfusionMatrix::kFalseNegative];
      } else {
        ++confusion[ConfusionMatrix::kTrueNegative];
      }
    }
  }
}

Matrix<Neuron> *Network::SenseData(QImage normal_image) {
  Matrix<Neuron> *sense_matrix =
      new Matrix<Neuron>(normal_image.width() * normal_image.height(), 1U);
  for (int rows = 0; rows < normal_image.height(); ++rows) {
    for (int cols = 0; cols < normal_image.width(); ++cols) {
      sense_matrix->operator()(rows * normal_image.width() + cols, 0U).data =
          QColor(normal_image.pixel(rows, cols)).blackF();
    }
  }
  return sense_matrix;
}

std::vector<double> Network::GraphTesting() {
  std::vector<unsigned> confusion(ConfusionMatrix::kConfusionMatrixAttribute,
                                  0U);
  for (unsigned position = 0U;
       position < m_input_csv_tests.size() && !m_break_flag; ++position) {
    GraphForwardPropagation(m_input_csv_tests[position].second);
    GraphFormConfusionMatrix(confusion, m_input_csv_tests[position].first - 1U);
    emit NetworkTestingStage((int)test_stage);
  }
  return FormMetrixNetwork(confusion);
}

QPointF Network::MatrixTrainingError(unsigned position, unsigned ans) {
  QPointF point(position, 0.0);
  Matrix<Neuron> *&out = m_layers_f.back();
  double deviation{};
  for (unsigned row = 0U; row < out->GetRows(); ++row) {
    deviation +=
        qPow(out->operator()(row, 0U).data - (ans != row ? 0.0 : 1.0), 2);
  }
  point.setY(deviation / out->GetRows());
  return point;
}

QPointF Network::GraphTrainingError(unsigned position, unsigned ans) {
  QPointF point(static_cast<float>(position), 0.0);

  const std::vector<GNeuron> &out = m_graph_layers.back();
  double deviation{};
  for (unsigned row = 0U; row < out.size(); ++row) {
    deviation += qPow(out[row].data - (ans != row ? 0.0 : 1.0), 2.0);
  }
  point.setY(deviation / out.size());
  return point;
}

void Network::MatrixValidation() {
  QList<QPointF> error_values;
  int stage_last{};
  unsigned block = m_input_csv_learn.size() / m_validation_parts;
  for (unsigned itteration = 0U;
       itteration < m_validation_parts && !m_break_flag; ++itteration) {
    unsigned s_skip_block = itteration * block;
    unsigned e_skip_block = (itteration + 1U) * block;
    for (unsigned position = 0U;
         position < m_input_csv_learn.size() && !m_break_flag; ++position) {
      if ((position < s_skip_block) || (position > e_skip_block)) {
        DestroyLayers();
        MatForwardPropagation(m_input_csv_learn[position].second);
        error_values.push_back(MatrixTrainingError(
            position, m_input_csv_learn[position].first - 1U));
        MatBackPropagation(m_input_csv_learn[position].first);
        learn_stage += learn_step;
        if ((int)learn_stage > stage_last) {
          stage_last = (int)learn_stage;
          emit SendNewMistake(error_values);
          emit NetworkLearningStage(stage_last);
          error_values.clear();
        }
      }
    }
  }
}

void Network::GpaphValidation() {
  QList<QPointF> error_values;
  int stage_last{};
  unsigned block = m_input_csv_learn.size() / m_validation_parts;
  for (unsigned itteration = 0U;
       itteration < m_validation_parts && !m_break_flag; ++itteration) {
    unsigned s_skip_block = itteration * block;
    unsigned e_skip_block = (itteration + 1U) * block;
    for (unsigned position = 0U;
         position < m_input_csv_learn.size() && !m_break_flag; ++position) {
      if ((position < s_skip_block) || (position > e_skip_block)) {
        GraphForwardPropagation(m_input_csv_learn[position].second);
        error_values.push_back(GraphTrainingError(
            position, m_input_csv_learn[position].first - 1U));
        GraphBackPropagation(m_input_csv_learn[position].first - 1U);
        learn_stage += learn_step;
        if ((int)learn_stage > stage_last) {
          stage_last = (int)learn_stage;
          emit SendNewMistake(error_values);
          emit NetworkLearningStage(stage_last);
          error_values.clear();
        }
      }
    }
  }
}
