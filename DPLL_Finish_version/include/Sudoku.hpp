#pragma once
#include "definitions.h"
#include "myArray.hpp"

class Sudoku {
private:
  int grid[9][9];

  bool isValid(int row, int col, int num) const {
    // 检查行
    for (int x = 0; x < 9; x++) {
      if (grid[row][x] == num)
        return false;
    }

    // 检查列
    for (int x = 0; x < 9; x++) {
      if (grid[x][col] == num)
        return false;
    }

    // 检查3x3子网格
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (grid[i + startRow][j + startCol] == num)
          return false;
      }
    }

    return true;
  }

  bool generate(int row, int col) {
    if (row == 9)
      return true;

    int nextRow = (col == 8) ? row + 1 : row;
    int nextCol = (col + 1) % 9;

    // 如果当前位置已经有数字，跳过
    if (grid[row][col] != 0) {
      return generate(nextRow, nextCol);
    }

    int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i = 8; i > 0; i--) {
      int j = rand() % (i + 1);
      swap(nums[i], nums[j]);
    }

    for (int i = 0; i < 9; i++) {
      if (isValid(row, col, nums[i])) {
        grid[row][col] = nums[i];
        if (generate(nextRow, nextCol)) {
          return true;
        }
        grid[row][col] = 0; // 回溯
      }
    }
    return false; // 触发回溯
  }

  void countSolution(int row, int col, int &count, int maxCount) const {
    if (count >= maxCount)
      return;

    if (row == 9) {
      count++;
      return;
    }

    int nextRow = (col == 8) ? row + 1 : row;
    int nextCol = (col + 1) % 9;

    if (grid[row][col] != 0) {
      countSolution(nextRow, nextCol, count, maxCount);
      return;
    }

    for (int num = 1; num <= 9; num++) {
      if (isValid(row, col, num)) {

        countSolution(nextRow, nextCol, count, maxCount);
        // 不需要回溯，因为我们没有实际修改grid
      }
    }
  }

public:
  Sudoku() {
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        grid[i][j] = 0;
  }

  // 生成完整数独
  void generateFull() {
    // 不需要重新初始化，构造函数已经做了
    bool success = generate(0, 0);
    if (!success) {
      cout << "生成数独失败" << endl;
    }
  }

  // 检查是否有唯一解
  bool hasUniqueSolution() const {
    Sudoku temp;
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        temp.grid[i][j] = grid[i][j];

    int count = 0;
    temp.countSolution(0, 0, count, 2);
    return count == 1;
  }

  void generatePuzzle(int numHoles = 40) { // 默认挖40个洞，留下41个数字
    generateFull();

    // 创建所有位置列表
    myArray<std::pair<int, int>> positions;
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        positions.push_back({i, j});
      }
    }

    // 随机打乱位置顺序
    for (int i = positions.size() - 1; i > 0; i--) {
      int j = rand() % (i + 1);
      std::swap(positions[i], positions[j]);
    }

    int removed = 0;
    for (auto &pos : positions) {
      if (removed >= numHoles)
        break;

      int row = pos.first;
      int col = pos.second;
      int backup = grid[row][col];

      if (backup == 0)
        continue;

      grid[row][col] = 0;
      if (hasUniqueSolution()) {
        removed++;
      } else {
        grid[row][col] = backup;
      }
    }
  }

  // 打印数独
  void print() const {
    cout << "+-------+-------+-------+" << endl;
    for (int i = 0; i < 9; i++) {
      cout << "| ";
      for (int j = 0; j < 9; j++) {
        if (grid[i][j] == 0) {
          cout << ". ";
        } else {
          cout << grid[i][j] << " ";
        }
        if ((j + 1) % 3 == 0)
          cout << "| ";
      }
      cout << endl;
      if ((i + 1) % 3 == 0)
        cout << "+-------+-------+-------+" << endl;
    }
  }

  bool isSolved() const {
    // 检查是否有空格
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        if (grid[i][j] == 0) {
          return false;
        }
      }
    }

    // 检查所有行是否有重复数字
    for (int i = 0; i < 9; i++) {
      bool seen[10] = {false}; // 索引1-9
      for (int j = 0; j < 9; j++) {
        int num = grid[i][j];
        if (seen[num]) {
          return false; // 重复数字
        }
        seen[num] = true;
      }
    }

    // 检查所有列是否有重复数字
    for (int j = 0; j < 9; j++) {
      bool seen[10] = {false};
      for (int i = 0; i < 9; i++) {
        int num = grid[i][j];
        if (seen[num]) {
          return false;
        }
        seen[num] = true;
      }
    }

    // 检查所有3x3子网格是否有重复数字
    for (int startRow = 0; startRow < 9; startRow += 3) {
      for (int startCol = 0; startCol < 9; startCol += 3) {
        bool seen[10] = {false};
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 3; j++) {
            int num = grid[startRow + i][startCol + j];
            if (seen[num]) {
              return false;
            }
            seen[num] = true;
          }
        }
      }
    }

    return true; // 所有检查通过
  }

  // 用于交互，检查用户输入的数字是否正确
  bool isCorrect(int row, int col, int num) const {
    Sudoku temp;
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        temp.grid[i][j] = grid[i][j];

    temp.grid[row][col] = num;
    return temp.isValid(row, col, num);
  }

  // 检查某个位置是否是初始空格（可填写）
  bool isEditable(int row, int col, const Sudoku &originalPuzzle) const {
    return (originalPuzzle.grid[row][col] == 0);
  }

  // 获取特定位置的值
  int getValue(int row, int col) const { return grid[row][col]; }

  // 设置特定位置的值
  void setValue(int row, int col, int value) { grid[row][col] = value; }
};

int var(int row, int col, int num) {
  return (row - 1) * 81 + (col - 1) * 9 + num;
}

// 将数独谜题转换为CNF格式并保存到文件
void sudokuToCnf(const Sudoku &puzzle) {
  const string filename = "sudoku.cnf";
  ofstream outFile(filename);
  if (!outFile.is_open()) {
    cerr << "无法打开文件: " << filename << endl;
    return;
  }

  outFile << "c sudoku conversion to cnf file" << endl;
  int numvars = 729; // 9x9x9

  // 正确计算预设数字数量
  int initialClues = 0;
  for (int row = 1; row <= 9; row++) {
    for (int col = 1; col <= 9; col++) {
      if (puzzle.getValue(row - 1, col - 1) != 0) {
        initialClues++;
      }
    }
  }
  cout << "预设数字数量: " << initialClues << endl;

  // 正确计算总子句数 - 包含所有8类约束
  // 单元格约束: 81 + 2916
  // 行约束: 81 + 2916
  // 列约束: 81 + 2916
  // 宫约束: 81 + 2916
  // 预设数字: initialClues
  int numclauses = 81 * 4 + 2916 * 4 + initialClues;

  outFile << "p cnf " << numvars << " " << numclauses << endl;

  // 1. 单元格约束1: 每个单元格至少有一个数字
  for (int row = 1; row <= 9; row++) {
    for (int col = 1; col <= 9; col++) {
      for (int num = 1; num <= 9; num++) {
        outFile << var(row, col, num) << " ";
      }
      outFile << "0" << endl;
    }
  }

  // 2. 单元格约束2: 每个单元格最多有一个数字
  for (int row = 1; row <= 9; row++) {
    for (int col = 1; col <= 9; col++) {
      for (int num1 = 1; num1 <= 9; num1++) {
        for (int num2 = num1 + 1; num2 <= 9; num2++) {
          outFile << -var(row, col, num1) << " " << -var(row, col, num2) << " 0"
                  << endl;
        }
      }
    }
  }

  // 3. 行约束1: 每行每个数字至少出现一次
  for (int row = 1; row <= 9; row++) {
    for (int num = 1; num <= 9; num++) {
      for (int col = 1; col <= 9; col++) {
        outFile << var(row, col, num) << " ";
      }
      outFile << "0" << endl;
    }
  }

  // 4. 行约束2: 每行每个数字最多出现一次
  for (int row = 1; row <= 9; row++) {
    for (int num = 1; num <= 9; num++) {
      for (int col1 = 1; col1 <= 9; col1++) {
        for (int col2 = col1 + 1; col2 <= 9; col2++) {
          outFile << -var(row, col1, num) << " " << -var(row, col2, num) << " 0"
                  << endl;
        }
      }
    }
  }

  // 5. 列约束1: 每列每个数字至少出现一次
  for (int col = 1; col <= 9; col++) {
    for (int num = 1; num <= 9; num++) {
      for (int row = 1; row <= 9; row++) {
        outFile << var(row, col, num) << " ";
      }
      outFile << "0" << endl;
    }
  }

  // 6. 列约束2: 每列每个数字最多出现一次
  for (int col = 1; col <= 9; col++) {
    for (int num = 1; num <= 9; num++) {
      for (int row1 = 1; row1 <= 9; row1++) {
        for (int row2 = row1 + 1; row2 <= 9; row2++) {
          outFile << -var(row1, col, num) << " " << -var(row2, col, num) << " 0"
                  << endl;
        }
      }
    }
  }

  // 7. 宫约束1: 每个3x3子网格每个数字至少出现一次
  for (int boxRow = 0; boxRow < 3; boxRow++) {
    for (int boxCol = 0; boxCol < 3; boxCol++) {
      for (int num = 1; num <= 9; num++) {
        for (int row = 1 + boxRow * 3; row <= 3 + boxRow * 3; row++) {
          for (int col = 1 + boxCol * 3; col <= 3 + boxCol * 3; col++) {
            outFile << var(row, col, num) << " ";
          }
        }
        outFile << "0" << endl;
      }
    }
  }

  // 8. 宫约束2: 每个3x3子网格每个数字最多出现一次
  for (int boxRow = 0; boxRow < 3; boxRow++) {
    for (int boxCol = 0; boxCol < 3; boxCol++) {
      for (int num = 1; num <= 9; num++) {
        for (int row1 = 1 + boxRow * 3; row1 <= 3 + boxRow * 3; row1++) {
          for (int col1 = 1 + boxCol * 3; col1 <= 3 + boxCol * 3; col1++) {
            for (int row2 = row1; row2 <= 3 + boxRow * 3; row2++) {
              for (int col2 = (row2 == row1 ? col1 + 1 : 1 + boxCol * 3);
                   col2 <= 3 + boxCol * 3; col2++) {
                outFile << -var(row1, col1, num) << " " << -var(row2, col2, num)
                        << " 0" << endl;
              }
            }
          }
        }
      }
    }
  }

  // 9. 预设数字约束
  for (int row = 1; row <= 9; row++) {
    for (int col = 1; col <= 9; col++) {
      if (puzzle.getValue(row - 1, col - 1) != 0) {
        int num = puzzle.getValue(row - 1, col - 1);
        outFile << var(row, col, num) << " 0" << endl;
      }
    }
  }

  // 验证实际子句数
  int actualClauses = 0;
  actualClauses += 81;           // 单元格存在性
  actualClauses += 2916;         // 单元格唯一性
  actualClauses += 81;           // 行存在性
  actualClauses += 2916;         // 行唯一性
  actualClauses += 81;           // 列存在性
  actualClauses += 2916;         // 列唯一性
  actualClauses += 81;           // 宫存在性
  actualClauses += 2916;         // 宫唯一性
  actualClauses += initialClues; // 预设数字

  if (actualClauses != numclauses) {
    cout << "警告：子句数量不匹配！计算值: " << numclauses
         << "，实际值: " << actualClauses << endl;
  }

  outFile.close();
  cout << "CNF文件已保存到: " << filename << endl;
  cout << "变量数: " << numvars << ", 子句数: " << numclauses << endl;
  cout << "预设数字数量: " << initialClues << " (81 - 挖洞数量)" << endl;
}