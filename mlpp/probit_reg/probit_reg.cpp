//
//  ProbitReg.cpp
//
//  Created by Marc Melikyan on 10/2/20.
//

#include "probit_reg.h"
#include "../activation/activation.h"
#include "../cost/cost.h"
#include "../lin_alg/lin_alg.h"
#include "../regularization/reg.h"
#include "../utilities/utilities.h"

#include <iostream>
#include <random>


MLPPProbitReg::MLPPProbitReg(std::vector<std::vector<double>> inputSet, std::vector<double> outputSet, std::string reg, double lambda, double alpha) :
		inputSet(inputSet), outputSet(outputSet), n(inputSet.size()), k(inputSet[0].size()), reg(reg), lambda(lambda), alpha(alpha) {
	y_hat.resize(n);
	weights = MLPPUtilities::weightInitialization(k);
	bias = MLPPUtilities::biasInitialization();
}

std::vector<double> MLPPProbitReg::modelSetTest(std::vector<std::vector<double>> X) {
	return Evaluate(X);
}

double MLPPProbitReg::modelTest(std::vector<double> x) {
	return Evaluate(x);
}

void MLPPProbitReg::gradientDescent(double learning_rate, int max_epoch, bool UI) {
	MLPPActivation avn;
	MLPPLinAlg alg;
	MLPPReg regularization;
	double cost_prev = 0;
	int epoch = 1;
	forwardPass();

	while (true) {
		cost_prev = Cost(y_hat, outputSet);

		std::vector<double> error = alg.subtraction(y_hat, outputSet);

		// Calculating the weight gradients
		weights = alg.subtraction(weights, alg.scalarMultiply(learning_rate / n, alg.mat_vec_mult(alg.transpose(inputSet), alg.hadamard_product(error, avn.gaussianCDF(z, 1)))));
		weights = regularization.regWeights(weights, lambda, alpha, reg);

		// Calculating the bias gradients
		bias -= learning_rate * alg.sum_elements(alg.hadamard_product(error, avn.gaussianCDF(z, 1))) / n;
		forwardPass();

		if (UI) {
			MLPPUtilities::CostInfo(epoch, cost_prev, Cost(y_hat, outputSet));
			MLPPUtilities::UI(weights, bias);
		}
		epoch++;

		if (epoch > max_epoch) {
			break;
		}
	}
}

void MLPPProbitReg::MLE(double learning_rate, int max_epoch, bool UI) {
	MLPPActivation avn;
	MLPPLinAlg alg;
	MLPPReg regularization;
	double cost_prev = 0;
	int epoch = 1;
	forwardPass();

	while (true) {
		cost_prev = Cost(y_hat, outputSet);

		std::vector<double> error = alg.subtraction(outputSet, y_hat);

		// Calculating the weight gradients
		weights = alg.addition(weights, alg.scalarMultiply(learning_rate / n, alg.mat_vec_mult(alg.transpose(inputSet), alg.hadamard_product(error, avn.gaussianCDF(z, 1)))));
		weights = regularization.regWeights(weights, lambda, alpha, reg);

		// Calculating the bias gradients
		bias += learning_rate * alg.sum_elements(alg.hadamard_product(error, avn.gaussianCDF(z, 1))) / n;
		forwardPass();

		if (UI) {
			MLPPUtilities::CostInfo(epoch, cost_prev, Cost(y_hat, outputSet));
			MLPPUtilities::UI(weights, bias);
		}
		epoch++;

		if (epoch > max_epoch) {
			break;
		}
	}
}

void MLPPProbitReg::SGD(double learning_rate, int max_epoch, bool UI) {
	// NOTE: ∂y_hat/∂z is sparse
	MLPPActivation avn;
	MLPPLinAlg alg;
	MLPPReg regularization;
	double cost_prev = 0;
	int epoch = 1;

	while (true) {
		std::random_device rd;
		std::default_random_engine generator(rd());
		std::uniform_int_distribution<int> distribution(0, int(n - 1));
		int outputIndex = distribution(generator);

		double y_hat = Evaluate(inputSet[outputIndex]);
		double z = propagate(inputSet[outputIndex]);
		cost_prev = Cost({ y_hat }, { outputSet[outputIndex] });

		double error = y_hat - outputSet[outputIndex];

		// Weight Updation
		weights = alg.subtraction(weights, alg.scalarMultiply(learning_rate * error * ((1 / sqrt(2 * M_PI)) * exp(-z * z / 2)), inputSet[outputIndex]));
		weights = regularization.regWeights(weights, lambda, alpha, reg);

		// Bias updation
		bias -= learning_rate * error * ((1 / sqrt(2 * M_PI)) * exp(-z * z / 2));

		y_hat = Evaluate({ inputSet[outputIndex] });

		if (UI) {
			MLPPUtilities::CostInfo(epoch, cost_prev, Cost({ y_hat }, { outputSet[outputIndex] }));
			MLPPUtilities::UI(weights, bias);
		}
		epoch++;

		if (epoch > max_epoch) {
			break;
		}
	}
	forwardPass();
}

void MLPPProbitReg::MBGD(double learning_rate, int max_epoch, int mini_batch_size, bool UI) {
	MLPPActivation avn;
	MLPPLinAlg alg;
	MLPPReg regularization;
	double cost_prev = 0;
	int epoch = 1;

	// Creating the mini-batches
	int n_mini_batch = n / mini_batch_size;
	auto [inputMiniBatches, outputMiniBatches] = MLPPUtilities::createMiniBatches(inputSet, outputSet, n_mini_batch);

	// Creating the mini-batches
	for (int i = 0; i < n_mini_batch; i++) {
		std::vector<std::vector<double>> currentInputSet;
		std::vector<double> currentOutputSet;
		for (int j = 0; j < n / n_mini_batch; j++) {
			currentInputSet.push_back(inputSet[n / n_mini_batch * i + j]);
			currentOutputSet.push_back(outputSet[n / n_mini_batch * i + j]);
		}
		inputMiniBatches.push_back(currentInputSet);
		outputMiniBatches.push_back(currentOutputSet);
	}

	if (double(n) / double(n_mini_batch) - int(n / n_mini_batch) != 0) {
		for (int i = 0; i < n - n / n_mini_batch * n_mini_batch; i++) {
			inputMiniBatches[n_mini_batch - 1].push_back(inputSet[n / n_mini_batch * n_mini_batch + i]);
			outputMiniBatches[n_mini_batch - 1].push_back(outputSet[n / n_mini_batch * n_mini_batch + i]);
		}
	}

	while (true) {
		for (int i = 0; i < n_mini_batch; i++) {
			std::vector<double> y_hat = Evaluate(inputMiniBatches[i]);
			std::vector<double> z = propagate(inputMiniBatches[i]);
			cost_prev = Cost(y_hat, outputMiniBatches[i]);

			std::vector<double> error = alg.subtraction(y_hat, outputMiniBatches[i]);

			// Calculating the weight gradients
			weights = alg.subtraction(weights, alg.scalarMultiply(learning_rate / outputMiniBatches.size(), alg.mat_vec_mult(alg.transpose(inputMiniBatches[i]), alg.hadamard_product(error, avn.gaussianCDF(z, 1)))));
			weights = regularization.regWeights(weights, lambda, alpha, reg);

			// Calculating the bias gradients
			bias -= learning_rate * alg.sum_elements(alg.hadamard_product(error, avn.gaussianCDF(z, 1))) / outputMiniBatches.size();
			y_hat = Evaluate(inputMiniBatches[i]);

			if (UI) {
				MLPPUtilities::CostInfo(epoch, cost_prev, Cost(y_hat, outputMiniBatches[i]));
				MLPPUtilities::UI(weights, bias);
			}
		}
		epoch++;
		if (epoch > max_epoch) {
			break;
		}
	}
	forwardPass();
}

double MLPPProbitReg::score() {
	MLPPUtilities   util;
	return util.performance(y_hat, outputSet);
}

void MLPPProbitReg::save(std::string fileName) {
	MLPPUtilities   util;
	util.saveParameters(fileName, weights, bias);
}

double MLPPProbitReg::Cost(std::vector<double> y_hat, std::vector<double> y) {
	MLPPReg regularization;
	class MLPPCost cost;
	return cost.MSE(y_hat, y) + regularization.regTerm(weights, lambda, alpha, reg);
}

std::vector<double> MLPPProbitReg::Evaluate(std::vector<std::vector<double>> X) {
	MLPPLinAlg alg;
	MLPPActivation avn;
	return avn.gaussianCDF(alg.scalarAdd(bias, alg.mat_vec_mult(X, weights)));
}

std::vector<double> MLPPProbitReg::propagate(std::vector<std::vector<double>> X) {
	MLPPLinAlg alg;
	return alg.scalarAdd(bias, alg.mat_vec_mult(X, weights));
}

double MLPPProbitReg::Evaluate(std::vector<double> x) {
	MLPPLinAlg alg;
	MLPPActivation avn;
	return avn.gaussianCDF(alg.dot(weights, x) + bias);
}

double MLPPProbitReg::propagate(std::vector<double> x) {
	MLPPLinAlg alg;
	return alg.dot(weights, x) + bias;
}

// gaussianCDF ( wTx + b )
void MLPPProbitReg::forwardPass() {
	MLPPLinAlg alg;
	MLPPActivation avn;

	z = propagate(inputSet);
	y_hat = avn.gaussianCDF(z);
}