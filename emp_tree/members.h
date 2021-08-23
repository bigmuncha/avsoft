#include <iostream>
#include <set>
#include <functional>
#include <memory>

struct Employee
{
  std::string name;
  std::string function;
  unsigned int salary;
  Employee():salary(0){}
};

class DepartmentBase
{
public:

  typedef std::shared_ptr<Employee> EmployeeSP;

  DepartmentBase(std::string name);

  unsigned int getEmployeesCount();
  bool addEmployee(EmployeeSP);
  bool removeEmployee(EmployeeSP);
  bool changeEmployee(EmployeeSP,EmployeeSP);
  void changeDepartmentName(std::string name)
  {m_name = name;}
  
  std::string getName()
  {return m_name;}

  const std::set<EmployeeSP>& getEmpList()
  {return m_employees;}

  unsigned int averageSalary()
  { return m_average_salary;};

private:

  void countAverageSalary();
  bool tryAddEmployee( EmployeeSP,std::function<void()>handler);
  bool tryRemoveEmployee( EmployeeSP,std::function<void()>handler);
  bool tryChangeEmployee(EmployeeSP, EmployeeSP, std::function<void()>handler);
  std::set<EmployeeSP> m_employees;
  std::string m_name;
  unsigned int m_average_salary;
};


