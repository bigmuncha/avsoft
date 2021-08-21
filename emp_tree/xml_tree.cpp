#include "xml_tree.h"

#include <fstream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

//Tree section

void EmpTree::PrintTree()
{
  std::cout <<"EmpTree Print\n";
}
void EmpTree::GetDataFromSrc(const std::string &)
{
  std::cout <<"Get data from src\n";
}

Company::Company()
  :  m_departments(std::vector<DepartmentBase>{}),
     m_undoStack(std::stack<std::function<void()>>{}),
     m_redoStack(std::stack<std::function<void()>>{})
{}

void Company::PrintTree()
{
  for(auto &dep: m_departments)
    {
      std::string  delimiter = "    ";
      std::cout << dep.getName() <<' ';
      std::cout <<'\n';
      
      std::cout <<delimiter <<"Средняя ЗП: "
		<<  dep.averageSalary() <<'\n';
      
      std::cout << delimiter <<"Количество сотрудников: "
		<< dep.getEmployeesCount() << '\n';
    
      std::cout <<delimiter<<"Сотрудники: \n";
      
      auto mySet = dep.getEmpList();
      for(auto &emp:mySet){
	std::cout <<delimiter + delimiter
		  << emp->name << " " << emp->salary  <<'\n';
      }
    }
}

void Company::GetDataFromSrc(const std::string& path)
{
  std::string xmlString;
  std::ifstream file(path);
  if(file.is_open()){
    xmlString.assign((std::istreambuf_iterator<char>(file)),
		     std::istreambuf_iterator<char>());
  }
  file.close();

  boost::property_tree::ptree tree;
  std::stringstream stream(xmlString);
  try{

    boost::property_tree::read_xml(stream,tree);
    auto tempTree = tree.get_child("departments");

    for(auto &xmlDep:tempTree){
      auto attr =
	xmlDep.second.get_child("<xmlattr>", boost::property_tree::ptree{});
      auto name = attr.get<std::string>("name","NONE");
      DepartmentBase dep(name);    
      fillEmployeesVec(dep,xmlDep);
      m_departments.push_back(dep);    
    }
  
  }catch(std::exception& e){
    std::cerr <<e.what();
  }
}

void Company::fillEmployeesVec(DepartmentBase& department,
			       boost::property_tree::ptree::value_type &xml_dep)
{
  std::cout<< xml_dep.first <<'\n';
  auto emplsList = xml_dep.second.get_child("employments");
  
  for(auto& emp:emplsList){

    std::shared_ptr<Employee> currEmp(new Employee);
    
    currEmp->name =
      emp.second.get<std::string>("name","") + " " + 
      emp.second.get<std::string>("surname","") + " " +
      emp.second.get<std::string>("middleName","");
    
    currEmp->salary = emp.second.get<unsigned int>("salary",0);
    currEmp->function = emp.second.get<std::string>("function","NONE");
    department.addEmployee(currEmp);
  }
}

void Company::addDepartment(DepartmentBase & dep)
{
  m_departments.push_back(dep);
}

void Company::addDepartment(const std::string& name)
{
  std::cout <<"Try add dep\n";
  DepartmentBase dep(name);
  m_departments.push_back(dep);
}


void Company::removeDepartment(const std::string & dep_name)
{
  std::cout <<"Try remove dep\n" << dep_name;
  auto temp = findDepartment(dep_name);
  if (temp != m_departments.end()) m_departments.erase(temp);
}

void Company::changeDepartmentName(const std::string& dep_name,const std::string& to)
{
  auto temp = findDepartment(dep_name);
  temp->changeDepartmentName(to);
}

void Company::addEmployee(const std::string & dep_name, EmployeeSP emp)
{
  std::cout <<"Try add emp\n";
  auto temp = findDepartment(dep_name);
  temp->addEmployee(emp);
}

void Company::removeEmployee(const std::string & dep_name, EmployeeSP emp)
{
  std::cout <<"Try remove emp\n";
  auto temp = findDepartment(dep_name);
  temp->removeEmployee(emp);
}

void Company::MakeChange()
{
  std::cout << "What you wanna do?(take num):\n" 
	    << "1: Add Department " <<"2: Remove department " << "3: Change department name\n"
	    << "4: Add Employee " << "5: Remove Employee" << "6: Change Employee property\n" ;
  int num;
  std::cin >> num;

  switch(num){
  case 1:
    {    //Adddepartment
      std::string depName;
      std::cout <<"add name: ";
      std::cin >> depName;
      addDepartment(depName);
      m_undoStack.push([this,depName]
		       (){removeDepartment(depName);});
      std::cout <<'\n';
    }
    break;
    
  case 2:
    {    //Removedepartment
      std::string depName;
      std::cout <<"choose dep: ";
      std::cin >> depName;
      removeDepartment(depName);
      m_undoStack.push([this,depName]
		       (){addDepartment(depName);});
      std::cout <<'\n';
    }
    break;
    
  case 3:
    {    //Changedepartmentname
      std::string depName;
      std::cout <<"current name: ";
      std::cin >> depName;
      std::cout <<"change to: ";
      std::string newName;
      std::cin >> newName;
      changeDepartmentName(depName, newName);
    
      m_undoStack.push([this,depName,newName]
		       (){changeDepartmentName(newName, depName);});
      std::cout <<'\n';
    }
    break;
    
  case 4:
    {    //Addemployee
      std::string depName;
      std::cout << "choose dep: ";
      std::cin >> depName;
      std::cout << "fill emp info: ";
      std::cout <<"FIO function salary \n";
      std::string fio,function;
      unsigned int salary;
      std::cin >> fio >> function >> salary;
      std::shared_ptr<Employee> emp(new Employee);
      emp->name = fio;
      emp->function = function;
      emp->salary = salary;
      addEmployee(depName, emp);
      m_undoStack.push([this,depName,emp]
		       (){removeEmployee(depName, emp);});
      std::cout <<'\n';
    }
    break;
    
  case 5:
    {    //Removeemployee
      std::string depName;
      std::cout << "choose dep: ";
      std::cin >> depName;
      std::cout << "fill emp info: ";
      std::cout <<"FIO function salary \n";
      std::string fio;
      std::cin >> fio;
      auto dep = findDepartment(depName);
      auto empList = dep->getEmpList();
      auto temp = std::find_if(std::begin(empList),std::end(empList),
			       [&fio] (EmployeeSP mp)
			       {return fio == mp->name;});
      if(temp != empList.end()){
	removeEmployee(depName, *temp);
	m_undoStack.push([this,depName,temp]
			 (){addEmployee(depName, *temp);});
      }
      else
	std::cout << "Cannot remove this emp ";
      std::cout <<'\n';
    }
    break;
    
  case 6:
    //TODO
    break;
    
  default:
    break;
  }
}

void Company::Undo()
{
  std::cout <<"here\n";
  if(m_undoStack.empty()) return;
  std::cout <<"here\n";
  auto func = m_undoStack.top();
  func();
  m_undoStack.pop();
  std::cout <<"here\n";
}

void Company::Redo()
{
  //TODO
}
