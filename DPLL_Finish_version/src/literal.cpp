#include "literal.h"

Literal::Literal() { this->var = 0; }

Literal::Literal(int var) { this->var = var; }

Literal::~Literal() {}

bool Literal::isNegative() { return this->var < 0; }

int Literal::getAbs() { return abs(this->var); }

void Literal::print() { cout << var << " "; }

bool Literal::operator==(const Literal &other) const {
  return this->var == other.var;
}

bool Literal::operator!=(const Literal &other) const {
  return this->var != other.var;
}