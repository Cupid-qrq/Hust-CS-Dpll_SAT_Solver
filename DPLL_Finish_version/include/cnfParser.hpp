#pragma once
#include "cnf.h"
#include "definitions.h"

class CnfParser {
public:
  static bool parse(Cnf &cnf, const string &fliename, int &VarsNum,
                    int &ClausesNum) {
    ifstream infile(fliename);
    string line;

    if (!infile.is_open()) {
      cerr << "Error opening file: " << fliename << endl;
      return false;
    } else {
      cout << "Successfully opened file: " << fliename << endl;
    }

    if (!cnf.isEmpty()) {
      cnf.clauses.clear();
    }

    // 逐行读取文件内容
    while (getline(infile, line)) {
      istringstream iss(line);

      // 忽略注释行
      if (line.empty() || line[0] == 'c') {
        // cout << "Comment line: " << line << endl;
        continue;
      }

      // 处理信息行
      if (line[0] == 'p') {
        string tmp;
        int varNum, clauseNum;
        iss >> tmp >> tmp >> varNum >> clauseNum; // 读取变量数和子句数
        VarsNum = varNum;
        ClausesNum = clauseNum;
        // cout << varNum << " variables, " << clauseNum << " clauses." << endl;
        continue;
      }

      // 处理子句行
      Clause clause;
      int var;
      while (iss >> var && var != 0) {
        Literal literal(var);
        clause.addLiteral(literal); // 添加文字到子句
      }
      cnf.addClause(clause); // 添加子句到CNF
      // cout << "Added clause: ";
      // clause.print();
    }

    infile.close();
    return true;
  }
};