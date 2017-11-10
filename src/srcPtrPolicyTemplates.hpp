/**
 * @file srcPtrPolicyTemplates.hpp
 *
 * @copyright Copyright (C) 2015-2016 srcML, LLC. (www.srcML.org)
 *
 * This file is part of srcPtr.
 *
 * srcPtr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * srcPtr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with srcPtr.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_SRC_PTR_POLICY_TEMPLATES_HPP
#define INCLUDED_SRC_PTR_POLICY_TEMPLATES_HPP

#include <DeclTypePolicy.hpp>
#include <srcPtrUtilities.hpp>


#include <set>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/pending/detail/disjoint_sets.hpp>

class srcPtrEmptyAlgorithm {
public:
   ~srcPtrEmptyAlgorithm() { }; 

   void AddPointsToRelationship(Variable lhs, Variable rhs) { }

   void AddAssignmentRelationship(Variable lhs, Variable rhs) { }

   void Print() { }

   void PrintGraphViz() { }

   std::vector<Variable> GetPointsTo(Variable ptr) { return std::vector<Variable>(); }

   std::vector<Variable> GetPointers() { return std::vector<Variable>(); }

   srcPtrEmptyAlgorithm *Clone() const { return (new srcPtrEmptyAlgorithm(*this)); }
};

class srcPtrAndersen {
public:
   const int MAX_DEPTH = 50; //Max recursion depth for computing the transitive closure. Used to prevent infinite loops with cyclic dependencies of pointers. 

   ~srcPtrAndersen() { }; 

   void AddPointsToRelationship(Variable lhs, Variable rhs) {
      pointsto[lhs].insert(rhs);// Adds reference only if lhs doesn't already point to rhs
   }

   void AddAssignmentRelationship(Variable lhs, Variable rhs) {
      if (std::find(pointerqueue[lhs].begin(), pointerqueue[lhs].end(), rhs) == pointerqueue[lhs].end()) {
         pointerqueue[lhs].push_back(rhs);
      }
   }

   void Print() {
      Finalize();
      std::cout << "_____________________________________________________" << std::endl << std::endl;
      for (auto x : pointsto) {
         std::cout << x.first.linenumber << " - " << x.first.nameoftype << (x.first.isPointer ? " * " : " & ") << x.first << std::endl << std::endl;
         for (auto y : x.second)
               std::cout << y.linenumber << " - " << y.nameoftype << " " << y << " " << std::endl;
         std::cout << "_____________________________________________________" << std::endl << std::endl;
      }
   }

   void PrintGraphViz() {
      Finalize();
      std::cout << "digraph pointers {\n";

      for (auto x : pointsto) {
         for (auto y : x.second) {
            if(y.nameofidentifier != "")
               std::cout << "   \"" << x.first << "\" -> \"" << y << "\" \n";
         }
      }

      std::cout << "}";
   }

   std::vector<Variable> GetPointsTo(Variable ptr) {
      Finalize();
      std::set<Variable> x = pointsto.at(ptr); // No const overload for operator[]
      
      std::vector<Variable> result; 
      for(Variable member : x) 
         result.push_back(member);
      return result; 
   }

   std::vector<Variable> GetPointers() {
      Finalize();
      std::vector<Variable> pointers;
      for(auto x : pointsto)
         pointers.push_back(x.first);
      return pointers;
   }

   srcPtrAndersen *Clone() const {
      return (new srcPtrAndersen(*this));
   }

   //Resolves dependencies of each pointer
   void Finalize() {
      for(auto var : pointsto) {
         FinalizeVar(var.first, 0);
      }
      for(auto var : pointerqueue) {
         FinalizeVar(var.first, 0);
      }
   }

private:

   //Resolves dependencies of pointer
   void FinalizeVar(Variable ptr, int depth) {
      if(depth < MAX_DEPTH) {
         for (auto var : pointerqueue[ptr]) {
            if(pointerqueue[var].size() == 0) {
               for (auto element : pointsto[var])
                  pointsto[ptr].insert(element);
            } else {
               FinalizeVar(var, ++depth); 
               for (auto element : pointsto[var])
                  pointsto[ptr].insert(element);
            }
         }
         pointerqueue[ptr].clear();
      }
   }

   std::map<Variable, std::set<Variable>> pointsto;
   std::map<Variable, std::vector<Variable>> pointerqueue;
};

/*
class srcPtrDataSteensgaard {
public:
   void AddPointsToRelationship(Variable lhs, Variable rhs) {
		
   }

   void Print() {

   }

   std::vector<Variable> GetPointsTo(Variable ptr) {

   }
private:
	std::vector<int>  rank (100);
	std::vector<int>  parent (100);
   disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);
};*/

#endif
