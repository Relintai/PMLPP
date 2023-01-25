
#ifndef MLPP_MULTINOMIAL_NB_H
#define MLPP_MULTINOMIAL_NB_H

//
//  MultinomialNB.hpp
//
//  Created by Marc Melikyan on 1/17/21.
//

#include <map>
#include <vector>


class MLPPMultinomialNB {
public:
	MLPPMultinomialNB(std::vector<std::vector<double>> inputSet, std::vector<double> outputSet, int class_num);
	std::vector<double> modelSetTest(std::vector<std::vector<double>> X);
	double modelTest(std::vector<double> x);
	double score();

private:
	void computeTheta();
	void Evaluate();

	// Model Params
	std::vector<double> priors;

	std::vector<std::map<double, int>> theta;
	std::vector<double> vocab;
	int class_num;

	// Datasets
	std::vector<std::vector<double>> inputSet;
	std::vector<double> outputSet;
	std::vector<double> y_hat;
};

#endif /* MultinomialNB_hpp */