#ifndef LE_GEN_SCREEN_MENU_H
#define LE_GEN_SCREEN_MENU_H

#include "gfx/legato/legato.h"

#include "gfx/legato/generated/le_gen_scheme.h"
#include "gfx/legato/generated/le_gen_assets.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

// screen member widget declarations
extern leWidget* Menu_PanelWidget0;
extern leGradientWidget* Menu_GradientWidget2;
extern leImageWidget* Menu_ImageWidget1;
extern leLabelWidget* Menu_LabelWidget1;
extern leButtonWidget* Menu_ButtonWidget_0;
extern leButtonWidget* Menu_ButtonWidget_1;

// event handlers
// !!THESE MUST BE IMPLEMENTED IN THE APPLICATION CODE!!
void event_Menu_ButtonWidget_0_OnPressed(leButtonWidget* btn);
void event_Menu_ButtonWidget_1_OnPressed(leButtonWidget* btn);

// screen lifecycle functions
// DO NOT CALL THESE DIRECTLY
leResult screenInit_Menu(void); // called when Legato is initialized
leResult screenShow_Menu(void); // called when screen is shown
void screenHide_Menu(void); // called when screen is hidden
void screenDestroy_Menu(void); // called when Legato is destroyed
void screenUpdate_Menu(void); // called when Legato is updating

leWidget* screenGetRoot_Menu(uint32_t lyrIdx); // gets a root widget for this screen

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // LE_GEN_SCREEN_MENU_H
