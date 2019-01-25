#include "trigram.h"

trigram::trigram() {
    _1 = _2 = _3 = 1;
}

trigram::trigram(signed char a, signed char b, signed char c) : _1(a), _2(b), _3(c) {}

void trigram::add(signed char ch) {
    _1 = _2;
    _2 = _3;
    _3 = ch;
}

std::ostream &operator<<(std::ostream &s, trigram const &a) {
    s << a._1 << a._2 << a._3;
    return s;
}

std::istream &operator>>(std::istream &s, trigram &a) {
    a._1 = static_cast<signed char>(s.get());
    a._2 = static_cast<signed char>(s.get());
    a._3 = static_cast<signed char>(s.get());
    return s;
}

bool operator<(trigram const &a, trigram const &b) {
    if (a._1 < b._1) return true;
    else if (a._1 > b._1) return false;
    if (a._2 < b._2) return true;
    else if (a._2 > b._2) return false;
    return a._3 < b._3;
}

bool operator==(trigram const &a, trigram const &b) {
    return (a._1 == b._1 && a._2 == b._2 && a._3 == b._3);
}

bool operator!=(trigram const &a, trigram const &b) {
    return !(a == b);
}

bool operator>(trigram const &a, trigram const &b) {
    return !(a == b || a < b);
}
