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
#include <srcPtrPolicyTemplates.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>

#include <CLI11.hpp>

int main(int argc, char *argv[])
{
   bool runTimer = false, graphvizOutput = false, useAndersen = false,
         useSteensgaard = false;
   std::string inputFile = "";
   
   CLI::App app{"srcPtr: A lightweight, fast, and flexible 'close to the code' pointer analysis tool.  "};
   // Options hold extra data
   app.add_option  ("-i, --input",  inputFile,              "Name of srcML file of source code with --position option");
   // flags are booleans
   app.add_flag  ("-a, --andersen",  useAndersen,         "Use Andersen's pointer analysis algorithm");
   app.add_flag  ("-s, --steensgaard",  useSteensgaard,   "Use Steensgaard's pointer analysis algorithm");
   app.add_flag  ("-g, --graphviz",  graphvizOutput,      "Generate GraphViz output");
   app.add_flag  ("-t, --timer",  runTimer,               "Measure time it takes srcPtr to execute");
   
   CLI11_PARSE(app, argc, argv);

   try
   {
      auto start = std::chrono::high_resolution_clock::now();

      srcPtrDeclPolicy *declpolicy = new srcPtrDeclPolicy();

      // First Run
      srcSAXController control(inputFile.c_str());
      srcSAXEventDispatch::srcSAXEventDispatcher<> handler{declpolicy}; // TODO: correct policy usage
      control.parse(&handler);

      if (runTimer)
      {
         auto end = std::chrono::high_resolution_clock::now();
         std::cerr << "\n\n"
                  << std::chrono::duration<double, std::milli>(end - start).count() << "ms passed from the first policy's execution." << std::endl;
         start = std::chrono::high_resolution_clock::now();
      }

      if (useAndersen)
      {
         srcPtrDeclPolicy::srcPtrDeclData declData = declpolicy->GetData();

         srcPtrAndersen *data;
         srcPtrPolicy<srcPtrAndersen> *policy = new srcPtrPolicy<srcPtrAndersen>(declData);

         // Second Run
         srcSAXController control2(inputFile.c_str());
         srcSAXEventDispatch::srcSAXEventDispatcher<> handler2{policy};
         control2.parse(&handler2);

         data = policy->GetData();
         if (graphvizOutput)
            data->PrintGraphViz();
         else
            data->Print();
      }
      else if (useSteensgaard)
      {

         srcPtrDeclPolicy::srcPtrDeclData declData = declpolicy->GetData();

         srcPtrSteensgaard *data;
         srcPtrPolicy<srcPtrSteensgaard> *policy = new srcPtrPolicy<srcPtrSteensgaard>(declData);

         // Second Run
         srcSAXController control2(inputFile.c_str());
         srcSAXEventDispatch::srcSAXEventDispatcher<> handler2{policy};
         control2.parse(&handler2);

         data = policy->GetData();
         if (graphvizOutput)
            data->PrintGraphViz();
         else
            data->Print();
      }
      else
      {
         srcPtrPolicy<srcPtrEmptyAlgorithm> *policy = new srcPtrPolicy<srcPtrEmptyAlgorithm>(declpolicy->GetData());

         // Second Run
         srcSAXController control2(inputFile.c_str());
         srcSAXEventDispatch::srcSAXEventDispatcher<> handler2{policy};
         control2.parse(&handler2);

         std::cout << "You specified no algorithm so no pointer data was collected. Please specify an algorithm as defined in --help." << std::endl;
      }

      if (runTimer)
      {
         auto end = std::chrono::high_resolution_clock::now();
         std::cerr << "\n\n"
                  << std::chrono::duration<double, std::milli>(end - start).count() << "ms passed from the second policy's execution." << std::endl;
      }

      std::cout << std::endl;
   } catch (std::string& error) {
      std::cerr << "Error: " << error << std::endl;
   } catch (SAXError& error) {
      std::cerr << "Error: " << error.message << " " << error.error_code << std::endl;
   }

   return 0;
}