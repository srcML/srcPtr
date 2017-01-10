/**
 * @file srcptr.cpp
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

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrDeclPolicy.hpp>
#include <srcPtrPolicy.hpp>
#include <srcPtrPolicyData.hpp>

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
   bool graphViz = false;

   if (argc > 2) {

      for (size_t i = 1; i < argc; i++) {
         if (std::string(argv[i]) == "--graphviz")
            graphViz = true;
      }

      std::ifstream srcmlfile(argv[1]);
      std::string srcmlstr((std::istreambuf_iterator<char>(srcmlfile)), std::istreambuf_iterator<char>());

      // First Run
      srcPtrDeclPolicy *declpolicy = new srcPtrDeclPolicy();
      srcSAXController control(srcmlstr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler{declpolicy};
      control.parse(&handler);

      // Second Run
      srcPtrPolicy *policy = new srcPtrPolicy(declpolicy->GetData(), new srcPtrDataMap());
      srcSAXController control2(srcmlstr);
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler2{policy};
      control2.parse(&handler2);

      srcPtrData const *data = policy->GetData();
      if(!graphViz)
         data->Print();
      else
         data->PrintGraphViz();
      delete data;
   }
   return 0;
}
