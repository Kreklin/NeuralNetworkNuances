#include "stdafx.h"
#include <iostream> // for cout
#include <ctime> // for time(0)
#include <cstdlib> // for RAND_MAX, srand() and rand()
#include <vector> // for vector object
#include <string> // for string object
#include <fstream> // for fstream object

using namespace std;

float getRandomNumber(float min, float max)
{
	static const float fraction = 1.0 / (RAND_MAX);
	return min + ((max - min) * (rand() * fraction));
}

void floatOperation(const float &x)
{
	float y = x * x;
}

void doubleOperation(const double &x)
{
	double y = x * x;
}

float max(float x, float y)
{
	if (x > y) return x;
	else return y;
}

void testSinglePrecision()
{
	int start = time(0);
	const int operations = 1E8;
	const float a = 0.0;
	for (int i = 0; i < operations; ++i)
		floatOperation(a);
	int end = time(0);
	int difference = end - start;
	cout << "Single precision time: " << difference << "\nSingle precision operations per second: " << operations / difference;
}

void testDoublePrecision()
{
	int start = time(0);
	const int operations = 1E8;
	const double a = 0.0;
	for (int i = 0; i < operations; ++i)
		doubleOperation(a);
	int end = time(0);
	int difference = end - start;
	cout << "Double precision time: " << difference << "\nDouble precision operations per second: " << operations / difference;
}

class Node
{
private:
	vector<float> m_weights;
	float m_bias;
public:
	Node() {}
	Node(unsigned int length)
	{
		m_weights.resize(length);
		m_bias = getRandomNumber(-1.0, 1.0);
		for (unsigned int i = 0; i < length; ++i)
			m_weights[i] = getRandomNumber(-1.0, 1.0);
	}
	~Node()
	{
	}
	float activation(vector<float> inputs)
	{
		if (inputs.size() != m_weights.size()) cerr << "\nNode passed wrong inputs. " << inputs.size() << " inputs for a node with " << m_weights.size() << " weights.";
		float sum = m_bias;
		for (unsigned int i = 0; i < inputs.size(); ++i)
			sum += inputs[i] * m_weights[i];
		return max(0.0, sum);
	}
	void printWeights()
	{
		cout << "\nBias: " << m_bias << " Weights: ";
		for (unsigned int i = 0; i < m_weights.size(); ++i) cout << m_weights[i] << " ";
	}
	void randomizeWeights(float min, float max)
	{
		m_bias = getRandomNumber(min, max);
		for (unsigned int i = 0; i < m_weights.size(); ++i)
			m_weights[i] = getRandomNumber(min, max);
	}
};

class HiddenLayer
{
public:
	vector<Node> m_hiddenNode;
	HiddenLayer(unsigned int layerLength, unsigned int nodeLength)
	{
		m_hiddenNode = vector<Node>(layerLength, Node(nodeLength));
	}
	unsigned int length()
	{
		return m_hiddenNode.size();
	}
};

class NeuralNetwork
{
private:
	unsigned int m_inputs;
	vector<HiddenLayer> m_hiddenLayers;
	vector<Node> m_outputNode;
	const float m_minWeight;
	const float m_maxWeight;
public:
	NeuralNetwork(unsigned int inputs, unsigned int hiddens, unsigned int outputs, unsigned int hiddenLayers = 1, float minWeight = -1.0, float maxWeight = 1.0) : m_inputs(inputs), m_minWeight(minWeight), m_maxWeight(maxWeight)
	{
		// cout << "Creating nodes...\n";
		m_hiddenLayers = vector<HiddenLayer>(1, HiddenLayer(hiddens, m_inputs));
		m_hiddenLayers.resize(hiddenLayers, HiddenLayer(hiddens, hiddens));
		m_outputNode = vector<Node>(outputs, Node(hiddens));
		// cout << "Randomizing weights...\n";
		for (unsigned int i = 0; i < hiddenLayers; ++i) {
			for (unsigned int j = 0; j < hiddens; ++j) {
				m_hiddenLayers[i].m_hiddenNode[j].randomizeWeights(minWeight, maxWeight);
			}
		}
		// cout << "Hidden nodes created.\n";
		for (unsigned int i = 0; i < m_outputNode.size(); ++i) {
			m_outputNode[i].randomizeWeights(minWeight, maxWeight);
		}
		// cout << "Output nodes created.\n";
	}
	~NeuralNetwork()
	{
	}
	vector<float> feedForward(vector<float> inputs)
	{
		vector<float> layerIn;
		layerIn.resize(inputs.size());
		for (unsigned int i = 0; i < inputs.size(); ++i) {
			layerIn[i] = inputs[i]; // copy inputs into LayerIn
		}

		// hidden layer calculations
		vector<float> layerOut;
		for (unsigned int i = 0; i < m_hiddenLayers.size(); ++i) {
			layerOut.resize(m_hiddenLayers[i].m_hiddenNode.size()); // resize layerOut to be the right length for this hidden layers output
			for (unsigned int j = 0; j < m_hiddenLayers[i].length(); ++j) {
				layerOut[j] = m_hiddenLayers[i].m_hiddenNode[j].activation(layerIn); // calculate output of each node in this layer
			}
			layerIn.resize(layerOut.size());
			for (unsigned int j = 0; j < layerIn.size(); ++j) {
				layerIn[j] = layerOut[j]; // copy layerOut to layerIn for next layer
			}
		}

		// output layer calculations
		layerOut.resize(m_outputNode.size());
		for (unsigned int i = 0; i < m_outputNode.size(); ++i) {
			layerOut[i] = m_outputNode[i].activation(layerIn); // calculate output of each node in this layer
		}
		// cout << "\nFinished feedforward.";
		return layerOut;
	}
	void printWeights()
	{
		cout << "\n\nHidden Node Weights\n";
		for (unsigned int i = 0; i < m_hiddenLayers.size(); ++i) {
			cout << "\nHidden Layer " << i + 1;
			for (unsigned int j = 0; j < m_hiddenLayers[i].length(); ++j)
				m_hiddenLayers[i].m_hiddenNode[j].printWeights();
		}
		cout << "\n\nOutput Node Weights\n";
		for (unsigned int i = 0; i < m_outputNode.size(); ++i) {
			m_outputNode[i].printWeights();
		}
		cout << "\n";
	}
};

class ResultsTable
{
private:
	vector<vector<vector<vector<float> > > > m_results;
public:
	ResultsTable() 
	{
	}
	ResultsTable(const unsigned int &inputs, const unsigned int &hiddenLayers, const unsigned int &hiddenNodes, const unsigned int &outputs)
	{
		m_results.resize(inputs);
		for (unsigned int i = 0; i < inputs; ++i) {
			m_results[i].resize(hiddenLayers);
			for (unsigned int j = 0; j < hiddenLayers; ++j) {
				m_results[i][j].resize(hiddenNodes);
				for (unsigned int k = 0; k < outputs; ++k) {
					m_results[i][j][k].resize(outputs);
				}
			}
		}
	}
	ResultsTable(string data)
	{
		// to do: turn string into a table
	}
	void setValue(const unsigned int &input, const unsigned int &hiddenLayer, const unsigned int &hiddenNode, const unsigned int &output, const float &value)
	{
		if (input > m_results.size()) {
			m_results.resize(input);
			//cout << "Resizing IN ";
		}
		if (hiddenLayer > m_results[input - 1].size()) {
			m_results[input - 1].resize(hiddenLayer);
			//cout << "Resizing HL ";
		}
		if (hiddenNode > m_results[input - 1][hiddenLayer - 1].size()) {
			m_results[input - 1][hiddenLayer - 1].resize(hiddenNode);
			//cout << "Resizing HN ";
		}
		if (output > m_results[input - 1][hiddenLayer - 1][hiddenNode - 1].size()) {
			m_results[input - 1][hiddenLayer - 1][hiddenNode - 1].resize(output);
			//cout << "Resizing ON ";
		}
		//cout << "Saving value to [" << input - 1 << "][" << hiddenLayer -1 << "][" << hiddenNode -1 << "][" << output -1 << "]\n";
		m_results[input - 1][hiddenLayer - 1][hiddenNode - 1][output - 1] = value;
	}
	string getData()
	{
		string data = "";
		for (int i = 0; i < m_results.size(); ++i)
		{
			for (int j = 0; j < m_results[0].size(); ++j)
			{
				for (int k = 0; k < m_results[0][0].size(); ++k)
				{
					for (int l = 0; l < m_results[0][0][0].size(); ++l)
						data += to_string(i) + "." + to_string(j) + "." + to_string(k) + "." + to_string(l) + "." + to_string(m_results[i][j][k][l]) + "\n";
				}
			}
		}
		cout << data;
		return data;
	}
	void print(const int &input, const int &hiddenLayer, const int &hiddenNode, const int &output) // magically prints data, don't even try reading the code
	{
		if (input >= int(m_results.size()) || hiddenLayer >= int(m_results[0].size()) || hiddenNode >= int(m_results[0][0].size()) || output >= int(m_results[0][0][0].size())) {
			cout << "\nParameters out of range." << input << " " << hiddenLayer << " " << hiddenNode << " " << output;
			return;
		}
		if (input < 0) {
			for (unsigned int i = 0; i < m_results.size(); ++i) {
				if (hiddenLayer < 0) {
					for (unsigned int j = 0; j < m_results[i].size(); ++j) {
						if (hiddenNode < 0) {
							for (unsigned int k = 0; k < m_results[i][j].size(); ++k) {
								if (output < 0) {
									for (unsigned int l = 0; l < m_results[i][j][k].size(); ++l) {
										cout << "\nIN-" << i+1 << " HL-" << j+1 << " HN-" << k+1 << " ON-" << l+1 << ": " << m_results[i][j][k][l] << " FF/s";
									}
								}
								else {
									cout << "\nIN-" << i+1 << " HL-" << j+1 << " HN-" << k+1 << " ON-" << output+1 << ": " << m_results[i][j][k][output] << " FF/s";
								}
							}
						}
						else {
							if (output < 0) {
								for (unsigned int l = 0; l < m_results[i][j][hiddenNode].size(); ++l) {
									cout << "\nIN-" << i+1 << " HL-" << j+1 << " HN-" << hiddenNode+1 << " ON-" << l+1 << ": " << m_results[i][j][hiddenNode][l] << " FF/s";
								}
							}
							else {
								cout << "\nIN-" << i+1 << " HL-" << j+1 << " HN-" << hiddenNode+1 << " ON-" << output+1 << ": " << m_results[i][j][hiddenNode][output] << " FF/s";
							}
						}
					}
				}
				else {
					if (hiddenNode < 0) {
						for (unsigned int k = 0; k < m_results[i][hiddenLayer].size(); ++k) {
							if (output < 0) {
								for (unsigned int l = 0; l < m_results[i][hiddenLayer][k].size(); ++l) {
									cout << "\nIN-" << i+1 << " HL-" << hiddenLayer+1 << " HN-" << k+1 << " ON-" << l+1 << ": " << m_results[i][hiddenLayer][k][l] << " FF/s";
								}
							}
							else {
								cout << "\nIN-" << i+1 << " HL-" << hiddenLayer+1 << " HN-" << k+1 << " ON-" << output+1 << ": " << m_results[i][hiddenLayer][k][output] << " FF/s";
							}
						}
					}
					else {
						if (output < 0) {
							for (unsigned int l = 0; l < m_results[i][hiddenLayer][hiddenNode].size(); ++l) {
								cout << "\nIN-" << i+1 << " HL-" << hiddenLayer+1 << " HN-" << hiddenNode+1 << " ON-" << l+1 << ": " << m_results[i][hiddenLayer][hiddenNode][l] << " FF/s";
							}
						}
						else {
							cout << "\nIN-" << i+1 << " HL-" << hiddenLayer+1 << " HN-" << hiddenNode+1 << " ON-" << output+1 << ": " << m_results[i][hiddenLayer][hiddenNode][output] << " FF/s";
						}
					}
				}
			}
		}
		else {
			if (hiddenLayer < 0) {
				for (unsigned int j = 0; j < m_results[input].size(); ++j) {
					if (hiddenNode < 0) {
						for (unsigned int k = 0; k < m_results[input][j].size(); ++k) {
							if (output < 0) {
								for (unsigned int l = 0; l < m_results[input][j][k].size(); ++l) {
									cout << "\nIN-" << input+1 << " HL-" << j+1 << " HN-" << k+1 << " ON-" << l+1 << ": " << m_results[input][j][k][l] << " FF/s";
								}
							}
							else {
								cout << "\nIN-" << input+1 << " HL-" << j+1 << " HN-" << k+1 << " ON-" << output+1 << ": " << m_results[input][j][k][output] << " FF/s";
							}
						}
					}
					else {
						if (output < 0) {
							for (unsigned int l = 0; l < m_results[input][j][hiddenNode].size(); ++l) {
								cout << "\nIN-" << input+1 << " HL-" << j+1 << " HN-" << hiddenNode+1 << " ON-" << l+1 << ": " << m_results[input][j][hiddenNode][l] << " FF/s";
							}
						}
						else {
							cout << "\nIN-" << input+1 << " HL-" << j+1 << " HN-" << hiddenNode+1 << " ON-" << output+1 << ": " << m_results[input][j][hiddenNode][output] << " FF/s";
						}
					}
				}
			}
			else {
				if (hiddenNode < 0) {
					for (unsigned int k = 0; k < m_results[input][hiddenLayer].size(); ++k) {
						if (output < 0) {
							for (unsigned int l = 0; l < m_results[input][hiddenLayer][k].size(); ++l) {
								cout << "\nIN-" << input+1 << " HL-" << hiddenLayer+1 << " HN-" << k+1 << " ON-" << l+1 << ": " << m_results[input][hiddenLayer][k][l] << " FF/s";
							}
						}
						else {
							cout << "\nIN-" << input+1 << " HL-" << hiddenLayer+1 << " HN-" << k+1 << " ON-" << output+1 << ": " << m_results[input][hiddenLayer][k][output] << " FF/s";
						}
					}
				}
				else {
					if (output < 0) {
						for (unsigned int l = 0; l < m_results[input][hiddenLayer][hiddenNode].size(); ++l) {
							cout << "\nIN-" << input+1 << " HL-" << hiddenLayer+1 << " HN-" << hiddenNode+1 << " ON-" << l+1 << ": " << m_results[input][hiddenLayer][hiddenNode][l] << " FF/s";
						}
					}
					else {
						cout << "\nIN-" << input+1 << " HL-" << hiddenLayer+1 << " HN-" << hiddenNode+1 << " ON-" << output+1 << ": " << m_results[input][hiddenLayer][hiddenNode][output] << " FF/s";
					}
				}
			}
		}
	}
};

void testFeedForwardSpeed(const unsigned int &maxInputs, const unsigned int &maxHiddenLayers, const unsigned int &maxHiddenNodes, const unsigned int &maxOutputs, ResultsTable &results)
{
	const unsigned long maxFeedForwards = 1E6;
	const unsigned int maxtime = 30;

	while (true)
	{
		cout << "\nEnter topology to test in the format 'INPUT_NODES.HIDDEN_LAYERS.HIDDEN_NODES.OUTPUT_NODES'";
		"\ne.g. '5.2.0.3' for an ANN with 5 inputs, 2 hidden layers, any number of hidden nodes per layer, and 3 outputs";
		cout << "\nEnter 'x' to close.\n";
		string userInput;
		cin >> userInput;
		cin.ignore(32767, '\n');
		if (userInput == "x")
			return;
		else {
			string inputs[4];
			for (int i = 0, period = 0; i < userInput.length() && period < 4; ++i)
			{
				if (userInput[i] == '.') ++period;
				else inputs[period] += userInput[i];
			}
			int inputNodes = stoi(inputs[0]);
			int hiddenLayers = stoi(inputs[1]);
			int hiddenNodes = stoi(inputs[2]);
			int outputNodes = stoi(inputs[3]);
			
			int top;
			(inputNodes == 0) ? top = inputNodes : top = 1;
			if (hiddenLayers == 0) top *= hiddenLayers;
			if (hiddenNodes == 0) top *= hiddenNodes;
			if (outputNodes == 0) top *= outputNodes;
			cout << "\nNumber of topographies to test: " << top << "\nTime to complete test: " << top * 30;

			// WARNING: illegible code, read at own risk
			if (inputNodes == 0) for (int in = 1; in <= maxInputs; ++in) {
				if (hiddenLayers == 0) for (int hl = 1; hl <= maxHiddenLayers; ++hl) {
					if (hiddenNodes == 0) for (int hn = 1; hn <= maxHiddenNodes; ++hn) {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hn << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(in, hn, on, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hl, hn, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hn << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(in, hn, outputNodes, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hl, hn, outputNodes, rate);
						}
					}
					else {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hiddenNodes << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(in, hiddenNodes, on, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hl, hiddenNodes, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hiddenNodes << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(in, hiddenNodes, outputNodes, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hl, hiddenNodes, outputNodes, rate);
						}
					}
				}
				else {
					if (hiddenNodes == 0) for (int hn = 1; hn <= maxHiddenNodes; ++hn) {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hn << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(in, hn, on, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hiddenLayers, hn, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hn << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(in, hn, outputNodes, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hiddenLayers, hn, outputNodes, rate);
						}
					}
					else {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hiddenNodes << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(in, hiddenNodes, on, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hiddenLayers, hiddenNodes, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << in << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hiddenNodes << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(in, hiddenNodes, outputNodes, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(in);

							for (unsigned int i = 0; i < in; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(in, hiddenLayers, hiddenNodes, outputNodes, rate);
						}
					}
				}
			}
			else {
				if (hiddenLayers == 0) for (int hl = 1; hl <= maxHiddenLayers; ++hl) {
					if (hiddenNodes == 0) for (int hn = 1; hn <= maxHiddenNodes; ++hn) {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hn << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hn, on, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hl, hn, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hn << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hn, outputNodes, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hl, hn, outputNodes, rate);
						}
					}
					else {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hiddenNodes << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hiddenNodes, on, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hl, hiddenNodes, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hl; ++i) cout << hiddenNodes << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hiddenNodes, outputNodes, hl);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hl, hiddenNodes, outputNodes, rate);
						}
					}
				}
				else {
					if (hiddenNodes == 0) for (int hn = 1; hn <= maxHiddenNodes; ++hn) {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on)
						{
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hn << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hn, on, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hiddenLayers, hn, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hn << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hn, outputNodes, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hiddenLayers, hn, outputNodes, rate);
						}
					}
					else {
						if (outputNodes == 0) for (int on = 1; on <= maxOutputs; ++on) {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hiddenNodes << "-H, ";
							cout << on << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hiddenNodes, on, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hiddenLayers, hiddenNodes, on, rate);
						}
						else {
							cout << "\n\nStarting neural network test...\nInitiating network of structure " << inputNodes << "-I, ";
							for (unsigned int i = 0; i < hiddenLayers; ++i) cout << hiddenNodes << "-H, ";
							cout << outputNodes << "-O\n";
							NeuralNetwork testNetwork(inputNodes, hiddenNodes, outputNodes, hiddenLayers);
							cout << "Network initialised, running feed-forward operation.\n\n";
							vector<float> inputs;
							inputs.resize(inputNodes);

							for (unsigned int i = 0; i < inputNodes; ++i) {
								inputs[i] = getRandomNumber(-1.0, 1.0);
								// cout << "\nInput " << i << " " << inputs[i];
							}
							// cout << "\nInput and output vectors initialised.\n";
							// testNetwork.printWeights();
							unsigned long feedForwards;
							unsigned long start = time(0);
							for (feedForwards = 1; (feedForwards <= maxFeedForwards) && (time(0) < start + maxtime); ++feedForwards) {
								vector<float> outputs = testNetwork.feedForward(inputs);
								/* cout << "\n\nOutput of feed-forward #" << feedForwards << ":";
								for (int i = 0; i < outputs.size(); ++i) cout << " " << outputs[i]; */
							}
							unsigned long end = time(0);
							unsigned int difference = end - start;
							float rate;
							if (difference == 0) rate = 0;
							else rate = float(feedForwards) / float(difference);
							cout << "Time taken to complete " << feedForwards << " feed-forward operations: " << difference << " seconds" << "\nFeed-forwards per second: " << rate << "\n";
							results.setValue(inputNodes, hiddenLayers, hiddenNodes, outputNodes, rate);
						}
					}
				}
			}
		}
	}
}

void accessDatabase(ResultsTable &results)
{
	while (true) {
		cout << "\nSearch database by entering topology of ANN in format 'INPUT_NODES.HIDDEN_LAYERS.HIDDEN_NODES.OUTPUT_NODES'";
		cout << "\ne.g. '5.2.0.3' for an ANN with 5 inputs, 2 hidden layers, any number of hidden nodes per layer, and 3 outputs";
		cout << "\nEnter 'x' to close.\n";
		string userInput;
		cin >> userInput;
		cin.ignore(32767, '\n');
		if (userInput == "x")
			return;
		else {
			string inputs[4];
			for (int i = 0, period = 0; i < userInput.length() && period < 4; ++i)
			{
				if (userInput[i] == '.') ++period;
				else inputs[period] += userInput[i];
			}
			int inputNodes = stoi(inputs[0]);
			int hiddenLayers = stoi(inputs[1]);
			int hiddenNodes = stoi(inputs[2]);
			int outputNodes = stoi(inputs[3]);
			cout << "\nInput format conversion success.";
			results.print(inputNodes-1, hiddenLayers-1, hiddenNodes-1, outputNodes-1);
		}
	}
}

void saveDatabase(ResultsTable results)
{
	ofstream ofile;
	cout << "\nEnter file name to save database to: ";
	string filename;
	cin >> filename;
	cin.ignore(32767, '\n');
	ofile.open(filename + ".dat");
	string data = results.getData();
	char *datacstring = new char[data.length()];
	for (unsigned int i = 0; i < data.length(); ++i)
	{
		datacstring[i] = data[i];
	}
	ofile.write(datacstring, data.length());
	ofile.close();
	cout << "Data saved to " + filename + ".dat\n\n";
}

void loadDatabase(ResultsTable results)
{
	ifstream ifile;
	cout << "\nEnter file name of database to read: ";
	string filename;
	cin >> filename;
	cin.ignore(32767, '\n');
	ifile.open(filename + ".dat");
	if (!ifile) {
		cerr << "File not found.\n";
		return;
	}
	string datatransfer;
	string data = "";

	while (ifile) {
		getline(ifile, datatransfer);
		data += datatransfer;
	}

}

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	unsigned int inputNodes = 1000;
	unsigned int hiddenLayers = 10;
	unsigned int hiddenNodes = 1000;
	unsigned int outputNodes = 1000;

	ResultsTable results = ResultsTable();

	char userinput;
	
	while (true) {
		cout << "Enter '1' to run test.\nEnter '2' to read database.\nEnter '3' to save database to file.\nEnter '4' to load database from file.\n";
		cin >> userinput;
		cin.ignore(32767, '\n');
		switch (userinput) {
			case '0': return 0;
				break;
			case '1': testFeedForwardSpeed(inputNodes, hiddenLayers, hiddenNodes, outputNodes, results);
				break;
			case '2': accessDatabase(results);
				break;
			case '3': saveDatabase(results);
				break;
			case '4': loadDatabase(results);
				break;
		}
	}


	/*
	ofstream ofile;
	ifstream ifile;
	

	string userInput;
	while (true) {
		
		cout << "Enter 'w' to write to file, 'r' to read file. Enter 'x' to close.\n";
		cin >> userInput;
		cin.ignore(32767, '\n');
		char action = userInput[0];
		char *cstringInput;
		switch (action)
		{
		case 'x': return 0;
		case 'w': cout << "Enter message to write to file: ";
			getline(cin , userInput);
			cout << "Input retrieved\n";
			cout << "Buffer cleared.\n";
			ofile.open("sample.dat");
			cout << "Opened file.\n";
			if (!ofile) {
				cerr << "File not found.\n";
			}
			cstringInput = new char[userInput.length()];
			for (unsigned int i = 0; i < userInput.length(); ++i)
			{
				cstringInput[i] = userInput[i];
			}
			ofile.write(cstringInput, userInput.length());
			ofile.close();
			cout << "File closed.\n";
			break;
		case 'r': "Printing contents of file...\n";
			ifile.open("sample.dat");
			if (!ifile) {
				cerr << "File not found.\n";
			}
			ifile.seekg(0);
			while (ifile) {
				string fileInput;
				getline(ifile, fileInput);
				cout << fileInput << "\n";
			}
			ifile.close();
			break;
		default:
			cout << "Unrecognised input. ";
		}
	}
	*/

}