#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <FunctionSignaturePolicy.hpp>

#include <srcPtrUtilities.hpp>

#include <cassert>
#include <vector>
#include <string>

void TestSrcPtrVar() {
	{
		std::vector<std::string> s;
		s.push_back("std");

		DeclTypePolicy::DeclTypeData data;
		data.nameoftype = "i";
		data.nameofidentifier = "int";
		data.namespaces = s;
		data.linenumber = 12;
		data.isConst = true;
		data.isReference = false;
		data.isPointer = true;
		data.isStatic = false;

		srcPtrVar x(data);

		assert(x == data);
	}
}

void TestDeclStack() {
   {
      srcPtrVar var1;
      var1.nameofidentifier = "var1";
      var1.linenumber = 1;

      srcPtrVar var2;
      var2.nameofidentifier = "var2";
      var2.linenumber = 2;

      srcPtrVar var3;
      var3.nameofidentifier = "var3";
      var3.linenumber = 3;

      srcPtrDeclStack stack;
      stack.CreateFrame();
      stack.AddVarToFrame(var1);
      stack.AddVarToFrame(var2);
      stack.AddVarToFrame(var3);

      assert(stack.GetPreviousOccurence("var1") == var1);
      assert(stack.GetPreviousOccurence("var2") == var2);
      assert(stack.GetPreviousOccurence("var3") == var3);
   }
   {
      srcPtrVar var1;
      var1.nameofidentifier = "var";
      var1.linenumber = 1;

      srcPtrVar var2;
      var2.nameofidentifier = "var";
      var2.linenumber = 2;

      srcPtrVar var3;
      var3.nameofidentifier = "var";
      var3.linenumber = 3;

      srcPtrDeclStack stack;
      stack.CreateFrame(var1);
      stack.CreateFrame(var2);
      stack.CreateFrame(var3);
      stack.AddVarToFrame(var3);

      assert(stack.GetPreviousOccurence("var") == var3);
      stack.PopFrame();

      srcPtrVar var4;
      var4.nameofidentifier = "var4";
      var4.linenumber = 10;

      stack.AddVarToFrame(var4);
      stack.CreateFrame();
      stack.PopFrame();

      assert(stack.GetPreviousOccurence("var") == var2);
      assert(stack.GetPreviousOccurence("var4") == var4);
      stack.PopFrame();
      assert(stack.GetPreviousOccurence("var") == var1);
   }
}

void TestsrcPtrFunction() {
	{
		FunctionSignaturePolicy::SignatureData sigData;
		ParamTypePolicy::ParamData paramData;

		paramData.nameofidentifier = "x";
		paramData.nameoftype = "int";
		
		sigData.linenumber = 12;
		sigData.returnType = "string";
		sigData.parameters.push_back(paramData);

		srcPtrFunction func = sigData;
		assert(func == sigData);
	}
}

int main() {
	TestSrcPtrVar();
   TestDeclStack();
	TestsrcPtrFunction();
   return 0;
}
