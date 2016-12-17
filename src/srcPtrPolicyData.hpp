#ifndef INCLUDED_SRC_PTR_POLICY_DATA_HPP
#define INCLUDED_SRC_PTR_POLICY_DATA_HPP

#include <DeclTypePolicy.hpp>
#include <srcPtrUtilities.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/pending/detail/disjoint_sets.hpp>

class srcPtrData {
public:
   virtual void AddPointsToRelationship(srcPtrVar, srcPtrVar) = 0;
   virtual void Print() const = 0;
   virtual std::vector<srcPtrVar> GetPointsTo(srcPtrVar) const = 0;
   virtual srcPtrData* Clone() const = 0;
};

class srcPtrDataMap : public srcPtrData {
public:
   void AddPointsToRelationship(srcPtrVar lhs, srcPtrVar rhs) {
      if (std::find(data[lhs].begin(), data[lhs].end(), rhs) == data[lhs].end()) {
         data[lhs].push_back(rhs); // Adds reference only if lhs doesn't already point to rhs
      }
   }

   void Print() const {
      for (auto x : data) {
         std::cout << x.first.linenumber << " : " << x.first.nameoftype << " " << x.first << " ";
         for (auto y : x.second)
            std::cout << y << " ";
         std::cout << std::endl;
      }
   }

   std::vector<srcPtrVar> GetPointsTo(srcPtrVar ptr) const {
      return data.at(ptr); //No const overload for operator[]
   }

   virtual srcPtrDataMap* Clone() const {
      return (new srcPtrDataMap(*this));
   }

private:
   std::map<srcPtrVar, std::vector<srcPtrVar>> data;
};

/*
class srcPtrDataSteensgaard : srcPtrData {
public:
   void AddPointsToRelationship(srcPtrVar lhs, srcPtrVar rhs) {

   }

   void Print() {

   }

   std::vector<srcPtrVar> GetPointsTo(srcPtrVar ptr) {

   }
private:
   disjoint_sets<
};*/

#endif
