## *dpll_sat_solver by Cupid_qrq*
    
本程序为 HUST-CS 程序设计综合课程设计 的课设项目，由于不能使用STL，就通过cpp模拟了vector（程序中的myArray）。

数据结构采用顺序表+顺序表，因此很多对于很大的样例能跑但是比较慢），推荐优化方向包括：

1. **数据结构优化**：考虑使用bitset位运算
2. **算法优化**：改进DPLL算法，比如CDCL，VSIDS等
3. **并行计算**：利用多线程或分布式计算加速求解过程，如OpenMP

---

## *项目简介与主要功能*

本项目实现了基于 DPLL 算法的 SAT 求解器，支持标准 CNF 文件的解析与求解，并扩展支持数独、X 数独等问题的 SAT 化建模与自动求解。

- **CNF 文件解析与求解**：支持 DIMACS 格式的 CNF 文件，自动解析变量与子句，使用 DPLL 算法进行求解。
- **多策略变量选择**：实现了多种变量选择策略（首选、出现次数、加权、随机），可灵活切换以优化求解效率。
- **数独与 X 数独建模**：内置数独、X 数独生成与 SAT 化模块，可自动生成题目、转化为 CNF 并调用 DPLL 求解。
- **结果输出与验证**：求解结果自动保存为 `.res` 文件，支持结果验证工具。
- **命令行交互式菜单**：支持多功能选择，包括加载/显示 CNF、求解、数独玩法等。

---

## *目录结构*

```
├── include/         # 头文件，算法与数据结构声明
│   ├── clause.h
│   ├── cnf.h
│   ├── dpllSolver.h
│   ├── literal.h
│   ├── cnfParser.hpp
│   ├── Sudoku.hpp
│   └── XSudoku.hpp
├── src/             # 源码实现
│   ├── clause.cpp
│   ├── cnf.cpp
│   ├── dpllSolver.cpp
│   ├── literal.cpp
│   ├── main.cpp     # 主程序入口
├── examples/        # 测试用例与验证工具
│   ├── *.cnf        # 测试 CNF 文件
│   ├── *.res        # 求解结果文件
│   ├── verify.exe   # 结果验证工具
├── build/           # 构建输出
│   └── sat_solver.exe
├── CMakeLists.txt   # CMake 编译文件
├── README.md        
```
---

## *核心模块说明*

- **DPLL 算法实现**（`dpllSolver.cpp`/`dpllSolver.h`）
	- 递归式 DPLL 主流程，支持单子句传播、回溯、变量选择多策略。
	- 支持超时控制与结果保存。
- **CNF 数据结构**（`cnf.h`/`cnf.cpp`）
	- 子句集管理，支持添加、删除、打印、判空等操作。
- **数独/X 数独建模**（`Sudoku.hpp`/`XSudoku.hpp`）
	- 自动生成题目、解答、SAT 化转化，支持多种玩法。
- **结果输出与验证**
	- 自动生成 `.res` 文件，格式兼容验证工具。


## *其他说明*
- *前人栽树，后人乘凉，传承不息。如果你觉得本项目对你有或许一点帮助，欢迎给个*⭐
- *contact me: <781823561@qq.com>*

---
