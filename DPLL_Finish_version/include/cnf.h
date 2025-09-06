#pragma once
#include "clause.h"
#include "definitions.h"

class Cnf {
public:
  // 子句集合
  myArray<Clause> clauses;

  Cnf();

  ~Cnf();

  // 添加子句
  void addClause(Clause c);

  // 删除包含target单子句L的子句
  void removeClause(const Clause &C);

  // 删除剩下所有~L的子句
  void removeLiteral(const Literal &L);

  // 打印CNF
  void print();

  // 判断CNF是否为空
  bool isEmpty() const;

  // 判断是不是空CNF
  bool isEmptyClause() const;
};