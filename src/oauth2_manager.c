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

#include <efl_extension.h>

#include "oauth2_manager.h"
#include "oauth2_util.h"
#include "oauth2_private.h"

static void __send_response_to_caller(oauth2_manager_s *mgr_handle);
static void __convert_tizen_error_to_oauth_error(int tizen_error,
	char *error_desc, oauth2_response_s **response_handle);
static void __handle_back_key(void *data, Evas_Object *p, void *info);

OAUTH2_API int
oauth2_manager_create(oauth2_manager_h *handle)
{
	OAUTH2_LOG_I("oauth2_manager_create");

	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"Null Input");

	oauth2_manager_s *mgr_impl = (oauth2_manager_s *) calloc(1,
		sizeof(oauth2_manager_s));
	OAUTH2_RETURN_VAL(mgr_impl, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
		"Out of memory");

	curl_global_init(CURL_GLOBAL_DEFAULT);

	/* g_type_init(); */

	mgr_impl->parser_handle = json_parser_new();
	OAUTH2_RETURN_VAL(mgr_impl->parser_handle, { OAUTH2_FREE(mgr_impl); },
		OAUTH2_ERROR_OUT_OF_MEMORY, "Out of memory");

	mgr_impl->is_active = FALSE;

	*handle = (oauth2_manager_h)mgr_impl;
	return OAUTH2_ERROR_NONE;
}

OAUTH2_API int
oauth2_manager_destroy(oauth2_manager_h handle)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"Null Input");

	OAUTH2_FREE(handle);
	return OAUTH2_ERROR_NONE;
}

/*LCOV_EXCL_START*/
static void
__hide_web_view(oauth2_manager_s *mgr_handle)
{
	mgr_handle->is_webview_active = false;

	if (mgr_handle->ewk_view) {
		ewk_view_stop(mgr_handle->ewk_view);
		evas_object_hide(mgr_handle->ewk_view);
		evas_object_hide(mgr_handle->login_win);
	}

	if (mgr_handle->loading_popup) {
		evas_object_hide(mgr_handle->loading_popup);
		mgr_handle->loading_popup = NULL;
	}
}

static void
__reset_all_cb(oauth2_manager_s *mgr_handle)
{
	mgr_handle->token_cb = NULL;
	mgr_handle->grant_cb = NULL;
	mgr_handle->access_token_cb = NULL;
	mgr_handle->refresh_cb = NULL;
	mgr_handle->user_data = NULL;
	mgr_handle->is_active = FALSE;
}

static void
__convert_tizen_error_to_oauth_error(int tizen_error, char *error_desc,
	oauth2_response_s **response_handle)
{
	if (*response_handle == NULL) {
		*response_handle = (oauth2_response_s *)calloc(1,
			sizeof(oauth2_response_s));
		if (*response_handle == NULL) {
			OAUTH2_LOG_E("Out of memory");
			return;
		}
	}

	(*response_handle)->error = (oauth2_error_s *)calloc(1,
		sizeof(oauth2_error_s));
	(*response_handle)->error->error_data = bundle_create();

	char error_code_str[128] = {0,};
	char error_desc_str[128] = {0,};

	snprintf(error_code_str, 127, "%d", tizen_error);
	snprintf(error_desc_str, 127, "%s", error_desc);

	bundle_add_str((*response_handle)->error->error_data,
		OAUTH2_PARAMETER_KEY_ERROR_CODE, error_code_str);
	bundle_add_str((*response_handle)->error->error_data,
		OAUTH2_PARAMETER_KEY_ERROR_DESCRIPTION, error_desc_str);
}


static char*
__get_grant_type(char *grant_type_str)
{
	OAUTH2_LOG_I("__get_grant_type");
	if (!grant_type_str)
		return NULL;

	char *str_val = NULL;
	int grant_type = OAUTH2_GRANT_TYPE_MIN;
	sscanf(grant_type_str, "%d", &grant_type);

	if (grant_type <= OAUTH2_GRANT_TYPE_MIN
			|| grant_type >= OAUTH2_GRANT_TYPE_MAX) {
		OAUTH2_LOG_E("Invalid grant_type [%d]", grant_type);
		return NULL;
	} else if (grant_type == OAUTH2_GRANT_TYPE_AUTH_CODE)
		str_val = OAUTH2_PARAMETER_VAL_GRANT_TYPE_AUTH_CODE;
	else if (grant_type == OAUTH2_GRANT_TYPE_PASSWORD)
		str_val = OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD;
	else if (grant_type == OAUTH2_GRANT_TYPE_CLIENT_CREDENTIALS)
		str_val = OAUTH2_PARAMETER_VAL_GRANT_TYPE_CLIENT_CREDENTIALS;
	else if (grant_type == OAUTH2_GRANT_TYPE_REFRESH)
		str_val = OAUTH2_PARAMETER_VAL_GRANT_TYPE_REFRESH_TOKEN;

	return str_val;
}

static char*
__get_response_type(char *response_type_str)
{
	OAUTH2_LOG_I("__get_response_type");
	if (!response_type_str)
		return NULL;

	char *str_val = NULL;
	int response_type = OAUTH2_RESPONSE_TYPE_MIN;
	sscanf(response_type_str, "%d", &response_type);

	if (response_type <= OAUTH2_RESPONSE_TYPE_MIN
			|| response_type >= OAUTH2_RESPONSE_TYPE_MAX) {
		OAUTH2_LOG_E("Invalid response_type [%d]", response_type);
		return NULL;
	} else if (response_type == OAUTH2_RESPONSE_TYPE_CODE)
		str_val = OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_CODE;
	else if (response_type == OAUTH2_RESPONSE_TYPE_TOKEN)
		str_val = OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_TOKEN;

	return str_val;
}


static void
__handle_back_key(void *data, Evas_Object *p, void *info)
{
	if (data) {
		oauth2_manager_s *mgr_handle = (oauth2_manager_s *)data;
		__convert_tizen_error_to_oauth_error(
			OAUTH2_ERROR_USER_CANCELLED,
			"User cancelled the operation",
			&(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	}
}

static void
__send_response_to_caller(oauth2_manager_s *mgr_handle)
{
	OAUTH2_LOG_I("__send_response_to_caller start");

	mgr_handle->is_active = FALSE;
	__hide_web_view(mgr_handle);

	if (mgr_handle->token_cb) {
		if (mgr_handle->response) {
			OAUTH2_LOG_I("__send_response_to_caller calling \
				application cb");
			(mgr_handle->token_cb)((oauth2_response_h)
				(mgr_handle->response), mgr_handle->user_data);
		} else
			(mgr_handle->token_cb)(NULL, mgr_handle->user_data);
	} else if (mgr_handle->grant_cb) {
		if (mgr_handle->response) {
			(mgr_handle->grant_cb)((oauth2_response_h)
				(mgr_handle->response), mgr_handle->user_data);
		} else
			(mgr_handle->grant_cb)(NULL, mgr_handle->user_data);
	} else if (mgr_handle->access_token_cb) {
		if (mgr_handle->response) {
			(mgr_handle->access_token_cb)((oauth2_response_h)
				(mgr_handle->response), mgr_handle->user_data);
		} else {
			(mgr_handle->access_token_cb)(NULL,
				mgr_handle->user_data);
		}
	} else if (mgr_handle->refresh_cb) {
		if (mgr_handle->response) {
			(mgr_handle->refresh_cb)((oauth2_response_h)
				(mgr_handle->response), mgr_handle->user_data);
		} else
			(mgr_handle->refresh_cb)(NULL, mgr_handle->user_data);
	}
	__reset_all_cb(mgr_handle);

	eext_object_event_callback_del(mgr_handle->login_win,
		EEXT_CALLBACK_BACK, __handle_back_key);

	OAUTH2_LOG_I("__send_response_to_caller end");
	return;
}

static bool
__is_auth_grant_request_param(const char *param_key)
{
	if (strcmp(param_key, OAUTH2_PARAMETER_KEY_AUTHORIZATION_URL)
		&& strcmp(param_key, OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL)
		&& strcmp(param_key, OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL)
		&& strcmp(param_key, OAUTH2_PARAMETER_KEY_GRANT_TYPE)
		&& strcmp(param_key, OAUTH2_PARAMETER_KEY_CLIENT_SECRET)
		&& strcmp(param_key, OAUTH2_PARAMETER_KEY_CLIENT_AUTH_TYPE))
		return TRUE;

	return FALSE;
}

static void
__request_data_iter_cb(const char *key, const int type,
	const bundle_keyval_t *kv, void *user_data)
{
	if (__is_auth_grant_request_param(key)) {
		OAUTH2_LOG_I("__is_auth_grant_request_param TRUE");

		char **final_url = (char **)user_data;
		if (final_url) {
			char *value = NULL;
			size_t value_len = 0;
			bundle_keyval_get_basic_val((bundle_keyval_t *)kv,
				(void **)&value, &value_len);
			if (value) {
				OAUTH2_LOG_I("Appending val=[%s]", value);

				strncat(*final_url, key, strlen(key));
				strncat(*final_url, "=", 1);

				if (strcmp(key,
					OAUTH2_PARAMETER_KEY_GRANT_TYPE) == 0)
					value = __get_grant_type(value);
				else if (strcmp(key,
					OAUTH2_PARAMETER_KEY_RESPONSE_TYPE)
					== 0)
					value = __get_response_type(value);

				if (value != NULL) {
					strncat(*final_url, value, strlen(value));
					strncat(*final_url, "&", 1);
				}
			}
		}
	}
}

static char *
__get_authorization_url(oauth2_request_s *request_impl)
{
	OAUTH2_LOG_I("__get_authorization_url");

	if (!request_impl)
		return NULL;

	char *authorization_url = NULL;
	bundle_get_str(request_impl->request_data,
		OAUTH2_PARAMETER_KEY_AUTHORIZATION_URL, &authorization_url);
	if (!authorization_url) {
		OAUTH2_LOG_E("Missing mandatory [%s]",
			OAUTH2_PARAMETER_KEY_AUTHORIZATION_URL);
		return NULL;
	}

	char *final_url = (char *) calloc(1, 1024);
	strncat(final_url, authorization_url, 1023);
	strncat(final_url, "?", 1023);

	bundle_foreach(request_impl->request_data, __request_data_iter_cb,
		&final_url);

	if (final_url[strlen(final_url) - 1] == '&')
		final_url[strlen(final_url) - 1] = '\0';
	OAUTH2_LOG_I("Final url for auth grant = [%s]", final_url);

	return final_url;
}

static void
__exit_back_cb(void *data, Evas_Object *p, void *info)
{
	if (data) {
		oauth2_manager_s *mgr_handle = (oauth2_manager_s *)data;
		evas_object_hide(mgr_handle->ewk_view);
		evas_object_hide(mgr_handle->login_win);
	}
}

static void
__append_to_post_data(CURL *curl_handle, char *post_data, const char *key,
	const char *value, bool append_delim)
{
	OAUTH2_LOG_I("__append_to_post_data start");

	char *encoded_key = curl_easy_escape(curl_handle, key, 0);
	char *encoded_val = curl_easy_escape(curl_handle, value, 0);

	strncat(post_data, encoded_key, strlen(encoded_key));
	strncat(post_data, "=", 1);
	strncat(post_data, encoded_val, strlen(encoded_val));
	if (append_delim)
		strncat(post_data, "&", 1);
	OAUTH2_LOG_I("__append_to_post_data end");
}

static size_t
__store_curl_response(void *ptr, size_t size, size_t nmemb, void *data)
{
	char **result = (char **)data;

	if (NULL == *result) {
		*result = calloc(size * nmemb + 1, sizeof(char));
		OAUTH2_RETURN_VAL(*result, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
		memcpy(*result, ptr, size * nmemb);
	} else {
		size_t len = strlen(*result);
		*result = realloc(*result, len + size * nmemb + 1);
		OAUTH2_RETURN_VAL(*result, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
		memcpy(*result + len, ptr, size * nmemb);
		(*result)[len+size*nmemb] = '\0';
	}
	return size * nmemb;
}

static void
__iterate_json_response(JsonObject *object, const gchar *member_name,
	JsonNode *member_node, gpointer user_data)
{
	if (object) {
		bundle *response_bundle = user_data;

		if (json_node_get_node_type(member_node) != JSON_NODE_VALUE)
			return;

		if (json_node_get_value_type(member_node) == G_TYPE_STRING) {
			OAUTH2_LOG_I("bundle_add_str with string");
			if (json_node_get_string(member_node) != NULL)
				bundle_add_str(response_bundle, member_name,
					json_node_get_string(member_node));
		} else if (json_node_get_value_type(member_node)
			== G_TYPE_INT64) {
			char int_str[128] = {0,};
			int int_val = json_node_get_int(member_node);
			snprintf(int_str, 128, "%d", int_val);
			OAUTH2_LOG_I("bundle_add_str with int");
			bundle_add_str(response_bundle, strdup(member_name),
				strdup(int_str));
		}
	}
}

static void
__parse_acc_token_response(const char *response_json,
	oauth2_response_s **response)
{
	OAUTH2_LOG_I("__parse_acc_token_response start=[%s]", response_json);

	if (*response)
		OAUTH2_FREE(*response);

	oauth2_response_s *response_temp = (oauth2_response_s *)calloc(1,
		sizeof(oauth2_response_s));
	/*
	 * First find if error key is there, if present only fillup
	 * oauth2_error_s else fillup oauth2_response_s without
	 * oauth2_error_s component
	 */

	JsonParser *parser = json_parser_new();
	GError *error = NULL;
	if (json_parser_load_from_data(parser, response_json,
		strlen(response_json), &error) == FALSE) {
		OAUTH2_LOG_I("json_parser_load_from_data() failed [%d][%s]",
			error->code, error->message);

		/*
		 * Some svc providers (example: GitHub) do not send json
		 * response, which violates
		 * http://tools.ietf.org/html/rfc6749#section-4.1.4 and
		 * http://tools.ietf.org/html/rfc6749#section-5.1
		 */
		/* Handling those violations */
		bundle  *params = NULL;
		params = bundle_create();
		int ret = oauth2_util_get_params(response_json, &params);
		if (ret != OAUTH2_ERROR_NONE && params == NULL) {
			OAUTH2_LOG_E("Server response parse failed");
			goto CATCH;
		}
		response_temp->response_data = params;
		response_temp->error = NULL;
		*response = response_temp;

		g_object_unref(parser);

		return;
	}

	JsonNode *root_node;
	root_node = json_parser_get_root(parser);
	if (root_node == NULL) {
		OAUTH2_LOG_E("json_parser_get_root() failed");
		goto CATCH;
	}

	JsonObject *root_obj = NULL;
	root_obj = json_node_get_object((JsonNode *) root_node);
	if (root_obj == NULL) {
		OAUTH2_LOG_E("json_node_get_object() failed");
		goto CATCH;
	}

	if (json_object_has_member(root_obj, OAUTH2_PARAMETER_KEY_ERROR)) {
		oauth2_error_s *error = (oauth2_error_s *) calloc(1,
			sizeof(oauth2_error_s));
		error->error_data = bundle_create();

		json_object_foreach_member(root_obj, __iterate_json_response,
			error->error_data);
		response_temp->error = error;
	} else {
		OAUTH2_LOG_I("Filling up response struct");
		response_temp->response_data = bundle_create();
		json_object_foreach_member(root_obj, __iterate_json_response,
			response_temp->response_data);
		response_temp->error = NULL;
	}

	OAUTH2_LOG_I("__parse_acc_token_response parse finished");

	*response = response_temp;

CATCH:
	g_object_unref(parser);
	OAUTH2_LOG_I("__parse_acc_token_response end");
}

static char *
__curl_post_request(oauth2_manager_s *mgr_handle, const char *url,
	const char *post_body, int *curl_err, long *http_code)
{
	OAUTH2_LOG_I("__curl_post_request start");
	if (post_body)
		OAUTH2_LOG_I("__curl_post_request post body=[%s]", post_body);

	curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_POSTFIELDS,
		post_body);
	char *data = NULL;
	curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_WRITEDATA, &data);
	curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_WRITEFUNCTION,
		__store_curl_response);
	curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_SSL_VERIFYPEER,
		FALSE);

	*curl_err = curl_easy_perform(mgr_handle->curl_handle);
	OAUTH2_LOG_I("Response id curl code=[%d]", *curl_err);
	if (*curl_err != CURLE_OK) {
		curl_easy_cleanup(mgr_handle->curl_handle);
		return NULL;
	}
	*http_code = 0;
	curl_easy_getinfo(mgr_handle->curl_handle, CURLINFO_RESPONSE_CODE,
		http_code);
	OAUTH2_LOG_I("Response id curl http_code=[%ld]", *http_code);
	if (*http_code != 200) {
		OAUTH2_LOG_I("http_code=[%ld]", *http_code);
		curl_easy_cleanup(mgr_handle->curl_handle);
		return NULL;
	}
	curl_easy_cleanup(mgr_handle->curl_handle);
	mgr_handle->curl_handle = NULL;

	OAUTH2_LOG_I("__curl_post_request end");

	return data;
}

/* It shoud be called after all param adding is done */
static void
__add_client_authentication_post_body(oauth2_manager_s *mgr_handle,
	char **post_body)
{
	char *client_secret = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_SECRET, &client_secret);
	if (client_secret) {
		 __append_to_post_data(mgr_handle->curl_handle, *post_body,
			OAUTH2_PARAMETER_KEY_CLIENT_SECRET, client_secret,
			TRUE);
	}

	char *client_id = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_ID, &client_id);
	__append_to_post_data(mgr_handle->curl_handle, *post_body,
		OAUTH2_PARAMETER_KEY_CLIENT_ID, client_id, FALSE);
}

static void
__add_client_authentication_others(oauth2_manager_s *mgr_handle,
	long curl_auth_type)
{
	OAUTH2_LOG_I("__add_client_authentication_others start");

	curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_HTTPAUTH,
		curl_auth_type);
	char *client_secret = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_SECRET, &client_secret);
	if (client_secret) {
		OAUTH2_LOG_I("__add_client_authentication_others pwd=[%s]",
			client_secret);
		curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_PASSWORD,
			client_secret);
	}

	char *client_id = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_ID, &client_id);
	if (client_id) {
		OAUTH2_LOG_I("__add_client_authentication_others user=[%s]",
			client_id);
		curl_easy_setopt(mgr_handle->curl_handle, CURLOPT_USERNAME,
			client_id);
	}
	OAUTH2_LOG_I("__add_client_authentication_others end");
}

static void
__add_client_authentication(oauth2_manager_s *mgr_handle, char **post_body)
{
	OAUTH2_LOG_I("__add_client_authentication start");

	/*
	 * TODO:
	 * Default is post-body, but there is no RFC point for the default
	 */
	int client_auth_type = OAUTH2_CLIENT_AUTHENTICATION_TYPE_REQUEST_BODY;
	char *client_auth_type_str = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_CLIENT_AUTH_TYPE, &client_auth_type_str);
	if (client_auth_type_str)
		sscanf(client_auth_type_str, "%d", &client_auth_type);

	if (client_auth_type == OAUTH2_CLIENT_AUTHENTICATION_TYPE_REQUEST_BODY)
		__add_client_authentication_post_body(mgr_handle, post_body);
	else if (client_auth_type == OAUTH2_CLIENT_AUTHENTICATION_TYPE_BASIC)
		__add_client_authentication_others(mgr_handle, CURLAUTH_BASIC);
	else if (client_auth_type == OAUTH2_CLIENT_AUTHENTICATION_TYPE_DIGEST)
		__add_client_authentication_others(mgr_handle, CURLAUTH_DIGEST);
	else {
		/*
		 * TODO: Support for other http auth types
		 * __add_client_authentication_others(mgr_handle,
		 *	CURLAUTH_DIGEST);
		 */
	}
	OAUTH2_LOG_I("_add_client_authentication end");
}

static void
_request_access_token_by_code(oauth2_manager_s *mgr_handle, const char *code)
{
	OAUTH2_LOG_I("_request_access_token_by_code start");

	char *post_body = (char *)calloc(1, 2048);
	if (post_body == NULL) {
		OAUTH2_LOG_E("Out of memory");
		return;
	}

	mgr_handle->curl_handle = curl_easy_init();

	__append_to_post_data(mgr_handle->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE,
		OAUTH2_PARAMETER_VAL_GRANT_TYPE_AUTH_CODE, TRUE);
	__append_to_post_data(mgr_handle->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_CODE, code, TRUE);

	char *redirect_url = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, &redirect_url);
	__append_to_post_data(mgr_handle->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, redirect_url, TRUE);

	__add_client_authentication(mgr_handle, &post_body);

	char *token_url = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL, &token_url);

	int curl_err = CURLE_OK;
	long http_code = 200;
	char *data = __curl_post_request(mgr_handle, token_url, post_body,
		&curl_err, &http_code);

	if (curl_err == CURLE_OK && http_code == 200 && data != NULL) {
		__parse_acc_token_response(data, &(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	} else {
		OAUTH2_LOG_E("curl error=[%d]", http_code);
		__convert_tizen_error_to_oauth_error(http_code, "Network error",
			&(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	}
	OAUTH2_FREE(post_body);

	OAUTH2_LOG_I("_request_access_token_by_code end");
}

static void
__request_access_token_for_client_cred(oauth2_manager_s *mgr_handle)
{
	mgr_handle->curl_handle = curl_easy_init();

	char *post_body = (char *)calloc(1, 2048);
	if (post_body == NULL) {
		OAUTH2_LOG_E("Out of memory");
		return;
	}

	__append_to_post_data(mgr_handle->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE,
		OAUTH2_PARAMETER_VAL_GRANT_TYPE_CLIENT_CREDENTIALS, TRUE);

	char *scope = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_SCOPE, &scope);
	if (scope)
		__append_to_post_data(mgr_handle->curl_handle, post_body,
			OAUTH2_PARAMETER_KEY_SCOPE, scope, FALSE);

	__add_client_authentication(mgr_handle, &post_body);

	OAUTH2_LOG_I("post_body[strlen(post_body) - 1]=%c",
		post_body[strlen(post_body) - 1]);

	if (post_body[strlen(post_body) - 1] == '&')
		post_body[strlen(post_body) - 1] = '\0';

	char *token_url = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL, &token_url);

	int curl_err = CURLE_OK;
	long http_code = 200;
	char *data = __curl_post_request(mgr_handle, token_url, post_body,
		&curl_err, &http_code);

	if (curl_err == CURLE_OK && http_code == 200 && data != NULL) {
		__parse_acc_token_response(data, &(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	} else {
		OAUTH2_LOG_E("curl error=[%d]", http_code);
		__convert_tizen_error_to_oauth_error(http_code,
			"Network error", &(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	}
	OAUTH2_FREE(post_body);
}

static void
_request_access_token_for_grant_type_pwd(oauth2_manager_s *mgr_handle)
{
	mgr_handle->curl_handle = curl_easy_init();

	char *post_body = (char *)calloc(1, 2048);
	if (post_body == NULL) {
		OAUTH2_LOG_E("Out of memory");
		return;
	}

	__append_to_post_data(mgr_handle->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE,
		OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD, TRUE);

	char *user_name = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_USER_NAME, &user_name);
	if (user_name)
		__append_to_post_data(mgr_handle->curl_handle, post_body,
			OAUTH2_PARAMETER_KEY_USER_NAME, user_name, TRUE);

	char *password = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_PASSWORD, &password);
	if (password)
		__append_to_post_data(mgr_handle->curl_handle, post_body,
			OAUTH2_PARAMETER_KEY_PASSWORD, password, TRUE);

	char *scope = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_SCOPE, &scope);
	if (scope)
		__append_to_post_data(mgr_handle->curl_handle, post_body,
			OAUTH2_PARAMETER_KEY_SCOPE, scope, TRUE);

	__add_client_authentication(mgr_handle, &post_body);

	char *token_url = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_ACCESS_TOKEN_URL, &token_url);

	int curl_err = CURLE_OK;
	long http_code = 200;
	char *data = __curl_post_request(mgr_handle, token_url, post_body,
		&curl_err, &http_code);

	if (curl_err == CURLE_OK && http_code == 200 && data != NULL) {
		__parse_acc_token_response(data, &(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	} else {
		OAUTH2_LOG_E("curl error=[%d]", http_code);
		__convert_tizen_error_to_oauth_error(http_code, "Network error",
			&(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	}
	OAUTH2_FREE(post_body);
}

static void
_on_auth_grant_received(oauth2_manager_s *mgr_handle, const char *response_url)
{
	int ret = OAUTH2_ERROR_NONE;
	char *response_type_str = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_RESPONSE_TYPE, &response_type_str);
	char *response_type = __get_response_type(response_type_str);
	if (!response_type) {
		mgr_handle->is_active = FALSE;

		OAUTH2_LOG_E("Missing response_type");
		__convert_tizen_error_to_oauth_error(
			OAUTH2_ERROR_INVALID_PARAMETER,
			"Missing response_type in request",
			&(mgr_handle->response));
		__send_response_to_caller(mgr_handle);

		return;
	}

	bundle  *params = NULL;
	params = bundle_create();
	bool is_two_step = FALSE;
	if (strcmp(response_type, OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_CODE)
		== 0) {
		char *query = NULL;
		ret = oauth2_util_get_query(response_url, &query);
		if (ret != OAUTH2_ERROR_NONE && query != NULL) {
			__convert_tizen_error_to_oauth_error(
				OAUTH2_ERROR_SERVER,
				"Server response does not contain query",
				&(mgr_handle->response));
			__send_response_to_caller(mgr_handle);

			return;
		}

		ret = oauth2_util_get_params(query, &params);
		if (ret != OAUTH2_ERROR_NONE && params != NULL) {
			__convert_tizen_error_to_oauth_error(
				OAUTH2_ERROR_PARSE_FAILED,
				"Server response parse failed",
				&(mgr_handle->response));
			__send_response_to_caller(mgr_handle);
			OAUTH2_FREE(query);
			return;
		}

		/* step 2 */
		if (mgr_handle->request_func == REQUEST_ACCESS_TOKEN)
			is_two_step = TRUE;

		OAUTH2_FREE(query);
	} else if (strcmp(response_type,
		OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_TOKEN) == 0) {
		char *fragment = NULL;
		ret = oauth2_util_get_fragment(response_url, &fragment);
		if (ret != OAUTH2_ERROR_NONE && fragment != NULL) {
			__convert_tizen_error_to_oauth_error(
				OAUTH2_ERROR_SERVER,
				"Server response does not contain fragment",
				&(mgr_handle->response));
			__send_response_to_caller(mgr_handle);
			return;
		}

		ret = oauth2_util_get_params(fragment, &params);
		if (ret != OAUTH2_ERROR_NONE && params == NULL) {
			__convert_tizen_error_to_oauth_error(
				OAUTH2_ERROR_PARSE_FAILED,
				"Server response parse failed",
				&(mgr_handle->response));
			__send_response_to_caller(mgr_handle);
			OAUTH2_FREE(fragment);
			return;
		}
		OAUTH2_FREE(fragment);
	} else {
		/* TODO: Handle custom response_type (for eg, Facebook,
		 * Soundcloud supports "token_and_code") */
	}

	if (is_two_step) {
		char *code_val = NULL;
		bundle_get_str(params, OAUTH2_PARAMETER_KEY_CODE, &code_val);
		if (!code_val) {
			OAUTH2_LOG_E("Error in response: [%s] not found",
				OAUTH2_PARAMETER_KEY_CODE);

			/* Server Error response handling */

			/*
			 * google sample error: error=access_denied&
			 * state=security_token%3D138r5719ru3e1%26
			 * url%3Dhttps://oa2cb.example.com/myHome
			 */

			if (mgr_handle->response == NULL) {
				mgr_handle->response = (oauth2_response_s *)
					calloc(1, sizeof(oauth2_response_s));
			}

			mgr_handle->response->error = (oauth2_error_s *)
				calloc(1, sizeof(oauth2_error_s));
			mgr_handle->response->error->error_data = params;

			__send_response_to_caller(mgr_handle);
			return;
		}
		_request_access_token_by_code(mgr_handle, code_val);
	} else {
		mgr_handle->is_active = FALSE;
		mgr_handle->response = (oauth2_response_s *)calloc(1,
			sizeof(oauth2_response_s));

		/* Check if its error response or success resopnse */
		/*
		 * FB sample error: YOUR_REDIRECT_URI?error_reason=user_denied&
		 * error=access_denied&
		 * error_description=The+user+denied+your+request.
		 */

		char *error = NULL;
		char *error_desc = NULL;
		char *error_uri = NULL;

		bundle_get_str(params, OAUTH2_PARAMETER_KEY_ERROR, &error);
		bundle_get_str(params, OAUTH2_PARAMETER_KEY_ERROR_DESCRIPTION,
			&error_desc);
		bundle_get_str(params, OAUTH2_PARAMETER_KEY_ERROR_URI,
			&error_uri);

		if (!error && !error_desc && !error_uri) {
			/* Success */
			mgr_handle->response->response_data = params;
		} else {
			/* Error */
			mgr_handle->response->error = (oauth2_error_s *)calloc(
				1, sizeof(oauth2_error_s));
			mgr_handle->response->error->error_data = params;
		}
		__send_response_to_caller(mgr_handle);
	}
}

static void
on_web_url_load_error(void *data, Evas_Object *obj, void *event_info)
{
	/* wearable webkit does not have ewk_error defined */
#ifndef WITHOUT_EWK_ERROR
	oauth2_manager_s *mgr_handle = data;
	const Ewk_Error *error = (const Ewk_Error *)event_info;

	if (mgr_handle && mgr_handle->is_webview_active == true
		&& (ewk_error_code_get(error) != EWK_ERROR_CODE_CANCELED)) {
		__convert_tizen_error_to_oauth_error(OAUTH2_ERROR_NETWORK_ERROR,
			"Network Error", &(mgr_handle->response));
		__send_response_to_caller(mgr_handle);
	}
#endif
}

static void
on_web_url_load_finished(void *data, Evas_Object *obj, void *event_info)
{
	oauth2_manager_s *mgr_handle = data;

	if (mgr_handle->loading_popup) {
		evas_object_hide(mgr_handle->loading_popup);
		mgr_handle->loading_popup = NULL;
	}
}

static void
on_web_url_change(void *data, Evas_Object *obj, void *event_info)
{
	const char *uri = event_info;

	oauth2_manager_s *mgr_handle = data;
	char *redirect_uri = NULL;
	bundle_get_str(mgr_handle->request->request_data,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, &redirect_uri);

	if (!g_str_has_prefix(uri, redirect_uri)) {
		if (mgr_handle->loading_popup == NULL) {
			mgr_handle->loading_popup = elm_popup_add(mgr_handle->login_win);
			elm_popup_content_text_wrap_type_set(mgr_handle->loading_popup, ELM_WRAP_MIXED);
			elm_object_text_set(mgr_handle->loading_popup, OAUTH2_LOADING_POP_UP_TEXT);
			elm_popup_orient_set(mgr_handle->loading_popup, ELM_POPUP_ORIENT_BOTTOM);
			evas_object_show(mgr_handle->loading_popup);
		}
		return;
	}

	__hide_web_view(mgr_handle);

	char *oauth_response = strdup(uri);
	OAUTH2_LOG_I("Response oauth_response=[%s]", oauth_response);

	_on_auth_grant_received(mgr_handle, oauth_response);
	OAUTH2_FREE(oauth_response);
}

static int
__start_auth_grant_request(oauth2_manager_s *mgr_handle)
{
	char *authorization_url = __get_authorization_url(mgr_handle->request);
	if (!authorization_url) {
		OAUTH2_LOG_E("Failed to form authorization url");
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	mgr_handle->login_win = elm_win_util_standard_add("main_win", "");
	eext_object_event_callback_add(mgr_handle->login_win,
		EEXT_CALLBACK_BACK, __exit_back_cb, mgr_handle);

	mgr_handle->loading_popup = elm_popup_add(mgr_handle->login_win);
	elm_popup_content_text_wrap_type_set(mgr_handle->loading_popup, ELM_WRAP_MIXED);
	elm_object_text_set(mgr_handle->loading_popup, OAUTH2_LOADING_POP_UP_TEXT);
	elm_popup_orient_set(mgr_handle->loading_popup, ELM_POPUP_ORIENT_BOTTOM);

    /* ewk_init(); */

	Evas *canvas = NULL;

	mgr_handle->content_box = elm_box_add(mgr_handle->login_win);
	elm_box_padding_set(mgr_handle->content_box, 0, 3);
	evas_object_size_hint_weight_set(mgr_handle->content_box,
		EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(mgr_handle->content_box, 0.0, 0.0);
	evas_object_show(mgr_handle->content_box);

	canvas = evas_object_evas_get(mgr_handle->login_win);

	mgr_handle->ewk_view = ewk_view_add(canvas);
	if (NULL == mgr_handle->ewk_view) {
		LOGE("ewk_view_add() failed");
		__send_response_to_caller(mgr_handle);

		return OAUTH2_ERROR_UNKNOWN;
	}

	mgr_handle->is_webview_active = true;

	eext_object_event_callback_add(mgr_handle->login_win,
		EEXT_CALLBACK_BACK, __handle_back_key, mgr_handle);

	ewk_view_url_set(mgr_handle->ewk_view, authorization_url);

	evas_object_size_hint_min_set(mgr_handle->ewk_view, 480, 800);
	evas_object_size_hint_weight_set(mgr_handle->ewk_view,
		EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(mgr_handle->ewk_view,
		EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_smart_callback_add(mgr_handle->ewk_view, "url,changed",
		on_web_url_change, mgr_handle);
	evas_object_smart_callback_add(mgr_handle->ewk_view, "load,error",
		on_web_url_load_error, mgr_handle);
	evas_object_smart_callback_add(mgr_handle->ewk_view, "load,finished",
		on_web_url_load_finished, mgr_handle);

	elm_box_pack_end(mgr_handle->content_box, mgr_handle->ewk_view);
	evas_object_show(mgr_handle->ewk_view);

	evas_object_show(mgr_handle->login_win);

	evas_object_show(mgr_handle->loading_popup);

	return OAUTH2_ERROR_NONE;
}
/*LCOV_EXCL_STOP*/

OAUTH2_API int
oauth2_manager_request_token(oauth2_manager_h handle, oauth2_request_h request,
	oauth2_token_cb callback, void *user_data)
{
	OAUTH2_RETURN_VAL(callback, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"callback is Null");

	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"handle is Null");

	OAUTH2_RETURN_VAL(request, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"request is Null");
	/*LCOV_EXCL_START*/
	oauth2_manager_s *mgr_impl = (oauth2_manager_s *)handle;
	OAUTH2_RETURN_VAL(!mgr_impl->is_active, {},
		OAUTH2_ERROR_ALREADY_IN_PROGRESS, "Already in progress");

	mgr_impl->request = (oauth2_request_s *)request;

	__reset_all_cb(mgr_impl);

	char *redirect_uri = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, &redirect_uri);
	if (!redirect_uri) {
		OAUTH2_LOG_E("Missing mandatory field [%s]",
			OAUTH2_PARAMETER_KEY_REDIRECT_URI);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char *grant_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE, &grant_type_str);
	char *grant_type = __get_grant_type(grant_type_str);

	char *response_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_RESPONSE_TYPE, &response_type_str);
	char *response_type = __get_response_type(response_type_str);

	if (!grant_type && !response_type) {
		OAUTH2_LOG_E("[%s] or [%s] must be specified",
			OAUTH2_PARAMETER_KEY_GRANT_TYPE,
			OAUTH2_PARAMETER_KEY_RESPONSE_TYPE);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	mgr_impl->token_cb = callback;
	mgr_impl->user_data = user_data;

	mgr_impl->is_active = TRUE;

	/* For authorization code and implicit, response_type is mentioned */
	if (response_type) {
		if (!strcmp(response_type,
			OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_CODE) ||
			!strcmp(response_type,
			OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_TOKEN)) {
			mgr_impl->request_func = REQUEST_ACCESS_TOKEN;
			int ret = __start_auth_grant_request(mgr_impl);
			if (ret != OAUTH2_ERROR_NONE) {
				__reset_all_cb(mgr_impl);
				return ret;
			}
			return OAUTH2_ERROR_NONE;
		}
	}
	/*
	 * For resource owner pwd and client credentials, grant_type
	 * is mentioned
	 */
	else {
		/*
		 * Here authorization grant handling is out of oauth 2.0 RFC, so
		 * unless a custom auth grant handler is set, we will proceed
		 * with access token request directly
		 */
		if (!strcmp(grant_type,
			OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD) ||
			!strcmp(grant_type,
			OAUTH2_PARAMETER_VAL_GRANT_TYPE_CLIENT_CREDENTIALS)) {
			/* For pwd: grant_type, username, password, scope */
			if (!strcmp(grant_type,
				OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD)) {
				_request_access_token_for_grant_type_pwd(
					mgr_impl);
			} else {
				/*For client cred: grant_type, scope */
				__request_access_token_for_client_cred(
					mgr_impl);
			}

			/* _request_access_token() */
			return OAUTH2_ERROR_NONE;
		} else {
			/*
			 * TODO:
			 * TBD, extension grant_type / response_type support
			 */
			return OAUTH2_ERROR_NOT_SUPPORTED;
		}
	}

	/* TODO: TBD, extension grant_type / response_type support */
	return OAUTH2_ERROR_NOT_SUPPORTED;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API int
oauth2_manager_request_authorization_grant(oauth2_manager_h handle,
	oauth2_request_h request, oauth2_auth_grant_cb callback,
	void *user_data)
{
	OAUTH2_RETURN_VAL(callback, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"callback is Null");

	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"handle is Null");

	OAUTH2_RETURN_VAL(request, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"request is Null");
	/*LCOV_EXCL_START*/
	oauth2_manager_s *mgr_impl = (oauth2_manager_s *)handle;
	OAUTH2_RETURN_VAL(!mgr_impl->is_active, {},
		OAUTH2_ERROR_ALREADY_IN_PROGRESS, "Already in progress");

	mgr_impl->request = (oauth2_request_s *)request;

	__reset_all_cb(mgr_impl);

	char *redirect_uri = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_REDIRECT_URI, &redirect_uri);
	if (!redirect_uri) {
		OAUTH2_LOG_E("Missing mandatory field [%s]",
			OAUTH2_PARAMETER_KEY_REDIRECT_URI);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char *grant_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE, &grant_type_str);
	char *grant_type = __get_grant_type(grant_type_str);

	char *response_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_RESPONSE_TYPE, &response_type_str);
	char *response_type = __get_response_type(response_type_str);

	if (!grant_type && !response_type) {
		OAUTH2_LOG_E("[%s] or [%s] must be specified",
			OAUTH2_PARAMETER_KEY_GRANT_TYPE,
			OAUTH2_PARAMETER_KEY_RESPONSE_TYPE);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	mgr_impl->grant_cb = callback;
	mgr_impl->user_data = user_data;

	mgr_impl->is_active = TRUE;

	/* For authorization code and implicit, response_type is mentioned */
	if (response_type) {
		if (!strcmp(response_type,
			OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_CODE) ||
			!strcmp(response_type,
			OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_TOKEN)) {
			mgr_impl->request_func = REQUEST_AUTHORIZATION_CODE;
			int ret = __start_auth_grant_request(mgr_impl);
			if (ret != OAUTH2_ERROR_NONE) {
				__reset_all_cb(mgr_impl);
				return ret;
			}
			return OAUTH2_ERROR_NONE;
		}
	} else if (!strcmp(grant_type, OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD)
		|| !strcmp(grant_type,
		OAUTH2_PARAMETER_VAL_GRANT_TYPE_CLIENT_CREDENTIALS)) {
		/*
		 * No additional grant request needed as per RFC
		 * (http://tools.ietf.org/html/rfc6749#section-4.3.1 and
		 * http://tools.ietf.org/html/rfc6749#section-4.4.1)
		 */
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	/* TODO: TBD, extension grant_type / response_type support */
	return OAUTH2_ERROR_NOT_SUPPORTED;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API int
oauth2_manager_request_access_token(oauth2_manager_h handle,
	oauth2_request_h request, oauth2_access_token_cb callback,
	void *user_data)
{
	OAUTH2_RETURN_VAL(callback, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"callback is Null");

	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"handle is Null");

	OAUTH2_RETURN_VAL(request, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"request is Null");
	/*LCOV_EXCL_START*/
	oauth2_manager_s *mgr_impl = (oauth2_manager_s *)handle;
	OAUTH2_RETURN_VAL(!mgr_impl->is_active, {},
		OAUTH2_ERROR_ALREADY_IN_PROGRESS, "Already in progress");

	mgr_impl->request = (oauth2_request_s *)request;

	__reset_all_cb(mgr_impl);

	char *response_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_RESPONSE_TYPE, &response_type_str);
	char *response_type = __get_response_type(response_type_str);
	if (response_type) {
		if (!strcmp(response_type,
			OAUTH2_PARAMETER_VAL_RESPONSE_TYPE_TOKEN)) {
			OAUTH2_LOG_E("Obtain token directly with authorization \
				grant for implicit type");
			return OAUTH2_ERROR_INVALID_PARAMETER;
		}
	}

	char *grant_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE, &grant_type_str);
	char *grant_type = __get_grant_type(grant_type_str);

	if (!grant_type) {
		OAUTH2_LOG_E("[%s] must be specified",
			OAUTH2_PARAMETER_KEY_GRANT_TYPE);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	mgr_impl->access_token_cb = callback;
	mgr_impl->user_data = user_data;

	mgr_impl->is_active = TRUE;

	if (!strcmp(grant_type, OAUTH2_PARAMETER_VAL_GRANT_TYPE_AUTH_CODE)) {
		char *code = NULL;
		bundle_get_str(mgr_impl->request->request_data,
			OAUTH2_PARAMETER_KEY_CODE, &code);
		if (code)
			_request_access_token_by_code(mgr_impl, code);
	} else {
		/*
		 * For resource owner pwd and client credentials, grant_type
		 * is mentioned
		 */

		/*
		 * Here authorization grant handling is out of oauth 2.0 RFC, so
		 * unless a custom auth grant handler is set, we will proceed
		 * with access token request directly
		 */
		if (!strcmp(grant_type,
			OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD) ||
			!strcmp(grant_type,
			OAUTH2_PARAMETER_VAL_GRANT_TYPE_CLIENT_CREDENTIALS)) {
			/* For pwd: grant_type, username, password, scope */
			if (!strcmp(grant_type,
				OAUTH2_PARAMETER_VAL_GRANT_TYPE_PASSWORD)) {
				_request_access_token_for_grant_type_pwd(
					mgr_impl);
			} else {
				/* For client cred: grant_type, scope */
				__request_access_token_for_client_cred(
					mgr_impl);
			}

			/* _request_access_token() */
			return OAUTH2_ERROR_NONE;
		} else {
			/*
			 * TODO:
			 * TBD, extension grant_type / response_type support
			 */
			return OAUTH2_ERROR_NOT_SUPPORTED;
		}
	}

	return OAUTH2_ERROR_NOT_SUPPORTED;
	/*LCOV_EXCL_STOP*/
}

/*LCOV_EXCL_START*/
static void
_request_refresh_token(oauth2_manager_s *mgr_impl)
{
	char *refresh_uri = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL, &refresh_uri);

	char *refresh_token_input = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN, &refresh_token_input);

	char *scope = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_SCOPE, &scope);

	char *post_body = (char *)calloc(1, 2048);
	if (post_body == NULL) {
		OAUTH2_LOG_E("Out of memory");
		OAUTH2_FREE(refresh_uri);
		OAUTH2_FREE(refresh_token_input);
		OAUTH2_FREE(scope);
		return;
	}

	mgr_impl->curl_handle = curl_easy_init();

	__append_to_post_data(mgr_impl->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE,
		OAUTH2_PARAMETER_VAL_GRANT_TYPE_REFRESH_TOKEN, TRUE);
	__append_to_post_data(mgr_impl->curl_handle, post_body,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN, refresh_token_input, TRUE);
	if (scope) {
		__append_to_post_data(mgr_impl->curl_handle, post_body,
			OAUTH2_PARAMETER_KEY_SCOPE, scope, TRUE);
	}

	__add_client_authentication(mgr_impl, &post_body);

	int curl_err = CURLE_OK;
	long http_code = 200;
	char *data = __curl_post_request(mgr_impl, refresh_uri, post_body,
		&curl_err, &http_code);

	if (curl_err == CURLE_OK && http_code == 200 && data != NULL) {
		__parse_acc_token_response(data, &(mgr_impl->response));
		__send_response_to_caller(mgr_impl);
	} else {
		/* TODO __convert_tizen_error_to_oauth_error */
		OAUTH2_LOG_E("curl error=[%d]", http_code);
		__convert_tizen_error_to_oauth_error(http_code, "Network error",
			&(mgr_impl->response));
		__send_response_to_caller(mgr_impl);
	}
	OAUTH2_FREE(post_body);
}
/*LCOV_EXCL_STOP*/

OAUTH2_API int
oauth2_manager_refresh_access_token(oauth2_manager_h handle,
	oauth2_request_h request, oauth2_refresh_token_cb callback,
	void *user_data)
{
	OAUTH2_RETURN_VAL(callback, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"callback is Null");

	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"handle is Null");

	OAUTH2_RETURN_VAL(request, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"request is Null");
	/*LCOV_EXCL_START*/
	oauth2_manager_s *mgr_impl = (oauth2_manager_s *)handle;
	OAUTH2_RETURN_VAL(!mgr_impl->is_active, {},
		OAUTH2_ERROR_ALREADY_IN_PROGRESS, "Already in progress");

	mgr_impl->request = (oauth2_request_s *)request;

	__reset_all_cb(mgr_impl);

	mgr_impl->refresh_cb = callback;

	char *refresh_uri = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL, &refresh_uri);
	if (!refresh_uri) {
		OAUTH2_LOG_E("Missing mandatory field [%s]",
			OAUTH2_PARAMETER_KEY_REFRESH_TOKEN_URL);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char *grant_type_str = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_GRANT_TYPE, &grant_type_str);
	char *grant_type = __get_grant_type(grant_type_str);

	if (!grant_type) {
		OAUTH2_LOG_E("Missing mandatory field [%s]",
			OAUTH2_PARAMETER_KEY_GRANT_TYPE);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	char *refresh_token_input = NULL;
	bundle_get_str(mgr_impl->request->request_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN, &refresh_token_input);
	if (!refresh_token_input) {
		OAUTH2_LOG_E("Missing mandatory field [%s]",
			OAUTH2_PARAMETER_KEY_REFRESH_TOKEN);
		return OAUTH2_ERROR_INVALID_PARAMETER;
	}

	OAUTH2_LOG_I("refresh token request start");

	_request_refresh_token(mgr_impl);

	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API bool
oauth2_manager_is_request_in_progress(oauth2_manager_h handle)
{
	OAUTH2_RETURN_VAL(handle, {}, FALSE, "handle is Null");

	/*LCOV_EXCL_START*/
	return ((oauth2_manager_s *)handle)->is_active;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API int
oauth2_manager_clear_cookies(oauth2_manager_h handle)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"handle is Null");
	/*LCOV_EXCL_START*/
	oauth2_manager_s *mgr_impl = (oauth2_manager_s *)handle;

	Evas_Object *web_view = mgr_impl->ewk_view;
	if (web_view == NULL) {
		/*
		 * Its possible to call clear_cookies without any prior calls to
		 *  request APIs, for these cases we need to add dummy web view
		 */
		Evas *canvas = evas_object_evas_get(mgr_impl->login_win);
		if (canvas)
			web_view = ewk_view_add(canvas);
		else
			return OAUTH2_ERROR_UNKNOWN;
	}

	Ewk_Context *ewk_ctx = ewk_view_context_get(web_view);
	if (ewk_ctx) {
		Ewk_Cookie_Manager *cookie_mgr =
			ewk_context_cookie_manager_get(ewk_ctx);
		if (cookie_mgr)
			ewk_cookie_manager_cookies_clear(cookie_mgr);
		else
			return OAUTH2_ERROR_UNKNOWN;
	} else
		return OAUTH2_ERROR_UNKNOWN;

	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API int
oauth2_manager_clear_cache(oauth2_manager_h handle)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"handle is Null");
	/*LCOV_EXCL_START*/
	oauth2_manager_s *mgr_impl = (oauth2_manager_s *)handle;

	Evas_Object *web_view = mgr_impl->ewk_view;
	if (web_view == NULL) {
		/*
		 * Its possible to call clear_cookies without any prior calls to
		 * request APIs, for these cases we need to add dummy web view
		 */
		Evas *canvas = evas_object_evas_get(mgr_impl->login_win);
		if (canvas)
			web_view = ewk_view_add(canvas);
		else
			return OAUTH2_ERROR_UNKNOWN;
	}

	Ewk_Context *ewk_ctx = ewk_view_context_get(web_view);
	if (ewk_ctx)
		ewk_context_resource_cache_clear(ewk_ctx);
	else
		return OAUTH2_ERROR_UNKNOWN;

	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}
