#include "primitive.h"

std::random_device Weight::rd;
std::default_random_engine Weight::generator(rd());
std::uniform_real_distribution<double> Weight::distribution(min_random_value,
                                                            max_random_value);

Weight::Weight(double data) : data{data} {}

void Neuron::ActivationFunction() { data = 1.0 / (1.0 + std::exp(-data)); }

double Neuron::DerivativeActivatioFunction() { return data * (1.0 - data); }

double Neuron::Delta(Neuron &target) {
  return (data - target.data) * DerivativeActivatioFunction();
}

void GNeuron::ForwardPropagation(std::vector<GNeuron> &layer) {
  data = bias->data;
  for (unsigned i = 0; i < layer.size(); ++i) {
    data += layer[i].data * prev[i]->data;
  }
  ActivationFunction();
}

void GNeuron::Delta(double ans) {
  delta = (data - ans) * DerivativeActivatioFunction();
}

void GNeuron::Delta(std::vector<GNeuron> &layer) {
  delta = {};
  for (unsigned i = 0U; i < layer.size(); ++i) {
    delta += next[i]->data * layer[i].delta;
  }
  delta *= DerivativeActivatioFunction();
}

void GNeuron::CorrectWeights(std::vector<GNeuron> &layer, double rate) {
  for (unsigned i = 0U; i < layer.size(); ++i) {
    prev[i]->data -= rate * layer[i].data * delta;
  }
  bias->data -= rate * delta;
}
