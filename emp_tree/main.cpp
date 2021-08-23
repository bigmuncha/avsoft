#include "xml_tree.h"
#include <unordered_set>
#include <vector>
int main()
{
  Company empTreeFromXml;
  EmpTree &tree = empTreeFromXml;
  std::string filename;
  std::cout << "insert file name: ";
  std::cin >> filename;
 
  tree.GetDataFromSrc(filename);
  
  int M;
  while(true){
    std::cout << "Print Tree: 1\n"
	      << "Make Change: 2\n"
	      << "Undo : 3\n"
	      << "Redo: 4\n";

    std::cin >> M;
    switch(M){
    case 1:
      tree.PrintTree();
      break;
    case 2:
      tree.MakeChange();
      break;
    case 3:
      tree.Undo();
      break;
    case 4:
      tree.Redo();
      break;
    default:
      std::cout <<"Wrong command\n";
    }
  }
  return 0;
}

///З.Ы. попробуйте не обращать
///внимание на тупые названия переменных
/// смесь камел кейса и 
