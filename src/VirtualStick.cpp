﻿#include "VirtualStick.h"
#include "Button.h"
#include "TextureManager.h"

#include "BattleSceneHades.h"
#include "BattleSceneSekiro.h"

VirtualStick::VirtualStick()
{
    Engine::getInstance()->getUISize(w_, h_);

    auto addButton = [this](std::shared_ptr<Button>& b, int id, int x, int y, int buttonid)
    {
        b = std::make_shared<Button>("title", id);
        b->setAlpha(128);
        b->button_id_ = buttonid;
        addChild(b, x, y);
    };

    {
        int x = w_ * 0.1;
        int y = h_ * 0.65;
        int r = h_ * 0.1;
        addButton(button_up_, 312, x, y - r, SDL_GAMEPAD_BUTTON_DPAD_UP);
        addButton(button_down_, 312, x, y + r, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        addButton(button_left_, 312, x - r, y, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        addButton(button_right_, 312, x + r, y, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    }

    {
        int x = w_ * 0.8;
        int y = h_ * 0.65;
        int r = h_ * 0.15;
        addButton(button_a_, 300, x, y + r, GAMEPAD_BUTTON_SOUTH);
        addButton(button_b_, 302, x + r, y, GAMEPAD_BUTTON_EAST);
        addButton(button_x_, 304, x - r, y, GAMEPAD_BUTTON_WEST);
        addButton(button_y_, 306, x, y - r, GAMEPAD_BUTTON_NORTH);
    }
    //addButton(button_lb_, 300, w_ * 0.1, h_ * 0.5, GAMEPAD_BUTTON_LEFT_SHOULDER);
    //addButton(button_rb_, 302, w_ * 0.1, h_ * 0.8, GAMEPAD_BUTTON_RIGHT_SHOULDER);
    addButton(button_left_axis_, 320, w_ * 0.04, h_ * 0.5, GAMEPAD_BUTTON_LEFT_STICK);
    button_left_axis_->setVisible(false);

    addButton(button_view_, 310, w_ * 0.4 - 25, h_ * 0.9, GAMEPAD_BUTTON_BACK);
    addButton(button_menu_, 308, w_ * 0.6 - 25, h_ * 0.9, GAMEPAD_BUTTON_START);
}

void VirtualStick::dealEvent(EngineEvent& e)
{
    bool is_real = RunNode::topIsType<BattleSceneHades>() || RunNode::topIsType<BattleSceneSekiro>();
    if (is_real)
    {
        setStyle(1);
    }
    else
    {
        setStyle(0);
    }
    int num = 0;
    SDL_GetTouchDevices(&num);
    //LOG("{}", num);
    auto engine = Engine::getInstance();
    engine->clearGameControllerButton();
    engine->clearGameControllerAxis();
    axis_x_ = 0;
    axis_y_ = 0;
    if (axis_radius_ == 0)
    {
        auto t = TextureManager::getInstance()->getTexture("title", 320);
        axis_center_x_ = t->w / 2 + w_ * 0.04;
        axis_center_y_ = t->h / 2 + h_ * 0.5;
        axis_radius_ = t->w / 2;
    }
    //LOG("{}", "clear button");
    for (auto c : childs_)
    {
        auto b = std::dynamic_pointer_cast<Button>(c);
        b->state_ = NodeNormal;
    }
    auto touch = SDL_GetTouchDevices(&num);
    for (int i_device = 0; i_device < num; i_device++)
    {        
        if (!touch)
        {
            continue;
        }
        int fingers;
        SDL_Finger** finger_pp;
        finger_pp = SDL_GetTouchFingers(*touch, &fingers);
        bool is_press = false;
        bool is_press2 = false;    //将按键范围扩大一点，避免按到键中间被当成鼠标操作
        for (int i = 0; i < fingers; i++)
        {
            auto s = finger_pp[i];
            //LOG("{}: {} {} ", i, s->x * w_, s->y * h_);
            int x = s->x * w_;
            int y = s->y * h_;
            for (auto c : childs_)
            {
                auto b = std::dynamic_pointer_cast<Button>(c);
                int x2, y2, w2, h2;
                b->getPosition(x2, y2);
                b->getSize(w2, h2);
                x2 -= w2 / 2;
                y2 -= h2 / 2;
                w2 *= 2;
                h2 *= 2;
                is_press2 = is_press2
                    || (x >= x2 && x < x2 + w2 && y >= y2 && y < y2 + h2);
                if (b != button_left_axis_)
                {
                    b->state_ = NodeNormal;
                    engine->setGameControllerButton(b->button_id_, 0);
                    if (b->inSideInStartWindow(x, y))
                    {
                        auto& intval = button_interval_[b];
                        if (engine->getTicks() - intval.prev_press > intval.interval)
                        {
                            b->state_ = NodePress;
                            engine->setGameControllerButton(b->button_id_, 1);
                            intval.prev_press = engine->getTicks();
                            is_press = true;
                            intval.interval = 20;
                            if (is_real)
                            {
                                intval.interval = 0;
                            }
                        }
                    }
                }
                else
                {
                    if (b->getVisible())
                    {
                        axis_x_ = x;
                        axis_y_ = y;
                        double r = sqrt((x - axis_center_x_) * (x - axis_center_x_) + (y - axis_center_y_) * (y - axis_center_y_));
                        //LOG("{}", r);
                        auto& intval = button_interval_[b];
                        if (r < axis_radius_ * 1.5)
                        {
                            engine->setGameControllerAxis(SDL_GAMEPAD_AXIS_LEFTX, (x - axis_center_x_) * 30000 / axis_radius_);
                            engine->setGameControllerAxis(SDL_GAMEPAD_AXIS_LEFTY, (y - axis_center_y_) * 30000 / axis_radius_);
                            button_interval_[b].prev_press = engine->getTicks();
                            is_press = true;
                            b->state_ = NodePress;
                            intval.interval = 0;
                            if (is_real)
                            {
                                intval.interval = 0;
                            }
                        }
                    }
                }
            }
        }
        if (!is_real)
        {
            if (is_press && button_a_->state_ == NodePress)
            {
                //LOG("{}", "press a");
                e.type = EVENT_KEY_UP;
                e.key.key = K_RETURN;
                button_interval_[button_a_].interval = 100;
            }
            if (is_press && button_b_->state_ == NodePress)
            {
                e.type = EVENT_KEY_UP;
                e.key.key = K_ESCAPE;
                button_interval_[button_b_].interval = 100;
            }
        }
        if (is_press && button_menu_->state_ == NodePress)
        {
            e.type = EVENT_KEY_UP;
            e.key.key = K_ESCAPE;
            if (!is_real)
            {
                button_interval_[button_menu_].interval = 100;
            }
        }
        if (is_press && button_view_->state_ == NodePress)
        {
            e.type = EVENT_KEY_UP;
            e.key.key = K_TAB;
            if (!is_real)
            {
                button_interval_[button_view_].interval = 100;
            }
        }
        if (is_press2)
        {
            prev_press_ = engine->getTicks();
        }
        //一定时间内忽略鼠标事件
        if ((e.type == EVENT_MOUSE_BUTTON_UP || e.type == EVENT_MOUSE_BUTTON_DOWN || e.type == EVENT_MOUSE_MOTION)
            && engine->getTicks() - prev_press_ < 500)
        {
            e.type = EVENT_FIRST;
        }
        if (is_press)
        {
            return;
        }
        touch++;
    }
}

void VirtualStick::draw()
{
    if (axis_x_ > 0 && axis_y_ > 0)
    {
        auto t = TextureManager::getInstance()->getTexture("title", 312);
        TextureManager::getInstance()->renderTexture("title", 312, axis_x_ - t->w / 2, axis_y_ - t->h / 2, { 255, 255, 255, 255 }, 128);
    }
}

void VirtualStick::setStyle(int style)
{
    if (style == 0)
    {
        button_up_->setVisible(true);
        button_down_->setVisible(true);
        button_left_->setVisible(true);
        button_right_->setVisible(true);
        button_left_axis_->setVisible(false);
    }
    else if (style == 1)
    {
        button_up_->setVisible(false);
        button_down_->setVisible(false);
        button_left_->setVisible(false);
        button_right_->setVisible(false);
        button_left_axis_->setVisible(true);
    }
}
