/*
 * Copyright (c) 2014 - 2015 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef OAUTH2_PRIVATE_H_
#define OAUTH2_PRIVATE_H_

#include <stdbool.h>
#include <bundle.h>
#include <curl/curl.h>

//#include <EWebKit.h>
#include <ewk_view.h>

#include <json-glib/json-glib.h>
#include "oauth2_manager.h"

#define OAUTH2_PARAMETER_VAL_GRANT_TYPE_AUTH_CODE	"authorization_code"

#define OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD	"password"

#define OAUTH2_PARAMETER_VAL_GRANT_TYPE_CLIENT_CREDENTIALS "client_credentials"

#define OAUTH2_PARAMETER_VAL_GRANT_TYPE_REFRESH_TOKEN	"refresh_token"

#define OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_CODE		"code"

#define OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_TOKEN	"token"

#define OAUTH2_PARAMETER_KEY_AUTHORIZATION_URL		"auth_url"

#define OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL		"token_url"

#define OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL		"refresh_token_url"

#define OAUTH2_PARAMETER_KEY_CLIENT_ID			"client_id"

#define OAUTH2_PARAMETER_KEY_CLIENT_SECRET		"client_secret"

#define OAUTH2_PARAMETER_KEY_CLIENT_AUTH_TYPE		"client_auth_type"

#define OAUTH2_PARAMETER_KEY_RESPONSE_TYPE		"response_type"

#define OAUTH2_PARAMETER_KEY_REDIRECT_URI		"redirect_uri"

#define OAUTH2_PARAMETER_KEY_SCOPE			"scope"

#define OAUTH2_PARAMETER_KEY_STATE			"state"

#define OAUTH2_PARAMETER_KEY_CODE			"code"

#define OAUTH2_PARAMETER_KEY_ERROR			"error"

#define OAUTH2_PARAMETER_KEY_ERROR_CODE			"error"

#define OAUTH2_PARAMETER_KEY_ERROR_DESCRIPTION		"error_description"

#define OAUTH2_PARAMETER_KEY_ERROR_URI			"error_uri"

#define OAUTH2_PARAMETER_KEY_ERROR_CODE_PLATFORM	"error_code_platform"

#define OAUTH2_PARAMETER_KEY_GRANT_TYPE			"grant_type"

#define OAUTH2_PARAMETER_KEY_ACCESS_TOKEN		"access_token"

#define OAUTH2_PARAMETER_KEY_TOKEN_TYPE			"token_type"

#define OAUTH2_PARAMETER_KEY_EXPIRES_IN			"expires_in"

#define OAUTH2_PARAMETER_KEY_USER_NAME			"username"

#define OAUTH2_PARAMETER_KEY_PASSWORD			"password"

#define OAUTH2_PARAMETER_KEY_REFRESH_TOKEN		"refresh_token"

#define OAUTH2_LOADING_POP_UP_TEXT			"Loading..."

typedef enum{
	REQUEST_ACCESS_TOKEN = 1,
	REQUEST_AUTHORIZATION_CODE
} _oauth_request_function;

typedef struct _oauth2_request_s
{
	bundle *request_data;
}oauth2_request_s;

typedef struct _oauth2_error_s
{
	bundle *error_data;
}oauth2_error_s;

typedef struct _oauth2_response_s
{
	oauth2_error_s *error;
	bundle *response_data;
}oauth2_response_s;

typedef struct _oauth2_manager_s
{
	oauth2_request_s *request;
	Evas_Object *login_win;
	Evas_Object *content_box;
	Evas_Object *ewk_view;
	Evas_Object *loading_popup;
	CURL *curl_handle;
	JsonParser *parser_handle;
	oauth2_response_s *response;
	_oauth_request_function request_func;
	bool is_active;

/* callbacks */
	oauth2_token_cb token_cb;
	oauth2_auth_grant_cb grant_cb;
	oauth2_access_token_cb access_token_cb;
	oauth2_refresh_token_cb refresh_cb;
	void *user_data;
	void *temp_data;
	bool is_webview_active;
}oauth2_manager_s;

int _oauth2_response_create(oauth2_response_s **response);

#endif /* OAUTH2_PRIVATE_H_ */
