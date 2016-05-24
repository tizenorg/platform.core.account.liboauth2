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

#include "oauth2_request.h"
#include "oauth2_private.h"
#include "oauth2_util.h"

OAUTH2_API int
oauth2_request_create(oauth2_request_h *handle)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *) calloc(1,
		sizeof(oauth2_request_s));
	if (!req_impl) {
		OAUTH2_LOG_E("Out of memory");
		return OAUTH2_ERROR_OUT_OF_MEMORY;
	}

	req_impl->request_data = bundle_create();
	if (!req_impl->request_data) {
		OAUTH2_LOG_E("Out of memory");
		OAUTH2_FREE(req_impl);
		return OAUTH2_ERROR_OUT_OF_MEMORY;
	}

	*handle = (oauth2_request_h)req_impl;

	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_destroy(oauth2_request_h handle)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *) handle;
	if (!req_impl) {
		OAUTH2_LOG_E("NULL handle");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (req_impl->request_data)
		bundle_free(req_impl->request_data);

	OAUTH2_FREE(req_impl);
	handle = NULL;

	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_set_auth_end_point_url(oauth2_request_h handle, const char *url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_AUTHORIZATION_URL, url);
}

OAUTH2_API int
oauth2_request_set_token_end_point_url(oauth2_request_h handle, const char *url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL, url);
}

OAUTH2_API int
oauth2_request_set_redirection_url(oauth2_request_h handle, const char *url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, url);
}

OAUTH2_API int
oauth2_request_set_refresh_token_url(oauth2_request_h handle, const char *url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL, url);
}

OAUTH2_API int
oauth2_request_set_refresh_token(oauth2_request_h handle, char *refresh_token)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN, refresh_token);
}

OAUTH2_API int
oauth2_request_set_response_type(oauth2_request_h handle,
	oauth2_response_type_e response_type)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	if (response_type < OAUTH2_RESPONSE_TYPE_CODE
			|| response_type > OAUTH2_RESPONSE_TYPE_TOKEN) {
		OAUTH2_LOG_E("Invalid response_type [%d]", response_type);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char response_type_str[128] = {0,};
	snprintf(response_type_str, 127, "%d", response_type);
	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_RESPONSE_TYPE, response_type_str);
}

OAUTH2_API int
oauth2_request_set_client_id(oauth2_request_h handle, const char *client_id)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_ID, client_id);
}

OAUTH2_API int
oauth2_request_set_client_secret(oauth2_request_h handle,
	const char *client_secret)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_SECRET, client_secret);
}

OAUTH2_API int
oauth2_request_set_client_authentication_type(oauth2_request_h handle,
	oauth2_client_authentication_type_e client_auth_type)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	if (client_auth_type < OAUTH2_CLIENT_AUTHENTICATION_TYPE_BASIC
		|| client_auth_type > OAUTH2_CLIENT_AUTHENTICATION_TYPE_REQUEST_BODY) {
		OAUTH2_LOG_E("Invalid client_auth_type [%d]", client_auth_type);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char client_auth_type_str[128] = {0,};
	snprintf(client_auth_type_str, 127, "%d", client_auth_type);
	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_AUTH_TYPE, client_auth_type_str);
}

OAUTH2_API int
oauth2_request_set_scope(oauth2_request_h handle, const char *scope)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_SCOPE, scope);
}

OAUTH2_API int
oauth2_request_set_state(oauth2_request_h handle, const char *state)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_STATE, state);
}

OAUTH2_API int
oauth2_request_set_grant_type(oauth2_request_h handle,
	oauth2_grant_type_e grant_type)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	if (grant_type < OAUTH2_GRANT_TYPE_AUTH_CODE
			|| grant_type > OAUTH2_GRANT_TYPE_REFRESH) {
		OAUTH2_LOG_E("Invalid grant_type [%d]", grant_type);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char grant_type_str[128] = {0,};
	snprintf(grant_type_str, 127, "%d", grant_type);
	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE, grant_type_str);
}

OAUTH2_API int
oauth2_request_set_authorization_code(oauth2_request_h handle, const char *code)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_CODE, code);
}

OAUTH2_API int
oauth2_request_set_user_name(oauth2_request_h handle, const char *user_name)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_USER_NAME, user_name);
}

OAUTH2_API int
oauth2_request_set_password(oauth2_request_h handle, const char *password)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data,
		OAUTH2_PARAMETER_KEY_PASSWORD, password);
}

OAUTH2_API int
oauth2_request_add_custom_data(oauth2_request_h handle, const char *key,
	const char *value)
{
	if (!key || !value) {
		OAUTH2_LOG_E("key and value must not be null");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *req_impl = (oauth2_request_s *)handle;
	if (!req_impl) {
		OAUTH2_LOG_E("Null Input");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	if (!req_impl->request_data) {
		OAUTH2_LOG_E("oauth2_request_h must be created first.");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	return bundle_add_str(req_impl->request_data, key, value);
}

OAUTH2_API int
oauth2_request_get_auth_end_point_url(oauth2_request_h handle, char **url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_AUTHORIZATION_URL, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*url = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_token_end_point_url(oauth2_request_h handle, char **url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*url = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_redirection_url(oauth2_request_h handle, char **url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*url = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_refresh_token_url(oauth2_request_h handle, char **url)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*url = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API
int oauth2_request_get_refresh_token(oauth2_request_h handle,
	char **refresh_token)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*refresh_token = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_response_type(oauth2_request_h handle,
	oauth2_response_type_e *response_type)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	int res;
	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_RESPONSE_TYPE, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	sscanf(val, "%d", &res);
	*response_type = (oauth2_response_type_e) res;

	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_client_id(oauth2_request_h handle, char **client_id)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_ID, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*client_id = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_client_secret(oauth2_request_h handle, char **client_secret)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_SECRET, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*client_secret = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_scope(oauth2_request_h handle, char **scope)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_SCOPE, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*scope = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_state(oauth2_request_h handle, char **state)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_STATE, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*state = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_grant_type(oauth2_request_h handle,
	oauth2_grant_type_e *grant_type)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	int res;
	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	sscanf(val, "%d", &res);
	*grant_type = (oauth2_grant_type_e) res;

	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_authorization_code(oauth2_request_h handle, char **code)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_CODE, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*code = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_user_name(oauth2_request_h handle, char **user_name)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_USER_NAME, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*user_name = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_password(oauth2_request_h handle, char **password)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");

	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;

	char *val = NULL;
	bundle_get_str(request_data_temp->request_data,
		OAUTH2_PARAMETER_KEY_PASSWORD, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*password = val;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_request_get_custom_data(oauth2_request_h handle, const char *custom_key,
	char **custom_value)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	oauth2_request_s *request_data_temp = (oauth2_request_s *)handle;
	char *val = NULL;
	bundle_get_str(request_data_temp->request_data, custom_key, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*custom_value = val;
	return OAUTH2_ERROR_NONE;
}
