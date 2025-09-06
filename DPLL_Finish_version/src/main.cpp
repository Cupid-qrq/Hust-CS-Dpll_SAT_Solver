#include "Sudoku.hpp"
#include "XSudoku.hpp"
#include "clause.h"
#include "cnf.h"
#include "cnfParser.hpp"
#include "definitions.h"
#include "dpllSolver.h"
#include "literal.h"
#include "myArray.hpp"

int numvars = 0, numclauses = 0;

myArray<double> varWeights; // 变量权值数组

void initTable() {
  varWeights.assign(numvars + 1, 0.0);
  for (int i = 1; i <= numvars; i++) {
    varWeights[i] = pow(1.5, -i); // 权值初始化为 1.5^(-i)
  }
}

int main() {
  Cnf cnf;
  string filename;
  int choice = 1,                   // 主菜单选择
      choice2 = 1,                  // 数独菜单选择
      strategy = 1;                 // 选择的策略
  int isread = 0;                   // 是否读取CNF文件
  int isbasic = 0;                  // 是否使用基本算法
  bool issudoku = false;            // 是否输出数独的dpll求解结果
  clock_t start, end, start2, end2; // 计时
  double basetime = 0,              // 基础算法时间
      opttime = 0;                  // 优化算法时间
  int *assignments = nullptr;       // 变量赋值数组

  Sudoku puzzle;   // 当前标准数独谜题，用户不断修改
  Sudoku solution; // 当前标准数独谜题的解
  Sudoku original; // 原始标准数独数组，记录哪些位置可填写

  // 新增：百分号数独对象
  XSudoku puzzlePercent;   // 当前百分号数独谜题，用户不断修改
  XSudoku solutionPercent; // 当前百分号数独谜题的解
  XSudoku originalPercent; // 原始百分号数独数组，记录哪些位置可填写

  srand(static_cast<unsigned int>(time(nullptr))); // 设置随机种子

  while (true) {
    cout << endl << endl;
    cout << "\t  DPLL求解器主菜单  " << endl;
    cout << "----------------------------------------------" << endl;
    cout << endl;
    cout << "     1. 读入cnf文件" << endl;
    cout << "     2. 输出cnf文件" << endl;
    cout << "     3. 使用DPLL求解cnf文件" << endl << endl;
    cout << "     4. 标准数独游戏" << endl;
    cout << "     5. 百分号数独游戏" << endl << endl;
    cout << "     0. 退出程序" << endl;
    cout << endl;
    cout << "----------------------------------------------" << endl;
    cout << endl;
    cout << "请输入你的选择" << endl;
    cin >> choice;
    system("cls");
    switch (choice) {
    case 1:
      cout << "请输入cnf文件路径: ";
      cin >> filename;
      if (CnfParser::parse(cnf, filename, numvars, numclauses)) {
        cout << "cnf文件解析成功" << endl;
        isread = 1; // 文件读取
        if (assignments != nullptr) {
          delete[] assignments; // 释放内存
        }
        assignments = new int[numvars + 1]();
        initTable(); // 初始化权值数组

        cout << "CNF文件包含 " << cnf.clauses.size() << " 条子句" << endl;
        cout << "CNF文件包含 " << numvars << " 个变元" << endl;
      } else {
        cout << "cnf文件读取失败" << endl;
      }
      system("pause");
      system("cls");
      break;

    case 2:
      if (cnf.isEmpty())
        cout << "cnf为空，请先读取cnf" << endl;
      else {
        cout << "当前cnf内容如下: " << endl;
        cout << "包含 " << numvars << " 个变元" << endl;
        cout << "包含 " << cnf.clauses.size() << " 条子句" << endl;
        cnf.print();
      }
      system("pause");
      system("cls");
      break;

    case 3:
      strategy = 1; // 默认使用策略1
      while (isread && strategy) {
        cout << endl;
        cout << "----使用哪种选择策略求解----" << endl << endl;
        cout << "   1. first出现（第一子句第一文字）" << endl;
        cout << "   2. 频率最大" << endl;
        cout << "   3. 权值最大" << endl;
        cout << "   4. 随机选择" << endl;
        cout << "   0. 退出求解" << endl;
        cout << endl;
        cout << "请输入你的选择: " << endl;
        cin >> strategy;
        if (strategy == 0) {
          basetime = 0; // 重置基础时间
          opttime = 0;
          isbasic = 0;      // 重置是否使用过基础解法
          issudoku = false; // 重置数独求解结果输出标志

          system("cls");
          break;
        } else if (strategy < 1 || strategy > 4) {
          cout << "无效的策略选择，请重新输入" << endl;
          continue;
        }

        Cnf tempCnf = cnf; // 复制一份cnf用于求解
        opttime = 0;
        clock_t start = clock(); // 开始计时

        bool timeout = false;
        double timelimit = 1000; // 设置运行求解时间上限
        bool result = false;

        result =
            dpll(tempCnf, assignments, strategy, timeout, timelimit, start);
        // result = dpll_parallel(tempCnf, assignments, strategy, timeout,
        //                        timelimit, start, 0);

        if (timeout) {
          cout << endl
               << "该策略求解时间大于" << timelimit << " 秒，已自动退出求解cnf"
               << endl;
          system("pause");
          system("cls");
          continue;
        }

        if (result) {
          cout << "CNF文件解析成功，文件可满足" << endl << endl;
          printResult(assignments, issudoku);
        } else
          cout << "CNF文件不可满足，解析失败" << endl << endl;

        end = clock(); // 结束计时

        if (strategy == 1) {
          basetime = double(end - start);
          isbasic = 1; // 标记使用过基础解法
          saveResultToFile(assignments, filename, basetime, result);
          cout << endl << "求解时间: " << basetime << " ms" << endl << endl;
        } else {
          opttime = double(end - start);
          saveResultToFile(assignments, filename, opttime, result);
          cout << "求解时间: " << opttime << " ms" << endl << endl;
        }

        if (!isbasic) {
          cout << "还未使用基础解法，无法计算时间优化率" << endl << endl;
        } else if (basetime == 0) {
          cout << "基础求解时间为0ms或极小，无法求解时间优化率" << endl << endl;
        } else if (strategy == 1) {
          cout << "基础解法不计算时间优化率" << endl << endl;
        } else {
          double optimizationRate = (basetime - opttime) / basetime * 100;
          cout << "时间优化率: ( " << basetime << " - " << opttime << " )/ "
               << basetime << " *100% = " << optimizationRate << "%" << endl
               << endl;
        }

        system("pause");
        system("cls");
      }
      if (isread == 0) {
        cout << "请先读入cnf文件" << endl;
        system("pause");
        system("cls");
      }
      break;

    case 4:
      choice = 1;
      choice2 = 1;

      solution.generateFull();   // 生成完整标准数独
      original = solution;       // 记录完整标准数独
      original.generatePuzzle(); // 生成谜题
      puzzle = original;         // 初始化当前谜题

      while (true) {
        cout << endl;
        cout << "\t 标准数独游戏 " << endl;
        cout << "----------------------------------------------" << endl;
        cout << endl;
        cout << "      1. 填入数字" << endl;
        cout << "      2. 查看答案" << endl;
        cout << "      3. 重置当前谜题" << endl;
        cout << "      4. 归约为 CNF 文件保存" << endl;
        cout << "      5. 生成新的谜题" << endl;
        cout << "      0. 返回主菜单" << endl;
        cout << endl;
        cout << "----------------------------------------------" << endl;
        cout << endl;
        puzzle.print(); // 打印当前谜题
        if (puzzle.isSolved()) {
          cout << "恭喜你，成功完成标准数独！" << endl;
          cout << "即将回到主菜单" << endl;
          system("pause");
          system("cls");
          break;
        }
        cout << "请输入你的选择" << endl;
        cin >> choice2;

        switch (choice2) {
        case 1: {
          int r, w, num;
          cout << "请输入要填写的行(1-9) 列(1-9) 数字(1-9)，用空格分开: ";
          cin >> r >> w >> num;
          if (r < 1 || r > 9 || w < 1 || w > 9 || num < 1 || num > 9) {
            cout << "输入无效，请输入1-9之间的数字、" << endl;
            system("pause");
            system("cls");
            continue;
          }

          if (original.getValue(r - 1, w - 1) != 0) {
            cout << "该位置不可填写，请重新选择位置" << endl;
            system("pause");
            system("cls");
            continue;
          }

          if (solution.getValue(r - 1, w - 1) == num) {
            puzzle.setValue(r - 1, w - 1, num);
            cout << "填写正确！" << endl;
          } else {
            cout << "填写错误，请重新填写" << endl;
          }
          break;
        }

        case 2:
          cout << "标准数独答案如下: " << endl;
          solution.print();
          cout << "即将回到标准数独菜单" << endl;
          system("pause");
          system("cls");
          break;

        case 3:
          puzzle = original;
          cout << "当前谜题已重置" << endl;
          system("pause");
          system("cls");
          break;

        case 4:
          // 使用DPLL求解当前谜题
          sudokuToCnf(puzzle);
          issudoku = true; // 设置数独求解结果输出标志
          system("pause");
          system("cls");
          break;

        case 5:
          // 生成新的标准数独谜题
          solution.generateFull();   // 生成完整标准数独
          original = solution;       // 记录完整标准数独
          original.generatePuzzle(); // 生成谜题
          puzzle = original;         // 初始化当前谜题
          cout << "已生成新的标准数独谜题" << endl;
          system("pause");
          system("cls");
          break;

        case 0:
          cout << "即将回到主菜单" << endl;
          system("pause");
          system("cls");
          goto exit_sudoku;
          break;

        default:
          cout << "无效的输入，请重新选择菜单选项" << endl;
          system("pause");
          system("cls");
          continue;
        }

        // 每次操作后检查是否完成
        if (puzzle.isSolved()) {
          cout << "恭喜你，成功完成标准数独！" << endl;
          cout << "即将回到主菜单" << endl;
          system("pause");
          system("cls");
          break;
        }
      }
      break;

    case 5: // 百分号数独游戏
      choice = 1;
      choice2 = 1;

      solutionPercent.generateFull();    // 生成完整百分号数独
      originalPercent = solutionPercent; // 记录完整百分号数独
      originalPercent.generatePuzzle();  // 生成谜题
      puzzlePercent = originalPercent;   // 初始化当前谜题

      while (true) {
        cout << endl;
        cout << "\t 百分号数独游戏 " << endl;
        cout << "----------------------------------------------" << endl;
        cout << endl;
        cout << "      1. 填入数字" << endl;
        cout << "      2. 查看答案" << endl;
        cout << "      3. 重置当前谜题" << endl;
        cout << "      4. 归约为 CNF 文件保存,并采用DPLL求解" << endl;
        cout << "      5. 生成新的谜题" << endl;
        cout << "      0. 返回主菜单" << endl;
        cout << endl;
        cout << "----------------------------------------------" << endl;
        cout << endl;
        puzzlePercent.print(); // 打印当前谜题
        if (puzzlePercent.isSolved()) {
          cout << "恭喜你，成功完成百分号数独！" << endl;
          cout << "即将回到主菜单" << endl;
          system("pause");
          system("cls");
          break;
        }
        cout << "请输入你的选择" << endl;
        cin >> choice2;

        switch (choice2) {
        case 1: {
          int r, w, num;
          cout << "请输入要填写的行(1-9) 列(1-9) 数字(1-9)，用空格分开: ";
          cin >> r >> w >> num;
          if (r < 1 || r > 9 || w < 1 || w > 9 || num < 1 || num > 9) {
            cout << "输入无效，请输入1-9之间的数字、" << endl;
            system("pause");
            system("cls");
            continue;
          }

          if (originalPercent.getValue(r - 1, w - 1) != 0) {
            cout << "该位置不可填写，请重新选择位置" << endl;
            system("pause");
            system("cls");
            continue;
          }

          if (solutionPercent.getValue(r - 1, w - 1) == num) {
            puzzlePercent.setValue(r - 1, w - 1, num);
            cout << "填写正确！" << endl;
          } else {
            cout << "填写错误，请重新填写" << endl;
          }
          break;
        }

        case 2:
          cout << "百分号数独答案如下: " << endl;
          solutionPercent.print();
          cout << "即将回到百分号数独菜单" << endl;
          system("pause");
          system("cls");
          break;

        case 3:
          puzzlePercent = originalPercent;
          cout << "当前谜题已重置" << endl;
          system("pause");
          system("cls");
          break;

        case 4: {
          // 1. 生成 CNF 文件
          percentSudokuToCnf(puzzlePercent);
          issudoku = true; // 标记为数独求解，用于结果输出格式

          // 2. 自动读取并求解
          const string filename = "Xsudoku.cnf";
          Cnf tempCnf;
          int local_numvars = 0, local_numclauses = 0;

          cout << endl << "正在自动读取并求解 CNF 文件..." << endl;

          if (!CnfParser::parse(tempCnf, filename, local_numvars,
                                local_numclauses)) {
            cout << "CNF 文件读取失败！" << endl;
            system("pause");
            system("cls");
            break;
          }

          cout << "成功读取 CNF 文件，包含 " << local_numvars << " 个变元，"
               << tempCnf.clauses.size() << " 条子句。" << endl;

          // 3. 分配赋值数组（如果之前有，先释放）
          if (assignments != nullptr) {
            delete[] assignments;
          }
          assignments = new int[local_numvars + 1]();
          numvars = local_numvars; // 更新全局变量，用于 initTable
          initTable();             // 初始化权值数组

          // 4. 设置超时和计时
          bool timeout = false;
          double timelimit = 10000; // 10秒超时
          clock_t start = clock();

          // 5. 调用 DPLL 求解（使用策略1：first出现）
          bool result =
              dpll(tempCnf, assignments, 1, timeout, timelimit, start);

          clock_t end = clock();

          // 6. 输出结果
          if (timeout) {
            cout << "求解超时" << endl;
          } else if (result) {
            cout << "百分号数独有解！" << endl;
            printResult(assignments, issudoku); // 使用数独格式输出
          } else {
            cout << "百分号数独无解！" << endl;
          }

          double solveTime = double(end - start);
          cout << "求解耗时: " << solveTime << " ms" << endl;
          cout << endl;
          cout << "转换为数独形式的解如下：" << endl;
          solutionPercent.print();
          cout << endl;

          // 7. 清理
          issudoku = false; // 重置标志
          system("pause");
          system("cls");
          break;
        }

        case 5:
          // 生成新的百分号数独谜题
          solutionPercent.generateFull();    // 生成完整百分号数独
          originalPercent = solutionPercent; // 记录完整百分号数独
          originalPercent.generatePuzzle();  // 生成谜题
          puzzlePercent = originalPercent;   // 初始化当前谜题
          cout << "已生成新的百分号数独谜题" << endl;
          system("pause");
          system("cls");
          break;

        case 0:
          cout << "即将回到主菜单" << endl;
          system("pause");
          system("cls");
          goto exit_percent_sudoku;
          break;

        default:
          cout << "无效的输入，请重新选择菜单选项" << endl;
          system("pause");
          system("cls");
          continue;
        }

        // 每次操作后检查是否完成
        if (puzzlePercent.isSolved()) {
          cout << "恭喜你，成功完成百分号数独！" << endl;
          cout << "即将回到主菜单" << endl;
          system("pause");
          system("cls");
          break;
        }
      }
      break;

    exit_sudoku:
      // 处理标准数独返回主菜单的逻辑
      system("cls");
      break;

    exit_percent_sudoku:
      // 处理百分号数独返回主菜单的逻辑
      system("cls");
      break;

    case 0:
      system("cls");
      return 0;
      break;
    default:
      cout << "无效的输入，请重新选择菜单选项" << endl;
      continue;
    }
  }
  return 0;
}