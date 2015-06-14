/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#include "main.h"

#include <oauth2.h>

#define GOOGLE_CLIENT_ID "53492317821.apps.googleusercontent.com"
#define GOOGLE_CLIENT_SECRET "2SEBA-F4EV9jkqzT1UGJe7Aq"

#define WINDOWS_CLIENT_ID "0000000044139C34"
#define WINDOWS_CLIENT_SECRET "lq-od8DUGe9BrcaI9qVHM2Q9krFUNZDd"

#define LINKEDIN_CLIENT_ID "782p0522d2ri2i"
#define LINKEDIN_CLIENT_SECRET "Ibj6HdUpZj2M4XIs"

/* #define _THIS_PACKAGE "org.tizen.oauth2sample"
 *
 * #ifdef  LOG_TAG
 * #undef  LOG_TAG
 * #endif
 * #define LOG_TAG _THIS_PACKAGE
*/

#ifndef _ERR
#define _ERR(fmt, args...) LOGE("[%s:%d] "fmt"\n", __func__, __LINE__, ##args)
#endif

#ifndef _DBG
#define _DBG(fmt, args...) LOGD("[%s:%d] "fmt"\n", __func__, __LINE__, ##args)
#endif

#ifndef _INFO
#define _INFO(fmt, args...) LOGI("[%s:%d] "fmt"\n", __func__, __LINE__, ##args)
#endif

static oauth2_manager_h mgr1 = NULL;
char *google_refresh_token = NULL;
char *popStr;
Evas_Object *win_data = NULL;
int flag = 0;

static void
popup_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup = data;
	evas_object_del(popup);
}

static void
create_popup(char *popup_str, void * data)
{
	Evas_Object *popup = elm_popup_add(win_data);

	if (flag == 0)
		elm_object_part_text_set(popup, "title,text", "Response");
	else if (flag == 1)
		elm_object_part_text_set(popup, "title,text", "Success");

	elm_popup_content_text_wrap_type_set(popup, ELM_WRAP_MIXED);
	elm_object_text_set(popup, popup_str);

	Evas_Object *btn1 = elm_button_add(popup);
	elm_object_style_set(btn1, "popup");
	elm_object_text_set(btn1, "OK");
	elm_object_part_content_set(popup, "button1", btn1);
	evas_object_smart_callback_add(btn1, "clicked", popup_btn_clicked_cb,
		popup);

	evas_object_resize(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_move(popup, 10, 10);

	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	evas_object_show(popup);
	flag = 0;
	return;
}

void
bundle_cb(const char *key, const int type, const bundle_keyval_t *kv,
	void *user_data)
{
	char *display_str = (char *)user_data;

	char *err_val = NULL;
	size_t err_len = 0;
	bundle_keyval_get_basic_val((bundle_keyval_t *)kv, (void **)&err_val,
		&err_len);
	if (err_val) {
		strcat(display_str, key);
		strcat(display_str, "=");
		strcat(display_str, err_val);
		strcat(display_str, ",");
	}
}

static void
btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	int btn_num = (int)data;

	printf("clicked event on Button:%d\n", btn_num);
}

static Evas_Object*
create_scroller(Evas_Object *parent)
{
	Evas_Object *scroller = elm_scroller_add(parent);
	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF,
		ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);

	return scroller;
}

static Evas_Object*
create_button_view(Evas_Object *parent)
{
	Evas_Object *btn, *img, *box;

	box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_padding_set(box, 0, 5 * elm_config_scale_get());
	evas_object_show(box);

	/* icon_reorder style */
	btn = elm_button_add(box);
	elm_object_style_set(btn, "icon_reorder");
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)1);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	/* icon_expand_add style */
	btn = elm_button_add(box);
	elm_object_style_set(btn, "icon_expand_add");
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)2);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	/* icon_expand_delete style */
	btn = elm_button_add(box);
	elm_object_style_set(btn, "icon_expand_delete");
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)3);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	/* default style */
	btn = elm_button_add(box);
	elm_object_text_set(btn, "default");
	evas_object_size_hint_min_set(btn, ELM_SCALE_SIZE(150),
		ELM_SCALE_SIZE(58));
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)4);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);
	
	/* btn_circle style */
	btn = elm_button_add(box);
	elm_object_style_set(btn, "circle");
	elm_object_text_set(btn, "circle twoline");
	img = elm_image_add(btn);
	elm_image_file_set(img, ICON_DIR"/core_icon_brightness.png", NULL);
	elm_image_resizable_set(img, EINA_TRUE, EINA_TRUE);
	elm_object_part_content_set(btn, "icon", img);
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)5);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	/* bottom style */
	btn = elm_button_add(box);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "bottom");
	evas_object_size_hint_min_set(btn, ELM_SCALE_SIZE(150),
		ELM_SCALE_SIZE(58));
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)6);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	/* contacts style */
	btn = elm_button_add(box);
	elm_object_style_set(btn, "contacts");
	elm_object_text_set(btn, "contacts");
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)7);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	btn = elm_button_add(box);
	elm_object_style_set(btn, "option");
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb,
		(void *)8);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);

	return box;
}

void
token_response_cb(oauth2_response_h response, void *user_data)
{
	_INFO("token_response_cb");

	char *acc_token = NULL;
	oauth2_response_get_access_token(response, &acc_token);

	char *ref_token = NULL;
	oauth2_response_get_refresh_token(response, &ref_token);
	google_refresh_token = ref_token;

	long long int expires_in = 0;
	oauth2_response_get_expires_in(response, &expires_in);

	char displayStr[1024] = {0,};
	if (acc_token) {
		displayStr[0] = '\0';
		strcpy(displayStr, "access token= ");
		strcat(displayStr, acc_token);
	} else {
		oauth2_error_h e_handle =  NULL;

		oauth2_response_get_error(response, &e_handle);
		char *error_val = NULL;
		oauth2_error_get_custom_data(e_handle, "error", &error_val);
		if (error_val)
			strcpy(displayStr, error_val);
		else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code,
				&platform_error_code);
			if (error_code != 0 || platform_error_code != 0) {
				sprintf(displayStr, "Error=[%d][%d]",
				error_code, platform_error_code);
			} else
				strcpy(displayStr, "Unknown server error");
		}
	}

	if (ref_token) {
		strcat(displayStr, "\r\n");
		strcat(displayStr, "refresh token = ");
		strcat(displayStr, ref_token);
	}

	if (expires_in != 0) {
		strcat(displayStr, "\r\n");
		strcat(displayStr, "expires in= ");
		char expires_str[128] = {0};
		sprintf(expires_str, "%lld", expires_in);
		strcat(displayStr, expires_str);
	}

	create_popup(displayStr , win_data);
	return;
}


void
refresh_token_response_cb(oauth2_response_h response, void *user_data)
{
	_INFO("token_response_cb");

	char *acc_token = NULL;
	oauth2_response_get_access_token(response, &acc_token);

	char *ref_token = NULL;
	oauth2_response_get_refresh_token(response, &ref_token);

	if (ref_token) {
		oauth2_manager_h mgr = (oauth2_manager_h) user_data;

		oauth2_request_h request = NULL;

		int ret = oauth2_request_create(&request);

		ret = oauth2_request_set_refresh_token_url(request,
			"https://www.googleapis.com/oauth2/v3/token");

		ret = oauth2_request_set_refresh_token(request, ref_token);

		ret = oauth2_request_set_client_id(request, GOOGLE_CLIENT_ID);

		ret = oauth2_request_set_client_secret(request,
			GOOGLE_CLIENT_SECRET);

		ret = oauth2_request_set_grant_type(request,
			OAUTH2_GRANT_TYPE_REFRESH);

		ret = oauth2_request_set_client_authentication_type(request,
			OAUTH2_CLIENT_AUTHENTICATION_TYPE_REQUEST_BODY);

		if (mgr && request) {
			mgr1 = mgr;

			ret = oauth2_manager_refresh_access_token(mgr, request,
				token_response_cb, NULL);
		}
	} else {
		char displayStr[1024] = {0,};

		oauth2_error_h e_handle =  NULL;

		oauth2_response_get_error(response, &e_handle);
		char *error_val = NULL;
		oauth2_error_get_custom_data(e_handle, "error", &error_val);
		if (error_val) 
			strcpy(displayStr, error_val);
		else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code,
				&platform_error_code);

			if (error_code != 0 || platform_error_code != 0)
				sprintf(displayStr, "Error=[%d][%d]",
					error_code, platform_error_code);
			else {
				strcpy(displayStr, "Unknown server error");
			}
		}

		create_popup(displayStr , win_data);
	}
}

void
clear_cache_and_cookies_cb(void *data, Evas_Object *obj, void *event_info)
{
	win_data = (Evas_Object *) data;

	if (mgr1 != NULL) {
		oauth2_manager_clear_cache(mgr1);
		oauth2_manager_clear_cookies(mgr1);

		mgr1 = NULL;

		flag = 1;

		popStr = "cache and cookie data cleared!!!";
		create_popup(popStr , win_data);
		return;
	}
}

void
code_access_token_cb(oauth2_response_h response, void *user_data)
{
	_INFO("auth2_access_token_cb");

	char *acc_token = NULL;
	char displayStr[1024] = {0,};
	oauth2_response_get_access_token(response, &acc_token);
	if (acc_token) {
		displayStr[0] = '\0';
		strcpy(displayStr, "access token= ");
		strcat(displayStr, acc_token);
	} else {
		oauth2_error_h e_handle = NULL;

		oauth2_response_get_error(response, &e_handle);
		char *error_val = NULL;
		oauth2_error_get_custom_data(e_handle, "error", &error_val);
		if (error_val)
			strcpy(displayStr, error_val);
		else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code,
				&platform_error_code);
			if (error_code != 0 || platform_error_code != 0) {
				sprintf(displayStr, "Error=[%d][%d]",
					error_code, platform_error_code);
			} else {
				strcpy(displayStr, "Unknown server error");
			}
		}
	}
	create_popup(displayStr , win_data);
}

void
grant_response_cb(oauth2_response_h response, void *user_data)
{
	_INFO("grant_response_cb");

	char *auth_code = NULL;
	oauth2_response_get_authorization_code(response, &auth_code);
	if (auth_code) {
		oauth2_manager_h mgr = NULL;
		int ret = oauth2_manager_create(&mgr);

		oauth2_request_h request = (oauth2_request_h) user_data;

		ret = oauth2_request_set_authorization_code(request, auth_code);

		if (mgr && request) {
			ret = oauth2_manager_request_access_token(mgr,
				request, code_access_token_cb, NULL);
		}
	} else {
		char displayStr[1024] = {0,};

		oauth2_error_h e_handle =  NULL;

		oauth2_response_get_error(response, &e_handle);
		char *error_val = NULL;
		oauth2_error_get_custom_data(e_handle, "error", &error_val);
		if (error_val) {
			strcpy(displayStr, error_val);
		} else {
			int error_code = 0;
			int platform_error_code = 0;

			oauth2_error_get_code(e_handle, &error_code,
				&platform_error_code);
			if (error_code != 0 || platform_error_code != 0) {
				sprintf(displayStr, "Error=[%d][%d]",
					error_code, platform_error_code);
			} else {
				strcpy(displayStr, "Unknown server error");
			}
		}

		create_popup(displayStr , win_data);
	}
}


void
start_google_oauth_cb(void *data, Evas_Object *obj, void *event_info)
{
	win_data = (Evas_Object *) data;

	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://accounts.google.com/o/oauth2/auth");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://accounts.google.com/o/oauth2/token");

	ret = oauth2_request_set_redirection_url(request,
		"https://localhost:8080");

	ret = oauth2_request_set_client_id(request, GOOGLE_CLIENT_ID);

	ret = oauth2_request_set_client_secret(request, GOOGLE_CLIENT_SECRET);

	ret = oauth2_request_set_scope(request, "email");

	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_CODE);

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, NULL);
	}
}


void
start_fb_oauth_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	win_data = (Evas_Object *) data;

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://www.facebook.com/dialog/oauth");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request, "280875681986395");

	ret = oauth2_request_set_scope(request, "read_stream");

	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_TOKEN);

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, NULL);
	}
}


void
start_github_oauth_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	win_data = (Evas_Object *) data;

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://github.com/login/oauth/authorize");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://github.com/login/oauth/access_token");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request, "aa5ded12ececad47a22c");

	ret = oauth2_request_set_client_secret(request,
		"755806ea84c63f967b82d7fc451cf886d037f8f2");

	int state_int = rand()%1000;
	char state_str[128] = {0,};
	snprintf(state_str, 127, "%d", state_int);
	ret = oauth2_request_set_state(request, state_str);

	ret = oauth2_request_set_scope(request, "user");
	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_CODE);

	if (mgr && request) {
		mgr1 = mgr;
		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, NULL);
	}
}

void
start_google_refresh_token_cb(void *data, Evas_Object *obj, void *event_info)
{
	win_data = (Evas_Object *) data;

	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://accounts.google.com/o/oauth2/auth");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://accounts.google.com/o/oauth2/token");

	ret = oauth2_request_set_redirection_url(request,
		"https://localhost:8080");

	ret = oauth2_request_set_client_id(request, GOOGLE_CLIENT_ID);

	ret = oauth2_request_set_client_secret(request, GOOGLE_CLIENT_SECRET);

	ret = oauth2_request_set_scope(request, "email");

	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_CODE);

	if (mgr && request) {
		ret = oauth2_manager_request_token(mgr, request,
			refresh_token_response_cb, mgr);
	}
}

void
start_twitter_apponly_oauth_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	win_data = (Evas_Object *) data;

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://api.twitter.com/oauth2/token");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://api.twitter.com/oauth2/token");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request, "PiQeUGnE96DQxEw36rAPw");

	ret = oauth2_request_set_client_secret(request,
		"qxLHpdcAg5fCmE6b46GXO8UjDefr6H5C9jXF2SOFAE");

	ret = oauth2_request_set_grant_type(request,
		OAUTH2_GRANT_TYPE_CLIENT_CREDENTIALS);

	ret = oauth2_request_set_client_authentication_type(request,
		OAUTH2_CLIENT_AUTHENTICATION_TYPE_BASIC);

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, NULL);
	}
}


void
start_linkedin_oauth_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://www.linkedin.com/uas/oauth2/authorization");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://www.linkedin.com/uas/oauth2/accessToken");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request, "");

	ret = oauth2_request_set_client_secret(request, "");

	int state_int = rand()%1000;
	char state_str[128] = {0,};
	snprintf(state_str, 127, "%d", state_int);
	ret = oauth2_request_set_state(request, state_str);

	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_CODE);

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, NULL);
	}
}


void
start_windows_oauth_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	win_data = (Evas_Object *) data;

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://login.live.com/oauth20_authorize.srf");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://login.live.com/oauth20_token.srf");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request, WINDOWS_CLIENT_ID);

	ret = oauth2_request_set_client_secret(request, WINDOWS_CLIENT_SECRET);

	ret = oauth2_request_set_scope(request, "wl.basic");

	ret = oauth2_request_set_grant_type(request,
		OAUTH2_GRANT_TYPE_AUTH_CODE);

	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_CODE);

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_authorization_grant(mgr,
			request, grant_response_cb, request);
	}
}


void
start_linkedin_oauth_code_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	win_data = (Evas_Object *) data;

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://www.linkedin.com/uas/oauth2/authorization");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://www.linkedin.com/uas/oauth2/accessToken");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request, LINKEDIN_CLIENT_ID);

	ret = oauth2_request_set_client_secret(request, LINKEDIN_CLIENT_SECRET);

	ret = oauth2_request_set_scope(request, "r_basicprofile");

	ret = oauth2_request_set_state(request, "DCEEFWF45453sdffef424");

	ret = oauth2_request_set_grant_type(request,
		OAUTH2_GRANT_TYPE_AUTH_CODE);

	ret = oauth2_request_set_response_type(request,
		OAUTH2_RESPONSE_TYPE_CODE);

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_authorization_grant(mgr,
			request, grant_response_cb, request);

	}
} 

void
start_salesforce_oauth_code_cb(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_h mgr = NULL;
	int ret = oauth2_manager_create(&mgr);

	oauth2_request_h request = NULL;
	ret = oauth2_request_create(&request);

	win_data = (Evas_Object *) data;

	ret = oauth2_request_set_auth_end_point_url(request,
		"https://login.salesforce.com/services/oauth2/authorize");

	ret = oauth2_request_set_token_end_point_url(request,
		"https://login.salesforce.com/services/oauth2/token");

	ret = oauth2_request_set_redirection_url(request,
		"https://developer.tizen.org");

	ret = oauth2_request_set_client_id(request,
		"3MVG9ZL0ppGP5UrCxqVnY_izjlRzW6tCeDYs64KXns0wGEgbtfqK8cWx16Y4gM3wx2htt0GuoDiQ.CkX2ZuI1");

	ret = oauth2_request_set_client_secret(request, "3431205550072092349");

	ret = oauth2_request_set_grant_type(request,
		OAUTH2_GRANT_TYPE_PASSWORD);

	ret = oauth2_request_set_user_name(request, "sam_test1@outlook.com");

	ret = oauth2_request_set_password(request,
		"samsung@1gOeXzn5nKDGVNNQP4kJYEqNPp");

	if (mgr && request) {
		mgr1 = mgr;

		ret = oauth2_manager_request_token(mgr, request,
			token_response_cb, request);
	}
}
