/*******************************************************************************
********************************************************************************

    csrgraph: compressed sparse row directed graph

    Created by Seth Dale on 03/12/2021.
    Adapted from digraph.hh.
    Copyright © 2017-2021 Grame. All rights reserved.

 *******************************************************************************
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <stack>

#include "arrow.hh"
#include "absl/strings/str_join.h"
#include "absl/container/flat_hash_map.h"


//=====================================================================
// csrgraph : a directed graph, a set of nodes f type N and a
// set of connections between these nodes of type A. Connections have an
// associated value, by defaut 0. This value is used in Faust
// to represent the time dependency between computations.
// csrN and csrF follow the algorithm from "Programming Workbench
// Compressed Sparse Row Format for Representing Graphs" by Terence Kelly
//=====================================================================

template <typename N, typename A>
class csrgraph {
  public:
    using policy = arrow_traits<A>;

  private:
    //=================================================================
    // Real/internal structure of a graph. A graph is a set of nodes
    // and a set of connections between theses nodes. These connections
    // have an integer value attached.
    //=================================================================
    class internalgraph {
      private:
        absl::flat_hash_map<N, int>   fNodes;  //Key: node, Val: idx to relate to fNodes
        absl::flat_hash_map<int, int> csrN;    //Key: idx from fNodes, Val: connections in F (CSR algo)
        std::vector<std::pair<N, A> > csrF;    //Stores edges in CSR order

      public:
        // Find edge in graph
        bool find_edge(const N& n1, const N& n2) {
          if (fNodes.count(n1) == 0 or fNodes.count(n2) == 0) {
            return false;
          }
          for (int i = csrN[fNodes[n1]-1]; i < csrN[fNodes[n1]]; ++i) {
            if (csrF.at(i).first == n2) {
              return true;
            }
          }
          return false;
        }
        // Helper function to update csr_N as dictated by CSR algorithm when a new edge is added
        void incr_N(const N& n) {
          for (int i = fNodes[n]; i <= fNodes.size(); ++i) {
              csrN[i]++; //need to increment entire array
          }
        }
        // Add node to graph, updated fNodes and csrN as dictated by CSR algorithm
        void add(const N& n) {
            if (fNodes.count(n)) {
              return;
            }
            const int idx = fNodes.size() + 1;
            fNodes[n] = idx; // add node to node list, store index to reference N
            csrN.insert({idx, csrN[idx-1]});
            return;
        }
        // Add nodes + edge to graph, updated fNodes, csrN, and csrF as dictated by CSR algorithm
        void add(const N& n1, const N& n2, const A& d) {
            add(n1);
            if (find_edge(n1, n2)) {
              return;
            }
            incr_N(n1);
            add(n2);
            csrF.insert(csrF.begin() + csrN[fNodes[n1]]-1, {n2, d});
            return;
        }
        // Get nodes
        const absl::flat_hash_map<N, int>& nodes() const { return fNodes; }

        // Get edge vector for a given node
        const std::vector<std::pair<N, A> > connections(const N& n) {
          int size = csrN[fNodes[n]] - csrN[fNodes[n]-1];
          std::vector<std::pair<N, A> > e(size);
          std::copy(csrF.begin() + csrN[fNodes[n]-1], csrF.begin() + csrN[fNodes[n]], e.begin());
          return e;
        }

        // Test if two nodes are connected by d. Edge must have val d to return true
        bool areConnected(const N& n1, const N& n2, A& d) const {
          auto c = connections(n1);
          auto q = find(c.begin(), c.end(), {n2, d}); //TODO: switch to hashtable for faster find
          return (q == c.end() ? false : true);
        }

        bool areConnected(const N& n1, const N& n2) const {
          A d;
          return areConnected(n1, n2, d);
        }
    };
    std::shared_ptr<internalgraph> fContent;

  public:
    csrgraph() : fContent(new internalgraph) {}

    const absl::flat_hash_map<N, int>& nodes() const { return fContent->nodes(); }
    const std::vector<std::pair<N, A> > connections(const N& n) const { return fContent->connections(n); }

    bool areConnected(const N& n1, const N& n2, A& d) const { return fContent->areConnected(n1, n2, d); }
    bool areConnected(const N& n1, const N& n2) const { return fContent->areConnected(n1, n2); }


    void print_nodes() {
      std::cout << "Nodes: ";
      for (auto& [k, v] : nodes()) {std::cout << k << ' ';}
      std::cout << '\n';
      return;
    }

    void print_edges() {
      std::cout << "Edges: \n";
      for (auto& n : nodes()) {
        for (auto& c : connections(n.first)) {
          std::cout << n.first << "-" << c.second << "->" << c.first << '\n';
        }
      }
    }

    // Add node to graph's internal structure
    csrgraph& add(N n) {
      fContent->add(n);
      return *this;
    }

    csrgraph& add(const N& n1, const N& n2, const A& d) {
      fContent->add(n1, n2, d);
      return *this;
    }

    // Add graph to currgraph
    csrgraph& add(const csrgraph& g) {
      // get nodes and edges from g
      for (auto& n : g.nodes()) {
        add(n.first);
        for (auto& c : g.connections(n.first)) {
          add(n.first, c.first, c.second);
        }
      }
      return *this;
    }
    csrgraph<N, A> operator+(csrgraph<N, A> g) {
      add(g);
      return *this;
    }
    friend std::ostream& operator<<(std::ostream& out, csrgraph g) {
      g.print_nodes();
      g.print_edges();
      return out;
    }
    friend bool operator<(const csrgraph& p1, const csrgraph& p2) { return p1.fContent < p2.fContent; }
    friend bool operator==(const csrgraph& p1, const csrgraph& p2) { return p1.fContent == p2.fContent; }
};
