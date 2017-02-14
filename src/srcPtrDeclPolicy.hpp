#ifndef INCLUDED_SRC_PTR_DECL_POLICY_HPP
#define INCLUDED_SRC_PTR_DECL_POLICY_HPP

#include <DeclTypePolicy.hpp>
#include <FunctionSignaturePolicy.hpp>

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrUtilities.hpp>

#include <iostream>
#include <string>
#include <vector>

class srcPtrDeclPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
public:
   struct srcPtrDeclData {
      FunctionTracker functionTracker;
   };

   ~srcPtrDeclPolicy() {
      if (functionSignaturePolicy)
         delete functionSignaturePolicy;
   }

   srcPtrDeclPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) {
      functionSignaturePolicy = new FunctionSignaturePolicy{this};
      InitializeEventHandlers();
   }

   void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
      if (typeid(FunctionSignaturePolicy) == typeid(*policy)) {
         FunctionSignaturePolicy::SignatureData signatureData = *policy->Data<FunctionSignaturePolicy::SignatureData>();
         data.functionTracker.AddFunction(signatureData);
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

   void InitializeEventHandlers() {
      using namespace srcSAXEventDispatch;

      openEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
         ctx.dispatcher->AddListenerDispatch(functionSignaturePolicy);
      };
      closeEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
         ctx.dispatcher->RemoveListenerDispatch(functionSignaturePolicy);
         NotifyAll(ctx);
         InitializeEventHandlers();
      };
   }
};

#endif
