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

void DcPlot(DcResult result, PrintVariable print_variable);

Analyzer::Analyzer(Parser parser) {
    circuit = parser.GetCircuit();

    auto analysis_type = parser.GetAnalysisType();
    auto dc_analysis = parser.GetDcAnalysis();
    auto ac_analysis = parser.GetAcAnalysis();
    auto tran_analysis = parser.GetTranAnalysis();
    auto print_variable = parser.GetPrintVariable();

    switch (analysis_type) {
        case DC: {
            cout << "Running DC analysis" << endl;
            DoDcAnalysis(dc_analysis);
            if (print_variable.print_type)
                DcPlot(dc_result, print_variable);
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

int FindNode(vector<NodeName> node_vec, NodeName name) {
    for (std::size_t i = 0; i < node_vec.size(); i++) {
        if (node_vec[i] == name) {
            return i;
        }
    }
    cout << "Not found: " << name << endl;
    return -1;
}

void DcPlot(DcResult result, PrintVariable print_variable) {
    NodeName node = print_variable.node;
    int node_index = FindNode(result.node_vec, node);
    QVector<double> x;
    for (auto dc_value : result.dc_value_vec)
        x.push_back(dc_value);

    QVector<double> y;
    for (auto dc_result : result.dc_result_vec)
        y.push_back(dc_result(node_index));

    QCustomPlot* plot = new QCustomPlot();
    plot->addGraph(plot->xAxis, plot->yAxis);
    plot->graph()->setPen(QPen(Qt::blue));
    plot->graph()->setLineStyle(QCPGraph::lsLine);
    plot->graph()->setData(x, y);
    plot->graph()->rescaleAxes();

    plot->xAxis->setLabel(QString("Vsrc"));
    plot->yAxis->setLabel(QString("Value"));

    plot->setMinimumSize(450, 300);
    plot->show();
}