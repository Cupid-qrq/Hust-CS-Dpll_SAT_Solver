#include "dpllSolver.h"
#include "definitions.h"

extern int numvars, numclauses;    // 声明外部变量,在main.cpp中定义
extern myArray<double> varWeights; // 变量权值数组

/*--------------------------------------
author: hust Cupid_qrq                 |
data: 2025-08-20                       |
本文件为DPLL算法的核心实现               |
包括单子句传播策略、文字选择策略、        |
分类递归求解+主函数、结果打印与保存功能   |
--------------------------------------*/

/*------------------------------------------------------------------------------
DPLL算法思想一：单子句传播策略
****
单子句规则。
如果子句集S中有一个单子句L,那么L一定取真值，于是可以从S中删除所有包含L的子句（包括单子句本身），得到子句集S1，
如果它是空集，则S可满足。否则对S1中的每个子句，如果它包含文字L,则从该子句中去掉这个文字，这样可得到子句集合S2。
S可满足当且仅当S2可满足。单子句传播策略就是反复利用单子句规则化简S的过程。
****

具体步骤如下：
- Step 1:根据所使用的策略选择一个文字（单子句），将其赋值为真，然后对cnf进行遍历
- Step 2:检查cnf中的每个子句
  - 如果子句中包含该文字，则该子句被满足，可以从cnf中删除
  - 如果子句中包含该文字的否定，则从该子句中删除该否定文字
  - 如果被删除否定文字后，子句变为空子句，则说明出现冲突，返回false
  - 如果子句非空，将其添加到新的cnf中
- Step 3:如果新的cnf中没有子句了，说明所有子句都被满足，返回true
------------------------------------------------------------------------------*/
bool propagate(Cnf &cnf, Literal &literal, int *assignments) {
  Cnf newCnf; // 创建一个新的CNF对象来存储更新后的子句

  for (int i = 0; i < cnf.clauses.size(); i++) {
    const Clause &clause = cnf.clauses[i];

    bool contain = false; // 标志位，表示子句是否包含指定文字
    for (int j = 0; j < clause.literals.size(); j++) {
      if (clause.literals[j] == literal) {
        contain = true; // 子句包含指定文字
        break;
      }
    }

    if (contain) {
      continue; // 跳过该子句，不添加到newCnf中
    }

    Clause newClause = clause;           // 创建一个新的子句对象，初始为当前子句
    Literal negLiteral(-literal.var);    // 计算文字的否定
    newClause.removeLiteral(negLiteral); // 从子句中删除否定文字

    if (!newClause.literals.empty()) {
      newCnf.addClause(newClause); // 如果子句非空，添加到新的CNF中
    } else {
      return false; // 出现空子句，返回false
    }
  }

  assignments[abs(literal.var)] = literal.isNegative() ? -1 : 1; // 更新赋值数组
  cnf = newCnf; // 更新原始CNF为新的CNF
  return true;
}

/*------------------------------------------------------------------------------
DPLL算法思想二：分裂搜索并实现两个策略结合实现dpll算法
****
分裂策略。
按某种策略选取一个文字L.如果L取真值，则根据单子句传播策略，
可将S化成S2；若L取假值（即-L成立）时，S可化成S1.
****

具体步骤如下：
- Step 1:不断进行单子句传播，直到cnf中不再包含单子句
- Step 2:检查cnf的状态
  - 如果cnf为空，说明所有子句都被满足，返回true
  - 如果cnf中包含空子句，说明出现冲突，返回false
- Step 3:根据所使用的策略选择一个文字，进行分裂搜索
- Step 4:递归调用dpll函数，尝试将该文字赋值为真或假
  - 如果任意一种赋值方式能使cnf满足，返回true
  - 否则返回false
------------------------------------------------------------------------------*/
bool dpll(Cnf &cnf, int *assignments, int strategy, bool &timeout,
          double timelimit, clock_t start) {
  // 检查是否超时
  double duration = (double)(clock() - start) / CLOCKS_PER_SEC; // 计算执行时间
  if (timeout || duration > timelimit) {
    timeout = true; // 设置超时标志
    return false;   // 返回false表示未能在限定时间内完成求解
  }

  // Step 1: 不断进行单子句传播，直到cnf中不再包含单子句
  while (true) {
    bool unitClauseFound = false; // 标志位，表示是否找到单子句
    for (const Clause &clause : cnf.clauses) {
      if (clause.isUnit()) {                      // 如果找到单子句
        Literal unitLiteral = clause.literals[0]; // 获取单子句中的文字
        if (!propagate(cnf, unitLiteral, assignments)) {
          return false; // 如果传播失败，返回false
        }
        unitClauseFound = true; // 标记找到单子句
        break;                  // 重新开始检查cnf
      }
    }
    if (!unitClauseFound) {
      break; // 如果没有找到单子句，退出循环
    }
  }

  // Step 2: 检查cnf的状态
  if (cnf.isEmpty()) {
    return true; // 所有子句都被满足，返回true
  }

  // 如果还存在非空子句，改为分裂搜索cnf
  if (!cnf.isEmptyClause()) {

    Literal literal = selectLiteral(cnf, strategy);
    if (literal.var == 0) {
      return false; // 没有可选文字，返回false
    }

    // 尝试将文字赋值为真
    Cnf cnfTrue = cnf;                // 复制当前cnf
    Literal trueLiteral(literal.var); // 文字取真
    // 继续递归调用dpll
    if (propagate(cnfTrue, trueLiteral, assignments)) {
      if (dpll(cnfTrue, assignments, strategy, timeout, timelimit, start)) {
        return true; // 赋值为真时cnf满足，返回true
      }
    }

    // 尝试将文字赋值为假
    Cnf cnfFalse = cnf;                 // 复制当前cnf
    Literal falseLiteral(-literal.var); // 文字取假
    // 继续递归调用dpll
    if (propagate(cnfFalse, falseLiteral, assignments)) {
      if (dpll(cnfFalse, assignments, strategy, timeout, timelimit, start)) {
        return true; // 赋值为假时cnf满足，返回true
      }
    }
  }
  return false; // 两种赋值方式都不能满足cnf，返回false
}

/*------------------------------------------------------------------------------
指定策略选择一个文字，一般的文字选择策略有：
策略1：选择第一个出现的文字（最朴素的strategy）
策略2：选择出现频率最高的文字
策略3：选择权值最大的文字（加上区分正负的优化）
策略4：随机选择一个文字
策略5：CDCL（冲突学习与智能回溯）

在本项目中，提供策略1、策略2、策略3、策略4的实现
-------------------------------------------------------------------------------*/
Literal selectLiteral(const Cnf &cnf, int strategy) {
  // 策略1：选择第一个出现的文字
  if (strategy == 1) {
    for (const Clause &clause : cnf.clauses) {
      if (!clause.literals.empty()) {
        return clause.literals[0]; // 返回第一个文字
      }
    }
  }
  // 策略2：选择出现频率最高的文字
  else if (strategy == 2) {

    // 初始化计数数组
    myArray<int> cnt(numvars * 2 + 1, 0);

    // 计算每个变元的出现次数
    for (const Clause &clause : cnf.clauses) {
      for (const Literal &literal : clause.literals) {
        if (literal.var > 0) {
          cnt[literal.var]++;
        } else {
          cnt[numvars - literal.var]++;
        }
      }
    }

    // 选择出现次数最多的变元（正面）
    int maxBool = 0;
    int maxTimes = 0;
    for (int i = 1; i <= numvars; ++i) {
      if (cnt[i] > maxTimes) {
        maxTimes = cnt[i];
        maxBool = i;
      }
    }

    // 如果正面变元的出现次数都是 1，则选择反面变元
    if (maxTimes == 1) {
      for (int i = numvars + 1; i <= numvars * 2; ++i) {
        if (cnt[i] > maxTimes) {
          maxTimes = cnt[i];
          maxBool = numvars - i;
        }
      }
    }

    return Literal(maxBool);
  }

  // 策略3：选择权值最大的文字（区分正负）
  else if (strategy == 3) {

    // 权值数组，存储每个文字的累计权值
    myArray<double> weights(numvars * 2 + 1, 0.0); //

    for (const Clause &clause : cnf.clauses) {
      int len = clause.literals.size();
      if (len == 0)
        continue;                            // 跳过空子句
      double clauseWeight = varWeights[len]; // 使用预先计算的权值
      for (const Literal &literal : clause.literals) {
        if (literal.var > 0) {
          weights[literal.var] += clauseWeight; // 正文字权值
        } else {
          weights[-literal.var + numvars] += clauseWeight; // 负文字权值
        }
      }
    }

    double maxWeight = 0;
    int selectedVar = 0;
    for (int i = 1; i <= numvars; i++) {
      if (weights[i] > maxWeight) {
        maxWeight = weights[i];
        selectedVar = i; // 选择权值最大的正文字
      }
      if (weights[i + numvars] > maxWeight) {
        maxWeight = weights[i + numvars];
        selectedVar = -i; // 选择权值最大的负文字
      }
    }

    if (selectedVar != 0) {
      return Literal(selectedVar); // 返回选择的文字
    }
  }

  // 策略4：随机选择文字
  else if (strategy == 4) {
    srand(time(0));

    int clauseIndex = rand() % cnf.clauses.size(); // 随机选择一个子句
    while (cnf.clauses[clauseIndex].literals.empty()) {
      clauseIndex = rand() % cnf.clauses.size(); // 确保子句非空
    }

    int literalIndex =
        rand() %
        cnf.clauses[clauseIndex].literals.size(); // 随机选择子句中的一个文字
    return cnf.clauses[clauseIndex]
        .literals[literalIndex]; // 返回随机选择的文字
  }

  return Literal(0); // 如果没有可选文字，返回一个无效的文字
}

/*------------------------------------------------------------------------------
功能：打印结果
如果printchoice为true，则打印赋值为1的文字
否则也打印赋值为-1的文字
-------------------------------------------------------------------------------*/
void printResult(const int *assignments, int printchoice) {
  cout << "Result: ";
  for (int i = 1; i <= numvars; i++) {
    if (assignments[i] == 1) {
      cout << i << " "; // 打印赋值为1的文字
    } else if (assignments[i] == -1 && !printchoice) {
      cout << -i << " "; // 打印赋值为-1的文字
    }
  }
  cout << endl;
}

/*------------------------------------------------------------------------------
功能：保存结果到文件
对每个算例的求解结果要求输出到一个与算例同名的文件（文件扩展名为.res）
文件内容与格式如下：
s求解结果    //1表示满足，0表示不满足，-1表示在限定时间内未完成求解
v -1 2 -3 …
//满足时，每个变元的赋值序列，-1表示第一个变元1取假，2表示第二个变元取真，用空格分开
t 17        //以毫秒为单位的DPLL执行时间，可增加分支规则执行次数信息
-------------------------------------------------------------------------------*/
void saveResultToFile(const int *assignments, const string &filename,
                      double duration, bool result) {
  string resname = filename;
  resname.erase(resname.end() - 4, resname.end());
  resname += ".res"; // 添加.res后缀
  ofstream resfile(resname);

  if (!resfile.is_open()) {
    cerr << "Error opening result file: " << resname << endl;
    return; // 打开文件失败，返回
  }

  resfile << "s " << (result ? "1" : "0") << endl; // 写入求解结果
  resfile << "v ";

  // 写入每个变元的赋值序列
  // 如果求解成功，才写入每个bool的赋值
  if (result) {
    for (int i = 1; i <= numvars; i++) {
      if (assignments[i] == 1) {
        resfile << i << " "; // 变元i取真
      } else if (assignments[i] == -1) {
        resfile << -i << " "; // 变元i取假
      }
    }
  }

  resfile << endl;
  resfile << "t " << duration << endl; // 写入执行时间
  resfile.close();                     // 关闭文件
}