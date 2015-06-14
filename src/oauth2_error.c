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

#include "oauth2_error.h"
#include "oauth2_private.h"
#include "oauth2_util.h"

OAUTH2_API
int oauth2_error_get_code(oauth2_error_h handle, int *server_error_code,
	int *platform_error_code)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_error_s *request_data_temp = (oauth2_error_s *)handle;

	char *server_error_str = NULL;
	char *platform_error_str = NULL;

	bundle_get_str(request_data_temp->error_data,
		OAUTH2_PARAMETER_KEY_ERROR_CODE, &server_error_str);
	bundle_get_str(request_data_temp->error_data,
		OAUTH2_PARAMETER_KEY_ERROR_CODE_PLATFORM, &platform_error_str);

	if (!server_error_str && !platform_error_str)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	if (server_error_str)
		sscanf(server_error_str, "%d", server_error_code);

	if (platform_error_str)
		sscanf(platform_error_str, "%d", platform_error_code);

	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API
int oauth2_error_get_description(oauth2_error_h handle, char **description)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_error_s *error_data_temp = (oauth2_error_s *)handle;

	char *val = NULL;
	bundle_get_str(error_data_temp->error_data,
		OAUTH2_PARAMETER_KEY_ERROR_DESCRIPTION, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*description = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API
int oauth2_error_get_uri(oauth2_error_h handle, char **uri)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_error_s *error_data_temp = (oauth2_error_s *)handle;

	char *val = NULL;
	bundle_get_str(error_data_temp->error_data,
		OAUTH2_PARAMETER_KEY_ERROR_URI, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*uri = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}

OAUTH2_API
int oauth2_error_get_custom_data(oauth2_error_h handle, const char *custom_key,
	char **custom_value)
{
	OAUTH2_RETURN_VAL(handle, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"NULL handle");
	/*LCOV_EXCL_START*/
	oauth2_error_s *error_data_temp = (oauth2_error_s *)handle;

	char *val = NULL;
	bundle_get_str(error_data_temp->error_data, custom_key, &val);
	if (!val)
		return OAUTH2_ERROR_VALUE_NOT_FOUND;

	*custom_value = val;
	return OAUTH2_ERROR_NONE;
	/*LCOV_EXCL_STOP*/
}
