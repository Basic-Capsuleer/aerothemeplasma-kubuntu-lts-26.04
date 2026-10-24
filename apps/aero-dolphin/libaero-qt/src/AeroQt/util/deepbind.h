#pragma once

#include <QObject>
#include <QVariant>
#include <QMetaObject>
#include <QByteArray>
#include <functional>
#include <memory>
#include <vector>

// Assumes ConvertFn is already declared alongside bind_prop / bind_prop_bidi:
// using ConvertFn = std::function<QVariant(QVariant)>;

namespace detail {

// Extracts the declaring class of a pointer-to-member-function, used to
// deduce Src for a hop from its notify signal alone (the actual object at
// that hop is only known at runtime, unlike bind_prop where src is passed
// in directly).
template <typename T>
struct SignalClass;

template <typename C, typename... Args>
struct SignalClass<void (C::*)(Args...)> {
    using type = C;
};

// A single "resolve the next object" step, shared by DeepSource (chain on
// the read/source side) and DeepTarget (chain on the write/destination
// side). propName is read off the current object to obtain the next one;
// connectNotify wires up the signal that fires when that property changes
// (either its value, or - for a pointer property - which object it points
// to), triggering a rebuild of everything downstream.
struct ObjHop {
    QByteArray propName;
    std::function<QObject*(QObject*)> resolveObj;
    std::function<QMetaObject::Connection(QObject*, const std::function<void()>&)> connectNotify;
};

template <typename NotifyFn>
inline ObjHop makeObjHop(const char* propName, NotifyFn notify) {
    using Src = typename SignalClass<NotifyFn>::type;

    ObjHop hop;
    hop.propName = propName;
    hop.resolveObj = [propName](QObject* obj) -> QObject* {
        if (!obj)
            return nullptr;
        return obj->property(propName).value<QObject*>();
    };
    hop.connectNotify = [notify](QObject* obj, const std::function<void()>& cb) -> QMetaObject::Connection {
        Src* s = qobject_cast<Src*>(obj);
        if (!s)
            return QMetaObject::Connection();
        return QObject::connect(s, notify, s, cb);
    };
    return hop;
}

} // namespace detail

/**
 * Binds a target property to the end of a chain of property hops, akin to
 * repeated g_object_bind_property() through intermediate objects, except
 * that any intermediate object may itself be replaced at runtime (e.g. a
 * "titleAction" pointer property being reassigned), in which case the
 * remainder of the chain is rebuilt from that point on.
 *
 * Usage:
 *   DeepSource(this)
 *       .prop("titleAction", &ActionPgph::titleActionChanged)
 *       .prop("text", &QAction::changed)
 *       .bind(this, "text", true);
 *
 * Each .prop(name, notify) hop reads `name` off the object produced by the
 * previous hop (the root object for the first hop), and reconnects/rebuilds
 * everything downstream whenever `notify` fires. The LAST hop's property is
 * the actual value applied to the destination (it is read, not descended
 * into), unlike every hop before it.
 */
class DeepSource {
public:
    explicit inline DeepSource(QObject* root) : impl(std::make_shared<Impl>()) {
        impl->root = root;
    }

    template <typename NotifyFn>
    inline DeepSource& prop(const char* propName, NotifyFn notify) {
        impl->hops.push_back(detail::makeObjHop(propName, notify));
        return *this;
    }

private:
    struct Impl; // forward declaration, so Handle below can name it as an incomplete type

public:
    /// Handle allowing the whole chain of connections to be torn down later,
    /// analogous to BindingBidi::disconnect().
    struct Handle {
        std::shared_ptr<class DeepSource::Impl> impl;
        inline void disconnect() {
            if (impl)
                impl->disconnectAll();
        }
    };

    /// Terminates the chain, binding its resolved value to dst->DST_PROP.
    /// syncOnBind controls whether dst is updated immediately, matching the
    /// syncOnBind parameter of bind_prop.
    inline Handle bind(QObject* dst, const char* dstProp, bool syncOnBind = false, ConvertFn convert = {}) {
        impl->dst = dst;
        impl->dstProp = dstProp;
        impl->convert = convert ? convert : [](QVariant v) { return v; };
        impl->connections.assign(impl->hops.size(), QMetaObject::Connection());

        // dst is only ever read/written from the chain's own notify
        // callbacks - it never gets a hop connection of its own - so its
        // destruction must be watched explicitly. Without this, an upstream
        // chain object outliving dst keeps firing into a dangling pointer.
        auto implPtr = impl;
        QObject::connect(dst, &QObject::destroyed, dst, [implPtr]() {
            implPtr->dst = nullptr;
            implPtr->disconnectAll();
        });

        impl->rebind(0, impl->root, syncOnBind);
        return Handle{impl};
    }

private:
    struct Impl : std::enable_shared_from_this<Impl> {
        QObject* root = nullptr;
        std::vector<detail::ObjHop> hops;
        std::vector<QMetaObject::Connection> connections;
        QObject* dst = nullptr;
        QByteArray dstProp;
        ConvertFn convert;

        inline void disconnectFrom(size_t startIdx) {
            for (size_t i = startIdx; i < connections.size(); ++i) {
                if (connections[i]) {
                    QObject::disconnect(connections[i]);
                    connections[i] = QMetaObject::Connection();
                }
            }
        }

        inline void disconnectAll() { disconnectFrom(0); }

        inline void updateDestination(QObject* leafObj) {
            if (!dst || !leafObj || hops.empty())
                return;
            QVariant newValue = convert(leafObj->property(hops.back().propName.constData()));
            QVariant oldValue = dst->property(dstProp.constData());
            if (newValue != oldValue) // stops notify loops, same guard as bind_prop
                dst->setProperty(dstProp.constData(), newValue);
        }

        // Connects hop `idx` on `obj`, then either descends to the next hop
        // (resolving the next object) or, if this is the last hop, wires it
        // straight to the destination. pushUpdate controls whether the
        // destination is written immediately as part of this call (used for
        // syncOnBind on the initial bind, and unconditionally true whenever
        // this is invoked as a result of an upstream rebuild, since the
        // value at the leaf has necessarily changed).
        inline void rebind(size_t idx, QObject* obj, bool pushUpdate) {
            if (!obj || idx >= hops.size())
                return;

            auto self = shared_from_this();
            bool isLast = (idx + 1 == hops.size());

            if (!isLast) {
                connections[idx] = hops[idx].connectNotify(obj, [self, idx, obj]() {
                    self->disconnectFrom(idx + 1);
                    QObject* nextObj = self->hops[idx].resolveObj(obj);
                    self->rebind(idx + 1, nextObj, true);
                });
                QObject* nextObj = hops[idx].resolveObj(obj);
                rebind(idx + 1, nextObj, pushUpdate);
            } else {
                connections[idx] = hops[idx].connectNotify(obj, [self, obj]() {
                    self->updateDestination(obj);
                });
                if (pushUpdate)
                    updateDestination(obj);
            }
        }
    };

    std::shared_ptr<Impl> impl;
};

/**
 * Mirror image of DeepSource: binds a single (shallow) source property to
 * a property at the end of a chain of hops on the destination side. Every
 * .prop(name, notify) hop resolves to a further object; the trailing
 * .prop(name) call (no notify) instead names the property to be written on
 * the object the chain resolves to.
 *
 * Usage:
 *   DeepTarget(b)
 *       .prop("page", &Aero::Browser::pageChanged)
 *       .prop("searchText")
 *       .bindFrom(m_searchBox, "text", &QLineEdit::textChanged, true);
 *
 * This keeps b->page->searchText in sync with m_searchBox->text, re-pushing
 * the current source value into the new leaf object whenever page is
 * reassigned, not just whenever text itself changes.
 */
class DeepTarget {
public:
    explicit inline DeepTarget(QObject* root) : impl(std::make_shared<Impl>()) {
        impl->root = root;
    }

    template <typename NotifyFn>
    inline DeepTarget& prop(const char* propName, NotifyFn notify) {
        impl->hops.push_back(detail::makeObjHop(propName, notify));
        return *this;
    }

    /// Names the terminal property to write to, on the object reached after
    /// all preceding .prop(name, notify) hops. No notify signal is needed
    /// here since this property is only ever written, never watched.
    inline DeepTarget& prop(const char* propName) {
        impl->dstProp = propName;
        return *this;
    }

private:
    struct Impl; // forward declaration, so Handle below can name it as an incomplete type

public:
    struct Handle {
        std::shared_ptr<class DeepTarget::Impl> impl;
        inline void disconnect() {
            if (impl)
                impl->disconnectAll();
        }
    };

    /// Terminates the chain, keeping the object reached by following the
    /// .prop(name, notify) hops - written to at the property named by the
    /// trailing .prop(name) call - in sync with src->srcProp, per srcNotify.
    /// syncOnBind controls whether the destination is updated immediately.
    template <typename SrcNotifyFn>
    inline Handle bindFrom(QObject* src, const char* srcProp, SrcNotifyFn srcNotify,
                           bool syncOnBind = false, ConvertFn convert = {})
    {
        using Src = typename detail::SignalClass<SrcNotifyFn>::type;

        impl->src = src;
        impl->srcProp = srcProp;
        impl->convert = convert ? convert : [](QVariant v) { return v; };
        impl->destConnections.assign(impl->hops.size(), QMetaObject::Connection());

        auto implPtr = impl;
        Src* s = qobject_cast<Src*>(src);
        if (s) {
            impl->srcConnection = QObject::connect(s, srcNotify, s, [implPtr]() {
                implPtr->pushValue();
            });
        }

        // src is dereferenced from pushValue() whenever ANY dest-chain hop
        // fires, not only via srcConnection above - so even though that one
        // connection is self-cleaning, the raw src pointer itself still
        // needs its own destruction watched, or a later dest-chain rebuild
        // dereferences it after it's gone.
        QObject::connect(src, &QObject::destroyed, src, [implPtr]() {
            implPtr->src = nullptr;
            implPtr->disconnectAll();
        });

        impl->rebindDest(0, impl->root, syncOnBind);
        return Handle{impl};
    }

private:
    struct Impl : std::enable_shared_from_this<Impl> {
        QObject* root = nullptr;
        std::vector<detail::ObjHop> hops;
        std::vector<QMetaObject::Connection> destConnections;
        QMetaObject::Connection srcConnection;
        QMetaObject::Connection leafDestroyedConnection;
        QObject* src = nullptr;
        QByteArray srcProp;
        QByteArray dstProp;
        ConvertFn convert;
        QObject* currentLeaf = nullptr; // object the chain currently resolves to

        inline void disconnectDestFrom(size_t startIdx) {
            for (size_t i = startIdx; i < destConnections.size(); ++i) {
                if (destConnections[i]) {
                    QObject::disconnect(destConnections[i]);
                    destConnections[i] = QMetaObject::Connection();
                }
            }
        }

        inline void disconnectAll() {
            disconnectDestFrom(0);
            if (srcConnection)
                QObject::disconnect(srcConnection);
            if (leafDestroyedConnection)
                QObject::disconnect(leafDestroyedConnection);
        }

        inline void pushValue() {
            if (!src || !currentLeaf)
                return;
            QVariant newValue = convert(src->property(srcProp.constData()));
            QVariant oldValue = currentLeaf->property(dstProp.constData());
            if (newValue != oldValue) // stops notify loops, same guard as bind_prop
                currentLeaf->setProperty(dstProp.constData(), newValue);
        }

        // currentLeaf is write-only (no notify hop of its own), so its
        // destruction is otherwise invisible to this binding. Re-armed each
        // time the chain resolves to a (possibly new) leaf object.
        inline void setCurrentLeaf(QObject* leaf) {
            if (leafDestroyedConnection)
                QObject::disconnect(leafDestroyedConnection);
            currentLeaf = leaf;
            if (leaf) {
                auto self = shared_from_this();
                leafDestroyedConnection = QObject::connect(leaf, &QObject::destroyed, leaf, [self]() {
                    self->currentLeaf = nullptr;
                });
            }
        }

        // Walks the chain from `idx`/`obj` onward. Once the chain is
        // exhausted, obj IS the destination leaf: it is recorded and,
        // if requested, the current source value is pushed into it
        // immediately (used both for syncOnBind and, unconditionally,
        // whenever an upstream hop rebuild reaches this point, since the
        // leaf just changed identity and needs the current value pushed
        // in to stay consistent).
        inline void rebindDest(size_t idx, QObject* obj, bool pushOnReach) {
            if (!obj) {
                setCurrentLeaf(nullptr);
                return;
            }
            if (idx == hops.size()) {
                setCurrentLeaf(obj);
                if (pushOnReach)
                    pushValue();
                return;
            }

            auto self = shared_from_this();
            destConnections[idx] = hops[idx].connectNotify(obj, [self, idx, obj]() {
                self->disconnectDestFrom(idx + 1);
                QObject* nextObj = self->hops[idx].resolveObj(obj);
                self->rebindDest(idx + 1, nextObj, true);
            });
            QObject* nextObj = hops[idx].resolveObj(obj);
            rebindDest(idx + 1, nextObj, pushOnReach);
        }
    };

    std::shared_ptr<Impl> impl;
};
