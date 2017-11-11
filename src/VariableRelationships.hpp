/**
 * @file VariableRelationships.hpp
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

#ifndef INCLUDED_VARIABLE_RELATIONSHIPS
#define INCLUDED_VARIABLE_RELATIONSHIPS

#include <srcPtrUtilities.hpp>

#include <map>
#include <set>
#include <iostream>

class VariableRelationships {
public:

   void AddRelationship(Variable who, Variable what) {
      relationships[who].insert(what);
   };

   std::set<Variable> GetRelationships(Variable who) {
      return relationships[who];
   };

   void Print() {
      std::cout << "_____________________________________________________" << std::endl << std::endl;
      for (auto x : relationships) {
         std::cout << x.first.linenumber << " - " << x.first.nameoftype << (x.first.isPointer ? "*" : "") << (x.first.isReference ? "&" : "") << " " << x.first << std::endl << std::endl;
         for (auto y : x.second)
               std::cout << y.linenumber << " - " << y.nameoftype << " " << y << " " << std::endl;
         std::cout << "_____________________________________________________" << std::endl << std::endl;
      }
   };

   void PrintGraphViz() {
      std::cout << "digraph pointers {\n";

      for (auto x : relationships) {
         for (auto y : x.second) {
            if(y.nameofidentifier != "")
               std::cout << "   \"" << x.first << "\" -> \"" << y << "\" \n";
         }
      }

      std::cout << "}";
   };

   std::vector<Variable> GetKeys() {
      std::vector<Variable> keys;
      for(auto x : relationships)
         keys.push_back(x.first);
      return keys;
   }

   void Clear() {
      relationships.clear();
   }

   void ClearRelationships(Variable var) {
      relationships[var].clear();
   }

private:

   std::map<Variable, std::set<Variable>> relationships;
};

#endif