//****************************************************************************/
//  File:   PSModelPlacer.h
//  Date:   12.07.2006
//  Author: Ruslan Shestopalyuk
//****************************************************************************/
#ifndef __PSMODELPLACER_H__
#define __PSMODELPLACER_H__

//****************************************************************************/
//  Class:  PSModelPlacer
//  Desc:   Base class for particle system operators
//****************************************************************************/
class PSModelPlacer : public PSOperator
{
public: 
    PSModelPlacer();

    expose( PSModelPlacer )
    {
        parent(PSOperator);
    }
}; // class PSModelPlacer

#endif // __PSModelPlacer_H__


