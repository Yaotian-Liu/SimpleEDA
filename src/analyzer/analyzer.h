/**
 * @file analyzer.h
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#if !defined(ANALYZER_H)
#define ANALYZER_H

#include <armadillo>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../parser/parser.h"
#include "../utils/utils.h"
#include "analyzer_type.h"
#include "qcustomplot.h"

int FindNode(std::vector<NodeName> node_vec, NodeName name);

void DcPlot(DcResult result, std::vector<PrintVariable> print_variable_vec);
void AcPlot(AcResult result, std::vector<PrintVariable> print_variable_vec);
void TranPlot(TranResult result, std::vector<PrintVariable> print_variable_vec);
void Plot(std::vector<QVector<double>> x_vec, std::vector<QVector<double>> y_vec,
          std::vector<NodeName> name_vec, QString x_label, QString y_label, bool x_log,
          bool y_log);

arma::mat AddExpTerm(const std::vector<ExpTerm> exp_term_vec, const arma::vec result,
                     arma::mat mat);

double VecDifference(arma::vec vec_old, arma::vec vec_new);

const double EPSILON_ABS = 1e-5;
const double EPSILON_REL = 1e-1;

class Analyzer {
  public:
    Analyzer() {}
    Analyzer(Parser parser);
    ~Analyzer() {}

    std::vector<AnalysisMatrix> GetAnalysisResults() { return analysis_matrix_vec; }

    void PrintMatrix(arma::cx_mat mat, std::vector<NodeName> nodes);
    void PrintRHS(arma::cx_mat rhs, std::vector<NodeName> nodes);

  private:
    Circuit circuit;
    std::vector<NodeName> modified_node_vec;

    std::vector<AnalysisMatrix> analysis_matrix_vec;

    TranResult tran_result;
    DcResult dc_result;
    AcResult ac_result;

    void DoDcAnalysis(const DcAnalysis dc_analysis);
    void DoAcAnalysis(const AcAnalysis ac_analysis);
    void DoTranAnalysis(const TranAnalysis tran_analysis);

    AnalysisMatrix GetAnalysisMatrix(const double frequency);
};

#endif  // ANALYZER_H
