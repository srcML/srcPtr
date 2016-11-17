#ifndef INCLUDED_SRC_PTR_DECL_POLICY_HPP
#define INCLUDED_SRC_PTR_DECL_POLICY_HPP

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <vector>
#include <exception>
#include <FunctionSignaturePolicy.hpp>
#include <string>

#include <iostream>

class srcPtrDeclPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:
        struct srcPtrDeclData {
          std::vector<DeclTypePolicy::DeclTypeData> variableDeclarations;
          std::vector<FunctionSignaturePolicy::SignatureData> functionDeclarations;

          DeclTypePolicy::DeclTypeData* FindVarWithIdentifier(std::string s) {  //TODO: Implement as map to increase efficiency
            for(auto& t : variableDeclarations) {
              if(t.nameofidentifier == s)
                return &t;
            }
            return nullptr;
          }
        };

        ~srcPtrDeclPolicy() {
            if(declTypePolicy) delete declTypePolicy;
            if(functionSignaturePolicy) delete functionSignaturePolicy;
        }

        srcPtrDeclPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            declTypePolicy = new DeclTypePolicy{this};
            functionSignaturePolicy = new FunctionSignaturePolicy{this};
            InitializeEventHandlers();
        }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
          if(typeid(DeclTypePolicy) == typeid(*policy)) {
              DeclTypePolicy::DeclTypeData declData = *policy->Data<DeclTypePolicy::DeclTypeData>();
              data.variableDeclarations.push_back(declData);
          }
          else if(typeid(FunctionSignaturePolicy) == typeid(*policy)) {
              FunctionSignaturePolicy::SignatureData signatureData = *policy->Data<FunctionSignaturePolicy::SignatureData>();
              data.functionDeclarations.push_back(signatureData);
          }
        }
            srcPtrDeclData data;
    protected:
        void * DataInner() const override {
            return new srcPtrDeclData(data);
        }
    private:
        DeclTypePolicy* declTypePolicy = nullptr;
        FunctionSignaturePolicy* functionSignaturePolicy = nullptr;

        void InitializeEventHandlers(){
          using namespace srcSAXEventDispatch;

          openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx) {
              ctx.dispatcher->AddListenerDispatch(declTypePolicy);
              ctx.dispatcher->AddListenerDispatch(functionSignaturePolicy);
          };
          closeEventMap[ParserState::function] = [this](srcSAXEventContext& ctx) {
              NotifyAll(ctx);
              InitializeEventHandlers();
          };
        }
};

#endif
