#ifndef __I18N_H__
#define __I18N_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) (const char *)gettext(String)
#else
#define _(String) (String)
#endif

#endif
