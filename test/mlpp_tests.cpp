
#include "mlpp_tests.h"

#include "core/math/math_funcs.h"

//TODO remove
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>

#include "../mlpp/activation/activation.h"
#include "../mlpp/ann/ann.h"
#include "../mlpp/auto_encoder/auto_encoder.h"
#include "../mlpp/bernoulli_nb/bernoulli_nb.h"
#include "../mlpp/c_log_log_reg/c_log_log_reg.h"
#include "../mlpp/convolutions/convolutions.h"
#include "../mlpp/cost/cost.h"
#include "../mlpp/data/data.h"
#include "../mlpp/dual_svc/dual_svc.h"
#include "../mlpp/exp_reg/exp_reg.h"
#include "../mlpp/gan/gan.h"
#include "../mlpp/gaussian_nb/gaussian_nb.h"
#include "../mlpp/kmeans/kmeans.h"
#include "../mlpp/knn/knn.h"
#include "../mlpp/lin_alg/lin_alg.h"
#include "../mlpp/lin_reg/lin_reg.h"
#include "../mlpp/log_reg/log_reg.h"
#include "../mlpp/mann/mann.h"
#include "../mlpp/mlp/mlp.h"
#include "../mlpp/multinomial_nb/multinomial_nb.h"
#include "../mlpp/numerical_analysis/numerical_analysis.h"
#include "../mlpp/outlier_finder/outlier_finder.h"
#include "../mlpp/pca/pca.h"
#include "../mlpp/probit_reg/probit_reg.h"
#include "../mlpp/softmax_net/softmax_net.h"
#include "../mlpp/softmax_reg/softmax_reg.h"
#include "../mlpp/stat/stat.h"
#include "../mlpp/svc/svc.h"
#include "../mlpp/tanh_reg/tanh_reg.h"
#include "../mlpp/transforms/transforms.h"
#include "../mlpp/uni_lin_reg/uni_lin_reg.h"
#include "../mlpp/wgan/wgan.h"

Vector<double> dstd_vec_to_vec(const std::vector<double> &in) {
	Vector<double> r;

	r.resize(static_cast<int>(in.size()));
	double *darr = r.ptrw();

	for (uint32_t i = 0; i < in.size(); ++i) {
		darr[i] = in[i];
	}

	return r;
}

Vector<Vector<double>> dstd_mat_to_mat(const std::vector<std::vector<double>> &in) {
	Vector<Vector<double>> r;

	for (uint32_t i = 0; i < in.size(); ++i) {
		r.push_back(dstd_vec_to_vec(in[i]));
	}

	return r;
}

void MLPPTests::test_statistics() {
	ERR_PRINT("MLPPTests::test_statistics() Started!");

	MLPPStat stat;
	MLPPConvolutions conv;

	// STATISTICS
	std::vector<double> x = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector<double> y = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
	std::vector<double> w = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };

	is_approx_equalsd(stat.mean(x), 5.5, "Arithmetic Mean");
	is_approx_equalsd(stat.mean(x), 5.5, "Median");

	is_approx_equals_dvec(dstd_vec_to_vec(x), dstd_vec_to_vec(stat.mode(x)), "stat.mode(x)");

	is_approx_equalsd(stat.range(x), 9, "Range");
	is_approx_equalsd(stat.midrange(x), 4.5, "Midrange");
	is_approx_equalsd(stat.absAvgDeviation(x), 2.5, "Absolute Average Deviation");
	is_approx_equalsd(stat.standardDeviation(x), 3.02765, "Standard Deviation");
	is_approx_equalsd(stat.variance(x), 9.16667, "Variance");
	is_approx_equalsd(stat.covariance(x, y), -9.16667, "Covariance");
	is_approx_equalsd(stat.correlation(x, y), -1, "Correlation");
	is_approx_equalsd(stat.R2(x, y), 1, "R^2");

	// Returns 1 - (1/k^2)
	is_approx_equalsd(stat.chebyshevIneq(2), 0.75, "Chebyshev Inequality");
	is_approx_equalsd(stat.weightedMean(x, w), 5.5, "Weighted Mean");
	is_approx_equalsd(stat.geometricMean(x), 4.52873, "Geometric Mean");
	is_approx_equalsd(stat.harmonicMean(x), 3.41417, "Harmonic Mean");
	is_approx_equalsd(stat.RMS(x), 6.20484, "Root Mean Square (Quadratic mean)");
	is_approx_equalsd(stat.powerMean(x, 5), 7.39281, "Power Mean (p = 5)");
	is_approx_equalsd(stat.lehmerMean(x, 5), 8.71689, "Lehmer Mean (p = 5)");
	is_approx_equalsd(stat.weightedLehmerMean(x, w, 5), 8.71689, "Weighted Lehmer Mean (p = 5)");
	is_approx_equalsd(stat.contraHarmonicMean(x), 7, "Contraharmonic Mean");
	is_approx_equalsd(stat.heronianMean(1, 10), 4.72076, "Hernonian Mean");
	is_approx_equalsd(stat.heinzMean(1, 10, 1), 5.5, "Heinz Mean (x = 1)");
	is_approx_equalsd(stat.neumanSandorMean(1, 10), 3.36061, "Neuman-Sandor Mean");
	is_approx_equalsd(stat.stolarskyMean(1, 10, 5), 6.86587, "Stolarsky Mean (p = 5)");
	is_approx_equalsd(stat.identricMean(1, 10), 4.75135, "Identric Mean");
	is_approx_equalsd(stat.logMean(1, 10), 3.90865, "Logarithmic Mean");
	is_approx_equalsd(stat.absAvgDeviation(x), 2.5, "Absolute Average Deviation");

	ERR_PRINT("MLPPTests::test_statistics() Finished!");
}

void MLPPTests::test_linear_algebra() {
	MLPPLinAlg alg;

	std::vector<std::vector<double>> square = { { 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 } };
	std::vector<std::vector<double>> square_rot_res = { { 1.41421, 1.11022e-16 }, { -1.11022e-16, 1.41421 }, { 1.11022e-16, -1.41421 }, { -1.41421, -1.11022e-16 } };

	is_approx_equals_dmat(dstd_mat_to_mat(alg.rotate(square, M_PI / 4)), dstd_mat_to_mat(square_rot_res), "alg.rotate(square, M_PI / 4)");

	std::vector<std::vector<double>> A = {
		{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
	};
	std::vector<double> a = { 4, 3, 1, 3 };
	std::vector<double> b = { 3, 5, 6, 1 };

	std::vector<std::vector<double>> mmtr_res = {
		{ 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 },
		{ 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 },
		{ 6, 12, 18, 24, 30, 36, 42, 48, 54, 60 },
		{ 8, 16, 24, 32, 40, 48, 56, 64, 72, 80 },
		{ 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
		{ 12, 24, 36, 48, 60, 72, 84, 96, 108, 120 },
		{ 14, 28, 42, 56, 70, 84, 98, 112, 126, 140 },
		{ 16, 32, 48, 64, 80, 96, 112, 128, 144, 160 },
		{ 18, 36, 54, 72, 90, 108, 126, 144, 162, 180 },
		{ 20, 40, 60, 80, 100, 120, 140, 160, 180, 200 }
	};

	is_approx_equals_dmat(dstd_mat_to_mat(alg.matmult(alg.transpose(A), A)), dstd_mat_to_mat(mmtr_res), "alg.matmult(alg.transpose(A), A)");

	is_approx_equalsd(alg.dot(a, b), 36, "alg.dot(a, b)");

	std::vector<std::vector<double>> had_prod_res = {
		{ 1, 4, 9, 16, 25, 36, 49, 64, 81, 100 },
		{ 1, 4, 9, 16, 25, 36, 49, 64, 81, 100 }
	};

	is_approx_equals_dmat(dstd_mat_to_mat(alg.hadamard_product(A, A)), dstd_mat_to_mat(had_prod_res), "alg.hadamard_product(A, A)");

	std::vector<std::vector<double>> id_10_res = {
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	};

	is_approx_equals_dmat(dstd_mat_to_mat(alg.identity(10)), dstd_mat_to_mat(id_10_res), "alg.identity(10)");
}

void MLPPTests::is_approx_equalsd(double a, double b, const String &str) {
	if (!Math::is_equal_approx(a, b)) {
		ERR_PRINT("TEST FAILED: " + str + " Got: " + String::num(a) + " Should be: " + String::num(b));
	}
}

void MLPPTests::is_approx_equals_dvec(const Vector<double> &a, const Vector<double> &b, const String &str) {
	if (a.size() != b.size()) {
		goto IAEDVEC_FAILED;
	}

	for (int i = 0; i < a.size(); ++i) {
		if (!Math::is_equal_approx(a[i], b[i])) {
			goto IAEDVEC_FAILED;
		}
	}

	return;

IAEDVEC_FAILED:

	String fail_str = "TEST FAILED: ";
	fail_str += str;
	fail_str += " Got: [ ";

	for (int i = 0; i < a.size(); ++i) {
		fail_str += String::num(a[i]);
		fail_str += " ";
	}

	fail_str += "] Should be: [ ";

	for (int i = 0; i < b.size(); ++i) {
		fail_str += String::num(b[i]);
		fail_str += " ";
	}

	fail_str += "].";

	ERR_PRINT(fail_str);
}

String vmat_to_str(const Vector<Vector<double>> &a) {
	String str;

	str += "[ \n";

	for (int i = 0; i < a.size(); ++i) {
		str += "  [ ";

		const Vector<double> &aa = a[i];

		for (int j = 0; j < aa.size(); ++j) {
			str += String::num(aa[j]);
			str += " ";
		}

		str += "]\n";
	}

	str += "]\n";

	return str;
}

void MLPPTests::is_approx_equals_dmat(const Vector<Vector<double>> &a, const Vector<Vector<double>> &b, const String &str) {
	if (a.size() != b.size()) {
		goto IAEDMAT_FAILED;
	}

	for (int i = 0; i < a.size(); ++i) {
		const Vector<double> &aa = a[i];
		const Vector<double> &bb = b[i];

		if (aa.size() != bb.size()) {
			goto IAEDMAT_FAILED;
		}

		for (int j = 0; j < aa.size(); ++j) {
			if (!Math::is_equal_approx(aa[j], bb[j])) {
				goto IAEDMAT_FAILED;
			}
		}
	}

	return;

IAEDMAT_FAILED:

	String fail_str = "TEST FAILED: ";
	fail_str += str;
	fail_str += "\nGot:\n";
	fail_str += vmat_to_str(a);
	fail_str += "Should be:\n";
	fail_str += vmat_to_str(b);

	ERR_PRINT(fail_str);
}

MLPPTests::MLPPTests() {
}

MLPPTests::~MLPPTests() {
}

void MLPPTests::_bind_methods() {
	ClassDB::bind_method(D_METHOD("test_statistics"), &MLPPTests::test_statistics);
	ClassDB::bind_method(D_METHOD("test_linear_algebra"), &MLPPTests::test_linear_algebra);
}
