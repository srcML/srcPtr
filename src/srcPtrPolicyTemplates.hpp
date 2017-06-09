#ifndef INCLUDED_SRC_PTR_POLICY_TEMPLATES_HPP
#define INCLUDED_SRC_PTR_POLICY_TEMPLATES_HPP

#include <DeclTypePolicy.hpp>
#include <srcPtrUtilities.hpp>

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

class srcPtrDataMap {
public:
   ~srcPtrDataMap() { }; 

   void AddPointsToRelationship(Variable lhs, Variable rhs) {
      if (std::find(data[lhs].begin(), data[lhs].end(), rhs) == data[lhs].end()) {
         data[lhs].push_back(rhs);// Adds reference only if lhs doesn't already point to rhs
      }
   }

   void AddAssignmentRelationship(Variable lhs, Variable rhs) { //Same as function above, for this implementation.
      if (std::find(data[lhs].begin(), data[lhs].end(), rhs) == data[lhs].end()) {
         data[lhs].push_back(rhs);
      }
   }

   void Print() const {
      for (auto x : data) {
         std::cout << x.first.linenumber << " - " << x.first.nameoftype << (x.first.isPointer ? " * " : " & ") << x.first << std::endl;
         for (auto y : x.second)
			      std::cout << y.linenumber << " - " << y.nameoftype << " " << y << " " << std::endl;
         std::cout << std::endl << std::endl << std::endl;
      }
   }

   void PrintGraphViz() const {
      std::cout << "digraph pointers {\n";

      for (auto x : data) {
         for (auto y : x.second) {
            if(y.nameofidentifier != "")
               std::cout << "   " << x.first << " -> " << y << "\n";
         }
      }

      std::cout << "}";
   }

   std::vector<Variable> GetPointsTo(Variable ptr) const {
      return data.at(ptr); // No const overload for operator[]
   }

	std::vector<Variable> GetPointers() const {
		std::vector<Variable> pointers;
		for(auto x : data)
			pointers.push_back(x.first);
		return pointers;
	}

   srcPtrDataMap *Clone() const {
      return (new srcPtrDataMap(*this));
   }

private:
   std::map<Variable, std::vector<Variable>> data;
};

class srcPtrAndersen {
public:
   const int MAX_DEPTH = 200; //Max recursion depth for computing the transitive closure. Used to prevent infinite loops with cyclic dependencies of pointers. 

   ~srcPtrAndersen() { }; 

   void AddPointsToRelationship(Variable lhs, Variable rhs) {
      if (std::find(pointsto[lhs].begin(), pointsto[lhs].end(), rhs) == pointsto[lhs].end()) {
         pointsto[lhs].push_back(rhs);// Adds reference only if lhs doesn't already point to rhs
      }
   }

   void AddAssignmentRelationship(Variable lhs, Variable rhs) {
      if (std::find(pointerqueue[lhs].begin(), pointerqueue[lhs].end(), rhs) == pointerqueue[lhs].end()) {
         pointerqueue[lhs].push_back(rhs);
      }
   }

   void Print() {
      Finalize();
      for (auto x : pointsto) {
         std::cout << x.first.linenumber << " - " << x.first.nameoftype << (x.first.isPointer ? " * " : " & ") << x.first << std::endl;
         for (auto y : x.second)
               std::cout << y.linenumber << " - " << y.nameoftype << " " << y << " " << std::endl;
         std::cout << std::endl << std::endl << std::endl;
      }
   }

   void PrintGraphViz() {
      Finalize();
      std::cout << "digraph pointers {\n";

      for (auto x : pointsto) {
         for (auto y : x.second) {
            if(y.nameofidentifier != "")
               std::cout << "   " << x.first << " -> " << y << "\n";
         }
      }

      std::cout << "}";
   }

   std::vector<Variable> GetPointsTo(Variable ptr) {
      Finalize();
      return pointsto.at(ptr); // No const overload for operator[]
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

private:

   //Resolves dependencies of each pointer
   void Finalize() {
      for(auto var : pointsto) {
         FinalizeVar(var.first, 0);
      }
      for(auto var : pointerqueue) {
         FinalizeVar(var.first, 0);
      }
   }

   //Resolves dependencies of pointer
   void FinalizeVar(Variable ptr, int depth) {
      if(depth > MAX_DEPTH) {
         for (auto var : pointerqueue[ptr]) {
            if(pointerqueue[var].size() == 0) {
               for (auto element : pointsto[var])
                  pointsto[ptr].push_back(element);
            } else {
               FinalizeVar(var, ++depth);
               for (auto element : pointsto[var])
                  pointsto[ptr].push_back(element);
            }
         }
         pointerqueue[ptr].clear();
      }
   }

   std::map<Variable, std::vector<Variable>> pointsto;
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
