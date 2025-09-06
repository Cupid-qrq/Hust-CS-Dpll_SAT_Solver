#include "cnf.h"

Cnf::Cnf() {}

Cnf::~Cnf() {}

void Cnf::addClause(Clause c) { clauses.push_back(c); }

void Cnf::removeClause(const Clause &C) {
  // 创建一个新的数组来储存不包含C的子句
  myArray<Clause> new_clauses;
  for (int i = 0; i < clauses.size(); i++) {
    for (int j = 0; j < clauses[i].literals.size(); j++) {
      if (clauses[i].literals[j] == C.literals[0]) {
        // 如果子句包含目标单子句L，则不添加到新数组
        break;
      }
      if (j == clauses[i].literals.size() - 1) {
        // 如果循环到最后一个文字都没有找到目标单子句L，则添加到新数组
        new_clauses.push_back(clauses[i]);
      }
    }
  }
  // 替换旧的子句数组
  clauses = new_clauses;
}

void Cnf::removeLiteral(const Literal &L) {
  // 利用子句的removeLiteral方法删除所有包含L的子句，简化逻辑
  for (int i = 0; i < clauses.size(); i++) {
    // 删除包含L的子句
    clauses[i].removeLiteral(L);
  }
}

void Cnf::print() {
  // 同理， 利用子句的print方法打印CNF的所有子句，
  for (int i = 0; i < clauses.size(); i++) {
    clauses[i].print();
  }
}

bool Cnf::isEmpty() const { return clauses.empty(); }

bool Cnf::isEmptyClause() const {
  for (int i = 0; i < clauses.size(); i++) {
    if (clauses[i].literals.empty()) {
      return true; // 存在空子句
    }
  }
  return false; // 不存在空子句
}