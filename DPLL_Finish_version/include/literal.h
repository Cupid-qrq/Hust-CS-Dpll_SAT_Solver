#pragma once

#include "definitions.h"

// 文字类

class Literal {
public:
  int var;

  Literal();

  Literal(int var);

  ~Literal();

  bool isNegative();
  // 返回绝对值
  int getAbs();
  // 打印文字
  void print();
  // 重载运算符
  bool operator==(const Literal &other) const;

  bool operator!=(const Literal &other) const;
};
