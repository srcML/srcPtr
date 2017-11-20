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
#include <VariableRelationships.hpp>


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
   const int MAX_DEPTH = 10; //Max recursion depth for computing the transitive closure. Used to prevent infinite loops with cyclic dependencies of pointers. 

   ~srcPtrAndersen() { }; 

   void AddPointsToRelationship(Variable lhs, Variable rhs) {
      pointsto.AddRelationship(lhs, rhs);
   }

   void AddAssignmentRelationship(Variable lhs, Variable rhs) {
      pointerqueue.AddRelationship(lhs, rhs);
   }

   void Print() {
      Finalize();

      pointsto.Print();
   }

   void PrintGraphViz() {
      Finalize();

      pointsto.PrintGraphViz();
   }

   std::vector<Variable> GetPointsTo(Variable ptr) {
      Finalize();
      std::set<Variable> x = pointsto.GetRelationships(ptr); // No const overload for operator[]
      
      std::vector<Variable> result; 
      for(Variable member : x) 
         result.push_back(member);
      return result; 
   }

   std::vector<Variable> GetPointers() {
      Finalize();

      return pointsto.GetKeys();
   }

   srcPtrAndersen *Clone() const {
      return (new srcPtrAndersen(*this));
   }

   //Resolves dependencies of each pointer
   void Finalize() {
      std::vector<Variable> pointsto_keys = pointsto.GetKeys();
      for(auto var : pointsto_keys) {
         FinalizeVar(var, 0);
      }
      std::vector<Variable> pointerqueue_keys = pointerqueue.GetKeys();
      for(auto var : pointerqueue_keys) {
         FinalizeVar(var, 0);
      }
   }

private:

   //Resolves dependencies of pointer
   void FinalizeVar(Variable ptr, int depth) {
      if(depth < MAX_DEPTH) {
         for (auto var : pointerqueue.GetRelationships(ptr)) {
            if(pointerqueue.GetRelationships(var).size() == 0) {
               for (auto element : pointsto.GetRelationships(var))
                  pointsto.AddRelationship(ptr, element);
            } else {
               FinalizeVar(var, ++depth); 
               for (auto element : pointsto.GetRelationships(var))
                  pointsto.AddRelationship(ptr, element);
            }
         }

         pointerqueue.ClearRelationships(ptr);
      }
   }

   VariableRelationships pointsto;
   VariableRelationships pointerqueue;
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
