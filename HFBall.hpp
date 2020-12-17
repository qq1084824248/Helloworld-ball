//
//  ball.hpp
//  ShengJi
//
//  Created by duole on 2020/12/14.
//

#ifndef ball_hpp
#define ball_hpp

#include <stdio.h>
#include <cocos2d.h>

struct HFBall{
public:
    HFBall(cocos2d::DrawNode *ballpoint, cocos2d::Vec2 v, unsigned int r, unsigned int t)
    :ballObeject(ballpoint), speed(v), radius(r),weight(t)
    {
    }
    
    cocos2d::DrawNode *ballObeject;
    cocos2d::Vec2 speed;
    unsigned int radius;
    unsigned int weight;
};

#endif /* ball_hpp */
