//
//  main.cpp
//  graphlib
//
//  Created by Yann Orlarey on 31/01/2017.
//  Copyright © 2017-2021 GRAME. All rights reserved.
//

#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include "dglib/arrow.hh"
#include "dglib/csrgraph.hh"
#include "dglib/digraph.hh"
#include "dglib/digraphop.hh"
#include "dglib/stdprinting.hh"
#include "mtests.hh"
#include "tests.hh"
#include "absl/strings/str_join.h"
#include "absl/container/flat_hash_map.h"

void newtest()
{
    digraph<char, multidep> g;
    g.add('A', 'B', {{{"M", 2}, {"N", 0}}, 0});
    std::cout << "g = " << g << std::endl;
    dotfile(std::cout, g);
}

int main(int, const char**)
{
    // std::cout << "Tests digraph library\n";
    // // newtest();

    // absl::flat_hash_map<int, int> m;
    // m[1] = 2;
    // std::cout << m[1] << '\n';
    // bool r = true;
    // r &= mcheck0();
    // r &= mcheck1();
    // r &= mcheck2();
    // r &= mcheck3();
    // r &= mcheck4();
    // r &= mcheck5();
    // r &= check1();
    // // r &= check2();
    // // r &= check3();
    // r &= check4();
    // r &= check5();
    // // r &= check7();
    // r &= check8();
    // r &= check9();
    // r &= check10();
    // r &= check11();
    // r &= check12();
    // r &= check13();
    // // r &= check14();
    // // r &= check15();

    // return (r) ? 0 : 1;

    csrgraph<char, int> g1;
    g1.add('c', 'd', 1);
    g1.add('c', 'd', 1);
    g1.add('c', 'e', 3);
    g1.add('d', 'g', 1);
    std::cout << "Graph g1:\n";
    std::cout << g1;

    std::cout << '\n';

    csrgraph<char, int> g2;
    g2.add('q', 'r', 1);
    g2.add('f', 't', 1);
    g2.add('r', 'f', 3);
    g2.add('z', 'y', 1);
    g2.add('x');
    g2.add('m', 'p');
    std::cout << "Graph g2:\n";
    std::cout << g2;

    std::cout << '\n'
    ;
    csrgraph<char, int> g3 = g1 + g2;
    std::cout << "Graph g3:\n";
    std::cout << g3;

    std::cout << "Are d & g connected? " << std::boolalpha << g3.areConnected('d', 'g') << '\n';
    std::cout << "Are c & y connected? " << std::boolalpha << g3.areConnected('c', 'y') << '\n';
    return 0;
}
