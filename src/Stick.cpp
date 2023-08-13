#include "../include/Stick.h"
#include <iostream>



Stick::Stick( Object* obj1, Object* obj2, float length, int ID )
    : obj1Ptr(obj1)
      , obj2Ptr(obj2)
      , length(length)
      , ID(ID)
{
}




Object* Stick::getObject1( )
{
    return obj1Ptr;
}
Object* Stick::getObject2( )
{
    return obj2Ptr;
}

void Stick::resetObjectReferences( Object& obj1, Object& obj2 )
{
    obj1Ptr = &obj1;
    obj2Ptr = &obj2;
}


void Stick::update()
{
    if(!isActive)
        return;
    
    obj1Ptr = &objectsRef->at(0);
    obj2Ptr = &objectsRef->at(1);


    
    sf::Vector2f axis = obj2Ptr->currentPos - obj1Ptr->currentPos;
    float distance = sqrt(axis.x * axis.x + axis.y * axis.y);
    float diff = length - distance;
    float perc = (diff / distance) * 0.5f;
    sf::Vector2f offset = axis * perc;
    
    if(!obj1Ptr->isPinned)
        obj1Ptr->currentPos -= offset;
    if(!obj2Ptr->isPinned)
        obj2Ptr->currentPos += offset;

}
