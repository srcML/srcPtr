#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <FunctionSignaturePolicy.hpp>

#include <srcPtrUtilities.hpp>

#include <cassert>
#include <vector>
#include <string>

void TestVariable() {
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

		Variable x(data);

		assert(x == data);
	}
}

void TestDeclStack() {
   {
      Variable var1;
      var1.nameofidentifier = "var1";
      var1.linenumber = 1;

      Variable var2;
      var2.nameofidentifier = "var2";
      var2.linenumber = 2;

      Variable var3;
      var3.nameofidentifier = "var3";
      var3.linenumber = 3;

      DeclStack stack;
      stack.CreateFrame();
      stack.AddVarToFrame(var1);
      stack.AddVarToFrame(var2);
      stack.AddVarToFrame(var3);

      assert(stack.GetPreviousVarOccurence("var1") == var1);
      assert(stack.GetPreviousVarOccurence("var2") == var2);
      assert(stack.GetPreviousVarOccurence("var3") == var3);
   }
   {
      Variable var1;
      var1.nameofidentifier = "var";
      var1.linenumber = 1;

      Variable var2;
      var2.nameofidentifier = "var";
      var2.linenumber = 2;

      Variable var3;
      var3.nameofidentifier = "var";
      var3.linenumber = 3;

      DeclStack stack;
      stack.CreateFrame(var1);
      stack.CreateFrame(var2);
      stack.CreateFrame(var3);
      stack.AddVarToFrame(var3);

      assert(stack.GetPreviousVarOccurence("var") == var3);
      stack.PopFrame();

      Variable var4;
      var4.nameofidentifier = "var4";
      var4.linenumber = 10;

      stack.AddVarToFrame(var4);
      stack.CreateFrame();
      stack.PopFrame();

      assert(stack.GetPreviousVarOccurence("var") == var2);
      assert(stack.GetPreviousVarOccurence("var4") == var4);
      stack.PopFrame();
      assert(stack.GetPreviousVarOccurence("var") == var1);
   }
}

void TestFunction() {
	{
		FunctionSignaturePolicy::SignatureData sigData;
		ParamTypePolicy::ParamData paramData;

		paramData.nameofidentifier = "x";
		paramData.nameoftype = "int";
		
		sigData.linenumber = 12;
		sigData.returnType = "string";
		sigData.parameters.push_back(paramData);

		Function func = sigData;
		assert(func == sigData);
		func.Clear();
		Function empty;
		assert(func == empty);
	}
}

int main() {
	TestVariable();
   TestDeclStack();
	TestFunction();
   return 0;
}
