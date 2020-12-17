#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "HFBall.hpp"
#include <vector>

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    bool detectboundary(HFBall &ball);
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    

    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    float widthLeft,widthRight,heightUp,heightDown;
    std::vector<HFBall> balllist;

};
float square(float x);
#endif // __HELLOWORLD_SCENE_H__
