#ifndef INCLUDED_SRC_PTR_POLICY_HPP
#define INCLUDED_SRC_PTR_POLICY_HPP

#include <srcPtrDeclPolicy.hpp>
#include <srcPtrPolicyData.hpp>
#include <srcPtrUtilities.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class srcPtrPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
public:
   ~srcPtrPolicy() {
   }

   srcPtrPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) {
      InitializeEventHandlers();
   }

   void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
   }

   void SetDeclData(srcPtrDeclPolicy::srcPtrDeclData toset) {
      declData = toset;
   }

   srcPtrDataMap GetData() {
      return data;
   }

protected:
   void *DataInner() const override {
      return new srcPtrDataMap(data);
   }

private:
   void ResetVariables() {
      lhs.Clear();
      rhs.Clear();
      assignmentOperator = false;
      modifierlhs = "";
      modifierrhs = "";
   }

   srcPtrDataMap data;
   srcPtrDeclPolicy::srcPtrDeclData declData;

   // For use in collecting assignments
   srcPtrVar lhs;
   std::string modifierlhs;
   std::string modifierrhs;
   srcPtrVar rhs;
   bool assignmentOperator = false;

   void InitializeEventHandlers() {
      using namespace srcSAXEventDispatch;

      openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) { ResetVariables(); };

      closeEventMap[ParserState::name] = [this](srcSAXEventContext &ctx) {
         if (ctx.IsOpen(ParserState::expr)) {
            if (lhs.nameofidentifier == "") {
               DeclTypePolicy::DeclTypeData *decl = declData.FindVarWithIdentifier(ctx.currentToken);
               if (decl != nullptr) {
                  lhs = *decl;
               }
            } else {
               DeclTypePolicy::DeclTypeData *decl = declData.FindVarWithIdentifier(ctx.currentToken);
               if (decl != nullptr)
                  rhs = *decl;
            }
         }
      };

      closeEventMap[ParserState::op] = [this](srcSAXEventContext &ctx) {
         if ((ctx.currentToken == "=") && (lhs.nameofidentifier != ""))
            assignmentOperator = true;
         else if ((lhs.nameofidentifier == "")) {
            modifierlhs = ctx.currentToken;
         } else if ((rhs.nameofidentifier == ""))
            modifierrhs = ctx.currentToken;
      };

      closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
         bool lhsIsPointer = (modifierlhs != "*");
         bool rhsIsAddress = (((modifierrhs == "&") || (rhs.isPointer)) || (rhs.isReference));

         if ((lhsIsPointer && rhsIsAddress) && assignmentOperator) {
            data.AddPointsToRelationship(lhs, rhs);
         }
         ResetVariables();
      };

      closeEventMap[ParserState::function] = [this](srcSAXEventContext &ctx) { // End of Policy
         NotifyAll(ctx);
         InitializeEventHandlers();
      };
   }
};

#endif
