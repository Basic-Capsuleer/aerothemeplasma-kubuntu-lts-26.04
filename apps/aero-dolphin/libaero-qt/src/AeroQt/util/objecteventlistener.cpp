#include "objecteventlistener.h"

ObjectEventListener::ObjectEventListener(QObject *parent)
	: QObject{parent}, watched()
{}

ObjectEventListener *ObjectEventListener::instance()
{
	static ObjectEventListener inst;
	return &inst;
}

bool ObjectEventListener::eventFilter(QObject *recipient, QEvent *event)
{
	if (recipient == nullptr || event == nullptr) return false;

	Q_EMIT eventSignal(recipient, event);

	if (watched.contains(recipient))
	{
		if (watched[recipient].contains(event->type()))
		{
            HandlerList cbList = watched[recipient][event->type()];

            for (qsizetype i = 0; i < cbList.callbacks.size(); i++)
                cbList.callbacks.at(i)(event);

            if (cbList.blockPropagation)
                return true;
		}
	}

	return false;
}

void ObjectEventListener::onEvent(QObject *obj, QEvent::Type type, std::function<void (QEvent *)> cb)
{
    onEvent(obj, type, cb, false);
}

void ObjectEventListener::onEvent(QObject *obj, QEvent::Type eventType, std::function<void(QEvent *)> cb, bool block)
{
	if (!watched.contains(obj))
        watched[obj] = QHash<QEvent::Type, HandlerList>();

	if (!watched[obj].contains(eventType))
        watched[obj][eventType] = HandlerList();

    watched[obj][eventType].callbacks.append(cb);
    watched[obj][eventType].blockPropagation = block;

	obj->installEventFilter(this);

	connect(obj, &QObject::destroyed, [=](QObject *that){
		this->watched.remove(that);
	});
}
