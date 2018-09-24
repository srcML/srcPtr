#include <srcPtrUtilities.hpp>
#include <srcPtrPolicyTemplates.hpp>

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>

void TestAndersen() {
   {
      srcPtrAndersen andersen;

      Variable ptr1;
      ptr1.nameofidentifier = "x";
      ptr1.isPointer = true;

      Variable ptr2;
      ptr2.nameofidentifier = "y";
      ptr2.isPointer = true;

      andersen.AddAssignmentRelationship(ptr1, ptr2);
      andersen.AddAssignmentRelationship(ptr2, ptr1);

      andersen.Finalize();
   }
   {
      srcPtrAndersen andersen;

      Variable ptr1;
      ptr1.nameofidentifier = "x";
      ptr1.isPointer = true;

      Variable ptr2;
      ptr2.nameofidentifier = "y";
      ptr2.isPointer = true;

      Variable temp;
      temp.nameofidentifier = "temp";
      temp.isPointer = true;

      andersen.AddAssignmentRelationship(temp, ptr2);
      andersen.AddAssignmentRelationship(ptr2, ptr1);
      andersen.AddAssignmentRelationship(ptr1, temp);

      andersen.Finalize();
   }
   {
      srcPtrAndersen andersen;

      Variable ptr1;
      ptr1.nameofidentifier = "ptr1";
      ptr1.isPointer = true;

      Variable ptr2;
      ptr2.nameofidentifier = "ptr2";
      ptr2.isPointer = true;


      Variable var1;
      var1.nameofidentifier = "var1";

      Variable var2;
      var2.nameofidentifier = "var2";

      andersen.AddPointsToRelationship(ptr1, var1);
      andersen.AddPointsToRelationship(ptr2, var2);
      andersen.AddAssignmentRelationship(ptr2, ptr1);

      andersen.Finalize();

      assert(andersen.GetPointsTo(ptr1)[0].nameofidentifier == "var1");

      std::vector<Variable> ptr2PointsTo = andersen.GetPointsTo(ptr2);

      assert(std::find(ptr2PointsTo.begin(), ptr2PointsTo.end(), var1) != ptr2PointsTo.end());
      assert(std::find(ptr2PointsTo.begin(), ptr2PointsTo.end(), var2) != ptr2PointsTo.end());
   }
}

void TestSteensgaard() {
   Variable var1;
   Variable var2;
   Variable var3;
   Variable ptr1;
   Variable ptr2;

   var1.linenumber = 1;
   var1.nameofidentifier = "var1";
   var1.nameoftype = "int";

   var2.linenumber = 2;
   var2.nameofidentifier = "var2";
   var2.nameoftype = "int";

   var3.linenumber = 3;
   var3.nameofidentifier = "var3";
   var3.nameoftype = "int";

   ptr1.linenumber = 4;
   ptr1.nameofidentifier = "ptr1";
   ptr1.nameoftype = "int";
   ptr1.isPointer = true;

   ptr2.linenumber = 5; 
   ptr2.nameofidentifier = "ptr2";
   ptr2.nameoftype = "int";
   ptr2.isPointer = true;

   {
      srcPtrSteensgaard steensgaard;

      steensgaard.AddPointsToRelationship(ptr1, var1);
      steensgaard.AddPointsToRelationship(ptr1, var2);

      std::vector<Variable> pointsTo = steensgaard.GetPointsTo(ptr1);
      assert((std::count(pointsTo.begin(), pointsTo.end(), var1) == 1));
      assert((std::count(pointsTo.begin(), pointsTo.end(), var2) == 1));

      steensgaard.AddAssignmentRelationship(ptr1, ptr2);

      assert(steensgaard.GetPointsTo(ptr1) == steensgaard.GetPointsTo(ptr2));
   }
}

int main() {
   TestAndersen();
   TestSteensgaard();
   return 0;
}
