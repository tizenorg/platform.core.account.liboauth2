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

#ifndef OAUTH2_ERROR_H_
#define OAUTH2_ERROR_H_

#include "oauth2_types.h"
#include <bundle.h>
#include <tizen_error.h>

/**
 * @file oauth2_error.h
 * @brief The oauth 2.0 error APIs are used to get error fields received from server.
 * Refer to service provider documentation about the allowed and additional fields.
 */

/**
 * @addtogroup CAPI_OAUTH2_MODULE
 * @{
 */

/**
 * @brief The structure type for OAuth2 Error handle.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 */
typedef struct oauth2_error_s *oauth2_error_h;

/**
 * @brief Gets error code.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 *
 * @remarks		You must not free server_error_code and platform_error_code
 * @param[in] handle The error handle.
 * @param [out] server_error_code Error code (if any) returned by the server.
 * @param [out] platform_error_code Tizen platform related error code (if any), value is of type oauth2_error_e
 *
 * @return     @c 0 on success,
 *             otherwise a negative error value
 * @retval     #OAUTH2_ERROR_NONE               Successful
 * @retval     #OAUTH2_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval     #OAUTH2_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval     #OAUTH2_ERROR_VALUE_NOT_FOUND    Value not found
 */
OAUTH2_API int oauth2_error_get_code(oauth2_error_h handle, int *server_error_code, int *platform_error_code);

/**
 * @brief Gets error description.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 *
 * @remarks		You must not free description
 * @param[in] handle The error handle.
 * @param[out] description The Error description.
 *
 * @return     @c 0 on success,
 *             otherwise a negative error value
 * @retval     #OAUTH2_ERROR_NONE               Successful
 * @retval     #OAUTH2_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval     #OAUTH2_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval     #OAUTH2_ERROR_VALUE_NOT_FOUND    Value not found
 */
OAUTH2_API int oauth2_error_get_description(oauth2_error_h handle, char **description);

/**
 * @brief Gets error uri.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 *
 * @remarks		You must not free uri
 * @param[in] handle The error handle.
 * @param[out] uri The Error URI.
 *
 * @return     @c 0 on success,
 *             otherwise a negative error value
 * @retval     #OAUTH2_ERROR_NONE               Successful
 * @retval     #OAUTH2_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval     #OAUTH2_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval     #OAUTH2_ERROR_VALUE_NOT_FOUND    Value not found
 */
OAUTH2_API int oauth2_error_get_uri(oauth2_error_h handle, char **uri);

/**
 * @brief Gets the custom data.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 *
 * @remarks		You must not free custom_value
 * @param[in] handle The error handle.
 * @param[in] custom_key The custom key.
 * @param[out] custom_value The custom value.
 *
 * @return     @c 0 on success,
 *             otherwise a negative error value
 * @retval     #OAUTH2_ERROR_NONE               Successful
 * @retval     #OAUTH2_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval     #OAUTH2_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval     #OAUTH2_ERROR_VALUE_NOT_FOUND    Value not found
 */
OAUTH2_API int oauth2_error_get_custom_data(oauth2_error_h handle, const char *custom_key, char **custom_value);

/* End of oauth2 APIs */
/**
 * @}
 */

#endif /* OAUTH2_ERROR_H_ */
