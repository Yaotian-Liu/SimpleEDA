/**
 * @file analyzer_utils.cpp
 * @author Yaotian Liu
 * @brief utils for analyzer
 * @date 2022-10-23
 */

#include "analyzer.h"

using arma::cx_mat;
using std::complex;
using std::cout;
using std::endl;
using std::setw;
using std::vector;

Analyzer::Analyzer(Parser parser) {
    vsrc_vec = parser.GetVsrc();
    isrc_vec = parser.GetIsrc();
    vccs_vec = parser.GetVCCS();
    vcvs_vec = parser.GetVCVS();
    res_vec = parser.GetResistor();
    cap_vec = parser.GetCapacitor();
    ind_vec = parser.GetInductor();
    node_vec = parser.GetNode();

    auto analysis_type = parser.GetAnalysisType();
    auto dc_analysis = parser.GetDcAnalysis();
    auto ac_analysis = parser.GetAcAnalysis();
    auto tran_analysis = parser.GetTranAnalysis();

    switch (analysis_type) {
        case DC: {
            cout << "Running DC analysis" << endl;
            DoDcAnalysis(dc_analysis);
            break;
        }
        case AC: {
            cout << "Running AC analysis" << endl;
            DoAcAnalysis(ac_analysis);
            break;
        }
        case TRAN: {
            cout << "Running TRAN analysis" << endl;
            DoTranAnalysis(tran_analysis);
            break;
        }
        default: break;
    }
}

int Analyzer::FindNode(vector<NodeName> node_vec, NodeName name) {
    for (std::size_t i = 0; i < node_vec.size(); i++) {
        if (node_vec[i] == name) {
            return i;
        }
    }
    cout << "Not found: " << name << endl;
    return -1;
}

void Analyzer::PrintMatrix(cx_mat mat, vector<NodeName> nodes) {
    cout << "Matrix: " << endl << ' ';
    for (auto node : nodes) {
        cout << setw(18) << node << "\t";
    }
    cout << endl;
    for (uint r = 0; r < mat.n_rows; r++) {
        cout << setw(4) << nodes[r] << "\t";
        if (r == 0)
            cout << "╭";
        else if (r == mat.n_rows - 1)
            cout << "╰";
        else
            cout << "│";
        for (uint c = 0; c < mat.n_cols; c++) {
            cout << " " << setw(7) << std::real(mat(r, c)) << " + "
                 << "j"
                 << "(" << setw(7) << std::imag(mat(r, c)) << ")"
                 << "\t";
        }
        if (r == 0)
            cout << "╮";
        else if (r == mat.n_rows - 1)
            cout << "╯";
        else
            cout << "│";
        cout << endl;
    }
    cout << endl;
}

void Analyzer::PrintRHS(cx_mat rhs, vector<NodeName> nodes) {
    cout << "RHS: " << endl;
    // cout << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    for (uint i = 0; i < rhs.n_rows; i++) {
        cout << setw(4) << nodes[i] << "\t";
        if (i == 0)
            cout << "╭";
        else if (i == rhs.n_rows - 1)
            cout << "╰";
        else
            cout << "│";

        cout << setw(8) << std::real(rhs(i, 0)) << " ";

        if (i == 0)
            cout << "╮" << endl;
        else if (i == rhs.n_rows - 1)
            cout << "╯" << endl;
        else
            cout << "│" << endl;
    }
}