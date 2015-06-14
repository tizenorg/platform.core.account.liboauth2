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

#include "oauth2_response.h"
#include "oauth2_private.h"
#include "oauth2_util.h"

/*LCOV_EXCL_START*/
int
_oauth2_response_create(oauth2_response_s **response)
{
	oauth2_response_s *response_temp = 
		(oauth2_response_s *)calloc(1, sizeof(oauth2_response_s));
	OAUTH2_RETURN_VAL(response_temp, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
	response_temp->error = NULL;
	response_temp->response_data = bundle_create();

	*response = response_temp;
	return 0;
}
/*LCOV_EXCL_STOP*/

OAUTH2_API int
oauth2_response_destroy(oauth2_response_h handle)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER, "NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_temp = (oauth2_response_s *)handle;
	if (response_temp->response_data)
		bundle_free(response_temp->response_data);

	if (response_temp->error) {
		bundle_free(response_temp->error->error_data);
		OAUTH2_FREE(response_temp->error);
	}
	OAUTH2_FREE(response_temp);
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets the authorization code.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_authorization_code(oauth2_response_h handle, char **code)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_CODE, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*code = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets state.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_state(oauth2_response_h handle, char **state)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_STATE, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*state = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets access token.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_access_token(oauth2_response_h handle, char **access_token)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_ACCESS_TOKEN, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*access_token = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets token type.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_token_type(oauth2_response_h handle, char **token_type)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_TOKEN_TYPE, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*token_type = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets expiry time.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_expires_in(oauth2_response_h handle,
	long long int *expires_in)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_EXPIRES_IN, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	g_message("oauth2_response_get_expires_in [%s]=[%s]",
		OAUTH2_PARAMETER_KEY_EXPIRES_IN, val);
	sscanf(val, "%lld", expires_in);
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets refresh token.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_refresh_token(oauth2_response_h handle,
	char **refresh_token)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_REFRESH_TOKEN, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*refresh_token = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets scope.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_scope(oauth2_response_h handle, char **scope)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data,
		OAUTH2_PARAMETER_KEY_SCOPE, &val);

	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*scope = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets error.
 * @see @oauth2_error_h
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_error(oauth2_response_h handle, oauth2_error_h *error)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	*error = (oauth2_error_h)response_data_temp->error;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

/**
 * @brief Gets the bundle corresponding to the oauth2_response_h handle.
 * @since_tizen 2.3
 */
OAUTH2_API int
oauth2_response_get_custom_data(oauth2_response_h handle,
	const char *custom_key, char **custom_value)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER, "NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_response_s *response_data_temp = (oauth2_response_s *)handle;

	char *val = NULL;
	bundle_get_str(response_data_temp->response_data, custom_key, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*custom_value = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}
