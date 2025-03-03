#include "gfx/legato/generated/le_gen_init.h"

static int32_t currentScreen;
static int32_t changingToScreen;

void legato_initializeScreenState(void)
{
    leSetStringTable(&stringTable);

    initializeStrings();

    legato_preprocessImages();

    screenInit_Screen0();
    screenInit_Screen1();

    currentScreen = -1;
    changingToScreen = -1;

    legato_showScreen(screenID_Screen1);
}

uint32_t legato_getCurrentScreen(void)
{
    return currentScreen;
}

static void legato_hideCurrentScreen(void)
{
    switch(currentScreen)
    {
        case screenID_Screen0:
        {
            screenHide_Screen0();
            currentScreen = 0;
            break;
        }
        case screenID_Screen1:
        {
            screenHide_Screen1();
            currentScreen = 0;
            break;
        }
    }
}

void legato_showScreen(uint32_t id)
{
    if(changingToScreen >= 0)
        return;

    changingToScreen = id;
}

void legato_updateScreenState(void)
{
    if(changingToScreen >= 0)
    {
        legato_hideCurrentScreen();

        switch(changingToScreen)
        {
            case screenID_Screen0:
            {
                screenShow_Screen0();
                break;
            }
            case screenID_Screen1:
            {
                screenShow_Screen1();
                break;
            }
        }

        currentScreen = changingToScreen;
        changingToScreen = -1;
    }

    switch(currentScreen)
    {
        case screenID_Screen0:
        {
            screenUpdate_Screen0();
            break;
        }
        case screenID_Screen1:
        {
            screenUpdate_Screen1();
            break;
        }
    }
}

leBool legato_isChangingScreens(void)
{
    return changingToScreen != -1;
}

