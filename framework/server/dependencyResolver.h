/*
 * dependencyResolver.h
 *
 *  Created on: Nov 2, 2013
 *      Author: vagrant
 */

#ifndef FRAMEWORK_SERVER_DEPENDENCYRESOLVER_H_
#define FRAMEWORK_SERVER_DEPENDENCYRESOLVER_H_

#include "service.h"
#include "logging.h"
#include <list>
#include <map>
#include <vector>
#include <string.h>
#include <algorithm>

#define int32 int


enum SORTTYPE {INITSORT = 0, STARTSORT, STOPSORT};

class DependencyResolver
{
public:
	DependencyResolver();
    ~DependencyResolver();

public:
    std::list<std::string> GetSort(std::map<std::string, Service*> &services, SORTTYPE type);
    std::list<std::string> GetLackServices(std::map<std::string, Service*> &services);

protected:
    bool TopSort(std::map<std::string, Service*> services);
    std::map<std::string, Service*> GetNewServices(std::map<std::string, Service*> &services);
    std::list<std::string> LoadInitSort(std::map<std::string, Service*> &services);

private:
    std::list<std::string> orders_;
};


#endif  // FRAMEWORK_SERVER_DEPENDENCYRESOLVER_H_
