#include <srcPtrUtilities.hpp>
#include <srcPtrPolicyTemplates.hpp>

#include <cassert>
#include <vector>
#include <string>

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

      andersen.Print();
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

      andersen.Print();
   }
}

int main() {
   TestAndersen();
   return 0;
}
