// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CHAMELEONCONFIG_H
#define CHAMELEONCONFIG_H

#include <QObject>

#include <deepin_kwineffects.h>

// 标记窗口当前是否正在使用变色龙窗口标题栏主题
#define _DEEPIN_CHAMELEON "_DEEPIN_CHAMELEON_THEME"
// 如果这个属性值为1，则表示此窗口的标题栏高度为0, 会隐藏标题栏
#define _DEEPIN_NO_TITLEBAR "_DEEPIN_NO_TITLEBAR"
// 强制对窗口开启边框修饰，对kde override类型的窗口，将会去除override标记。对unmanaged类型的窗口不生效
#define _DEEPIN_FORCE_DECORATE "_DEEPIN_FORCE_DECORATE"
// 设置窗口的裁剪区域，数据内容为QPainterPath导入到QDataStream
#define _DEEPIN_SCISSOR_WINDOW "_DEEPIN_SCISSOR_WINDOW"
// kwin窗口阴影属性，在没有窗口修饰器的窗口上会使用此属性绘制窗口阴影
#define _KDE_NET_WM_SHADOW "_KDE_NET_WM_SHADOW"
#define _NET_WM_WINDOW_TYPE "_NET_WM_WINDOW_TYPE"

namespace KWin {
class Client;
class Unmanaged;
class EffectWindow;
class Toplevel;
class ShellClient;
class AbstractClient;
}

QT_BEGIN_NAMESPACE

class QDBusPendingCallWatcher;

QT_END_NAMESPACE

class X11Shadow;
class ChameleonConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool activated  READ isActivated NOTIFY activatedChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    enum EffectDataRole {
        BaseRole = KWin::DataRole::LanczosCacheRole + 100,
        WindowRadiusRole = BaseRole + 1,
        WindowClipPathRole = BaseRole + 2,
        WindowMaskTextureRole = BaseRole + 3,
        ShadowMaskRole = KWin::DataRole::LanczosCacheRole + 201,
        ShadowOffsetRole
    };

    enum ShadowCacheType {
        ActiveType,
        InactiveType,
        UnmanagedType,
        ShadowCacheTypeCount
    };

    static ChameleonConfig *instance();

    quint32 atomDeepinChameleon() const;
    quint32 atomDeepinNoTitlebar() const;
    quint32 atomDeepinScissorWindow() const;

    bool isActivated() const;

    QString theme() const;

    qreal screenScaleFactor() const;

public Q_SLOTS:
    bool setTheme(QString theme);

Q_SIGNALS:
    void activatedChanged(bool activated);
    void themeChanged(QString theme);
    void windowNoTitlebarPropertyChanged(quint32 windowId);
    void windowForceDecoratePropertyChanged(quint32 windowId);
    void windowScissorWindowPropertyChanged(quint32 windowId);
    void windowTypeChanged(QObject *window);
    void screenScaleFactorChanged(qreal scale);
    void appearanceChanged(const QString& key, const QString& value);

protected:
    explicit ChameleonConfig(QObject *parent = nullptr);

private Q_SLOTS:
    void onConfigChanged();
    void onClientAdded(KWin::AbstractClient *client);
    // 针对X11BypassWindowManagerHint类型的窗口需要做一些特殊处理
    void onUnmanagedAdded(KWin::Unmanaged *client);
    void onCompositingToggled(bool active);
    void onWindowPropertyChanged(quint32 windowId, quint32 atom);
    void onWindowDataChanged(KWin::EffectWindow *window, int role);
    void onWindowShapeChanged(quint32 windowId);

    void onAppearanceChanged(const QString& key, const QString& value);
    void onScreenScaleFactorChanged(QDBusPendingCallWatcher *watcher);

    void updateWindowNoBorderProperty(QObject *window);
    void updateWindowBlurArea(KWin::EffectWindow *window, int role);
    void updateWindowSize();
    void updateClientX11Shadow();
    void updateClientNoBorder(QObject *client, bool allowReset = true);
    void updateClientWindowRadius(QObject *client);
    void updateClientClipPath(QObject *client);

    // 用于调试窗口启动速度
    void debugWindowStartupTime(QObject *toplevel);
    void onToplevelDamaged(KWin::Toplevel* toplevel, const QRegion& damage);

    void onShellClientAdded(KWin::ShellClient *client);
    void updateWindowRadius();

private:
    void init();

    void setActivated(const bool active);
    void buildKWinX11Shadow(QObject *client);
    void buildKWinX11ShadowDelay(QObject *client, int delay = 100);
    void buildKWinX11ShadowForNoBorderWindows();
    void clearKWinX11ShadowForWindows();
    void clearX11ShadowCache();
    // 处理所有额外支持的窗口属性，比如_DEEPIN_SCISSOR_WINDOW、_DEEPIN_FORCE_DECORATE
    // 第二个参数表示这个调用是否由属性变化引起
    void enforceWindowProperties(QObject *client);
    void enforcePropertiesForWindows(bool enable);
    bool setWindowOverrideType(QObject *client, bool enable);

    bool m_activated = false;
    QString m_theme;

    quint32 m_atom_deepin_chameleon;
    quint32 m_atom_deepin_no_titlebar;
    quint32 m_atom_deepin_force_decorate;
    quint32 m_atom_deepin_scissor_window;
    quint32 m_atom_kde_net_wm_shadow;
    quint32 m_atom_net_wm_window_type;

    qreal m_scaleFactor;

    QMap<QString, X11Shadow*> m_x11ShadowCache;
    QHash<QObject*, quint32> m_pendingWindows;
};

#endif // CHAMELEONCONFIG_H
