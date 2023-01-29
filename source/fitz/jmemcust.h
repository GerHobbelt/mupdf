/* Copyright (C) 2001-2017 Artifex Software, Inc.
 * All Rights Reserved.
 *
 * This software is provided AS-IS with no warranty, either express or
 * implied.
 *
 * This software is distributed under license and may not be copied,
 * modified or distributed except as expressly authorized under the terms
 * of the license contained in the file LICENSE in this distribution.
 *
 * Refer to licensing information at http://www.artifex.com or contact
 * Artifex Software, Inc., 7 Mt. Lassen Drive - Suite A-134, San Rafael,
 * CA 94903, U.S.A., +1(415)492-9861, for further information.
 */

 #ifndef MUPDF_JMEMCUST_H
 #define MUPDF_JMEMCUST_H

/*
* Callback which sets up libmupdf default libjpeg/jpeg-turbo's system memory
* manager.
*
* Serves as an alternative to jpeg/jpeg-turbo's `jpeg_nobs_sys_mem_register()`
* and can therefor be used as the callback for `cinfo->client_callback` and
* parameter value for `jpeg_sys_mem_set_default_setup(callback)`.
*
* Seee also the new (patched) jpeg-turbo repo related with our monolithic build
* rig: github/GerHobbelt/jpeg-turbo/
*/
int fz_jpeg_dflt_sys_mem_register(j_common_ptr cinfo);

#endif
