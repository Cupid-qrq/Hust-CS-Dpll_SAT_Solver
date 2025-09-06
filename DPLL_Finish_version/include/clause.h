#pragma once
#include "definitions.h"
#include "literal.h"
#include "myArray.hpp"

// 子句类

class Clause {
public:
  // 文字集合
  myArray<Literal> literals;

  Clause();

  ~Clause();

  // 添加文字
  void addLiteral(Literal &l);

  // 删除特定文字
  void removeLiteral(const Literal &l);

  // 判断是否是单子句
  bool isUnit() const;

  // 打印子句
  void print();
};