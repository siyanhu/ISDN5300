#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <vector>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

  Group(){

  }
	
  Group(int num_objects);

  ~Group(){
   
  }

  virtual bool intersect(const Ray& r, Hit& h, float tmin);
  void addObject(int index, Object3D* obj);
  int getGroupSize();

 private:
	 vector<Object3D *>object_list;
};

#endif
	
