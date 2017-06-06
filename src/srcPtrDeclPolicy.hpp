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
         FunctionSignaturePolicy::SignatureData signatureData = *policy->Data<FunctionSignaturePolicy::SignatureData>();
         data.functionTracker.AddFunction(signatureData);
      } else if (typeid(ClassPolicy) == typeid(*policy)) {
         Class classData = *policy->Data<Class>();
         data.classTracker.AddClass(classData);
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
