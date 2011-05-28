/*

  Script must have 'int main()' or 'void main()' as the entry point.

  Some functions that are available:

   void           print(const string &in str);
   array<string> @getCommandLineArgs();

  Some objects that are available:

   string
   array<T>
   file

*/

string g_str = getDefaultString();

int main()
{
  array<string> @args = getCommandLineArgs();

  print("Received the following args : " + join(args, "|") + "\n");

  function();

  // Garbage collection is automatic
  // Set up a circular reference to prove this
  Link @link = Link();
  @link.next = link;
  for( int n = 0; n < 1000; n++ )
  {
    Link @link = Link();
  }

  return 0;
}

void function()
{
  print("Currently in a different function\n");

  int n = 0;
  {
    int n = 1;
    string s = "hello";
    print(s + "\n");
  }
  {
    int n = 2;
  }
}

string getDefaultString()
{
  return "default";
}

class Link
{
  Link @next;
}