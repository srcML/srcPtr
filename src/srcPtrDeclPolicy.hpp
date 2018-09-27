/**
 * @file srcPtrDeclPolicy.hpp
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

#ifndef INCLUDED_SRC_PTR_DECL_POLICY_HPP
#define INCLUDED_SRC_PTR_DECL_POLICY_HPP

#include <DeclTypePolicy.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <ClassPolicy.hpp>

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrUtilities.hpp>

#include <iostream>
#include <string>
#include <vector>

class srcPtrDeclPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
public:
   struct srcPtrDeclData {
      ClassTracker classTracker;
      FunctionTracker functionTracker;
   };

   ~srcPtrDeclPolicy() {
      if (functionSignaturePolicy)
         delete functionSignaturePolicy;
      if (classPolicy)
         delete classPolicy;
   }

   srcPtrDeclPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) {
      functionSignaturePolicy = new FunctionSignaturePolicy({this});
      classPolicy = new ClassPolicy({this});
      InitializeEventHandlers();
   }

   void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
      if (typeid(FunctionSignaturePolicy) == typeid(*policy)) {
         SignatureData signatureData = *policy->Data<SignatureData>();
         data.functionTracker.AddFunction(signatureData);
      } else if (typeid(ClassPolicy) == typeid(*policy)) {
         std::vector<Class> classData = *policy->Data<std::vector<Class>>();
         for(int i = 0; i < classData.size(); ++i) {
            data.classTracker.AddClass(classData[i]);
         }
      }
   }

   srcPtrDeclData GetData() {
      return data;
   }

   srcPtrDeclData data;

   protected:
   void *DataInner() const override {
      return new srcPtrDeclData(data);
   }

   private:
   FunctionSignaturePolicy *functionSignaturePolicy = nullptr;
   ClassPolicy *classPolicy = nullptr;

   void InitializeEventHandlers() {
      using namespace srcSAXEventDispatch;

      openEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
         ctx.dispatcher->AddListenerDispatch(functionSignaturePolicy);
         ctx.dispatcher->AddListenerDispatch(classPolicy);
      };

      closeEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
         ctx.dispatcher->RemoveListenerDispatch(functionSignaturePolicy);
         ctx.dispatcher->RemoveListenerDispatch(classPolicy);
         NotifyAll(ctx);
      };
   }
};

#endif
