/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _OAUTH2_UTIL_H_
#define _OAUTH2_UTIL_H_

#include <stdlib.h>
#include <bundle.h>
#include <dlog.h>
#include <glib.h>
#include "oauth2_types.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "oauth2"

#ifdef OAUTH2_LOG_E
#undef OAUTH2_LOG_E
#endif

#ifdef OAUTH2_LOG_D
#undef OAUTH2_LOG_D
#endif

#ifdef OAUTH2_LOG_I
#undef OAUTH2_LOG_I
#endif

#define OAUTH2_LOG_E(fmt, args...) LOGE("[%s:%d] "fmt"\n", __func__, \
		__LINE__, ##args)	/* LCOV_EXCL_LINE */

#define OAUTH2_LOG_D(fmt, args...) LOGD("[%s:%d] "fmt"\n", __func__, \
		__LINE__, ##args)	/* LCOV_EXCL_LINE */

#define OAUTH2_LOG_I(fmt, args...) LOGI("[%s:%d] "fmt"\n", __func__, \
		__LINE__, ##args)	/* LCOV_EXCL_LINE */

#define OAUTH2_RETURN_VAL(eval, expr, ret_val, X) \
	if (!(eval)) { \
	expr; \
	OAUTH2_LOG_E(X); \
	return ret_val; \
} else { ; }	/* LCOV_EXCL_LINE */

#define OAUTH2_FREE(ptr) \
	if (ptr != NULL) { \
		free(ptr); \
		ptr = NULL; \
	}	/* LCOV_EXCL_LINE */

int oauth2_util_get_query(const char *url, char **query);
int oauth2_util_get_fragment(const char *url, char **fragment);
int oauth2_util_get_params(const char *url_part, bundle **params);

#endif /* _OAUTH2_UTIL_H_ */
