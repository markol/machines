#pragma once

#include <map>
#include "gui/displaya.hpp"
#include "machlog/p1handlr.hpp"

class MachGuiFPCommand : public GuiDisplayable
{
public:
    enum class CommandIconState : int
    {
        INVALID,
        VALID,
        ACTIVATED,
        NUM_STATES
    };
    static constexpr int NumCommandIconStates { static_cast<int>(CommandIconState::NUM_STATES) };

    ///////////////////////////////////////////////////

    MachGuiFPCommand( GuiDisplayable* pParent, const Gui::Coord& relPos );
    ~MachGuiFPCommand();
    // NON-COPYABLE
    MachGuiFPCommand(const MachGuiFPCommand&) = delete;
    MachGuiFPCommand& operator=(const MachGuiFPCommand&) = delete;

    ///////////////////////////////////////////////////

    void logHandler( MachLog1stPersonHandler* logHandler);
    void resetLogHandler();

    // Change the icon that displays within the control
    void updateSquadIcon();
    void clearSquadIcon();

    // Update the displayed number that shows which squad is selected
    void updateSquadNumber();
    void resetSquadNumber();

    // Change command icon states
    void setAttackIconState(CommandIconState state);
    void setFollowIconState(CommandIconState state);
    void setMoveIconState(CommandIconState state);

protected:
    virtual void doDisplay() override;

private:
    static GuiBitmap& noSquadronSelected();
    static GuiBitmap& widgetBody();

    static GuiBitmap* attackCommandIcons();
    static GuiBitmap* followCommandIcons();
    static GuiBitmap* moveCommandIcons();

    CommandIconState attackCommandState_;
    CommandIconState followCommandState_;
    CommandIconState moveCommandState_;

    GuiBitmap activeSquadIcon_;

    // Will either point to noSquadronSelected() OR activeSquadIcon_
    GuiBitmap* pActiveSquadIcon_;
    int64_t    activeSquadNumber_;
    MachLog1stPersonHandler* pLogHandler_;

    // Rendered text of the squad number.
    // 0 is blank
    // Squadron ID's from getActiveSquadronId() are 1...10
    std::map<int64_t, RenSurface*> mapSquadNumbers_;
};
