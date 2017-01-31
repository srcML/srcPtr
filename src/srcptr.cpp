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
#include <chrono>

#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {

   namespace po = boost::program_options;
   
   po::options_description desc("Options");
   desc.add_options()
      ("help", "produce help message")
      ("graphviz", "generate graphViz output")
      ("timer,t", "measure time it takes srcPtr to execute")
      ("input",po::value<std::vector<std::string>>()->required(), "name of srcML file to analyze")
   ;

   po::positional_options_description p;
   p.add("input", -1);

   po::variables_map vm;
   po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm); 


   if(vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
   }

   if(vm.count("input")) {
      auto start = std::chrono::high_resolution_clock::now();

      std::ifstream srcmlfile(vm["input"].as< std::vector<std::string> >()[0]);
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

      if(vm.count("graphviz"))
         data->PrintGraphViz();
      else
         data->Print();
      delete data;

      if(vm.count("timer")) {
         auto end = std::chrono::high_resolution_clock::now();
         std::cout << "\n\n" << std::chrono::duration<double, std::milli>(end-start).count() << "ms passed from program start." << std::endl;
      }
   }

   return 0;
}
