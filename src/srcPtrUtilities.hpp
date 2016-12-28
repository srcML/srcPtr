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

struct srcPtrVar {
   srcPtrVar() : linenumber{-1}, isConst{false}, isReference{false}, isPointer{false}, isStatic{false} {
   }

   srcPtrVar(const DeclTypePolicy::DeclTypeData &data) {
      nameoftype = data.nameoftype;
      nameofidentifier = data.nameofidentifier;
      namespaces = data.namespaces;
      linenumber = data.linenumber;
      isConst = data.isConst;
      isReference = data.isReference;
      isPointer = data.isPointer;
      isStatic = data.isPointer;
   }

   srcPtrVar(const ParamTypePolicy::ParamData &data) {
      nameoftype = data.nameoftype;
      nameofidentifier = data.nameofidentifier;
      namespaces = data.namespaces;
      linenumber = data.linenumber;
      isConst = data.isConst;
      isReference = data.isReference;
      isPointer = data.isPointer;
      isStatic = data.isPointer;
   }

   bool operator==(const srcPtrVar &rhs) const {
      // Compare each of the variables
      return ((this->nameoftype == rhs.nameoftype) && (this->nameofidentifier == rhs.nameofidentifier) && (this->namespaces == rhs.namespaces) && (this->linenumber == rhs.linenumber) && (this->isConst == rhs.isConst) && (this->isPointer == rhs.isPointer) && (this->isReference == rhs.isReference) &&
              (this->isStatic == rhs.isStatic));
   }

   bool operator<(const srcPtrVar &rhs) const { // Function required for STL datastructures
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
      srcPtrVar empty;
      return (*this == empty);
   }

   std::string UniqueIdentifier() {
      return nameoftype + nameofidentifier + std::to_string(linenumber) + filename;
   }
   std::string SimpleIdentifier() {
      return nameofidentifier;
   }

   friend std::ostream &operator<<(std::ostream &stream, const srcPtrVar &var);

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

std::ostream &operator<<(std::ostream &stream, const srcPtrVar &var) {
   stream << var.nameofidentifier;
   return stream;
}



class srcPtrDeclFrame {
public:
   srcPtrDeclFrame(){};
   srcPtrDeclFrame(srcPtrVar var) {
      declarations.insert(std::pair<std::string, srcPtrVar>(var.SimpleIdentifier(), var));
   }
   bool ContainsName(std::string name) {
      return (declarations.find(name) != declarations.end());
   }
   srcPtrVar GetVar(std::string name) {
      return declarations[name];
   }
   void AddVar(srcPtrVar var) {
      if (!ContainsName(var.SimpleIdentifier()))
         declarations.insert(std::pair<std::string, srcPtrVar>(var.SimpleIdentifier(), var));
   }

private:
   std::map<std::string, srcPtrVar> declarations; // Name to srcPtrVar
};

class srcPtrDeclStack {
public:
   void CreateFrame() {
      declared.push_front(srcPtrDeclFrame());
   }
   void CreateFrame(srcPtrVar var) {
      declared.push_front(srcPtrDeclFrame(var));
   }
   void PopFrame() {
      declared.pop_front();
   }
   void AddVarToFrame(srcPtrVar var) {
      declared.begin()->AddVar(var);
   }
   srcPtrVar GetPreviousOccurence(std::string name) {
      for (auto it = declared.begin(); it != declared.end(); ++it) {
         if (it->ContainsName(name))
            return it->GetVar(name);
      }
      return srcPtrVar();
   }

private:
   std::deque<srcPtrDeclFrame> declared;
};



class srcPtrFunction {
public:
	srcPtrFunction():isConst{false}, constPointerReturn{false}, isMethod{false}, isStatic{false}, pointerToConstReturn{false}, hasAliasedReturn{false} {}

	srcPtrFunction(const FunctionSignaturePolicy::SignatureData& rhs) {
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

	bool operator==(const srcPtrFunction& rhs) const {
		return ((linenumber == rhs.linenumber) && (returnType == rhs.returnType) && (functionName == rhs.functionName) && (returnTypeModifier == rhs.returnTypeModifier) &&
				  (functionNamespaces == rhs.functionNamespaces) && (returnTypeNamespaces == rhs.returnTypeNamespaces) && (parameters == rhs.parameters) && (isConst == rhs.isConst) &&
				  (isMethod == rhs.isMethod) && (isStatic == rhs.isStatic) && (pointerToConstReturn == rhs.pointerToConstReturn) && (constPointerReturn == rhs.constPointerReturn) &&
				  (hasAliasedReturn == rhs.hasAliasedReturn));
	}

	bool operator<(const srcPtrFunction &rhs) const { // Function required for STL datastructures
		return (functionName < rhs.functionName);
	}

	int linenumber;
	std::string returnType;
	std::string functionName;
	std::string returnTypeModifier;
	std::vector<std::string> functionNamespaces;
	std::vector<std::string> returnTypeNamespaces;
	std::vector<srcPtrVar> parameters;
	bool isConst;
	bool isMethod;
	bool isStatic;
	bool pointerToConstReturn;
	bool constPointerReturn;
	bool hasAliasedReturn;
};



class FunctionTracker {
public:
	srcPtrFunction GetFunction(std::string name, int paramCount) {
      return functionNames[(name + std::to_string(paramCount))];
   }

   void AddFunction(srcPtrFunction toAdd) {
      std::string key = toAdd.functionName + std::to_string(toAdd.parameters.size()); //TODO: Take into account type of parameter
      functionNames[key] = toAdd;
   }
private:
	std::map<std::string, srcPtrFunction> functionNames; // (Name of function + param count) -> srcPtrFunction
};

#endif
