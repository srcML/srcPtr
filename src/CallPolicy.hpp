/**
 * @file CallPolicy.hpp
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

#ifndef INCLUDED_SRC_PTR_CALL_POLICY
#define INCLUDED_SRC_PTR_CALL_POLICY

#include <srcSAXEventDispatcher.hpp>
#include <srcSAXHandler.hpp>
#include <exception>
#include <stack>
#include <iostream>
#include <list>
/*
 *Record current function being called
 *Record argument names and positions
 */
class CallPolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener {
    /*
    {CalledFunction1{arg1, line#}, {arg2, line#}, ..., {argn, line#},
        NestedCalledFunction1{arg1, line#},{arg2, line#}, ..., {argn, line#}
        }
    */
    public:
        struct CallData{
            void clear(){
                fnName.clear();
                callargumentlist.clear();
            }
            std::list<std::string> callargumentlist;
            std::string fnName;
        };
        ~CallPolicy(){}
        CallPolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener *> listeners = {}): srcSAXEventDispatch::PolicyDispatcher(listeners){
            InitializeEventHandlers();
        }
        void Notify(const PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {}
    protected:
        void * DataInner() const override {
            return new CallData(data);
        }
    private:
        CallData data;
        std::string currentTypeName, currentCallName, currentModifier, currentSpecifier;
        std::string fullFuncIdentifier;
        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            closeEventMap[ParserState::call] = [this](srcSAXEventContext& ctx){
                if(ctx.triggerField[ParserState::call] == 1){ //TODO: Fix
                    data.callargumentlist.push_back(")");
                    NotifyAll(ctx);
                    data.clear();
                }else{
                    data.callargumentlist.push_back(")");
                }
            };

            closeEventMap[ParserState::modifier] = [this](srcSAXEventContext& ctx){
                if(currentModifier == "*") { }
                else if(currentModifier == "&") { }
            };

            closeEventMap[ParserState::tokenstring] = [this](srcSAXEventContext& ctx){
                if(ctx.IsOpen(ParserState::name) && ctx.IsGreaterThan(ParserState::call,ParserState::argumentlist) && ctx.IsClosed(ParserState::genericargumentlist)){
                    data.fnName = ctx.currentToken;
                    fullFuncIdentifier += ctx.currentToken;
                }
                
                if(ctx.And({ParserState::name, ParserState::argument, ParserState::argumentlist}) && ctx.IsEqualTo(ParserState::call,ParserState::argumentlist) && ctx.IsClosed(ParserState::genericargumentlist)){
                    data.callargumentlist.push_back(ctx.currentToken);
                }

                if(ctx.And({ParserState::literal, ParserState::argument, ParserState::argumentlist}) && ctx.IsEqualTo(ParserState::call,ParserState::argumentlist) && ctx.IsClosed(ParserState::genericargumentlist)){
                    data.callargumentlist.push_back("*LITERAL*");   //Illegal c++ identifier as marker for literals
                }
            };

            openEventMap[ParserState::argumentlist] = [this](srcSAXEventContext& ctx) {
                data.callargumentlist.push_back("(");
                data.callargumentlist.push_back(fullFuncIdentifier);
                data.fnName = fullFuncIdentifier;
                fullFuncIdentifier = "";
            };
        }
};

#endif 
