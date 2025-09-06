#include "clause.h"

Clause::Clause() {}

Clause::~Clause() {}

void Clause::addLiteral(Literal &l) { literals.push_back(l); }

void Clause::removeLiteral(const Literal &l) {
  // 创建一个新的数组来存储剩余的文字
  myArray<Literal> new_literals;
  for (int i = 0; i < literals.size(); i++) {
    if (literals[i] != l) {
      new_literals.push_back(literals[i]);
    }
  }
  // 替换旧的文字数组
  literals = new_literals;
}

void Clause::print() {
  for (int i = 0; i < literals.size(); i++) {
    literals[i].print();
  }
  cout << endl;
}

bool Clause::isUnit() const {
  return literals.size() == 1; // 如果文字数量为1，则是单子句
}
