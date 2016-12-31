#ifndef INCLUDED_SRC_PTR_POLICY_HPP
#define INCLUDED_SRC_PTR_POLICY_HPP

#include <DeclTypePolicy.hpp>
#include <FunctionCallPolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrDeclPolicy.hpp>
#include <srcPtrPolicyData.hpp>
#include <srcPtrUtilities.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class srcPtrPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
public:
   ~srcPtrPolicy() {
      delete data;
      delete declTypePolicy;
      delete callPolicy;
   }
   srcPtrPolicy(srcPtrDeclPolicy::srcPtrDeclData decldata, srcPtrData *outputtype, std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) {
      declData = decldata;
      data = outputtype;
      declared.CreateFrame();
      declTypePolicy = new DeclTypePolicy{this};
      callPolicy = new CallPolicy{this};
      InitializeEventHandlers();
   }

   void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
      if (typeid(DeclTypePolicy) == typeid(*policy)) {
         DeclTypePolicy::DeclTypeData declarationData = *policy->Data<DeclTypePolicy::DeclTypeData>();
         declared.AddVarToFrame(srcPtrVar(declarationData));

         if((withinDeclAssignment) && ((declarationData.isPointer || declarationData.isReference)))   //Pointer assignment on initialization
            data->AddPointsToRelationship(declarationData, lhs);
      } else if (typeid(CallPolicy) == typeid(*policy)) {
         CallPolicy::CallData callData = *policy->Data<CallPolicy::CallData>();

         std::string calledFuncName;
         std::vector<std::string> params;
         bool pickedUpFuncName = false;
         for(auto it = callData.callargumentlist.begin(); it != callData.callargumentlist.end(); ++it) {
            if((*it != "(") && (*it != ")")) {
               if(!pickedUpFuncName) {
                  pickedUpFuncName = true;
                  calledFuncName = *it;
               }
               else
                  params.push_back(*it);
            }
         }

         srcPtrFunction called = declData.functionTracker.GetFunction(calledFuncName, params.size());

         int i = 0;
         for(auto it = params.begin(); it != params.end(); ++it) {
            std::string name = *it;
            srcPtrVar var1 = called.parameters[i];
            srcPtrVar var2 = declared.GetPreviousOccurence(name);
            data->AddPointsToRelationship(var1, var2);
            ++i;
         }
      }
   }

   srcPtrData const *GetData() {
      srcPtrData const *p = data;
      return p;
   }

protected:
   void *DataInner() const override {
      return data->Clone();
   }

private:
   srcPtrData *data;
   srcPtrDeclPolicy::srcPtrDeclData declData;
   srcPtrDeclStack declared;
   DeclTypePolicy *declTypePolicy;
   CallPolicy *callPolicy;

   // For use in collecting assignments
   srcPtrVar lhs;
   std::string modifierlhs;
   std::string modifierrhs;
   srcPtrVar rhs;
   bool assignmentOperator = false;

   bool withinDeclAssignment = false;

   void ResetVariables() {
      lhs.Clear();
      rhs.Clear();
      assignmentOperator = false;
      modifierlhs = "";
      modifierrhs = "";
      withinDeclAssignment = false;
   }

   void InitializeEventHandlers() {
      using namespace srcSAXEventDispatch;

      openEventMap[ParserState::unit] = [this](srcSAXEventContext &ctx) {
         ctx.dispatcher->AddListenerDispatch(declTypePolicy);
         ctx.dispatcher->AddListenerDispatch(callPolicy);
      };

      openEventMap[ParserState::block] = [this](srcSAXEventContext &ctx) { declared.CreateFrame(); };

      closeEventMap[ParserState::block] = [this](srcSAXEventContext &ctx) { declared.PopFrame(); };

      //
      // Collecting pointer assignment data
      //

      openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) { ResetVariables(); };
      openEventMap[ParserState::declstmt] = [this](srcSAXEventContext &ctx) { ResetVariables(); };

      closeEventMap[ParserState::name] = [this](srcSAXEventContext &ctx) {
         if (ctx.IsOpen(ParserState::expr)) {
            if (lhs.nameofidentifier == "") {
               srcPtrVar decl = declared.GetPreviousOccurence(ctx.currentToken);
               lhs = decl;

            } else {
               srcPtrVar decl = declared.GetPreviousOccurence(ctx.currentToken);
               rhs = decl;
            }
         }
      };

      closeEventMap[ParserState::op] = [this](srcSAXEventContext &ctx) {
         if ((ctx.currentToken == "=") && (lhs.nameofidentifier != ""))
            assignmentOperator = true;
         else if ((lhs.nameofidentifier == ""))
            modifierlhs = ctx.currentToken;
         else if ((rhs.nameofidentifier == ""))
            modifierrhs = ctx.currentToken;
      };

      closeEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
         withinDeclAssignment = true;
      };

      closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
         bool lhsIsPointer = ((lhs.isPointer && (modifierlhs != "*")) || lhs.isReference);

         if (lhsIsPointer && assignmentOperator) {
            if(!rhs.empty())
               data->AddPointsToRelationship(lhs, rhs);
         }
         ResetVariables();
      };

      closeEventMap[ParserState::unit] = [this](srcSAXEventContext &ctx) { // End of Policy
         NotifyAll(ctx);
         InitializeEventHandlers();
      };
   }
};

#endif
