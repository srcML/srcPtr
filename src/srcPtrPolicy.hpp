#ifndef INCLUDED_SRC_PTR_POLICY_HPP
#define INCLUDED_SRC_PTR_POLICY_HPP

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
//#include <DeclTypePolicy.hpp>
#include <srcPtrDeclPolicy.hpp>
#include <vector>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <srcPtrSingleton.hpp>


class srcPtrPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:

        struct srcPtrPolicyData {
          std::map<std::string, std::vector<std::string>> references; //TODO: This would need to return the datastructure described by Steensgard.
        };

        ~srcPtrPolicy() {

        }

        srcPtrPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            InitializeEventHandlers();
            declData = srcPtrSingleton::Instance()->GetValue();
        }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {

        }

        void SetData(srcPtrDeclPolicy::srcPtrDeclData toset) {
          declData = toset;
        }

    protected:
        void * DataInner() const override {
            return new srcPtrPolicyData(data);
        }
    private:

        void ResetVariables() {
          lhs.clear();
          rhs.clear();
          assignmentOperator = false;
          modifierlhs = "";
          modifierrhs = "";
        }

        srcPtrPolicyData data;
        srcPtrDeclPolicy::srcPtrDeclData declData;

        //For use in collecting assignments
        std::string modifierlhs;
        DeclTypePolicy::DeclTypeData lhs;
        std::string modifierrhs;
        DeclTypePolicy::DeclTypeData rhs;
        bool assignmentOperator = false;

        void InitializeEventHandlers(){
          using namespace srcSAXEventDispatch;

          openEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx) {
            ResetVariables();
          };

          closeEventMap[ParserState::name] = [this](srcSAXEventContext& ctx) {
            if(ctx.IsOpen(ParserState::expr)) {
              if(lhs.nameofidentifier == "") {
                DeclTypePolicy::DeclTypeData * decl = declData.FindVarWithIdentifier(ctx.currentToken);
                if (decl != nullptr) {
                  lhs = * decl;
                }
              } else {
                DeclTypePolicy::DeclTypeData * decl = declData.FindVarWithIdentifier(ctx.currentToken);
                if (decl != nullptr)
                  rhs = * decl;
              }
            }
          };

          closeEventMap[ParserState::op] = [this](srcSAXEventContext& ctx) {
            if((ctx.currentToken == "=") && (lhs.nameofidentifier != ""))
              assignmentOperator = true;
            else if((lhs.nameofidentifier == "")) {
              modifierlhs = ctx.currentToken;
            }
            else if((rhs.nameofidentifier == ""))
              modifierrhs = ctx.currentToken;
          };

          closeEventMap[ParserState::exprstmt] = [this](srcSAXEventContext& ctx) {
            bool lhsIsPointer = (modifierlhs != "*");
            bool rhsIsAddress = (((modifierrhs == "&") || (rhs.isPointer)) || (rhs.isReference));

            if((lhsIsPointer && rhsIsAddress) && assignmentOperator) {
              if(std::find(data.references[lhs.nameofidentifier].begin(), data.references[lhs.nameofidentifier].end(), rhs.nameofidentifier) == data.references[lhs.nameofidentifier].end())  //If not in vector
                 data.references[lhs.nameofidentifier].push_back(rhs.nameofidentifier);
            }
            ResetVariables();
          };

          closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx) {  //End of Policy
              NotifyAll(ctx);
              InitializeEventHandlers();
          };
        }
};

#endif
