#ifndef OBJECTEVENTLISTENER_H
#define OBJECTEVENTLISTENER_H

#include <QObject>
#include <QHash>
#include <QEvent>
#include <functional>

class ObjectEventListener : public QObject
{
	Q_OBJECT

public:
	static ObjectEventListener *instance();

	bool eventFilter(QObject *recipient, QEvent *event) override;

    void onEvent(QObject *obj, QEvent::Type type, std::function<void(QEvent *)> cb);
    void onEvent(QObject *obj, QEvent::Type type, std::function<void(QEvent *)> cb, bool block);

Q_SIGNALS:
	void eventSignal(QObject *obj, QEvent *ev);	  // This is an alternative method of using this object. You can connect to this signal, but it's slower: On(num. connected listeners) vs On(num objects)

private:
	explicit ObjectEventListener(QObject *parent = nullptr);

    struct HandlerList {
        QList<std::function<void(QEvent *)>> callbacks;
        bool blockPropagation;
    };

	QHash<
		QObject *,	// Each watched object
		QHash<		// Has a map
			QEvent::Type,	// Between event types
            HandlerList     // And lists of callbacks to be called upon each one
		>
	> watched;
};

template <typename T = QEvent, typename Fn>
void onEvent(
    QObject *obj,
    QEvent::Type type,
    Fn&& cb,
    bool runCbNow = false   /// If true, callback is triggered on this function call, but with a nullptr event. Only use with callbacks that do not access the event
)
{
	ObjectEventListener::instance()->onEvent(obj, type,
		[cb = std::forward<Fn>(cb)](QEvent *ev) {
			cb(static_cast<T *>(ev));
		}
	);

    if (runCbNow) {
        cb(nullptr);
    }
}

template <typename T = QEvent, typename Fn>
void overrideEvent(
    QObject *obj,
    QEvent::Type type,
    Fn&& cb
)
{
    ObjectEventListener::instance()->onEvent(obj, type,
        [cb = std::forward<Fn>(cb)](QEvent *ev) {
            cb(static_cast<T *>(ev));
        },
        true
    );
}

#endif // OBJECTEVENTLISTENER_H
