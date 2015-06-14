/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <dlog.h>
#include <vconf.h>
#include <efl_extension.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "oauth2sample"

#define KEY_END "XF86Stop"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.oauth2sample"
#endif

#define ELM_DEMO_EDJ "/opt/usr/apps/org.tizen.oauth2sample/res/ui_controls.edj"
#define ICON_DIR "/opt/usr/apps/org.tizen.oauth2sample/res/images"


typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Evas_Object *datetime;
	Evas_Object *popup;
	Evas_Object *button;
	struct tm saved_time;
} appdata_s;

void ctxpopup_cb(void *data, Evas_Object *obj, void *event_info);
void colorselector_cb(void *data, Evas_Object *obj, void *event_info);
void conformant_cb(void *data, Evas_Object *obj, void *event_info);
void gengrid_cb(void *data, Evas_Object *obj, void *event_info);
void genlist_cb(void *data, Evas_Object *obj, void *event_info);
void drawer_cb(void *data, Evas_Object *obj, void *event_info);
void fastscroll_cb(void *data, Evas_Object *obj, void *event_info);
void naviframe_cb(void *data, Evas_Object *obj, void *event_info);
void toolbar_cb(void *data, Evas_Object *obj, void *event_info);
void toolbar_tab_style_cb(void *data, Evas_Object *obj, void *event_info);
void toolbar_navigation_style_cb(void *data, Evas_Object *obj, void *event_info);
void entry_cb(void *data, Evas_Object *obj, void *event_info);
void datetime_cb(void *data, Evas_Object *obj, void *event_info);
void slider_cb(void *data, Evas_Object *obj, void *event_info);
void progressbar_cb(void *data, Evas_Object *obj, void *event_info);

void start_google_oauth_cb(void *data, Evas_Object *obj, void *event_info);
void start_fb_oauth_cb(void *data, Evas_Object *obj, void *event_info);
void start_github_oauth_cb(void *data, Evas_Object *obj, void *event_info);
void start_linkedin_oauth_cb(void *data, Evas_Object *obj, void *event_info);
void start_twitter_apponly_oauth_cb(void *data, Evas_Object *obj, void *event_info);
void start_google_refresh_token_cb(void *data, Evas_Object *obj, void *event_info);
void start_windows_oauth_cb(void *data, Evas_Object *obj, void *event_info);
void start_linkedin_oauth_code_cb(void *data, Evas_Object *obj, void *event_info);
void start_salesforce_oauth_code_cb(void *data, Evas_Object *obj, void *event_info);
void clear_cache_and_cookies_cb(void *data, Evas_Object *obj, void *event_info);

void check_cb(void *data, Evas_Object *obj, void *event_info);
void nocontents_cb(void *data, Evas_Object *obj, void *event_info);
void radio_cb(void *data, Evas_Object *obj, void *event_info);
void pagecontrol_cb(void *data, Evas_Object *obj, void *event_info);
void pagecontrol_horizontal_cb(void *data, Evas_Object *obj, void *event_info);
void pagecontrol_horizontal_loop_cb(void *data, Evas_Object *obj, void *event_info);
void popup_cb(void *data, Evas_Object *obj, void *event_info);
void handler_cb(void *data, Evas_Object *obj, void *event_info);
void multibuttonentry_cb(void *data, Evas_Object *obj, void *event_info);
void spinner_cb(void *data, Evas_Object *obj, void *event_info);
void label_cb(void *data, Evas_Object *obj, void *event_info);
void label_text_styles_cb(void *data, Evas_Object *obj, void *event_info);
void label_linebreak_modes_cb(void *data, Evas_Object *obj, void *event_info);
void label_slide_cb(void *data, Evas_Object *obj, void *event_info);
void label_ellipsis_cb(void *data, Evas_Object *obj, void *event_info);
void label_color_styles_cb(void *data, Evas_Object *obj, void *event_info);
void notify_cb(void *data, Evas_Object *obj, void *event_info);
void bg_cb(void *data, Evas_Object *obj, void *event_info);
