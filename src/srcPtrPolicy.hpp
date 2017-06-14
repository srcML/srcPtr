#ifndef INCLUDED_SRC_PTR_POLICY_HPP
#define INCLUDED_SRC_PTR_POLICY_HPP

#include <DeclTypePolicy.hpp>
#include <CallPolicy.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>

#include <srcPtrDeclPolicy.hpp>
#include <srcPtrUtilities.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <utility>

/*
class T must implement:
   virtual void AddPointsToRelationship(Variable, Variable);
   virtual void AddAssignmentRelationship(Variable, Variable);
   virtual void Print();
   virtual void PrintGraphViz();
   virtual std::vector<Variable> GetPointsTo(Variable);
   virtual std::vector<Variable> GetPointers();
   virtual srcPtrData *Clone();
*/

template <class T>
class srcPtrPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
public:
   
   ~srcPtrPolicy() {
      delete data;
      delete declTypePolicy;
      delete callPolicy;
      delete funcSigPolicy;
   }

   srcPtrPolicy(srcPtrDeclPolicy::srcPtrDeclData decldata, std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners) {
      declData = decldata;
      declared.CreateFrame();

      declTypePolicy = new DeclTypePolicy({this});
      callPolicy = new CallPolicy({this});
      funcSigPolicy = new FunctionSignaturePolicy({this});

      InitializeEventHandlers();
   }

   void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
      if (typeid(DeclTypePolicy) == typeid(*policy)) {
         DeclTypePolicy::DeclTypeData declarationData = *policy->Data<DeclTypePolicy::DeclTypeData>();
         declared.AddVarToFrame(Variable(declarationData));

         //If variable is an object, keep track of it's methods and members
         if(declData.classTracker.ContainsKey(declarationData.nameoftype)) {
            Class classType = declData.classTracker.GetClass(declarationData.nameoftype);

            for(int i = 0; i < classType.methods.size(); ++i) {
               declared.AddFuncToFrame(declarationData.nameofidentifier + "." + classType.methods[i].functionName, classType.methods[i]);
            }

            for(int i = 0; i < classType.members.size(); ++i) {
               Variable v = classType.members[i];
               std::string newName = declarationData.nameofidentifier + "." + v.nameofidentifier;
               v.nameofidentifier = newName;
               declared.AddVarToFrame(v);
            }
         }

         //Pointer assignment on initialization
         if(withinDeclAssignment) {
            ResolveAssignment(declarationData, "", lhs, modifierlhs);
            ResetVariables();
         }
      } 

      else if (typeid(CallPolicy) == typeid(*policy)) {
         CallPolicy::CallData callData = *policy->Data<CallPolicy::CallData>();

	 std::stack<std::pair<std::string, std::vector<std::string>>> openFuncs;
	 std::vector<std::pair<std::string, std::vector<std::string>>> funcCalls; //A list of names of function it's corresponding list of parameters

	 bool pickedUpFuncName = false;
         for(auto it = callData.callargumentlist.begin(); it != callData.callargumentlist.end(); ++it) {
	   if(*it == "(") {
	     pickedUpFuncName = false;
	   }
	   else if (pickedUpFuncName == false) {
	     openFuncs.push(std::pair<std::string, std::vector<std::string>>());
	     openFuncs.top().first = *it;
	     pickedUpFuncName = true;
	   }
	   else if (*it == ")") {
	     funcCalls.push_back(openFuncs.top());
	     openFuncs.pop();
	   }
	   else {
	     auto x = *it;
	     openFuncs.top().second.push_back(x);
	   }
         }
	 

	 for(auto it = funcCalls.begin(); it != funcCalls.end(); ++it) {
	   
	   //Identify function that was called
	   Function called = declared.GetPreviousFuncOccurence(it->first, it->second.size());

	   if(called.functionName == "")
	     called = declData.functionTracker.GetFunction(it->first, it->second.size());

	   if(called.functionName == "")
	     continue;

	   unsigned int i = 0;
	   for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
	     std::string name = *it2;
	     if(name != "*LITERAL*") {
	       Variable var = called.parameters[i];

	       ResolveAssignment(var, "", name, ""); //TODO: take into account modifiers
	     }
	     ++i;
	   }
	 }
      } 

      else if (typeid(FunctionSignaturePolicy) == typeid(*policy)) {
         FunctionSignaturePolicy::SignatureData signatureData = *policy->Data<FunctionSignaturePolicy::SignatureData>();
         Function funcSig = signatureData;
         for(Variable var : funcSig.parameters) {
            declared.AddVarToFrame(var);
         }
      }
   }

   T *GetData() {
      T *p = &data;
      return p;
   }

protected:
   void *DataInner() const override {
      return data.Clone();
   }

private:
   T data;

   // Code information
   srcPtrDeclPolicy::srcPtrDeclData declData;
   DeclStack declared;

   // Policies
   DeclTypePolicy *declTypePolicy;
   CallPolicy *callPolicy;
   FunctionSignaturePolicy *funcSigPolicy;

   // For use in collecting assignments
   std::string lhs;
   std::string modifierlhs;
   std::string rhs;
   std::string modifierrhs;

   bool assignmentOperator = false;
   bool withinDeclAssignment = false;

   void ResetVariables() {
      lhs = "";
      rhs = "";
      assignmentOperator = false;
      modifierlhs = "";
      modifierrhs = "";
      withinDeclAssignment = false;
   }


   void ResolveAssignment(std::string left, std::string modifierleft, std::string right, std::string modifierright) {
      Variable leftVar = declared.GetPreviousVarOccurence(left);
      Variable rightVar = declared.GetPreviousVarOccurence(right);

      if(!rightVar.empty()) {
         if(leftVar.isPointer && (modifierleft != "*")) {
            if(modifierright == "&")
               data.AddPointsToRelationship(leftVar, rightVar);
            else
               data.AddAssignmentRelationship(leftVar, rightVar);
         }
         else if (leftVar.isReference) {
            data.AddPointsToRelationship(leftVar, rightVar);
         }
      }
   }

   void ResolveAssignment(Variable leftVar, std::string modifierleft, std::string right, std::string modifierright) {
      Variable rightVar = declared.GetPreviousVarOccurence(right);

      if(!rightVar.empty()) {
         if(leftVar.isPointer && (modifierleft != "*")) {
            if(modifierright == "&")
               data.AddPointsToRelationship(leftVar, rightVar);
            else
               data.AddAssignmentRelationship(leftVar, rightVar);
         }
         else if (leftVar.isReference) {
            data.AddPointsToRelationship(leftVar, rightVar);
         }
      }
   }


   void InitializeEventHandlers() {
      using namespace srcSAXEventDispatch;

      openEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) {
         ctx.dispatcher->AddListenerDispatch(declTypePolicy);
         ctx.dispatcher->AddListenerDispatch(callPolicy);
         ctx.dispatcher->AddListenerDispatch(funcSigPolicy);
      };

      openEventMap[ParserState::block] = [this](srcSAXEventContext &ctx) { declared.CreateFrame(); };
      closeEventMap[ParserState::block] = [this](srcSAXEventContext &ctx) { declared.PopFrame(); };

      //
      // Collecting pointer assignment data
      //

      openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) { ResetVariables(); };
      openEventMap[ParserState::declstmt] = [this](srcSAXEventContext &ctx) { ResetVariables(); };

      closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext &ctx) {
         if (ctx.IsOpen(ParserState::expr) && ctx.IsOpen(ParserState::name)) {
            if (!assignmentOperator) {
               lhs += ctx.currentToken;
            } else {
               rhs += ctx.currentToken;
            }
         }
      };

      closeEventMap[ParserState::op] = [this](srcSAXEventContext &ctx) {
         if ((ctx.currentToken == "=") && (lhs != ""))
            assignmentOperator = true;
         else if ((lhs == ""))
            modifierlhs = ctx.currentToken;
         else if (rhs == "")
            modifierrhs = ctx.currentToken;
      };

      closeEventMap[ParserState::init] = [this](srcSAXEventContext &ctx) {
         withinDeclAssignment = true;
      };

      closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext &ctx) {
         if(assignmentOperator)
            ResolveAssignment(lhs, modifierlhs, rhs, modifierrhs);
         ResetVariables();
      };

      closeEventMap[ParserState::archive] = [this](srcSAXEventContext &ctx) { // End of Policy
         ctx.dispatcher->RemoveListenerDispatch(declTypePolicy);
         ctx.dispatcher->RemoveListenerDispatch(callPolicy);
         ctx.dispatcher->RemoveListenerDispatch(funcSigPolicy);
         NotifyAll(ctx);
      };
   }
};

#endif
