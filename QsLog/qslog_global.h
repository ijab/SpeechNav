#ifndef QSLOG_GLOBAL_H
#define QSLOG_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QSLOG_LIB
# define QSLOG_EXPORT Q_DECL_EXPORT
#else
# define QSLOG_EXPORT Q_DECL_IMPORT
#endif

#endif // QSLOG_GLOBAL_H
