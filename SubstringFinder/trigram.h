//
// Created by Dmitry on 17.01.2019.
//

#ifndef TRIGRAM_H
#define TRIGRAM_H

#include <iostream>
#include <string>
#include <fstream>
#include <memory.h>
#include <vector>

class trigram {
public:
    char _1, _2, _3;

    trigram();

    trigram(char a, char b, char c);

    trigram(const trigram &o) = default;

    friend bool operator<(trigram const &a, trigram const &b);

    friend bool operator==(trigram const &a, trigram const &b);

    friend bool operator!=(trigram const &a, trigram const &b);

    friend bool operator>(trigram const &a, trigram const &b);

    void add(unsigned char ch);

    friend std::ostream &operator<<(std::ostream &s, trigram const &a);

    friend std::istream &operator>>(std::istream &s, trigram &a);
};

static trigram NULL_T(0, 0, 0);

#endif // TRIGRAM_H
