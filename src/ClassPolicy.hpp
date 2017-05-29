#ifndef INCLUDED_CLASS_POLICY_HPP
#define INCLUDED_CLASS_POLICY_HPP

#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <srcPtrUtilities.hpp>
#include <stack>
#include <list>

class ClassPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    public:

        ~ClassPolicy() {
            delete funcSigPolicy;
            delete declTypePolicy;
        }

        ClassPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners) {
            funcSigPolicy = new FunctionSignaturePolicy({this});
            declTypePolicy = new DeclTypePolicy({this});

            InitializeEventHandlers();
        }

        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {
            if (typeid(FunctionSignaturePolicy) == typeid(*policy)) {
                FunctionSignaturePolicy::SignatureData signatureData = *policy->Data<FunctionSignaturePolicy::SignatureData>();
                Function funcSig = signatureData;
                data.methods.push_back(funcSig);
            }

            else if (typeid(DeclTypePolicy) == typeid(*policy)) {
                if(!(ctx.IsOpen(srcSAXEventDispatch::ParserState::function))) {
                    DeclTypePolicy::DeclTypeData declarationData = *policy->Data<DeclTypePolicy::DeclTypeData>();
                    Variable declVar = declarationData;
                    data.members.push_back(declVar);
                }
            }
        }

    protected:

        void * DataInner() const override {
            return new Class(data);
        }

    private:

        Class data;
        
        bool inClassDef = false;

        FunctionSignaturePolicy *funcSigPolicy;
        DeclTypePolicy *declTypePolicy;


        void InitializeEventHandlers() {
            using namespace srcSAXEventDispatch;

            openEventMap[ParserState::classn] = [this](srcSAXEventContext &ctx) {
                ctx.dispatcher->AddListenerDispatch(funcSigPolicy);
                ctx.dispatcher->AddListenerDispatch(declTypePolicy);

                inClassDef = true;
            };

            closeEventMap[ParserState::classn] = [this](srcSAXEventContext &ctx) {
                ctx.dispatcher->RemoveListenerDispatch(funcSigPolicy);
                ctx.dispatcher->RemoveListenerDispatch(declTypePolicy);

                inClassDef = false;

                NotifyAll(ctx);
                data.Clear();
            };

            closeEventMap[ParserState::name] = [this](srcSAXEventContext& ctx){
                if((ctx.IsOpen(ParserState::classn) && inClassDef) && (data.className == "")) {
                    data.className = ctx.currentToken;
                }
            };


        }
};

#endif
