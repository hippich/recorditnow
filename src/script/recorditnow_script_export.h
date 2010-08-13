// own
#include <config-recorditnow.h>

#ifdef HAVE_QTSCRIPT
    // own
    #include "helper.h"
    #include "script/scriptmanager.h"

    // Qt
    #include <QtScript/QScriptEngine>

    #define RECORDITNOW_SCRIPT_CREATE_OBJECT(ID, CLASS, NAME, PARENTTYPE) \
        static QScriptValue static_##ID##_Constructor(QScriptContext *context, QScriptEngine *engine) \
        { \
          \
            QWidget *parent = 0; \
            if (context->argumentCount() > 0) { \
                parent = qobject_cast<QWidget*>(context->argument(0).toQObject()); \
            } \
            CLASS *object = new CLASS(parent); \
            return engine->newQObject(object, QScriptEngine::ScriptOwnership); \
        } \
        Q_SCRIPT_DECLARE_QMETAOBJECT(CLASS, PARENTTYPE)

    #define RECORDITNOW_SCRIPT_REGISTER(ID, NAME) \
        registerClass(static_##ID##_Constructor, NAME);
#endif
