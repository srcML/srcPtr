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
                data_stack.top().methods.push_back(funcSig);
            }

            else if (typeid(DeclTypePolicy) == typeid(*policy)) {
                if(!(ctx.IsOpen(srcSAXEventDispatch::ParserState::function))) {
                    DeclTypePolicy::DeclTypeData declarationData = *policy->Data<DeclTypePolicy::DeclTypeData>();
                    Variable declVar = declarationData;
                    data_stack.top().members.push_back(declVar);
                }
            }
        }

    protected:

        void * DataInner() const override {
            return new std::vector<Class>(data);
        }

    private:

        std::stack<Class> data_stack;

        std::vector<Class> data;

        FunctionSignaturePolicy *funcSigPolicy;
        DeclTypePolicy *declTypePolicy;

        bool gotClassName = true;

        void InitializeEventHandlers() {
            using namespace srcSAXEventDispatch;

            //Classes
            openEventMap[ParserState::classn] = [this](srcSAXEventContext &ctx) {
                if(data_stack.empty()) {
                    ctx.dispatcher->AddListenerDispatch(funcSigPolicy);
                    ctx.dispatcher->AddListenerDispatch(declTypePolicy);
                }

                Class current; 
                current.isStruct = false;
                data_stack.push(current);

                gotClassName = false;
            };

            closeEventMap[ParserState::classn] = [this](srcSAXEventContext &ctx) {
                data.push_back(data_stack.top());
                data_stack.pop();

                if(data_stack.empty()) {
                    ctx.dispatcher->RemoveListenerDispatch(funcSigPolicy);
                    ctx.dispatcher->RemoveListenerDispatch(declTypePolicy);

                    NotifyAll(ctx);
                    data.clear();
                }
            };

            //Structs
            openEventMap[ParserState::structn] = [this](srcSAXEventContext &ctx) {
                if(data_stack.empty()) {
                    ctx.dispatcher->AddListenerDispatch(funcSigPolicy);
                    ctx.dispatcher->AddListenerDispatch(declTypePolicy);
                }

                Class current; 
                current.isStruct = true;
                data_stack.push(current);

                gotClassName = false;
            };

            closeEventMap[ParserState::structn] = [this](srcSAXEventContext &ctx) {
                data.push_back(data_stack.top());
                data_stack.pop();

                if(data_stack.empty()) {
                    ctx.dispatcher->RemoveListenerDispatch(funcSigPolicy);
                    ctx.dispatcher->RemoveListenerDispatch(declTypePolicy);

                    NotifyAll(ctx);
                    data.clear();
                }
            };

            closeEventMap[ParserState::name] = [this](srcSAXEventContext& ctx){
                if((ctx.IsOpen(ParserState::structn) || ctx.IsOpen(ParserState::classn)) && !gotClassName) {
                    data_stack.top().className = ctx.currentToken;
                    gotClassName = true;
                }
            };


        }
};

#endif
