#include <srcPtrUtilities.hpp>
#include <srcPtrPolicyTemplates.hpp>
#include <cassert>
#include <algorithm>


void TestsrcPtrDataMap() {
	{
		Variable var1;
		var1.nameofidentifier = "var1";
		var1.nameoftype = "int";
		var1.isPointer = true;
		
		Variable var2;
		var2.nameofidentifier = "var2";
		var2.nameoftype = "int";
		var2.isPointer = false;
		
		Variable var3;
		var3.nameofidentifier = "var3";
		var3.nameoftype = "int";
		var3.isPointer = false;
		
		Variable var4;
		var4.nameofidentifier = "var4";
		var4.nameoftype = "int";
		var4.isPointer = true;
		
		
		srcPtrDataMap map;
		map.AddPointsToRelationship(var1, var2);
		map.AddPointsToRelationship(var1, var3);
		map.AddPointsToRelationship(var1, var2);
		map.AddPointsToRelationship(var4, var1);

		std::vector<Variable> v = map.GetPointsTo(var1);
		assert(v.size() == 2);
		assert(std::find(v.begin(), v.end(), var2) != v.end());
		assert(std::find(v.begin(), v.end(), var3) != v.end());

		std::vector<Variable> v2 = map.GetPointsTo(var4);
		assert(v2.size() == 1);
		assert(std::find(v2.begin(), v2.end(), var1) != v2.end());
	}
}

int main() {
	TestsrcPtrDataMap();
		
 return 0;
}
