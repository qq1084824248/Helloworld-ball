//
//  test.cpp
//  ShengJi
//
//  Created by duole on 2020/12/14.
//

#include "test.hpp"
//auto ref = [&](float dt) {
//    float tempX,tempY;
//    //ball -> setPosition(ball->getPositionX() + speedX, ball->getPositionY() + speedY);
//    tempX = ball->getPositionX() + speedX;
//    //y也要按比例修改，
//    if((tempX < widthLeft + ball->getContentSize().width/2 )||
//       (tempX > widthRight - ball->getContentSize().width/2)){
//        if(tempX < widthLeft + ball->getContentSize().width/2){
//            tempX = widthLeft + ball->getContentSize().width/2;
//        }
//        else{
//            tempX = widthRight - ball->getContentSize().width/2;
//        }
//
//        speedX *= -1;
//    }
//
//
//    tempY = ball->getPositionY() + speedY;
//    if( tempY < heightDown + ball->getContentSize().height/2 ||
//       tempY > heightUp - ball->getContentSize().height/2){
//        if(tempY < heightDown + ball->getContentSize().height/2){
//            tempY = heightDown + ball->getContentSize().height/2;
//        }
//        else{
//            tempY = heightUp - ball->getContentSize().height/2;
//        }
//
//        speedY *= -1;
//    }
//
//    ball->setPosition(tempX, tempY);
//
//    };
