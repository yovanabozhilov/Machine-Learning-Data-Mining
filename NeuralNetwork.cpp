#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoidDerivative(double x) {
    return x * (1.0 - x);
}

double tanhActivation(double x) {
    return tanh(x);
}

double tanhDerivative(double x) {
    return 1.0 - x * x;
}

class NeuralNetwork {
private:
    vector<vector<double>> layers;
    vector<vector<vector<double>>> weights;
    vector<vector<double>> biases;
    int activationType;

    double randomWeight() {
        const double minValue = -0.05;
        const double maxValue = 0.05;

        double randomValue = ((double)rand() / (RAND_MAX)) * (maxValue - minValue) + minValue;

        return randomValue;
    }

    double activate(double x) {
        if (activationType == 0) {
            return sigmoid(x);
        }
        else {
            return tanhActivation(x);
        }
    }

    double activateDerivative(double x) {
        if (activationType == 0) {
            return sigmoidDerivative(x);
        }
        else {
            return tanhDerivative(x);
        }
    }

public:
    NeuralNetwork(const vector<int>& topology, int activationType)
        : activationType(activationType) {
        srand((unsigned int)time(nullptr));

        for (int nodes : topology) {
            layers.push_back(vector<double>(nodes, 0.0));
        }

        for (size_t i = 1; i < topology.size(); i++) {
            int prevLayerSize = topology[i - 1];
            int currentLayerSize = topology[i];

            weights.push_back(vector<vector<double>>(currentLayerSize, vector<double>(prevLayerSize)));
            biases.push_back(vector<double>(currentLayerSize));

            for (int j = 0; j < currentLayerSize; j++) {
                for (int k = 0; k < prevLayerSize; k++) {
                    weights[i - 1][j][k] = randomWeight();
                }
                biases[i - 1][j] = randomWeight();
            }
        }
    }

    vector<double> feedForward(const vector<double>& inputs) {
        layers[0] = inputs;

        for (size_t i = 1; i < layers.size(); i++) {
            for (size_t j = 0; j < layers[i].size(); j++) {
                double activation = biases[i - 1][j];
                for (size_t k = 0; k < layers[i - 1].size(); k++) {
                    activation += layers[i - 1][k] * weights[i - 1][j][k];
                }
                layers[i][j] = activate(activation);
            }
        }

        return layers.back();
    }

    void backPropagate(const vector<double>& targets, double learningRate) {
        vector<vector<double>> errors(layers.size());

        size_t outputLayer = layers.size() - 1;
        errors[outputLayer] = vector<double>(layers[outputLayer].size());

        for (size_t i = 0; i < layers[outputLayer].size(); i++) {
            errors[outputLayer][i] = targets[i] - layers[outputLayer][i];
        }

        for (int i = layers.size() - 2; i >= 0; i--) {
            errors[i] = vector<double>(layers[i].size(), 0.0);
            for (size_t j = 0; j < layers[i + 1].size(); j++) {
                double delta = errors[i + 1][j] * activateDerivative(layers[i + 1][j]);
                for (size_t k = 0; k < layers[i].size(); k++) {
                    errors[i][k] += delta * weights[i][j][k];
                    weights[i][j][k] += learningRate * delta * layers[i][k];
                }
                biases[i][j] += learningRate * delta;
            }
        }
    }

    void train(const vector<vector<double>>& inputs, const vector<vector<double>>& targets, int epochs, double learningRate) {
        for (int epoch = 0; epoch < epochs; epoch++) {
            for (size_t i = 0; i < inputs.size(); i++) {
                feedForward(inputs[i]);
                backPropagate(targets[i], learningRate);
            }
        }
    }
};

vector<vector<double>> getTargets(const string& function) {
    if (function == "AND")
        return { {0}, {0}, {0}, {1} };
    if (function == "OR")
        return { {0}, {1}, {1}, {1} };
    if (function == "XOR")
        return { {0}, {1}, {1}, {0} };
    return {};
}

int main() {
    string function;
    int activation, hiddenLayers;
    vector<int> neuronsPerLayer;

    cout << "Input boolean function: ";
    cin >> function;
    cout << "Input activation function: ";
    cin >> activation;
    cout << "Input number of hidden layers: ";
    cin >> hiddenLayers;

    neuronsPerLayer.resize(hiddenLayers);
    for (int i = 0; i < hiddenLayers; i++) {
        cout << "Input number of neurons in hidden layer " << i + 1 << ": ";
        cin >> neuronsPerLayer[i];
    }

    vector<int> topology = { 2 };
    topology.insert(topology.end(), neuronsPerLayer.begin(), neuronsPerLayer.end());
    topology.push_back(1);

    NeuralNetwork nn(topology, activation);

    vector<vector<double>> inputs = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };

    if (function == "ALL" || function == "AND") {
        cout << "\nAND:\n";
        vector<vector<double>> targets = getTargets("AND");
        nn.train(inputs, targets, 10000, 0.1);

        for (const auto& input : inputs) {
            auto output = nn.feedForward(input);
            cout << "(" << static_cast<int>(input[0]) << "," << static_cast<int>(input[1]) << ") -> " << fixed << setprecision(4) << output[0] << "\n";
        }
    }

    if (function == "ALL" || function == "OR") {
        cout << "\nOR:\n";
        vector<vector<double>> targets = getTargets("OR");
        nn.train(inputs, targets, 10000, 0.1);

        for (const auto& input : inputs) {
            auto output = nn.feedForward(input);
            cout << "(" << static_cast<int>(input[0]) << "," << static_cast<int>(input[1]) << ") -> " << fixed << setprecision(4) << output[0] << "\n";
        }
    }

    if (function == "ALL" || function == "XOR") {
        cout << "\nXOR:\n";
        vector<vector<double>> targets = getTargets("XOR");
        nn.train(inputs, targets, 10000, 0.1);

        for (const auto& input : inputs) {
            auto output = nn.feedForward(input);
            cout << "(" << static_cast<int>(input[0]) << "," << static_cast<int>(input[1]) << ") -> " << fixed << setprecision(4) << output[0] << "\n";
        }
    }

    return 0;
}
