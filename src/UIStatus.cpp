﻿#include "UIStatus.h"
#include "BattleMenu.h"
#include "Event.h"
#include "Font.h"
#include "Save.h"
#include "ShowRoleDifference.h"
#include "TeamMenu.h"
#include "TextureManager.h"
#include "UI.h"

#include <cassert>

UIStatus::UIStatus()
{
    menu_ = std::make_shared<Menu>();
    button_medicine_ = std::make_shared<Button>();
    button_medicine_->setText("醫療");
    menu_->addChild(button_medicine_, 350, 55);
    button_detoxification_ = std::make_shared<Button>();
    button_detoxification_->setText("解毒");
    menu_->addChild(button_detoxification_, 400, 55);
    button_leave_ = std::make_shared<Button>();
    button_leave_->setText("離隊");
    menu_->addChild(button_leave_, 450, 55);

    //menu_ = std::make_shared<Menu>();
    equip_magics_.resize(4);
    for (auto& em : equip_magics_)
    {
        em = std::make_shared<Button>();
    }
    equip_magics_[0]->setText("__________");
    menu_->addChild(equip_magics_[0], 40, 620);
    equip_magics_[1]->setText("__________");
    menu_->addChild(equip_magics_[1], 160, 610);
    equip_magics_[2]->setText("__________");
    menu_->addChild(equip_magics_[2], 280, 620);
    equip_magics_[3]->setText("__________");
    menu_->addChild(equip_magics_[3], 160, 635);

    equip_item_ = std::make_shared<Button>();
    //menu_ = std::make_shared<Menu>();

    equip_item_->setText("__________");
    menu_->addChild(equip_item_, 420, 620);

    //unfinished
    button_equip0_ = std::make_shared<Button>();
    menu_->addChild(button_equip0_);
    button_equip1_ = std::make_shared<Button>();
    menu_->addChild(button_equip1_);
    button_learning_book_ = std::make_shared<Button>();
    menu_->addChild(button_learning_book_);

    addChild(menu_);
    //addChild(menu_equip_magic_);
    //addChild(menu_equip_item_);
}

UIStatus::~UIStatus()
{
}

void UIStatus::draw()
{
    if (role_ == nullptr || !show_button_)
    {
        button_medicine_->setVisible(false);
        button_detoxification_->setVisible(false);
        button_leave_->setVisible(false);
        setExtentionVisible(false);
    }
    if (role_)
    {
        if (show_button_)
        {
            button_medicine_->setVisible(role_->Medicine > 0);
            button_detoxification_->setVisible(role_->Detoxification > 0);
            button_leave_->setVisible(role_->ID != 0);
            if (role_->ID == 0)
            {
                setExtentionVisible(true);
            }
            else
            {
                setExtentionVisible(false);
            }
        }
        menu_->setVisible(true);
    }
    else
    {
        menu_->setVisible(false);
        setExtentionVisible(false);
        return;
    }
    TextureManager::getInstance()->renderTexture("head", role_->HeadID, x_ + 10, y_ + 20);

    auto font = Font::getInstance();
    Color color_white = { 255, 255, 255, 255 };
    Color color_name = { 255, 215, 0, 255 };
    Color color_ability1 = { 255, 250, 205, 255 };
    Color color_ability2 = { 236, 200, 40, 255 };
    Color color_red = { 255, 90, 60, 255 };
    Color color_magic = { 236, 200, 40, 255 };
    Color color_magic_level1 = { 253, 101, 101, 255 };
    Color color_purple = { 208, 152, 208, 255 };
    Color color_magic_empty = { 236, 200, 40, 255 };
    Color color_equip = { 165, 28, 218, 255 };

    auto select_color1 = [&](int v, int max_v) -> Color
    {
        if (v >= max_v * 0.9)
        {
            return color_red;
        }
        else if (v >= max_v * 0.8)
        {
            return { 255, 165, 79, 255 };
        }
        else if (v >= max_v * 0.7)
        {
            return { 255, 255, 50, 255 };
        }
        else if (v < 0)
        {
            return color_purple;
        }
        return color_white;
    };

    auto select_color2 = [&](int v) -> Color
    {
        if (v > 0)
        {
            return color_red;
        }
        if (v < 0)
        {
            return color_purple;
        }
        return color_white;
    };

    int font_size = 22;

    int x, y;

    x = x_ + 200;
    y = y_ + 50;
    font->draw(std::format("{}", role_->Name), 30, x - 10, y, color_name);
    font->draw("等級", font_size, x, y + 50, color_ability1);
    font->draw(std::format("{:5}", role_->Level), font_size, x + 66, y + 50, color_white);
    font->draw("經驗", font_size, x, y + 75, color_ability1);
    font->draw(std::format("{:5}", role_->Exp), font_size, x + 66, y + 75, color_white);

    std::string str = "";
    font->draw("升級", font_size, x, y + 100, color_ability1);

    int exp_up = role_->getLevelUpExp(role_->Level);
    if (exp_up != INT_MAX)
    {
        str = std::format("{:6}", exp_up);
    }
    else
    {
        str = "------";
    }
    font->draw(str, font_size, x + 55, y + 100, color_white);
    font->draw("生命", font_size, x + 175, y + 50, color_ability1);
    font->draw(std::format("{:5}/", role_->HP), font_size, x + 219, y + 50, color_white);
    font->draw(std::format("{:5}", role_->MaxHP), font_size, x + 285, y + 50, color_white);
    font->draw("內力", font_size, x + 175, y + 75, color_ability1);

    Color c = color_white;
    if (role_->MPType == 0)
    {
        c = color_purple;
    }
    else if (role_->MPType == 1)
    {
        c = color_magic;
    }

    font->draw(std::format("{:5}/", role_->MP), font_size, x + 219, y + 75, c);
    font->draw(std::format("{:5}", role_->MaxMP), font_size, x + 285, y + 75, c);
    font->draw("體力", font_size, x + 175, y + 100, color_ability1);
    font->draw(std::format("{:5}/", role_->PhysicalPower), font_size, x + 219, y + 100, color_white);
    font->draw(std::format("{:5}", 100), font_size, x + 285, y + 100, color_white);

    x = x_ + 20;
    y = y_ + 200;

    font->draw("攻擊", font_size, x, y, color_ability1);
    font->draw(std::format("{:5}", role_->Attack), font_size, x + 44, y, select_color1(role_->Attack, Role::getMaxValue()->Attack));
    font->draw("防禦", font_size, x + 200, y, color_ability1);
    font->draw(std::format("{:5}", role_->Defence), font_size, x + 244, y, select_color1(role_->Defence, Role::getMaxValue()->Defence));
    font->draw("輕功", font_size, x + 400, y, color_ability1);
    font->draw(std::format("{:5}", role_->Speed), font_size, x + 444, y, select_color1(role_->Speed, Role::getMaxValue()->Speed));

    font->draw("醫療", font_size, x, y + 25, color_ability1);
    font->draw(std::format("{:5}", role_->Medicine), font_size, x + 44, y + 25, select_color1(role_->Medicine, Role::getMaxValue()->Medicine));
    font->draw("解毒", font_size, x + 200, y + 25, color_ability1);
    font->draw(std::format("{:5}", role_->Detoxification), font_size, x + 244, y + 25, select_color1(role_->Detoxification, Role::getMaxValue()->Detoxification));
    font->draw("用毒", font_size, x + 400, y + 25, color_ability1);
    font->draw(std::format("{:5}", role_->UsePoison), font_size, x + 444, y + 25, select_color1(role_->UsePoison, Role::getMaxValue()->UsePoison));

    x = x_ + 20;
    y = y_ + 270;
    font->draw("技能", 25, x - 10, y, color_name);

    font->draw("拳掌", font_size, x, y + 30, color_ability1);
    font->draw(std::format("{:5}", role_->Fist), font_size, x + 44, y + 30, select_color1(role_->Fist, Role::getMaxValue()->Fist));
    font->draw("御劍", font_size, x, y + 55, color_ability1);
    font->draw(std::format("{:5}", role_->Sword), font_size, x + 44, y + 55, select_color1(role_->Sword, Role::getMaxValue()->Sword));
    font->draw("耍刀", font_size, x, y + 80, color_ability1);
    font->draw(std::format("{:5}", role_->Knife), font_size, x + 44, y + 80, select_color1(role_->Knife, Role::getMaxValue()->Knife));
    font->draw("特殊", font_size, x, y + 105, color_ability1);
    font->draw(std::format("{:5}", role_->Unusual), font_size, x + 44, y + 105, select_color1(role_->Unusual, Role::getMaxValue()->Unusual));
    font->draw("暗器", font_size, x, y + 130, color_ability1);
    font->draw(std::format("{:5}", role_->HiddenWeapon), font_size, x + 44, y + 130, select_color1(role_->HiddenWeapon, Role::getMaxValue()->HiddenWeapon));

    x = x_ + 220;
    y = y_ + 270;
    font->draw("武學", 25, x - 10, y, color_name);
    for (int i = 0; i < 10; i++)
    {
        auto magic = Save::getInstance()->getRoleLearnedMagic(role_, i);
        std::string str = "__________";
        if (magic)
        {
            int x1 = x + i % 2 * 200;
            int y1 = y + 30 + i / 2 * 25;
            str = std::format("{}", magic->Name);
            font->draw(str, font_size, x1, y1, color_ability1);
            str = std::format("{:3}", role_->getRoleShowLearnedMagicLevel(i));
            font->draw(str, font_size, x1 + 100, y1, role_->getRoleShowLearnedMagicLevel(i) > 9 ? color_red : color_purple);
        }
        else
        {
            int x1 = x + i % 2 * 200;
            int y1 = y + 30 + i / 2 * 25;
            font->draw("", font_size, x1, y1, color_ability1);
        }
    }

    x = x_ + 420;
    y = y_ + 445;
    font->draw("修煉", 25, x - 10, y, color_name);
    auto book = Save::getInstance()->getItem(role_->PracticeItem);
    if (book)
    {
        //TextureManager::getInstance()->renderTexture("item", book->ID, x, y + 30);
        button_learning_book_->setTexture("item", book->ID);
        button_learning_book_->setPosition(x, y + 30);
        font->draw(std::format("{}", book->Name), font_size, x + 90, y + 30, color_name);
        font->draw(std::format("經驗{:5}", role_->ExpForItem), 18, x + 90, y + 55, color_ability1);
        std::string str = "升級 ----";
        int exp_up = role_->getFinishedExpForItem(book);
        if (exp_up != INT_MAX)
        {
            str = std::format("升級{:5}", exp_up);
        }
        font->draw(str, 18, x + 90, y + 75, color_ability1);
    }
    else
    {
        button_learning_book_->setTexture("title", 127);
        button_learning_book_->setPosition(x, y + 30);
    }
    x = x_ + 20;
    y = y_ + 445;
    font->draw("武器", 25, x - 10, y, color_name);
    auto equip = Save::getInstance()->getItem(role_->Equip0);
    if (equip)
    {
        //TextureManager::getInstance()->renderTexture("item", equip->ID, x, y + 30);
        button_equip0_->setTexture("item", equip->ID);
        button_equip0_->setPosition(x, y + 30);
        font->draw(std::format("{}", equip->Name), font_size, x + 90, y + 30, color_name);
        font->draw("攻擊", 18, x + 90, y + 55, color_ability1);
        font->draw(std::format("{:+}", equip->AddAttack), 18, x + 126, y + 55, select_color2(equip->AddAttack));
        font->draw("防禦", 18, x + 90, y + 75, color_ability1);
        font->draw(std::format("{:+}", equip->AddDefence), 18, x + 126, y + 75, select_color2(equip->AddDefence));
        font->draw("輕功", 18, x + 90, y + 95, color_ability1);
        font->draw(std::format("{:+}", equip->AddSpeed), 18, x + 126, y + 95, select_color2(equip->AddSpeed));
    }
    else
    {
        button_equip0_->setTexture("title", 127);
        button_equip0_->setPosition(x, y + 30);
    }

    x = x_ + 220;
    y = y_ + 445;
    font->draw("防具", 25, x - 10, y, color_name);
    equip = Save::getInstance()->getItem(role_->Equip1);
    if (equip)
    {
        //TextureManager::getInstance()->renderTexture("item", equip->ID, x, y + 30);
        button_equip1_->setTexture("item", equip->ID);
        button_equip1_->setPosition(x, y + 30);
        font->draw(std::format("{}", equip->Name), font_size, x + 90, y + 30, color_name);
        font->draw("攻擊", 18, x + 90, y + 55, color_ability1);
        font->draw(std::format("{:+}", equip->AddAttack), 18, x + 126, y + 55, select_color2(equip->AddAttack));
        font->draw("防禦", 18, x + 90, y + 75, color_ability1);
        font->draw(std::format("{:+}", equip->AddDefence), 18, x + 126, y + 75, select_color2(equip->AddDefence));
        font->draw("輕功", 18, x + 90, y + 95, color_ability1);
        font->draw(std::format("{:+}", equip->AddSpeed), 18, x + 126, y + 95, select_color2(equip->AddSpeed));
    }
    else
    {
        button_equip1_->setTexture("title", 127);
        button_equip1_->setPosition(x, y + 30);
    }
    x = x_ + 20;
    y = y_ + 575;
    equip = Save::getInstance()->getItem(role_->Equip1);

    if (role_->ID == 0)
    {
        font->draw("裝備武學", 25, x - 10, y, color_name);
        for (int i = 0; i < equip_magics_.size(); i++)
        {
            auto m = Save::getInstance()->getMagic(role_->EquipMagic[i]);
            if (m && role_->getMagicOfRoleIndex(m) < 0) { m = nullptr; }
            if (m)
            {
                std::string text = m->Name;
                text += std::string(10 - Font::getTextDrawSize(text), ' ');
                equip_magics_[i]->setText(text);
            }
            else
            {
                equip_magics_[i]->setText("__________");
            }
        }

        font->draw("裝備物品", 25, x + 390, y, color_name);
        auto m = Save::getInstance()->getItem(role_->EquipItem);
        if (m)
        {
            std::string text = m->Name;
            text += std::string(10 - Font::getTextDrawSize(text), ' ');
            equip_item_->setText(text);
        }
        else
        {
            equip_item_->setText("__________");
        }
    }
}

void UIStatus::dealEvent(EngineEvent& e)
{
}

void UIStatus::onPressedOK()
{
    if (role_ == nullptr)
    {
        return;
    }

    if (menu_->getResult() == 0)
    {
        auto team_menu = std::make_shared<TeamMenu>();
        team_menu->setText(std::format("{}要為誰醫療", role_->Name));
        team_menu->run();
        auto role = team_menu->getRole();
        if (role)
        {
            Role r = *role;
            role_->medicine(role);
            auto df = std::make_shared<ShowRoleDifference>(&r, role);
            df->setText(std::format("{}接受{}醫療", role->Name, role_->Name));
            df->run();
        }
    }
    else if (menu_->getResult() == 1)
    {
        auto team_menu = std::make_shared<TeamMenu>();
        team_menu->setText(std::format("{}要為誰解毒", role_->Name));
        team_menu->run();
        auto role = team_menu->getRole();
        if (role)
        {
            Role r = *role;
            role_->detoxification(role);
            auto df = std::make_shared<ShowRoleDifference>(&r, role);
            df->setText(std::format("{}接受{}解毒", role->Name, role_->Name));
            df->run();
        }
    }
    else if (menu_->getResult() == 2)
    {
        Event::getInstance()->callLeaveEvent(role_);
        //role_ = nullptr;
    }
    if (menu_->getResult() >= 3 && menu_->getResult() < 7)
    {
        auto menu = std::make_shared<BattleMagicMenu>();
        menu->setRole(role_);
        //int count = menu->getChildCount();
        //role_->Auto = 0;    //.....
        //role_->Team = 0;
        menu->setPosition(730, 630 - 30 * role_->getLearnedMagicCount());
        menu->run();
        if (menu->getMagic())
        {
            int id = menu->getMagic()->ID;
            for (auto& em : role_->EquipMagic)
            {
                if (em == id) { em = -1; }
            }
            role_->EquipMagic[menu_->getResult() - 3] = id;
        }
    }
    if (menu_->getResult() == 7)
    {
        auto menu = std::make_shared<BattleEquipItemMenu>();
        menu->setRole(role_);
        menu->setPosition(730, 630 - 30 * menu->getChildCount());
        menu->run();
        if (menu->getItem())
        {
            int id = menu->getItem()->ID;
            role_->EquipItem = id;
        }
    }

    auto uiitem_for_role = [this](const std::set<int>& f)
    {
        UI::getInstance()->setVisible(false);
        auto menu = std::make_shared<UIItem>();
        menu->setIsDark(1);
        menu->setRole(role_);
        menu->setForceItemType(f);
        auto head = std::make_shared<Head>();
        head->setRole(role_);
        head->setPosition(-280, 60);
        head->setAlwaysLight(1);
        menu->addChild(head);
        menu->setPosition(300, 0);
        setVisible(false);
        menu->runAtPosition(300, 0);
        setVisible(true);
        if (menu->getCurrentItem())
        {
            role_->equip(menu->getCurrentItem());
        }
        UI::getInstance()->setVisible(true);
    };
    if (menu_->getResult() == 8)
    {
        uiitem_for_role({ 1 });
    }
    if (menu_->getResult() == 9)
    {
        uiitem_for_role({ 2 });
    }
    if (menu_->getResult() == 10)
    {
        uiitem_for_role({ 5, 6, 7, 8, 9 });
    }
}

void UIStatus::setRoleName(const std::string& name)
{
    assert(role_ != nullptr);
    //memset(role_->Name, '\0', sizeof(role_->Name));
    //memcpy(role_->Name, name.c_str(), std::min(name.size(), sizeof(role_->Name)));
    role_->Name = name;
}

void UIStatus::setExtentionVisible(bool b)
{
    for (auto& em : equip_magics_)
    {
        em->setVisible(b);
    }
    equip_item_->setVisible(b);
}
