#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;
class Neuron 
{
    public:
        double error;
        double value;
        double bias;
        vector<double> weights;
};

class Layer 
{
    public:
        vector<Neuron> neurons;
};

class NeuralNetwork 
{
    private:
        double learningRate = 0.8;
    public:
        vector<Layer> layers;
        double sigmoid(double x) 
        {
            return 1.0 / (1.0 + exp(-x));
        }
        double sigmoidDerivative(double x) 
        {
            double sigmoidValue = sigmoid(x);
            return sigmoidValue * (1 - sigmoidValue);
        }
        NeuralNetwork() 
        {
            Layer inputLayer;
            for (int i = 0; i < 8; i++) 
            {
                Neuron neuron;
                neuron.value = 0;
                inputLayer.neurons.push_back(neuron);
            }
            layers.push_back(inputLayer);

            Layer hiddenLayer;
            double bias = 2.0 * rand() / RAND_MAX - 1;
            for (int i = 0; i < 8; i++) 
            {
                Neuron neuron;
                neuron.bias = bias;
                neuron.value = 2.0 * rand() / RAND_MAX - 1;
                for (int j = 0; j < 8; j++) 
                {
                    double weight = 2.0 * rand() / RAND_MAX - 1;
                    while(weight == 0)
                    {
                        weight = 2.0 * rand() / RAND_MAX - 1;
                    }
                    neuron.weights.push_back(weight);
                }
                hiddenLayer.neurons.push_back(neuron);
            }
            layers.push_back(hiddenLayer);
            Layer outputLayer;
            Neuron neuron;
            neuron.value = 2.0 * rand() / RAND_MAX - 1;
            neuron.bias = 2.0 * rand() / RAND_MAX - 1;
            for (int j = 0; j < 8; j++)
            {
                double weight = 2.0 * rand() / RAND_MAX - 1;
                while(weight == 0)
                {
                    weight = 2.0 * rand() / RAND_MAX - 1;
                }
                neuron.weights.push_back(weight);
            }
            outputLayer.neurons.push_back(neuron);
            layers.push_back(outputLayer);
        }
        int forwardPropagate(vector<double> input) 
        {
            for (size_t i = 0; i < layers.size(); i++) 
            {
                Layer &layer = layers[i];
                for (size_t j = 0; j < layer.neurons.size(); j++) 
                {
                    if (i == 0) 
                    {
                        layer.neurons[j].value = input[j];
                    }
                    else
                    {
                        Neuron &neuron = layer.neurons[j];
                        double sum = 0;
                        for (size_t k = 0; k < neuron.weights.size(); k++)
                        {
                            sum += neuron.weights[k] * layers[i - 1].neurons[k].value;
                        }
                        neuron.value = sigmoid(sum + neuron.bias);
                    }
                }
            }
            double output = layers.back().neurons[0].value;
            return output >= 0.5 ? 1 : 0;
        }

        void backPropagate(vector<double> actualOutputs) 
        {
            for (size_t i = 0; i < actualOutputs.size(); i++)
            {
                Layer &outputLayer = layers.back();
                Neuron &outputNeuron = outputLayer.neurons[i];
                double outputError = actualOutputs[i] - outputNeuron.value;
                outputNeuron.error = outputError * sigmoidDerivative(outputNeuron.value);
                double newWeights[8];
                for (int i = 0; i < 8; i++)
                {
                    newWeights[i] = outputNeuron.weights[i] + learningRate * outputNeuron.error * layers[1].neurons[i].value;
                }
                for (int i = 0; i < 8; i++)
                {
                    layers[1].neurons[i].error = outputNeuron.error * outputNeuron.weights[i] * sigmoidDerivative(layers[1].neurons[i].value);
                    for (int j = 0; j < 8; j++)
                    {
                        layers[1].neurons[i].weights[j] += learningRate * layers[1].neurons[i].error * layers[0].neurons[j].value;
                    }
                }
                for (int i = 0; i < 8; i++)
                {
                    outputNeuron.weights[i] = newWeights[i];
                }
            }
        }


        void train(vector<vector<double>> inputs, vector<double> outputs, int epochs) 
        {
            for (int epoch = 0; epoch < epochs; epoch++) 
            {
                double totalError = 0;
                for (size_t i = 0; i < inputs.size(); i++) 
                {
                    int prediction = forwardPropagate(inputs[i]);
                    backPropagate({outputs[i]});
                    double error = pow(outputs[i] - prediction, 2);
                    totalError += error;
                }
                double meanError = totalError / inputs.size();
                cout << "Epoch: " << epoch + 1 << ", Error: " << meanError << endl;
            }
        }
        vector<int> predict(vector<vector<double>> inputs) 
        {
            vector<int> predictions;
            for (size_t i = 0; i < inputs.size(); i++)
            {
                predictions.push_back(forwardPropagate(inputs[i]));
                
            }
            return predictions;
        }
};
vector<string> split(const string &s, char delimiter) 
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) 
    {
        tokens.push_back(token);
    }
    return tokens;
}
pair<vector<vector<double>>, vector<double>> readData(const string &filename) 
{
    ifstream file(filename);
    vector<vector<double>> inputs;
    vector<double> outputs;

    if (!file.is_open()) 
    {
        cout << "Unable to open file" << endl;
        return make_pair(inputs, outputs);
    }

    string line;
    getline(file, line);
    while (getline(file, line)) 
    {
        vector<string> tokens = split(line, ',');
        vector<double> input;
        for (size_t i = 0; i < tokens.size() - 1; i++) 
        {
            input.push_back(stod(tokens[i]));
        }
        inputs.push_back(input);
        outputs.push_back(stod(tokens.back()));
    }

    file.close();

    return make_pair(inputs, outputs);
}
int main() 
{
    cout << "Enter the seed: ";
    int seed;
    cin >> seed;
    srand(seed);
    pair<vector<vector<double>>, vector<double>> trainingData = readData("mushroom_train.csv");
    pair<vector<vector<double>>, vector<double>> testData = readData("mushroom_test.csv");

    NeuralNetwork nn;
    int epochs = 50;
    nn.train(trainingData.first, trainingData.second, epochs);

    vector<int> predictions = nn.predict(testData.first);

    int correct = 0;
    for (size_t i = 0; i < predictions.size(); i++)
    {
        if (predictions[i] == testData.second[i]) {
            correct++;
        }
    }
    double accuracy = 100.0 * correct / predictions.size();
    cout << "Accuracy: " << accuracy << "%" << endl;

    return 0;
}