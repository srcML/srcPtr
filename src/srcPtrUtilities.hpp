#ifndef INCLUDED_SRC_PTR_UTILITIES
#define INCLUDED_SRC_PTR_UTILITIES

#include <DeclTypePolicy.hpp>

#include <map>
#include <stack>
#include <string>

struct srcPtrVar {
   srcPtrVar() : linenumber{0}, isConst{false}, isReference{false}, isPointer{false}, isStatic{false} {
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

   bool operator==(const srcPtrVar &rhs) const {
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

/*
class srcPtrDeclFrame {
public:
   bool ContainsName
private:
   std::map<std::string, srcPtrVar> declarations; //Name to srcPtrVar
}

class srcPtrDeclStack {
public:

private:
   std::stack<srcPtrDeclFrame> declared;
}*/

#endif
