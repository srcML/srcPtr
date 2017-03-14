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

   void AddPointsToRelationship(srcPtrVar lhs, srcPtrVar rhs) { }

   void AddAssignmentRelationship(srcPtrVar lhs, srcPtrVar rhs) { }

   void Print() { }

   void PrintGraphViz() { }

   std::vector<srcPtrVar> GetPointsTo(srcPtrVar ptr) { return std::vector<srcPtrVar>(); }

   std::vector<srcPtrVar> GetPointers() { return std::vector<srcPtrVar>(); }

   srcPtrEmptyAlgorithm *Clone() const { return (new srcPtrEmptyAlgorithm(*this)); }
};

class srcPtrDataMap {
public:
   ~srcPtrDataMap() { }; 

   void AddPointsToRelationship(srcPtrVar lhs, srcPtrVar rhs) {
      if (std::find(data[lhs].begin(), data[lhs].end(), rhs) == data[lhs].end()) {
         data[lhs].push_back(rhs);// Adds reference only if lhs doesn't already point to rhs
      }
   }

   void AddAssignmentRelationship(srcPtrVar lhs, srcPtrVar rhs) { //Same as function above, for this implementation.
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

   std::vector<srcPtrVar> GetPointsTo(srcPtrVar ptr) const {
      return data.at(ptr); // No const overload for operator[]
   }

	std::vector<srcPtrVar> GetPointers() const {
		std::vector<srcPtrVar> pointers;
		for(auto x : data)
			pointers.push_back(x.first);
		return pointers;
	}

   srcPtrDataMap *Clone() const {
      return (new srcPtrDataMap(*this));
   }

private:
   std::map<srcPtrVar, std::vector<srcPtrVar>> data;
};

class srcPtrAnderson {
public:
   ~srcPtrAnderson() { }; 

   void AddPointsToRelationship(srcPtrVar lhs, srcPtrVar rhs) {
      if (std::find(pointsto[lhs].begin(), pointsto[lhs].end(), rhs) == pointsto[lhs].end()) {
         pointsto[lhs].push_back(rhs);// Adds reference only if lhs doesn't already point to rhs
      }
   }

   void AddAssignmentRelationship(srcPtrVar lhs, srcPtrVar rhs) {
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

   std::vector<srcPtrVar> GetPointsTo(srcPtrVar ptr) {
      Finalize();
      return pointsto.at(ptr); // No const overload for operator[]
   }

   std::vector<srcPtrVar> GetPointers() {
      Finalize();
      std::vector<srcPtrVar> pointers;
      for(auto x : pointsto)
         pointers.push_back(x.first);
      return pointers;
   }

   srcPtrAnderson *Clone() const {
      return (new srcPtrAnderson(*this));
   }

private:

   //Resolves dependencies of each pointer
   void Finalize() {
      for(auto var : pointsto) {
         FinalizeVar(var.first);
      }
      for(auto var : pointerqueue) {
         FinalizeVar(var.first);
      }
   }

   //Resolves dependencies of pointer
   // TODO: an infinite loop is possible with cyclic dependencies of two pointers. (x = y; y = x;)
   void FinalizeVar(srcPtrVar ptr) {
      for (auto var : pointerqueue[ptr]) {
         if(pointerqueue[var].size() == 0) {
            for (auto element : pointsto[var])
               pointsto[ptr].push_back(element);
         } else {
            FinalizeVar(var);
            for (auto element : pointsto[var])
               pointsto[ptr].push_back(element);
         }
      }
      pointerqueue[ptr].clear();
   }

   std::map<srcPtrVar, std::vector<srcPtrVar>> pointsto;
   std::map<srcPtrVar, std::vector<srcPtrVar>> pointerqueue;
};

/*
class srcPtrDataSteensgaard {
public:
   void AddPointsToRelationship(srcPtrVar lhs, srcPtrVar rhs) {
		
   }

   void Print() {

   }

   std::vector<srcPtrVar> GetPointsTo(srcPtrVar ptr) {

   }
private:
	std::vector<int>  rank (100);
	std::vector<int>  parent (100);
   disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);
};*/

#endif
