/**
 * @file Parser.cpp
 * @author Yaotian Liu
 * @brief Parser implementation
 * @date 2022-10-11
 */

#include "parser.h"

#include "../utils/utils.h"

using std::cout;
using std::endl;

const int MAGIC = 407000002;

Parser::Parser() {
    command_op = false;
    command_end = false;
    analysis_type = NONE;
}

Parser::Parser(QTextEdit* output) {
    command_op = false;
    command_end = false;
    analysis_type = NONE;
    this->output = output;
}

Parser::~Parser() {}

void Parser::DeviceParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    DeviceName device_name = elements[0];

    // Process Voltage Source
    // TODO: Update Vsrc grammer
    if (line.startsWith("v")) {
        if (CheckNameRepetition<Vsrc>(circuit.vsrc_vec, device_name)) {
            ParseError("which already exits.", device_name, lineNum);
            return;
        }
        AnalysisType analysis_type = DC;  // dc by default
        switch (num_elements) {
                // Vx 1 0 10
            case 4: {
                double value = ParseValue(elements[3]);
                NodeName node_1 = ReadNodeName(elements[1]);
                NodeName node_2 = ReadNodeName(elements[2]);

                circuit.vsrc_vec.push_back(
                    Vsrc(device_name, analysis_type, value, node_1, node_2));

                output->append(QString("Parsed Device Type: Voltage Source (Name: ") +
                               device_name +
                               QString("; Value: " + QString::number(value, 'f', 3)) +
                               QString("; Node1: ") + node_1 + QString("; Node2: ") +
                               node_2 + QString(")"));

                cout << "Parsed Device Type: Voltage Source ("
                     << "Name: " << device_name << "; "
                     << "Value: " << value << "; "
                     << "Node1: " << node_1 << "; "
                     << "Node2: " << node_2 << " )" << endl;

                break;
            }
                // Vx 1 0 dc 10
                // TODO: Wrong need to be fixed
            case 5: {
                double value = ParseValue(elements[4]);
                NodeName node_1 = ReadNodeName(elements[1]);
                NodeName node_2 = ReadNodeName(elements[2]);
                if (elements[3] == "dc")
                    analysis_type = DC;
                else if (elements[3] == "ac") {
                    analysis_type = AC;
                    ac_analysis.Vsrc_name = device_name;
                } else
                    ParseError("", device_name, lineNum);

                circuit.vsrc_vec.push_back(
                    Vsrc(device_name, analysis_type, value, node_1, node_2));

                output->append(QString("Parsed Device Type: Voltage Source (Name: ") +
                               device_name +
                               QString("; Value: " + QString::number(value, 'f', 3)) +
                               QString("; Node1: ") + node_1 + QString("; Node2: ") +
                               node_2 + QString("; Type: ") +
                               qstr(AnalysisType_lookup[analysis_type]) + QString(")"));

                cout << "Parsed Device Type: Voltage Source ("
                     << "Name: " << device_name << "; "
                     << "Value: " << value << "; "
                     << "Node1: " << node_1 << "; "
                     << "Node2: " << node_2 << "; "
                     << "Type: " << AnalysisType_lookup[analysis_type] << " )" << endl;
                break;
            }

            default: {
                // V1 2 0 PULSE 0 1 1 1 1 1 10
                if (elements[3] == "pulse") {
                    Pulse pulse;
                    NodeName node_1 = ReadNodeName(elements[1]);
                    NodeName node_2 = ReadNodeName(elements[2]);
                    pulse.chosen = true;
                    pulse.v1 = ParseValue(elements[4]);
                    pulse.v2 = ParseValue(elements[5]);
                    pulse.td = ParseValue(elements[6]);
                    pulse.tr = ParseValue(elements[7]);
                    pulse.tf = ParseValue(elements[8]);
                    pulse.pw = ParseValue(elements[9]);
                    pulse.per = ParseValue(elements[10]);

                    circuit.vsrc_vec.push_back(Vsrc(device_name, node_1, node_2, pulse));

                    // output->append(QString("Parsed Device Type: Voltage Source (Name:
                    // ") +
                    //                device_name +
                    //                QString("; Node1: ") + node_1 + QString("; Node2: ")
                    //                + node_2 + QString("; Type: pulse") + QString(")"));

                    cout << "Parsed Device Type: Voltage Source ("
                         << "Name: " << device_name << "; "
                         << "Node1: " << node_1 << "; "
                         << "Node2: " << node_2 << "; "
                         << "Type: pulse; "
                         << "V1: " << pulse.v1 << "; "
                         << "V2: " << pulse.v2 << "; "
                         << "TD: " << pulse.td << "; "
                         << "TR: " << pulse.tr << "; "
                         << "TF: " << pulse.tf << "; "
                         << "PW: " << pulse.pw << "; "
                         << "PER: " << pulse.per << " )" << endl;
                }

                // V1 1 0 TRAN sin (0 1 1 0 0)
                if (elements[4] == "sin") {
                    Sin sin;
                    NodeName node_1 = ReadNodeName(elements[1]);
                    NodeName node_2 = ReadNodeName(elements[2]);
                    sin.chosen = true;
                    sin.v0 = ParseValue(elements[5]);
                    sin.va = ParseValue(elements[6]);
                    sin.freq = ParseValue(elements[7]);
                    sin.td = ParseValue(elements[8]);
                    sin.theta = ParseValue(elements[9]);
                    circuit.vsrc_vec.push_back(Vsrc(device_name, node_1, node_2, sin));

                    cout << "Parsed Device Type: Voltage Source ("
                         << "Name: " << device_name << "; "
                         << "Node1: " << node_1 << "; "
                         << "Node2: " << node_2 << "; "
                         << "Type: pulse; "
                         << "V0: " << sin.v0 << "; "
                         << "VA: " << sin.va << "; "
                         << "FREQ: " << sin.freq << "; "
                         << "TD: " << sin.td << "; "
                         << "THETA: " << sin.theta << " )" << endl;
                }

                break;
            }
        }
    }

    // Process Current source
    // TODO: Update Isrc grammer
    else if (line.startsWith("i")) {
        if (CheckNameRepetition(circuit.isrc_vec, device_name)) {
            ParseError("which already exits.", device_name, lineNum);
            return;
        }

        NodeName node_1 = ReadNodeName(elements[1]);
        NodeName node_2 = ReadNodeName(elements[2]);

        double dc_value = 0;
        double ac_value = 0;
        double tran_const_value = 0;

        if (num_elements < 4)
            ParseError("", device_name, lineNum);

        // If the third is a number, it's dc_value.
        if (ParseValue(elements[3]) != MAGIC) {
            dc_value = ParseValue(elements[3]);
            // More elements
            if (num_elements >= 5) {
                // const(1)
                if (elements[4].startsWith("const")) {
                    tran_const_value = ParseValue(elements[4]);
                }
                // the forth is ac_value
                else if (ParseValue(elements[4]) != MAGIC) {
                    ac_value = ParseValue(elements[4]);

                    // More elements
                    if (num_elements >= 6) {
                        if (elements[5] == "tran") {
                            if (num_elements >= 7) {
                                if (elements[6].startsWith("const")) {
                                    tran_const_value = ParseValue(elements[6]);
                                }
                            }
                        }
                    }

                } else
                    ParseError("", device_name, lineNum);
            }
        }

        circuit.isrc_vec.push_back(
            Isrc(device_name, dc_value, node_1, node_2, ac_value, tran_const_value));

        cout << "Parsed Device Type: Current Source ("
             << "Name: " << device_name << "; "
             << "Node1: " << node_1 << "; "
             << "Node2: " << node_2 << "; "
             << "DC_Value: " << dc_value << "; "
             << "AC_Value: " << ac_value << "; "
             << "Tran const value: " << tran_const_value << " )" << endl;
    }

    // Process Resistor
    else if (line.startsWith("r")) {
        if (num_elements != 4) {
            ParseError("", device_name, lineNum);
        } else {
            if (CheckNameRepetition<Res>(circuit.res_vec, device_name)) {
                ParseError("which already exits.", device_name, lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            circuit.res_vec.push_back(Res(device_name, value, node_1, node_2));

            output->append(QString("Parsed Device Type: Register (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString(")"));

            cout << "Parsed Device Type: Resistor ("
                 << "Name: " << device_name << "; "
                 << "Value: " << value << "; "
                 << "Node1: " << node_1 << "; "
                 << "Node2: " << node_2 << " )" << endl;
        }
    }

    // Process Capacitor
    else if (line.startsWith("c")) {
        if (num_elements != 4) {
            ParseError("", device_name, lineNum);
        } else {
            if (CheckNameRepetition<Cap>(circuit.cap_vec, device_name)) {
                ParseError("which already exits.", device_name, lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            circuit.cap_vec.push_back(Cap(device_name, value, node_1, node_2));

            output->append(QString("Parsed Device Type: Capacitor (Name: ") +
                           device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString(")"));

            cout << "Parsed Device Type: Capacitor ("
                 << "Name: " << device_name << "; "
                 << "Value: " << value << "; "
                 << "Node1: " << node_1 << "; "
                 << "Node2: " << node_2 << " )" << endl;
        }
    }

    // Process Inductor
    else if (line.startsWith("l")) {
        if (num_elements != 4) {
            ParseError("", device_name, lineNum);
        } else {
            if (CheckNameRepetition<Ind>(circuit.ind_vec, device_name)) {
                ParseError("which already exits.", device_name, lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            circuit.ind_vec.push_back(Ind(device_name, value, node_1, node_2));

            output->append(QString("Parsed Device Type: Inductor (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString(")"));

            cout << "Parsed Device Type: Inductor ("
                 << "Name: " << device_name << "; "
                 << "Value: " << value << "; "
                 << "Node1: " << node_1 << "; "
                 << "Node2: " << node_2 << " )" << endl;
        }
    }

    // Process VCCS
    else if (line.startsWith("g")) {
        if (num_elements != 6) {
            ParseError("", device_name, lineNum);
        } else {
            if (CheckNameRepetition<VCCS>(circuit.vccs_vec, device_name)) {
                ParseError("which already exits.", device_name, lineNum);
                return;
            }

            double value = ParseValue(elements[5]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            NodeName ctrl_node_1 = ReadNodeName(elements[3]);
            NodeName ctrl_node_2 = ReadNodeName(elements[4]);
            circuit.vccs_vec.push_back(
                VCCS(device_name, value, node_1, node_2, ctrl_node_1, ctrl_node_2));

            output->append(QString("Parsed Device Type: VCCS (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString("; CtrlNode1: ") + ctrl_node_1 +
                           QString("; CtrlNode2: ") + ctrl_node_2 + QString(")"));

            cout << "Parsed Device Type: VCCS ("
                 << "Name: " << device_name << "; "
                 << "Value: " << value << "; "
                 << "Node1: " << node_1 << "; "
                 << "Node2: " << node_2 << "; "
                 << "CtrlNode1: " << ctrl_node_1 << "; "
                 << "CtrlNode2: " << ctrl_node_2 << " ) " << endl;
        }
    }

    // Process VCVS
    else if (line.startsWith("e")) {
        if (num_elements != 6) {
            ParseError("", device_name, lineNum);
        } else {
            if (CheckNameRepetition<VCVS>(circuit.vcvs_vec, device_name)) {
                ParseError("which already exits.", device_name, lineNum);
                return;
            }

            double value = ParseValue(elements[5]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            NodeName ctrl_node_1 = ReadNodeName(elements[3]);
            NodeName ctrl_node_2 = ReadNodeName(elements[4]);
            circuit.vcvs_vec.push_back(
                VCVS(device_name, value, node_1, node_2, ctrl_node_1, ctrl_node_2));

            output->append(QString("Parsed Device Type: VCVS (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString("; CtrlNode1: ") + ctrl_node_1 +
                           QString("; CtrlNode2: ") + ctrl_node_2 + QString(")"));

            cout << "Parsed Device Type: VCVS ("
                 << "Name: " << device_name << "; "
                 << "Value: " << value << "; "
                 << "Node1: " << node_1 << "; "
                 << "Node2: " << node_2 << "; "
                 << "CtrlNode1: " << ctrl_node_1 << "; "
                 << "CtrlNode2: " << ctrl_node_2 << " ) " << endl;
        }
    }

    // Diode
    else if (line.startsWith("d")) {
        if (num_elements != 4)
            ParseError("parameter error", device_name, lineNum);
        else {
            if (CheckNameRepetition(circuit.diode_vec, device_name)) {
                ParseError("already exits", device_name, lineNum);
                return;
            }

            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            ModelName model = elements[3];

            bool known_model = false;
            for (auto d_model : diode_model_lut) {
                if (d_model.model == model) {
                    known_model = true;
                    break;
                }
            }

            if (!known_model) {
                ParseError("unknown model", device_name, lineNum);
                return;
            }

            circuit.diode_vec.push_back(Diode(device_name, node_1, node_2, model));

            output->append(QString("Parsed Device Type: Diode (Name: ") + device_name +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString("; Model: ") + model + QString(")"));

            cout << "Parsed Device Type: Diode (Name: " << device_name
                 << "; Node1: " << node_1 << "; Node2:" << node_2 << "; Model: " << model
                 << ')' << endl;
        }
    }
}

/**
 * @brief Parser for command
 *
 * @param line a const QString
 * @param lineNum
 */
void Parser::CommandParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    QString command = elements[0];

    // .OP
    if (command == ".op") {
        if (num_elements != 1)
            ParseError("", ".op", lineNum);
        else {
            command_op = true;
            analysis_type = DC;
            cout << "Parsed Analysis Command .OP Token" << endl;
        }
    }
    // .END
    else if (command == ".end") {
        if (num_elements != 1)
            ParseError("", ".end", lineNum);
        else {
            command_end = true;
            cout << "Parsed .END Token" << endl;
            UpdateNodeVec();  // Program ends, update Node.
        }
    }
    // .PRINT / .PLOT
    else if (command == ".print" || command == ".plot") {
        if (num_elements == 1)
            ParseError("need parameters", command, lineNum);
        else if (elements[1].startsWith("i") ||
                 elements[1].startsWith("v")) {  // .print V(node)
            if (analysis_type == NONE)
                ParseError(" Analysis type is not determined.", ".print", lineNum);
            else {
                elements.removeFirst();  // remove the first one
                PrintCommandParser(elements);
            }
        }
        // .print / .plot dc V(..)
        else {
            if (elements[1] == "dc" && analysis_type == DC)
                print_type = DC;
            else if (elements[1] == "ac" && analysis_type == AC)
                print_type = AC;
            else if (elements[1] == "tran" && analysis_type == TRAN)
                print_type = TRAN;
            else {
                ParseError("invalid analysis type.", ".print", lineNum);
                return;
            }

            elements.removeFirst();
            elements.removeFirst();  // remove the first two
            PrintCommandParser(elements);
        }
    }
    // TODO: complete the logic
    else if (command == ".dc") {
        if (num_elements != 5)
            ParseError("", ".dc", lineNum);
        else {
            analysis_type = DC;
            DeviceName vsrc_name = elements[1];
            if (!CheckNameRepetition<Vsrc>(circuit.vsrc_vec, vsrc_name))
                ParseError("target voltage source not exists", ".dc", lineNum);
            else {
                dc_analysis.Vsrc_name = vsrc_name;
                dc_analysis.start = ParseValue(elements[2]);
                dc_analysis.end = ParseValue(elements[3]);
                dc_analysis.step = ParseValue(elements[4]);

                cout << "Parsed Analysis Command DC "
                     << "(Vsrc: " << dc_analysis.Vsrc_name << "; "
                     << "Start: " << dc_analysis.start << "; "
                     << "End: " << dc_analysis.end << "; "
                     << "Step: " << dc_analysis.step << ")" << endl;
            }
        }
    }

    // .ac
    else if (command == ".ac") {
        if (num_elements != 5)
            ParseError("", ".ac", lineNum);
        else {
            analysis_type = AC;
            AcVariationType variation_type = LIN;
            for (uint i = 0; i < AcVariationType_lookup.size(); i++) {
                if (elements[1] == qstr(AcVariationType_lookup[i])) {
                    variation_type = static_cast<AcVariationType>(i);
                    break;
                }
            }
            ac_analysis.variation_type = variation_type;
            ac_analysis.point_num = ParseValue(elements[2]);
            ac_analysis.f_start = ParseValue(elements[3]);
            ac_analysis.f_end = ParseValue(elements[4]);

            cout << "Parsed Analysis Command AC "
                 << "(Variation Type: "
                 << AcVariationType_lookup[ac_analysis.variation_type] << "; "
                 << "Points: " << ac_analysis.point_num << "; "
                 << "f_Start: " << ac_analysis.f_start << "; "
                 << "f_End: " << ac_analysis.f_end << ")" << endl;
        }
    }

    // .tran
    else if (command == ".tran") {
        switch (num_elements) {
            // .tran tstep tstop
            case 3: {
                analysis_type = TRAN;
                double t_step = ParseValue(elements[1]);
                double t_stop = ParseValue(elements[2]);
                double t_start = 0;  // default value
                tran_analysis = {t_step, t_stop, t_start};
                break;
            }
            // .tran tstep tstop tstart
            case 4: {
                analysis_type = TRAN;
                double t_step = ParseValue(elements[1]);
                double t_stop = ParseValue(elements[2]);
                double t_start = ParseValue(elements[3]);
                tran_analysis = {t_step, t_stop, t_start};
                break;
            }

            default: {
                ParseError("", ".tran", lineNum);
                return;
            }
        }
        cout << "Parsed Analysis Command TRAN "
             << "(Tstep: " << tran_analysis.t_step << "; tstop: " << tran_analysis.t_stop
             << "; tstart: " << tran_analysis.t_start << " )" << endl;
    }
}

// TODO: This method is far from complete.
void Parser::PrintCommandParser(const QStringList elements) {
    NodeName node;

    QRegularExpression bracket_re("(?<=\\().*(?=\\))");
    QRegularExpression analysis_variable_type_re("(?<=[v]).+?(?=\\()");

    for (QString e : elements) {
        if (e.startsWith("v")) {
            PrintVariable print_variable;
            print_variable.print_i_v = V;

            QRegularExpressionMatch match_node = bracket_re.match(e);
            if (match_node.hasMatch())
                print_variable.node = match_node.captured();

            QRegularExpressionMatch match_type = analysis_variable_type_re.match(e);
            if (match_type.hasMatch()) {
                QString type = match_type.captured();
                if (type == "r")
                    print_variable.analysis_variable_type = REAL;
                else if (type == "i")
                    print_variable.analysis_variable_type = IMAGINE;
                else if (type == "m")
                    print_variable.analysis_variable_type = MAG;
                else if (type == "p")
                    print_variable.analysis_variable_type = PHASE;
                else if (type == "db")
                    print_variable.analysis_variable_type = DB;
            } else {
                print_variable.analysis_variable_type = MAG;  // magnitude by default
            }

            cout << "Parsed Output Comand Print (Type: "
                 << AnalysisType_lookup[analysis_type] << "; "
                 << "Type: " << PrintIV_lookup[print_variable.print_i_v] << "; ";
            if (print_type == AC)
                cout << "Analysis variable: "
                     << AnalysisVariableT_lookup[print_variable.analysis_variable_type]
                     << "; ";
            cout << "Node: " << print_variable.node << ")" << endl;

            print_variable_vec.push_back(print_variable);
        }
    }
}

/**
 * @brief To parse the value correctly
 *
 * @param value_in_str
 * @return double
 */
double Parser::ParseValue(const QString value_in_str) {
    double value = MAGIC;  // If the return is MAGIC, means the value parse failed.

    QRegExp number_with_e("\\d+(\\.\\d+)?e-?\\d+");
    if (number_with_e.exactMatch(value_in_str)) {
        value = value_in_str.toDouble();
        return value;
    }

    QRegExp number("\\d+(\\.\\d+)?");
    int pos = number.indexIn(value_in_str);
    if (pos > -1)
        value = number.cap().toDouble();

    if (value_in_str.endsWith("db")) {
        value = 20 * log10(value);
        return value;
    }

    ScaledUnit f = {"f", 1e-15};
    ScaledUnit p = {"p", 1e-12};
    ScaledUnit n = {"n", 1e-9};
    ScaledUnit u = {"u", 1e-6};
    ScaledUnit m = {"m", 1e-3};
    ScaledUnit k = {"k", 1e3};
    ScaledUnit meg = {"meg", 1e6};
    ScaledUnit g = {"g", 1e9};
    ScaledUnit t = {"t", 1e12};

    std::vector<ScaledUnit> scaled_unit_vec = {f, p, n, u, m, k, meg, g, t};

    for (auto scaled_unit : scaled_unit_vec) {
        if (value_in_str.endsWith(scaled_unit.unit)) {
            value *= scaled_unit.scaledValue;
            break;
        }
    }
    return value;
}

/**
 * @brief Print an error message.
 *
 * @param error_msg
 * @param lineNum
 */
void Parser::ParseError(const QString error_msg, const QString name, const int lineNum) {
    cout << "Error: line " << lineNum << ": "
         << "failed to parse " << name << ", " << error_msg << endl;
}

/**
 * @brief Read node name from QString. \
 * If node is 'gnd', convert it to '0'
 *
 * @param qstrName
 * @return NodeName
 */
NodeName Parser::ReadNodeName(const QString qstr_name) {
    return (qstr_name == "gnd") ? QString("0") : qstr_name;
}

/**
 * @brief Update and sort node_vec
 */
void Parser::UpdateNodeVec() {
    std::vector<NodeName> temp_node_vec;

    for (auto Vsrc : circuit.vsrc_vec) {
        temp_node_vec.push_back(Vsrc.node_1);
        temp_node_vec.push_back(Vsrc.node_2);
    }

    for (auto Isrc : circuit.isrc_vec) {
        temp_node_vec.push_back(Isrc.node_1);
        temp_node_vec.push_back(Isrc.node_2);
    }

    for (auto vccs : circuit.vccs_vec) {
        temp_node_vec.push_back(vccs.node_1);
        temp_node_vec.push_back(vccs.node_2);
        temp_node_vec.push_back(vccs.ctrl_node_1);
        temp_node_vec.push_back(vccs.ctrl_node_2);
    }

    for (auto vcvs : circuit.vcvs_vec) {
        temp_node_vec.push_back(vcvs.node_1);
        temp_node_vec.push_back(vcvs.node_2);
        temp_node_vec.push_back(vcvs.ctrl_node_1);
        temp_node_vec.push_back(vcvs.ctrl_node_2);
    }

    for (auto Res : circuit.res_vec) {
        temp_node_vec.push_back(Res.node_1);
        temp_node_vec.push_back(Res.node_2);
    }

    for (auto Cap : circuit.cap_vec) {
        temp_node_vec.push_back(Cap.node_1);
        temp_node_vec.push_back(Cap.node_2);
    }

    for (auto Ind : circuit.ind_vec) {
        temp_node_vec.push_back(Ind.node_1);
        temp_node_vec.push_back(Ind.node_2);
    }

    for (auto Diode : circuit.diode_vec) {
        temp_node_vec.push_back(Diode.node_1);
        temp_node_vec.push_back(Diode.node_2);
    }

    std::set<NodeName> node_set(temp_node_vec.begin(), temp_node_vec.end());

    circuit.node_vec = std::vector<NodeName>(node_set.begin(), node_set.end());
    std::sort(circuit.node_vec.begin(), circuit.node_vec.end());
}

/**
 * @brief Check whether the name has existed
 *
 * @tparam T the type of the struct
 * @param struct_vec
 * @param name
 * @return true: Found \
 * @return false: Not found
 */
template <typename T>
bool Parser::CheckNameRepetition(std::vector<T> struct_vec, DeviceName name) {
    for (auto s : struct_vec)
        if (s.name == name) {
            return true;
        }
    return false;
}

bool Parser::CheckGndNode() {
    for (auto node : circuit.node_vec) {
        if (node == "0")
            return true;
    }
    return false;
}

// TODO: Need more checks
/**
 * @brief Parser check
 *
 * @return true : Check pass \
 * @return false : Not OK
 */
bool Parser::ParserFinalCheck() {
    if (command_end)
        return CheckGndNode();
    else
        return false;
}