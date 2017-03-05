#ifndef INCLUDED_SRC_PTR_POLICY_TEMPLATES_HPP
#define INCLUDED_SRC_PTR_POLICY_TEMPLATES_HPP

#include <DeclTypePolicy.hpp>
#include <srcPtrUtilities.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/pending/detail/disjoint_sets.hpp>

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

   virtual srcPtrDataMap *Clone() const {
      return (new srcPtrDataMap(*this));
   }

private:
   std::map<srcPtrVar, std::vector<srcPtrVar>> data;
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
