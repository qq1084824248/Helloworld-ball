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
    
    
    
    //监听鼠标点击，生成小球
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->onTouchBegan = [&](Touch* touch, Event* event)
    {
        //设定小球的属性，随机生成半径和速度
        const Vec2 touchPosition = touch->getLocation();
        const unsigned int radius = random(35, 50);
        const unsigned int weight = square(radius);
        const float color1 = random(0.0f, 1.0f);
        const float color2 = random(0.0f, 1.0f);
        const float color3 = random(0.0f, 1.0f);
        const float speedX = random(-10, 10),speedY = random(-10, 10);
        
        //构建小球对象，加入scene的子节点中
        DrawNode* tempball = DrawNode::create();
        tempball->drawDot(Vec2(0,0), radius, Color4F(color1, color2, color3, 1.0f));
        tempball->setPosition(touchPosition);
        this->addChild(tempball);
        
        
        //构建结构体，加入vector
        HFBall b(tempball, Vec2(speedX,speedY), radius, weight);
        this->balllist.push_back(b);
        
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    
    //调度器，每帧绘制
    auto ref = [&](float dt) {
        if (!balllist.empty())
        {
            //可以增加计算碰撞位置的代码
            
            //设定本帧结束时的位置
            for (auto& ball : balllist)
            {
                ball.nextPosition = Vec2(ball.ballObeject->getPositionX() + ball.speed.x,
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
            
            
            //更新位置
            for (auto& ball : balllist)
            {
                ball.ballObeject->setPosition(ball.nextPosition);
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

static bool calculateCollisionTime(HFBall &balla, HFBall &ballb, float &t) {
    
    float dx = balla.ballObeject->getPositionX() - ballb.ballObeject->getPositionX();
    float dy = balla.ballObeject->getPositionY() - ballb.ballObeject->getPositionY();
    //相对速度
    float dvx = balla.speed.x - ballb.speed.x;
    float dvy = balla.speed.y - ballb.speed.y;
    
    //二次方程的参数
    //        float a = dvx * dvx + dvy * dvy;
    //        float b = -2 * (dx * dvx + dy * dvy);
    //        float c = dx * dx - dy * dy - square(balla.radius + ballb.radius);
    
    t = 0;
    //公式解得根号部分
    float b = square(balla.radius + ballb.radius) * (square(dvx) + square(dvy))
    - square(dx * dvy - dy * dvx);
    if (b < 0)
    {
        return false;
    }
    //公式解的上半部分
    float p = -dx*dvx - dy*dvy + sqrt(b);
    float p2 = -dx*dvx - dy*dvy - sqrt(b);
    
    if (square(dvx) + square(dvy) == 0)
    {
        t = 1;
    }
    else
    {
        float temp = fmin(abs(p), abs(p2));
        t = temp/(square(dvx) + square(dvy));
    }
    
    return true;
}

//检测小球碰撞
bool detectCollision(HFBall& balla,HFBall& ballb)
{
    //不发生碰撞时下一帧的位置
    float ballax = balla.ballObeject->getPositionX() + balla.speed.x;
    float ballay = balla.ballObeject->getPositionY() + balla.speed.y;
    float ballbx = ballb.ballObeject->getPositionX() + ballb.speed.x;
    float ballby = ballb.ballObeject->getPositionY() + ballb.speed.y;
    
    //没有碰撞，返回false
    if (square(ballax - ballbx) + square(ballay - ballby)
       > square(balla.radius + ballb.radius))
    {
        return false;
    }
    //发生了碰撞
    else
    {
        //计算碰撞位置
        //相对位置
        float t;
        if (! calculateCollisionTime(balla, ballb, t))
        {
            return false;
        }
        
        //碰撞前的位置
        ballax = balla.ballObeject->getPositionX() + balla.speed.x*t;
        ballay = balla.ballObeject->getPositionY() + balla.speed.y*t;
        ballbx = ballb.ballObeject->getPositionX() + ballb.speed.x*t;
        ballby = ballb.ballObeject->getPositionY() + ballb.speed.y*t;
        
        //根据碰撞时的相对位置计算速度
        //对心碰撞
        if (balla.speed.cross(ballb.speed) == 0
            && balla.speed.cross(Vec2(ballax-ballbx,ballay-ballby)) == 0)
        {
            Vec2 tempspeeda = balla.speed;
            Vec2 tempspeedb = ballb.speed;
            balla.speed = ((0.0f + balla.weight - ballb.weight)*tempspeeda + 2*ballb.weight*tempspeedb)
                        / (balla.weight + ballb.weight);
            ballb.speed = ((0.0f + ballb.weight - balla.weight)*tempspeedb + 2*balla.weight*tempspeeda)
                        / (balla.weight + ballb.weight);
        }
        //非对心碰撞
        else if (ballax - ballbx == 0)
        {
            float tmp = balla.speed.y;
            balla.speed.y = ((0.0f + balla.weight - ballb.weight)*balla.speed.y + 2*ballb.weight*ballb.speed.y)
                        / (balla.weight + ballb.weight);
            ballb.speed.y = ((0.0f + ballb.weight - balla.weight)*ballb.speed.y + 2*balla.weight*tmp)
                        / (balla.weight + ballb.weight);
        }
        else if (ballay - ballby == 0)
        {
            float tmp = balla.speed.x;
            balla.speed.x = ((0.0f + balla.weight - ballb.weight)*balla.speed.x + 2*ballb.weight*ballb.speed.x)
                        / (balla.weight + ballb.weight);
            ballb.speed.x = ((0.0f + ballb.weight - balla.weight)*ballb.speed.x + 2*balla.weight*tmp)
                        / (balla.weight + ballb.weight);
        }
        else
        {

            //碰撞处切平面向量t，及其法向量s
            Vec2 s(ballax - ballbx, ballay - ballby);
            s.normalize();
            Vec2 t(s);
            t.rotate(Vec2(0,0),3.1415926f/2);
            t.normalize();

            //先计算球a的speed在s和t轴的投影值，分别设为speedAs和speedAt
            //再计算球b的speed在s和t轴的投影值，分别设为speedBs和speedBt
            Vec2 speedAs = balla.speed.project(s);
            Vec2 speedAt = balla.speed.project(t);
            Vec2 speedBs = ballb.speed.project(s);
            Vec2 speedBt = ballb.speed.project(t);

            //s轴上的速度交换
            Vec2 temp_speedAs = speedAs;
            speedAs = ((0.0f + balla.weight - ballb.weight)*speedAs + 2*ballb.weight*speedBs)
            / (balla.weight + ballb.weight);
            speedBs = ((0.0f + ballb.weight - balla.weight)*speedBs + 2*balla.weight*temp_speedAs)
            / (balla.weight + ballb.weight);
            
            balla.speed.x = (speedAs + speedAt).x;
            balla.speed.y = (speedAs + speedAt).y;
            ballb.speed.x = (speedBs + speedBt).x;
            ballb.speed.y = (speedBs + speedBt).y;
            
        }
        


        //碰撞后的位置
        ballax = ballax + balla.speed.x * (1 - t);
        ballay = ballay + balla.speed.y * (1 - t);
        ballbx = ballbx + ballb.speed.x * (1 - t);
        ballby = ballby + ballb.speed.y * (1 - t);
        
        //将碰撞后的位置存入小球的预测位置中
        balla.nextPosition = Vec2(ballax, ballay);
        ballb.nextPosition = Vec2(ballbx, ballby);


 
        //发生碰撞返回true
        return true;
    }
    
}

//检测边界碰撞
bool HelloWorld::detectboundary(HFBall &ball) {
    
    auto ballNextPosition = ball.nextPosition;
    bool state = false;
//    double distancex = 0;
//    double distancey = 0;
    //检测左右边界
    if (ballNextPosition.x < widthLeft + ball.radius)
    {
        //速度变为向右，防止小球在墙中抖动
        ball.speed.x = abs(ball.speed.x);
        state = true;
        //更改小球位置，防止动画穿模
//        distancex = abs(ball.radius - (ballNextPosition.x - widthLeft));
//        if (ball.speed.x == 0)
//        {
//            ball.ballObeject->setPositionX(ball.ballObeject->getPositionX() + distancex);
//        }
//        else
//        {
//            ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + distancex,
//                                          ball.ballObeject->getPositionY() + distancex*ball.speed.y/abs(ball.speed.x));
//        }

    }
    else if (ballNextPosition.x > widthRight - ball.radius)
    {
        ball.speed.x = abs(ball.speed.x) * -1;
        state = true;
        
//        distancex = abs(ball.radius - (widthRight - ballNextPosition.x));
//        if (ball.speed.x == 0)
//        {
//            ball.ballObeject->setPositionX(ball.ballObeject->getPositionX() - distancex);
//        }
//        else
//        {
//            ball.ballObeject->setPosition(ball.ballObeject->getPositionX() - distancex,
//                                          ball.ballObeject->getPositionY() + distancex*ball.speed.y/abs(ball.speed.x));
//        }
       
    }
    
    //检测上下边界
    if (ballNextPosition.y < heightDown + ball.radius)
    {
        ball.speed.y = abs(ball.speed.y);
        state = true;
        
//        distancey = abs(ball.radius - (ballNextPosition.y - heightDown));
//        if (ball.speed.y == 0)
//        {
//            ball.ballObeject->setPositionY(ball.ballObeject->getPositionY() + distancey);
//        }
//        else
//        {
//            ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + distancey*ball.speed.x/abs(ball.speed.y),
//                                          ball.ballObeject->getPositionY() + distancey);
//        }

    }
    else if (ballNextPosition.y > heightUp - ball.radius)
    {
        ball.speed.y = abs(ball.speed.y) * -1;
        state = true;
        
//        distancey = abs(ball.radius - (heightUp - ballNextPosition.y));
//        if (ball.speed.y == 0)
//        {
//            ball.ballObeject->setPositionY(ball.ballObeject->getPositionY() - distancey);
//        }
//        else
//        {
//            ball.ballObeject->setPosition(ball.ballObeject->getPositionX() + distancey*ball.speed.x/abs(ball.speed.y),
//                                          ball.ballObeject->getPositionY() - distancey);
//        }

    }
    
    return state;
    
}
