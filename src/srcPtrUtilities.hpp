/**
 * @file srcPtrUtilities.hpp
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

#ifndef INCLUDED_SRC_PTR_UTILITIES
#define INCLUDED_SRC_PTR_UTILITIES

#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <FunctionSignaturePolicy.hpp>

#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct Variable {
   Variable() {
     Clear();
   }

   Variable(const DeclTypePolicy::DeclTypeData &data) {
      nameoftype = data.nameoftype;
      nameofidentifier = data.nameofidentifier;
      namespaces = data.namespaces;
      linenumber = data.linenumber;
      isConst = data.isConst;
      isReference = data.isReference;
      isPointer = data.isPointer;
      isStatic = data.isPointer;
   }

   Variable(const ParamTypePolicy::ParamData &data) {
      nameoftype = data.nameoftype;
      nameofidentifier = data.nameofidentifier;
      namespaces = data.namespaces;
      linenumber = data.linenumber;
      isConst = data.isConst;
      isReference = data.isReference;
      isPointer = data.isPointer;
      isStatic = data.isPointer;
   }

   bool operator==(const Variable &rhs) const {
      // Compare each of the variables
      return ((this->nameoftype == rhs.nameoftype) && (this->nameofidentifier == rhs.nameofidentifier) && (this->namespaces == rhs.namespaces) && (this->linenumber == rhs.linenumber) && (this->isConst == rhs.isConst) && (this->isPointer == rhs.isPointer) && (this->isReference == rhs.isReference) &&
              (this->isStatic == rhs.isStatic));
   }

   bool operator<(const Variable &rhs) const { // Function required for STL datastructures
      return (this->nameofidentifier < rhs.nameofidentifier);
   }

   void Clear() {
      nameoftype.clear();
      nameofidentifier.clear();
      namespaces.clear();
      linenumber = -1;
      isConst = false;
      isReference = false;
      isPointer = false;
      isStatic = false;
   }

   bool empty() {
      Variable empty;
      return (*this == empty);
   }

   std::string UniqueIdentifier() {
      return nameoftype + nameofidentifier + std::to_string(linenumber) + filename;
   }
   std::string SimpleIdentifier() {
      return nameofidentifier;
   }

   friend std::ostream &operator<<(std::ostream &sout, const Variable &var);

   std::string nameoftype;
   std::string nameofidentifier;
   std::vector<std::string> namespaces;
   std::string filename;
   int linenumber;
   bool isConst;
   bool isReference;
   bool isPointer;
   bool isStatic;
};

std::ostream &operator<<(std::ostream &sout, const Variable &var) {
   sout << var.nameofidentifier;
   return sout;
}


class Function {
public:
   Function() {
      linenumber = 0;
      isConst = false;
      isMethod = false;
      isStatic = false;
      pointerToConstReturn = false;
      constPointerReturn = false;
      hasAliasedReturn = false;
      returnType = "";
      functionName = "";
      returnTypeModifier = "";
   }

   Function(const FunctionSignaturePolicy::SignatureData& rhs) {
      linenumber = rhs.linenumber;
      returnType = rhs.returnType;
      functionName = rhs.functionName;
      returnTypeModifier = rhs.returnTypeModifier;
      functionNamespaces = rhs.functionNamespaces;
      returnTypeNamespaces = rhs.returnTypeNamespaces;
      for(std::size_t i = 0; i < rhs.parameters.size(); ++i) //copy parameters
         parameters.push_back(rhs.parameters[i]);
      isConst = rhs.isConst;
      isMethod = rhs.isMethod;
      isStatic = rhs.isStatic;
      pointerToConstReturn = rhs.pointerToConstReturn;
      constPointerReturn = rhs.constPointerReturn;
      hasAliasedReturn = rhs.hasAliasedReturn;
   }

   void Clear(){
      linenumber = 0;
      returnType.clear();
      functionName.clear();
      returnTypeModifier.clear();
      parameters.clear();
      functionNamespaces.clear();
      returnTypeNamespaces.clear();
      isConst = false;
      isMethod = false;
      isStatic = false;
      pointerToConstReturn = false;
      constPointerReturn = false;
      hasAliasedReturn = false;
   }

   bool operator==(const Function& rhs) const {
      return ((linenumber == rhs.linenumber) && (returnType == rhs.returnType) && (functionName == rhs.functionName) && (returnTypeModifier == rhs.returnTypeModifier) &&
              (functionNamespaces == rhs.functionNamespaces) && (returnTypeNamespaces == rhs.returnTypeNamespaces) && (parameters == rhs.parameters) && (isConst == rhs.isConst) &&
              (isMethod == rhs.isMethod) && (isStatic == rhs.isStatic) && (pointerToConstReturn == rhs.pointerToConstReturn) && (constPointerReturn == rhs.constPointerReturn) &&
              (hasAliasedReturn == rhs.hasAliasedReturn));
   }

   bool operator<(const Function &rhs) const { // Function required for STL datastructures
      return (functionName < rhs.functionName);
   }

   int linenumber;
   std::string returnType;
   std::string functionName;
   std::string returnTypeModifier;
   std::vector<std::string> functionNamespaces;
   std::vector<std::string> returnTypeNamespaces;
   std::vector<Variable> parameters;
   bool isConst;
   bool isMethod;
   bool isStatic;
   bool pointerToConstReturn;
   bool constPointerReturn;
   bool hasAliasedReturn;
};


class DeclFrame {
public:
   DeclFrame() { };

   DeclFrame(Variable var) {
      variables.insert(std::pair<std::string, Variable>(var.SimpleIdentifier(), var));
   }

   bool ContainsVariableName(std::string name) {
      return (variables.find(name) != variables.end());
   }

   Variable GetVar(std::string name) {
      return variables[name];
   }

   void AddVar(Variable var) {
      if (!ContainsVariableName(var.SimpleIdentifier()))
         variables.insert(std::pair<std::string, Variable>(var.SimpleIdentifier(), var));
   }

   bool ContainsFunctionName(std::string name, int paramCount) {
      return ( functions.find( name + std::to_string(paramCount) ) != functions.end() );
   }

   Function GetFunc(std::string name, int paramCount) {
      return functions[(name + std::to_string(paramCount))];
   }

   void AddFunc(Function toAdd) {
      std::string key = toAdd.functionName + std::to_string(toAdd.parameters.size()); //TODO: Take into account type of parameter
      
      functions.insert(std::pair<std::string, Function>(key, toAdd));
   }

   void AddFunc(std::string name, Function toAdd) {
      std::string key = name + std::to_string(toAdd.parameters.size()); //TODO: Take into account type of parameter
      
      functions.insert(std::pair<std::string, Function>(key, toAdd));
   }

private:
   std::map<std::string, Variable> variables; // Name to Variable
   std::map<std::string, Function> functions; // (Name of function + param count) -> Function
};

class DeclStack {
public:

   void CreateFrame() {
      declared.push_front(DeclFrame());
   }

   void CreateFrame(Variable var) {
      declared.push_front(DeclFrame(var));
   }

   void PopFrame() {
      declared.pop_front();
   }

   void AddVarToFrame(Variable var) {
      declared.begin()->AddVar(var);
   }

   Variable GetPreviousVarOccurence(std::string name) {
      for (auto it = declared.begin(); it != declared.end(); ++it) {
         if (it->ContainsVariableName(name))
            return it->GetVar(name);
      }

      Variable var; 
      return var;
   }

   void AddFuncToFrame(Function func) {
      declared.begin()->AddFunc(func);
   }

   void AddFuncToFrame(std::string name, Function func) {
      declared.begin()->AddFunc(name, func);
   }

   Function GetPreviousFuncOccurence(std::string name, int paramCount) {
      for (auto it = declared.begin(); it != declared.end(); ++it) {
         if (it->ContainsFunctionName(name, paramCount))
            return it->GetFunc(name, paramCount);
      }

      Function func; 
      return func;
   }

private:
   std::deque<DeclFrame> declared;
};


class FunctionTracker {
public:
	Function GetFunction(std::string name, int paramCount) {
      return functionNames[(name + std::to_string(paramCount))];
   }

   void AddFunction(Function toAdd) {
      std::string key = toAdd.functionName + std::to_string(toAdd.parameters.size()); //TODO: Take into account type of parameter
      functionNames[key] = toAdd;
   }

private:
	std::map<std::string, Function> functionNames; // (Name of function + param count) -> Function
};

class Class {
public:
   void Clear() {
      className = "";
      methods.clear();
      members.clear();
      isStruct = false;
   }

   std::string className;
   bool isStruct = false;  //False -> Class; True -> Struct
   std::vector<Function> methods;
   std::vector<Variable> members;
};

class ClassTracker {
public:
   Class GetClass(std::string name) {
      return classes[name]; 
   }

   void AddClass(Class toAdd) {
      classes[toAdd.className] = toAdd;
   }

   bool ContainsKey(std::string name) { 
      return (classes.find(name) != classes.end());
   }

private:
   std::map<std::string, Class> classes;
};

#endif
