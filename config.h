/* config.h : configuration for this application. public domain */
#ifndef CONFIG_H
#define CONFIG_H
/*******************************************************************************
 * Config
 ******************************************************************************/

#define APP_COMMAND_LINE (0)
#define APP_WIDTH 320
#define APP_HEIGHT 240
#define APP_OUT_WIDTH 640
#define APP_OUT_HEIGHT 480
#define APP_PRE_INIT (app_pre_init())
#define APP_POST_INIT (app_do_nothing())
#define APP_KEEP_GOING (app_keep_going())
#define APP_STEP (app_do_nothing())
#define APP_ENVIRONMENT (APP_ENVIRONMENT_DESKTOP)
#define APP_ENVIRONMENT_DESKTOP 1
#define APP_ENVIRONMENT_MOBILE 2
#define APP_TITLE "GL_SIMPLE"
#define APP_FRAMERATE 50 /* target/maximum frame rate */

#endif
