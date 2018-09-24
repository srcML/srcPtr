#include <DisjointSet.hpp>
#include <cassert>

void test_disjoint_set() {
   Variable var1;
   Variable var2;
   Variable var3;

   var1.linenumber = 1;
   var1.nameofidentifier = "var1";
   var1.nameoftype = "int";

   var2.linenumber = 2;
   var2.nameofidentifier = "var2";
   var2.nameoftype = "int";

   var3.linenumber = 3;
   var3.nameofidentifier = "var3";
   var3.nameoftype = "int";


   {
      DisjointSet ds;

      ds.MakeSet(var1);
      ds.MakeSet(var2);

      assert(ds.GetSet(var1) != ds.GetSet(var2));
      assert(ds.Find(var1) != ds.Find(var2));

      ds.Union(var1, var2);

      assert(ds.Find(var1) == ds.Find(var2));

      ds.MakeSet(var3);

      assert(ds.Find(var3) != ds.Find(var2));

      ds.Union(var3, var1);

      assert(ds.Find(var1) == ds.Find(var2));
      assert(ds.Find(var2) == ds.Find(var3));

      assert(ds.GetSet(var1) == ds.GetSet(var2));
   }
}

int main() {
   test_disjoint_set();
}
