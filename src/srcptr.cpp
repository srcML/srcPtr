/**
 * @file srcptr.hpp
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
#include <iostream>
#include <fstream>
#include <string>
#include <srcPtrDeclPolicy.hpp>
#include <srcPtrPolicy.hpp>
#include <srcPtrSingleton.hpp>

class srcPtrDeclPolicyContainer : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:
        ~srcPtrDeclPolicyContainer(){}
        srcPtrDeclPolicyContainer(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){}
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            decldata = *policy->Data<srcPtrDeclPolicy::srcPtrDeclData>();
        }
    protected:
        void * DataInner() const override {
            return (void*)0; //To silence the warning
        }
    public:
        srcPtrDeclPolicy::srcPtrDeclData decldata;
};

class ptrPolicyContainer : public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener{
    public:
        ~ptrPolicyContainer(){}
        ptrPolicyContainer(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){}
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            pointerdata = *policy->Data<srcPtrPolicy::srcPtrPolicyData>();
        }
    protected:
        void * DataInner() const override {
            return (void*)0; //To silence the warning
        }
    public:
        srcPtrPolicy::srcPtrPolicyData pointerdata;
};

int main(int argc, char * argv[]) {
  if(argc == 2) {
    std::string srcmlstr;

    std::string line;
    std::ifstream srcmlfile (argv[1]);
    if (srcmlfile.is_open())
    {
      while (getline (srcmlfile,line))
      {
        srcmlstr = srcmlstr + (line + "\n");
      }
      srcmlfile.close();
    }
    else {
      std::cout << "Error reading file." << std::endl;
      return 1;
    }


    //First Run
    srcPtrDeclPolicyContainer sigData;
    srcSAXController control(srcmlstr);
    srcSAXEventDispatch::srcSAXEventDispatcher<srcPtrDeclPolicy> handler {&sigData};
    control.parse(&handler);

    srcPtrSingleton::Instance()->SetValue(sigData.decldata);

    //Second Run
    ptrPolicyContainer ptrpolicy;
    srcSAXController control2(srcmlstr);
    srcSAXEventDispatch::srcSAXEventDispatcher<srcPtrPolicy> handler2 {&ptrpolicy};
    control2.parse(&handler2);

    for(auto x : ptrpolicy.pointerdata.references) {
      std::cout << x.first << " ";
      for(auto y : x.second)
        std::cout << y << " ";
      std::cout << std::endl;
    }
}
  return 0;
}
