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

#include "oauth2_util.h"

/*LCOV_EXCL_START*/
int
oauth2_util_get_query(const char *url, char **query)
{
	OAUTH2_RETURN_VAL(url, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"url is Null");
	OAUTH2_RETURN_VAL(query, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"query is Null");

	int query_len = 0;
	char *part = NULL;
	char *query_end = NULL;
	char *query_start = strchr(url, '?');
	if (query_start) {
		query_start++;
		query_end = query_start;

		while (*query_end != '\0' && *query_end != '#')
			query_end++;

		query_len = query_end - query_start;
		part = (char *) malloc((query_len + 1) * sizeof(char));
		OAUTH2_RETURN_VAL(part, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
		memset(part, '\0', (query_len + 1) * sizeof(char));

		strncpy(part, query_start, query_len);
		part[query_len] = '\0';
	}
	*query = part;

	return OAUTH2_ERROR_NONE;
}

int
oauth2_util_get_fragment(const char *url, char **fragment)
{
	OAUTH2_RETURN_VAL(url, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"url is Null");
	OAUTH2_RETURN_VAL(fragment, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"fragment is Null");

	int fragment_len = 0;
	char *part = NULL;
	char *fragment_end = NULL;
	char *fragment_start = strchr(url, '#');
	if (fragment_start) {
		fragment_start++;
		fragment_end = fragment_start;

		while (*fragment_end != '\0')
			fragment_end++;

		fragment_len = fragment_end - fragment_start;
		part = (char *) malloc((fragment_len + 1) * sizeof(char));
		OAUTH2_RETURN_VAL(part, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
		memset(part, '\0', (fragment_len + 1) * sizeof(char));

		strncpy(part, fragment_start, fragment_len);
		part[fragment_len] = '\0';
	}
	*fragment = part;

	return OAUTH2_ERROR_NONE;
}

int
oauth2_util_get_params(const char *url_part, bundle **params)
{
	OAUTH2_RETURN_VAL(url_part, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"url part is Null");
	OAUTH2_RETURN_VAL(params, {}, OAUTH2_ERROR_INVALID_PARAMETER,
		"params is Null");

	char *url = (char *) url_part;

	int idx = 0;
	gchar **pch = g_strsplit_set(url, "&;", -1);
	while (pch && pch[idx] && strlen(pch[idx]))
	{
		char *tmp_start = pch[idx];
		char *tmp_end = tmp_start;
		while (*tmp_end != '\0')
			tmp_end++;

		char *eq_ptr = strchr(tmp_start, '=');
		int key_size = (eq_ptr - tmp_start) + 1;
		char *key = (char *) malloc((key_size) * sizeof(char));
		OAUTH2_RETURN_VAL(key, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
		memset(key, '\0', ((eq_ptr - tmp_start) + 1) * sizeof(char));
		strncpy(key, tmp_start, (eq_ptr - tmp_start));

		if (eq_ptr != NULL)
			eq_ptr++;

		key_size = (tmp_end - eq_ptr) + 1;
		char *val = (char *) malloc((key_size) * sizeof(char));
		OAUTH2_RETURN_VAL(val, {}, OAUTH2_ERROR_OUT_OF_MEMORY,
			"Out of memory");
		memset(val, '\0', ((tmp_end - eq_ptr) + 1) * sizeof(char));
		strncpy(val, eq_ptr, (tmp_end - eq_ptr));

		bundle_add_str(*params, key, val);

		idx++;

		OAUTH2_FREE(key);
		OAUTH2_FREE(val);
	}

	return OAUTH2_ERROR_NONE;
}
/*LCOV_EXCL_STOP*/
