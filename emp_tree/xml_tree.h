#include "members.h"
#include <vector>
#include <stack>

#include <boost/property_tree/ptree.hpp>

class EmpTree
{
public:
  virtual void PrintTree()=0;
  virtual void GetDataFromSrc(const std::string&)=0;
  virtual void Undo()=0;
  virtual void Redo()=0;
  virtual void MakeChange()=0;
};

class Company: public EmpTree
{
public:

  Company();
  typedef  std::vector<DepartmentBase>::iterator dep_finder_t;
  typedef std::shared_ptr<Employee> EmployeeSP;
  
  virtual void PrintTree() override;
  virtual void GetDataFromSrc(const std::string&) override;  
  virtual void Undo() override;
  virtual void Redo() override;
  virtual void MakeChange() override;

private:
  void fillEmployeesVec(DepartmentBase&,
			boost::property_tree::ptree::value_type&);
  dep_finder_t findDepartment(const std::string& dep_name)
  {
    auto temp = std::find_if(std::begin(m_departments),
			     std::end(m_departments),
			     [dep_name]
			     (DepartmentBase& dep)
			     {return dep.getName() == dep_name;});
    return temp;
  }

    //deps 
  void addDepartment(DepartmentBase&); //DONE
  void addDepartment(const std::string&);  //DONE
  void removeDepartment(const std::string&); //DONE
  void changeDepartmentName(const std::string&,const std::string&); //DONE
  
  //emps
  void addEmployee(const std::string&,EmployeeSP);
  void removeEmployee(const std::string&, EmployeeSP);
  void changeEmployee(const std::string&, EmployeeSP, EmployeeSP);
  
  typedef std::function<void()>  URFunc;
  std::vector<DepartmentBase> m_departments;
  std::stack<URFunc> m_undoStack;
  std::stack<URFunc>  m_redoStack;
  bool m_redoFlag;
};
  

