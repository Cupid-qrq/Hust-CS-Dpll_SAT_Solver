#pragma once

#include "cnfparser.hpp"
#include "definitions.h"

// 实现DPLL算法中的传播递归函数
bool propagate(Cnf &cnf, const Literal &literal, int *assignments);

// 实现dpll算法中每次选择文字来赋值的策略
Literal selectLiteral(const Cnf &cnf, int strategy);

// DPLL求解主函数
bool dpll(Cnf &cnf, int *assignments, int strategy, bool &timeout,
          double timelimit, clock_t start);

// 打印最终结果
void printResult(const int *assignments, int choice);

// 保存结果到文件
void saveResultToFile(const int *assignments, const string &filename,
                      double duration, bool result);

bool dpll_parallel(Cnf &cnf, int *assignments, int strategy, bool &timeout,
                   double timelimit, clock_t start, int depth = 0);