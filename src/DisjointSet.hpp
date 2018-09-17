/**
 * @file DisjointSet.hpp
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

#ifndef INCLUDED_DISJOINT_SET_HPP
#define INCLUDED_DISJOINT_SET_HPP

#include <srcPtrUtilities.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>


class DisjointSet {
public:

   struct DisjointSetElement {
      Variable data;
      DisjointSetElement* nextElement;
      std::list<DisjointSetElement*> previousElement;
   };

   ~DisjointSet() {
      for (auto element : map) {
         delete element.second;
      }
   }

   void Union(Variable a, Variable b) {
      DisjointSetElement* aTopOfTree = Find(a);
      DisjointSetElement* bTopOfTree = Find(b);

      bTopOfTree->nextElement = aTopOfTree;
      aTopOfTree->previousElement.push_back(bTopOfTree);
   };

   DisjointSetElement* Find(Variable toFind) {
 
      DisjointSetElement* currentElement = map[toFind.UniqueIdentifier()];

      while (currentElement->nextElement != currentElement) {
         currentElement = currentElement->nextElement;
      }

      return currentElement;
   };

   void MakeSet(Variable variable) {
      if (map.count(variable.UniqueIdentifier()) == 0) {
         DisjointSetElement* newElement = new DisjointSetElement;

         newElement->data = variable;
         newElement->nextElement = newElement;

         map[variable.UniqueIdentifier()] = newElement;
      }
   };

   std::list<Variable> GetSet(Variable variable) {
      DisjointSetElement* topOfTree = Find(variable);
      std::list<Variable> list;

      AddSubSet(topOfTree, &list);

      return list;
   };


private:
   std::unordered_map<std::string, DisjointSetElement*> map;

   void AddSubSet(DisjointSetElement* element, std::list<Variable>* list) {
      list->push_back(element->data);

      for (DisjointSetElement* var : element->previousElement) {
         AddSubSet(var, list);
      }
   };
};

#endif
