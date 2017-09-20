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
      ptr1.nameOfIdentifier = "x";
      ptr1.isPointer = true;

      Variable ptr2;
      ptr2.nameOfIdentifier = "y";
      ptr2.isPointer = true;

      andersen.AddAssignmentRelationship(ptr1, ptr2);
      andersen.AddAssignmentRelationship(ptr2, ptr1);

      andersen.Finalize();
   }
   {
      srcPtrAndersen andersen;

      Variable ptr1;
      ptr1.nameOfIdentifier = "x";
      ptr1.isPointer = true;

      Variable ptr2;
      ptr2.nameOfIdentifier = "y";
      ptr2.isPointer = true;

      Variable temp;
      temp.nameOfIdentifier = "temp";
      temp.isPointer = true;

      andersen.AddAssignmentRelationship(temp, ptr2);
      andersen.AddAssignmentRelationship(ptr2, ptr1);
      andersen.AddAssignmentRelationship(ptr1, temp);

      andersen.Finalize();
   }
   {
      srcPtrAndersen andersen;

      Variable ptr1;
      ptr1.nameOfIdentifier = "ptr1";
      ptr1.isPointer = true;

      Variable ptr2;
      ptr2.nameOfIdentifier = "ptr2";
      ptr2.isPointer = true;


      Variable var1;
      var1.nameOfIdentifier = "var1";

      Variable var2;
      var2.nameOfIdentifier = "var2";

      andersen.AddPointsToRelationship(ptr1, var1);
      andersen.AddPointsToRelationship(ptr2, var2);
      andersen.AddAssignmentRelationship(ptr2, ptr1);

      andersen.Finalize();

      assert(andersen.GetPointsTo(ptr1)[0].nameOfIdentifier == "var1");

      std::vector<Variable> ptr2PointsTo = andersen.GetPointsTo(ptr2);

      assert(std::find(ptr2PointsTo.begin(), ptr2PointsTo.end(), var1) != ptr2PointsTo.end());
      assert(std::find(ptr2PointsTo.begin(), ptr2PointsTo.end(), var2) != ptr2PointsTo.end());
   }
}

int main() {
   TestAndersen();
   return 0;
}
