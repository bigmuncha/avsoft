#include "members.h"
#include <numeric>
#include <iomanip>
#include <fstream>

//Emp Section

//End emp section

//Depart section
DepartmentBase::DepartmentBase(std::string name):
    m_name(name),
    m_average_salary(0),
    m_employees(std::set<EmployeeSP>{})
  {}

unsigned int DepartmentBase::getEmployeesCount()
{
  return m_employees.size();
}

void DepartmentBase::countAverageSalary()
{
  unsigned int temp;
  if(m_employees.size() == 0)
    temp = 0;
  else {
    temp  =
      std::accumulate(std::begin(m_employees),std::end(m_employees),0,
		      []
		      (long long sum,EmployeeSP emp)
		      { return sum + emp->salary;}) / m_employees.size();
  }
  m_average_salary = temp;
}
bool DepartmentBase::addEmployee(EmployeeSP emp)
{
  tryAddEmployee(emp, [this](void){countAverageSalary();});
  return true;
}

bool DepartmentBase::removeEmployee(EmployeeSP emp)
{
  tryRemoveEmployee(emp, [this](void){countAverageSalary();});
  return true;
}

bool DepartmentBase::tryAddEmployee(EmployeeSP emp,std::function<void()>handler)
{
  m_employees.insert(emp);
  handler();
  return true;
}

bool DepartmentBase::tryRemoveEmployee( EmployeeSP emp,std::function<void()>handler)
{
  m_employees.erase(emp);
  handler();
  return true;
}

//End depart section


  
