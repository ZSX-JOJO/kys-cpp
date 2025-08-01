﻿#include "TeamMenu.h"
#include "Button.h"
#include "Save.h"

TeamMenu::TeamMenu()
{
    int w, h;
    Engine::getInstance()->getUISize(w, h);
    for (int i = 0; i < TEAMMATE_COUNT; i++)
    {
        auto h = std::make_shared<Head>();
        addChild(h, i % 2 * 250, i / 2 * 100);
        h->setHaveBox(false);
        //h->setOnlyHead(true);
        heads_.push_back(h);
        //selected_.push_back(0);
    }
    button_all_ = std::make_shared<Button>();
    button_all_->setText("全選");
    button_all_->setFontSize(30);
    button_ok_ = std::make_shared<Button>();
    button_ok_->setText("確定");
    button_ok_->setFontSize(30);
    addChild(button_all_, 200, 300);
    addChild(button_ok_, 300, 300);
    setPosition(w / 2 - 250, 150);
    setTextPosition(20, -30);
}

TeamMenu::~TeamMenu()
{
}

void TeamMenu::onEntrance()
{
    for (int i = 0; i < TEAMMATE_COUNT; i++)
    {
        auto r = Save::getInstance()->getTeamMate(i);
        if (r)
        {
            heads_[i]->setRole(r);
            if (mode_ == 0 && item_)
            {
                if (!r->canUseItem(item_))
                {
                    heads_[i]->setText("不適合");
                }
                if (r->PracticeItem == item_->ID || r->Equip0 == item_->ID || r->Equip1 == item_->ID)
                {
                    heads_[i]->setText("使用中");
                }
            }
        }
    }
    if (mode_ == 0)
    {
        button_all_->setVisible(false);
        button_ok_->setVisible(false);
    }
    for (auto h : heads_)
    {
        h->setVisible(h->getRole());
    }
}

Role* TeamMenu::getRole()
{
    return role_;
}

std::vector<Role*> TeamMenu::getRoles()
{
    std::vector<Role*> roles;
    for (auto h : heads_)
    {
        if (h->getResult() == 0 && h->getRole())
        {
            roles.push_back(h->getRole());
        }
    }
    return roles;
}

void TeamMenu::draw()
{
    Engine::getInstance()->fillColor({ 0, 0, 0, 128 }, 0, 0, -1, -1);
    TextBox::draw();
}

void TeamMenu::onPressedOK()
{
    if (mode_ == 0)
    {
        role_ = nullptr;
        for (auto h : heads_)
        {
            if (h->getState() == NodePress)
            {
                role_ = h->getRole();
            }
        }
        if (role_ && (item_ == nullptr || role_->canUseItem(item_)))
        {
            result_ = 0;
            setExit(true);
        }
    }
    if (mode_ == 1)
    {
        for (auto h : heads_)
        {
            if (h->getState() == NodePress)
            {
                if (h->getResult() == -1)
                {
                    h->setResult(0);
                }
                else
                {
                    h->setResult(-1);
                }
            }
        }
        if (button_all_->getState() == NodePress)
        {
            //如果已经全选，则是清除
            int all = -1;
            for (auto h : heads_)
            {
                if (h->getResult() != 0)
                {
                    all = 0;
                    break;
                }
            }
            for (auto h : heads_)
            {
                h->setResult(all);
            }
        }
        if (button_ok_->getState() == NodePress)
        {
            //没有人被选中，不能确定
            for (auto h : heads_)
            {
                if (h->getResult() == 0)
                {
                    setExit(true);
                }
            }
        }
    }
}

void TeamMenu::onPressedCancel()
{
    if (mode_ == 0)
    {
        role_ = nullptr;
        result_ = -1;
        setExit(true);
    }
}

void TeamMenu::dealEvent(EngineEvent& e)
{
    Menu::dealEvent(e);
    if (mode_ == 0)
    {
        if (item_)
        {
            for (auto h : heads_)
            {
                //if (h->getState() != NodeNormal && !GameUtil::canUseItem(h->getRole(), item_))
                //{
                //    h->setState(NodeNormal);
                //}
            }
        }
    }
    if (mode_ == 1)
    {
        for (auto h : heads_)
        {
            if (h->getResult() == 0)
            {
                h->setText("已選中");
                h->setAlwaysLight(1);
            }
            else
            {
                h->setText("");
                h->setAlwaysLight(0);
            }
        }
        //getChild(active_child_)->setState(NodePress);
    }
    if (force_main_ && !heads_.empty())
    {
        heads_[0]->setResult(0);
    }
    if (e.type == EVENT_KEY_UP)
    {
        if (e.key.key == K_A)
        {
            for (auto h : heads_)
            {
                h->setState(NodeNormal);
            }
            button_all_->setState(NodePress);
            button_ok_->setState(NodeNormal);
            e.key.key = K_RETURN;
        }
        if (e.key.key == K_O)
        {
            button_all_->setState(NodeNormal);
            button_ok_->setState(NodePress);
            e.key.key = K_RETURN;
        }
    }
}
