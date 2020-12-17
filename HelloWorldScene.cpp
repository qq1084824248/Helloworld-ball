#include "HelloWorldScene.h"
#include "time.h"
#include "stdlib.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}
bool detectCollision(HFBall& a,HFBall& b);





bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    //初始化边界
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    widthLeft = origin.x;
    widthRight = origin.x + visibleSize.width;
    heightDown = origin.y;
    heightUp = origin.y + visibleSize.height;
    
    
    //绘制清除按钮
    auto button =  cocos2d::ui::Button::create();
    button->setTitleText("Clear Button");
    button->setTitleFontSize(25);
    button->setTitleColor(Color3B::ORANGE);
    button->setPosition(Vec2(visibleSize.width/4,visibleSize.height/4));
    button->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::BEGAN:
                for(auto ball : balllist)
                {
                    this->removeChild(ball.ballObeject);
                }
                balllist.clear();
                break;
            case ui::Widget::TouchEventType::ENDED:
                break;
            default:
                break;
        }
    });
    this->addChild(button, 200);
    
    
    
    //监听鼠标点击
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->onTouchBegan = [&](Touch* touch, Event* event)
    {
        //设定小球的属性，随机生成半径和速度
        const Vec2 touchpos = touch->getLocation();
        const int radius = random(20, 50);
        const float speedX = random(-10, 10),speedY = random(-10, 10);
        
        //构建小球对象，加入scene的子节点中
        DrawNode* tempball = DrawNode::create();
        tempball->drawDot(Vec2(0,0), radius, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
        tempball->setPosition(touchpos);
        this->addChild(tempball);
        
        
        //构建结构体，加入vector
        HFBall b(tempball,Vec2(speedX,speedY),radius);
        this->balllist.push_back(b);
        
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    
    //调度器，每帧绘制
    auto ref = [&](float dt) {
        if (!balllist.empty())
        {
            //可以增加计算碰撞位置的代码
            
            //设定位置
            for (auto& ball : balllist)
            {
                ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + ball.speed.x,
                                              ball.ballObeject->getPositionY() + ball.speed.y);
            }
            
            //判断边界条件，碰撞时速度取反
            for (auto& ball : balllist)
            {
                detectboundary(ball);
            }
            
            //检测小球碰撞
            for (std::vector<HFBall>::iterator iter1 = balllist.begin(); iter1 != balllist.end(); ++iter1)
            {
                for (std::vector<HFBall>::iterator iter2 = iter1 + 1; iter2 != balllist.end(); ++iter2){
                    detectCollision(*iter1, *iter2);
                }
            }

        }
    };
    schedule(ref, "moveball");
    
    return true;
}


//平方辅助内联函数
inline float square(float x)
{
    return x*x;
}

//检测小球碰撞
bool detectCollision(HFBall& a,HFBall& b)
{
    float ballax = a.ballObeject->getPositionX();
    float ballay = a.ballObeject->getPositionY();
    float ballbx = b.ballObeject->getPositionX();
    float ballby = b.ballObeject->getPositionY();
    //没有碰撞，返回false
    if (square(ballax - ballbx) + square(ballay - ballby)
       > square(a.radius + b.radius))
    {
        return false;
    }
    //发生了碰撞
    else
    {
        //对心碰撞
        if (a.speed.cross(b.speed) == 0
            && a.speed.cross(Vec2(ballax-ballbx,ballay-ballby)) == 0)
        {
            Vec2 tempspeed = a.speed;
            a.speed = b.speed;
            b.speed = tempspeed;
        }
        //非对心碰撞
        else if (ballax - ballbx == 0)
        {
            float tmp = a.speed.y;
            a.speed.y = b.speed.y;
            b.speed.y = tmp;
        }
        else if (ballay - ballby == 0)
        {
            float tmp = a.speed.x;
            a.speed.x = b.speed.x;
            b.speed.x = tmp;
        }
        else
        {
            Vec2 tempspeed = a.speed;
            a.speed = b.speed;
            b.speed = tempspeed;
            
//            //碰撞处切平面向量t，及其法向量s
//            Vec2 s(ballax - ballbx, ballay - ballby);
//            s.normalize();
//            Vec2 t(s);
//            t.rotate(Vec2(0,0),3.1415926f/2);
//            t.normalize();
//
//            //先计算球a的speed在s和t轴的投影值，分别设为speed1s和speed1t
//            //再计算球b的speed在s和t轴的投影值，分别设为speed2s和speed2t
//            Vec2 speed1s = a.speed.project(s);
//            Vec2 speed1t = a.speed.project(t);
//            Vec2 speed2s = b.speed.project(s);
//            Vec2 speed2t = b.speed.project(t);
//
//            Vec2 temp_speed1s = speed1s;
//            speed1s = speed2s;
//            speed2s = temp_speed1s;
//
//            a.speed.x = (speed1s + speed1t).x;
//            a.speed.y = (speed1s + speed1t).y;
//            b.speed.x = (speed2s + speed2t).x;
//            b.speed.y = (speed2s + speed2t).y;
        }


        
        //重新设定位置，防止小球重合
        float distance = sqrt(square(ballax - ballbx) + square(ballay - ballby));
        float overlap = (a.radius + b.radius - distance)/2;
        float speedavalue = sqrt(square(a.speed.x) + square(a.speed.y));
        float speedbvalue = sqrt(square(b.speed.x) + square(b.speed.y));
        

        a.ballObeject->setPosition(a.ballObeject->getPositionX() + overlap*a.speed.x/speedavalue,
                                   a.ballObeject->getPositionY() + overlap*a.speed.y/speedavalue);
        b.ballObeject->setPosition(b.ballObeject->getPositionX() + overlap*b.speed.x/speedbvalue,
                                   b.ballObeject->getPositionY() + overlap*b.speed.y/speedbvalue);
        //发生碰撞返回true
        return true;
    }
    
}

//检测边界碰撞
bool HelloWorld::detectboundary(HFBall &ball) {
    auto ballPosition = ball.ballObeject->getPosition();
    bool state = false;
    double distancex = 0;
    double distancey = 0;
    //检测左右边界
    if (ballPosition.x < widthLeft + ball.radius)
    {
        //速度变为向右，防止小球在墙中抖动
        ball.speed.x = abs(ball.speed.x);
        state = true;
        //更改小球位置，防止动画穿模
        distancex = abs(ball.radius - (ballPosition.x - widthLeft));
        ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + distancex,
                                      ball.ballObeject->getPositionY() + distancex*ball.speed.y/abs(ball.speed.x));
    }
    else if (ballPosition.x > widthRight - ball.radius)
    {
        ball.speed.x = abs(ball.speed.x) * -1;
        state = true;
        
        distancex = abs(ball.radius - (widthRight - ballPosition.x));
        ball.ballObeject->setPosition(ball.ballObeject->getPositionX() - distancex,
                                      ball.ballObeject->getPositionY() + distancex*ball.speed.y/abs(ball.speed.x));
    }
    
    //检测上下边界
    if (ballPosition.y < heightDown + ball.radius)
    {
        ball.speed.y = abs(ball.speed.y);
        state = true;
        
        distancey = abs(ball.radius - (ballPosition.y - heightDown));
        ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + distancey*ball.speed.x/abs(ball.speed.y),
                                      ball.ballObeject->getPositionY() + distancey);
    }
    else if (ballPosition.y > heightUp - ball.radius)
    {
        ball.speed.y = abs(ball.speed.y) * -1;
        state = true;
        
        distancey = abs(ball.radius - (heightUp - ballPosition.y));
        ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + distancey*ball.speed.x/abs(ball.speed.y),
                                      ball.ballObeject->getPositionY() - distancey);
    }
    
    return state;
    
}
