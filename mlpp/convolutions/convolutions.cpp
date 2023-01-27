//
//  Convolutions.cpp
//
//  Created by Marc Melikyan on 4/6/21.
//

#include "../convolutions/convolutions.h"
#include "../lin_alg/lin_alg.h"
#include "../stat/stat.h"
#include <cmath>
#include <iostream>

MLPPConvolutions::MLPPConvolutions() :
		prewittHorizontal({ { 1, 1, 1 }, { 0, 0, 0 }, { -1, -1, -1 } }), prewittVertical({ { 1, 0, -1 }, { 1, 0, -1 }, { 1, 0, -1 } }), sobelHorizontal({ { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } }), sobelVertical({ { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } }), scharrHorizontal({ { 3, 10, 3 }, { 0, 0, 0 }, { -3, -10, -3 } }), scharrVertical({ { 3, 0, -3 }, { 10, 0, -10 }, { 3, 0, -3 } }), robertsHorizontal({ { 0, 1 }, { -1, 0 } }), robertsVertical({ { 1, 0 }, { 0, -1 } }) {
}

std::vector<std::vector<real_t>> MLPPConvolutions::convolve(std::vector<std::vector<real_t>> input, std::vector<std::vector<real_t>> filter, int S, int P) {
	MLPPLinAlg alg;
	std::vector<std::vector<real_t>> featureMap;
	int N = input.size();
	int F = filter.size();
	int mapSize = (N - F + 2 * P) / S + 1; // This is computed as ⌊mapSize⌋ by def- thanks C++!

	if (P != 0) {
		std::vector<std::vector<real_t>> paddedInput;
		paddedInput.resize(N + 2 * P);
		for (int i = 0; i < paddedInput.size(); i++) {
			paddedInput[i].resize(N + 2 * P);
		}
		for (int i = 0; i < paddedInput.size(); i++) {
			for (int j = 0; j < paddedInput[i].size(); j++) {
				if (i - P < 0 || j - P < 0 || i - P > input.size() - 1 || j - P > input[0].size() - 1) {
					paddedInput[i][j] = 0;
				} else {
					paddedInput[i][j] = input[i - P][j - P];
				}
			}
		}
		input.resize(paddedInput.size());
		for (int i = 0; i < paddedInput.size(); i++) {
			input[i].resize(paddedInput[i].size());
		}
		input = paddedInput;
	}

	featureMap.resize(mapSize);
	for (int i = 0; i < mapSize; i++) {
		featureMap[i].resize(mapSize);
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			std::vector<real_t> convolvingInput;
			for (int k = 0; k < F; k++) {
				for (int p = 0; p < F; p++) {
					if (i == 0 && j == 0) {
						convolvingInput.push_back(input[i + k][j + p]);
					} else if (i == 0) {
						convolvingInput.push_back(input[i + k][j + (S - 1) + p]);
					} else if (j == 0) {
						convolvingInput.push_back(input[i + (S - 1) + k][j + p]);
					} else {
						convolvingInput.push_back(input[i + (S - 1) + k][j + (S - 1) + p]);
					}
				}
			}
			featureMap[i][j] = alg.dot(convolvingInput, alg.flatten(filter));
		}
	}
	return featureMap;
}

std::vector<std::vector<std::vector<real_t>>> MLPPConvolutions::convolve(std::vector<std::vector<std::vector<real_t>>> input, std::vector<std::vector<std::vector<real_t>>> filter, int S, int P) {
	MLPPLinAlg alg;
	std::vector<std::vector<std::vector<real_t>>> featureMap;
	int N = input[0].size();
	int F = filter[0].size();
	int C = filter.size() / input.size();
	int mapSize = (N - F + 2 * P) / S + 1; // This is computed as ⌊mapSize⌋ by def.

	if (P != 0) {
		for (int c = 0; c < input.size(); c++) {
			std::vector<std::vector<real_t>> paddedInput;
			paddedInput.resize(N + 2 * P);
			for (int i = 0; i < paddedInput.size(); i++) {
				paddedInput[i].resize(N + 2 * P);
			}
			for (int i = 0; i < paddedInput.size(); i++) {
				for (int j = 0; j < paddedInput[i].size(); j++) {
					if (i - P < 0 || j - P < 0 || i - P > input[c].size() - 1 || j - P > input[c][0].size() - 1) {
						paddedInput[i][j] = 0;
					} else {
						paddedInput[i][j] = input[c][i - P][j - P];
					}
				}
			}
			input[c].resize(paddedInput.size());
			for (int i = 0; i < paddedInput.size(); i++) {
				input[c][i].resize(paddedInput[i].size());
			}
			input[c] = paddedInput;
		}
	}

	featureMap.resize(C);
	for (int i = 0; i < featureMap.size(); i++) {
		featureMap[i].resize(mapSize);
		for (int j = 0; j < featureMap[i].size(); j++) {
			featureMap[i][j].resize(mapSize);
		}
	}

	for (int c = 0; c < C; c++) {
		for (int i = 0; i < mapSize; i++) {
			for (int j = 0; j < mapSize; j++) {
				std::vector<real_t> convolvingInput;
				for (int t = 0; t < input.size(); t++) {
					for (int k = 0; k < F; k++) {
						for (int p = 0; p < F; p++) {
							if (i == 0 && j == 0) {
								convolvingInput.push_back(input[t][i + k][j + p]);
							} else if (i == 0) {
								convolvingInput.push_back(input[t][i + k][j + (S - 1) + p]);
							} else if (j == 0) {
								convolvingInput.push_back(input[t][i + (S - 1) + k][j + p]);
							} else {
								convolvingInput.push_back(input[t][i + (S - 1) + k][j + (S - 1) + p]);
							}
						}
					}
				}
				featureMap[c][i][j] = alg.dot(convolvingInput, alg.flatten(filter));
			}
		}
	}
	return featureMap;
}

std::vector<std::vector<real_t>> MLPPConvolutions::pool(std::vector<std::vector<real_t>> input, int F, int S, std::string type) {
	MLPPLinAlg alg;
	std::vector<std::vector<real_t>> pooledMap;
	int N = input.size();
	int mapSize = floor((N - F) / S + 1);

	pooledMap.resize(mapSize);
	for (int i = 0; i < mapSize; i++) {
		pooledMap[i].resize(mapSize);
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			std::vector<real_t> poolingInput;
			for (int k = 0; k < F; k++) {
				for (int p = 0; p < F; p++) {
					if (i == 0 && j == 0) {
						poolingInput.push_back(input[i + k][j + p]);
					} else if (i == 0) {
						poolingInput.push_back(input[i + k][j + (S - 1) + p]);
					} else if (j == 0) {
						poolingInput.push_back(input[i + (S - 1) + k][j + p]);
					} else {
						poolingInput.push_back(input[i + (S - 1) + k][j + (S - 1) + p]);
					}
				}
			}
			if (type == "Average") {
				MLPPStat  stat;
				pooledMap[i][j] = stat.mean(poolingInput);
			} else if (type == "Min") {
				pooledMap[i][j] = alg.min(poolingInput);
			} else {
				pooledMap[i][j] = alg.max(poolingInput);
			}
		}
	}
	return pooledMap;
}

std::vector<std::vector<std::vector<real_t>>> MLPPConvolutions::pool(std::vector<std::vector<std::vector<real_t>>> input, int F, int S, std::string type) {
	std::vector<std::vector<std::vector<real_t>>> pooledMap;
	for (int i = 0; i < input.size(); i++) {
		pooledMap.push_back(pool(input[i], F, S, type));
	}
	return pooledMap;
}

real_t MLPPConvolutions::globalPool(std::vector<std::vector<real_t>> input, std::string type) {
	MLPPLinAlg alg;
	if (type == "Average") {
		MLPPStat  stat;
		return stat.mean(alg.flatten(input));
	} else if (type == "Min") {
		return alg.min(alg.flatten(input));
	} else {
		return alg.max(alg.flatten(input));
	}
}

std::vector<real_t> MLPPConvolutions::globalPool(std::vector<std::vector<std::vector<real_t>>> input, std::string type) {
	std::vector<real_t> pooledMap;
	for (int i = 0; i < input.size(); i++) {
		pooledMap.push_back(globalPool(input[i], type));
	}
	return pooledMap;
}

real_t MLPPConvolutions::gaussian2D(real_t x, real_t y, real_t std) {
	real_t std_sq = std * std;
	return 1 / (2 * M_PI * std_sq) * std::exp(-(x * x + y * y) / 2 * std_sq);
}

std::vector<std::vector<real_t>> MLPPConvolutions::gaussianFilter2D(int size, real_t std) {
	std::vector<std::vector<real_t>> filter;
	filter.resize(size);
	for (int i = 0; i < filter.size(); i++) {
		filter[i].resize(size);
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			filter[i][j] = gaussian2D(i - (size - 1) / 2, (size - 1) / 2 - j, std);
		}
	}
	return filter;
}

/*
Indeed a filter could have been used for this purpose, but I decided that it would've just
been easier to carry out the calculation explicitly, mainly because it is more informative,
and also because my convolution algorithm is only built for filters with equally sized
heights and widths.
*/
std::vector<std::vector<real_t>> MLPPConvolutions::dx(std::vector<std::vector<real_t>> input) {
	std::vector<std::vector<real_t>> deriv; // We assume a gray scale image.
	deriv.resize(input.size());
	for (int i = 0; i < deriv.size(); i++) {
		deriv[i].resize(input[i].size());
	}

	for (int i = 0; i < input.size(); i++) {
		for (int j = 0; j < input[i].size(); j++) {
			if (j != 0 && j != input.size() - 1) {
				deriv[i][j] = input[i][j + 1] - input[i][j - 1];
			} else if (j == 0) {
				deriv[i][j] = input[i][j + 1] - 0; // Implicit zero-padding
			} else {
				deriv[i][j] = 0 - input[i][j - 1]; // Implicit zero-padding
			}
		}
	}
	return deriv;
}

std::vector<std::vector<real_t>> MLPPConvolutions::dy(std::vector<std::vector<real_t>> input) {
	std::vector<std::vector<real_t>> deriv;
	deriv.resize(input.size());
	for (int i = 0; i < deriv.size(); i++) {
		deriv[i].resize(input[i].size());
	}

	for (int i = 0; i < input.size(); i++) {
		for (int j = 0; j < input[i].size(); j++) {
			if (i != 0 && i != input.size() - 1) {
				deriv[i][j] = input[i - 1][j] - input[i + 1][j];
			} else if (i == 0) {
				deriv[i][j] = 0 - input[i + 1][j]; // Implicit zero-padding
			} else {
				deriv[i][j] = input[i - 1][j] - 0; // Implicit zero-padding
			}
		}
	}
	return deriv;
}

std::vector<std::vector<real_t>> MLPPConvolutions::gradMagnitude(std::vector<std::vector<real_t>> input) {
	MLPPLinAlg alg;
	std::vector<std::vector<real_t>> xDeriv_2 = alg.hadamard_product(dx(input), dx(input));
	std::vector<std::vector<real_t>> yDeriv_2 = alg.hadamard_product(dy(input), dy(input));
	return alg.sqrt(alg.addition(xDeriv_2, yDeriv_2));
}

std::vector<std::vector<real_t>> MLPPConvolutions::gradOrientation(std::vector<std::vector<real_t>> input) {
	std::vector<std::vector<real_t>> deriv;
	deriv.resize(input.size());
	for (int i = 0; i < deriv.size(); i++) {
		deriv[i].resize(input[i].size());
	}

	std::vector<std::vector<real_t>> xDeriv = dx(input);
	std::vector<std::vector<real_t>> yDeriv = dy(input);
	for (int i = 0; i < deriv.size(); i++) {
		for (int j = 0; j < deriv[i].size(); j++) {
			deriv[i][j] = std::atan2(yDeriv[i][j], xDeriv[i][j]);
		}
	}
	return deriv;
}

std::vector<std::vector<std::vector<real_t>>> MLPPConvolutions::computeM(std::vector<std::vector<real_t>> input) {
	real_t const SIGMA = 1;
	real_t const GAUSSIAN_SIZE = 3;

	real_t const GAUSSIAN_PADDING = ((input.size() - 1) + GAUSSIAN_SIZE - input.size()) / 2; // Convs must be same.
	std::cout << GAUSSIAN_PADDING << std::endl;
	MLPPLinAlg alg;
	std::vector<std::vector<real_t>> xDeriv = dx(input);
	std::vector<std::vector<real_t>> yDeriv = dy(input);

	std::vector<std::vector<real_t>> gaussianFilter = gaussianFilter2D(GAUSSIAN_SIZE, SIGMA); // Sigma of 1, size of 3.
	std::vector<std::vector<real_t>> xxDeriv = convolve(alg.hadamard_product(xDeriv, xDeriv), gaussianFilter, 1, GAUSSIAN_PADDING);
	std::vector<std::vector<real_t>> yyDeriv = convolve(alg.hadamard_product(yDeriv, yDeriv), gaussianFilter, 1, GAUSSIAN_PADDING);
	std::vector<std::vector<real_t>> xyDeriv = convolve(alg.hadamard_product(xDeriv, yDeriv), gaussianFilter, 1, GAUSSIAN_PADDING);

	std::vector<std::vector<std::vector<real_t>>> M = { xxDeriv, yyDeriv, xyDeriv };
	return M;
}
std::vector<std::vector<std::string>> MLPPConvolutions::harrisCornerDetection(std::vector<std::vector<real_t>> input) {
	real_t const k = 0.05; // Empirically determined wherein k -> [0.04, 0.06], though conventionally 0.05 is typically used as well.
	MLPPLinAlg alg;
	std::vector<std::vector<std::vector<real_t>>> M = computeM(input);
	std::vector<std::vector<real_t>> det = alg.subtraction(alg.hadamard_product(M[0], M[1]), alg.hadamard_product(M[2], M[2]));
	std::vector<std::vector<real_t>> trace = alg.addition(M[0], M[1]);

	// The reason this is not a scalar is because xxDeriv, xyDeriv, yxDeriv, and yyDeriv are not scalars.
	std::vector<std::vector<real_t>> r = alg.subtraction(det, alg.scalarMultiply(k, alg.hadamard_product(trace, trace)));
	std::vector<std::vector<std::string>> imageTypes;
	imageTypes.resize(r.size());
	alg.printMatrix(r);
	for (int i = 0; i < r.size(); i++) {
		imageTypes[i].resize(r[i].size());
		for (int j = 0; j < r[i].size(); j++) {
			if (r[i][j] > 0) {
				imageTypes[i][j] = "C";
			} else if (r[i][j] < 0) {
				imageTypes[i][j] = "E";
			} else {
				imageTypes[i][j] = "N";
			}
		}
	}
	return imageTypes;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getPrewittHorizontal() {
	return prewittHorizontal;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getPrewittVertical() {
	return prewittVertical;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getSobelHorizontal() {
	return sobelHorizontal;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getSobelVertical() {
	return sobelVertical;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getScharrHorizontal() {
	return scharrHorizontal;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getScharrVertical() {
	return scharrVertical;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getRobertsHorizontal() {
	return robertsHorizontal;
}

std::vector<std::vector<real_t>> MLPPConvolutions::getRobertsVertical() {
	return robertsVertical;
}
