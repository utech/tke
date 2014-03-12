/****************************************************************************
**
** Copyright (C) 2003-2006 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** Licensees holding valid Qt Solutions licenses may use this file in
** accordance with the Qt Solutions License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/products/qt/addon/solutions/ 
** or email sales@trolltech.com for information about Qt Solutions
** License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef QTWIZARD_H
#define QTWIZARD_H

#include <QtCore/QBitArray>
#include <QtCore/QMetaProperty>
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtGui/QDialog>
#include <QtGui/QPixmap>

#if QT_VERSION < 0x040100
#error "QtWizard requires Qt 4.1.0 or above"
#endif

class QAbstractButton;
class QFrame;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;
class QtWizard;
class QtWizardHeader;
class QtWizardPage;
class QtWizardRuler;

#if defined(Q_WS_WIN)
#  if !defined(QT_QTWIZARD_EXPORT) && !defined(QT_QTWIZARD_IMPORT)
#    define QT_QTWIZARD_EXPORT
#  elif defined(QT_QTWIZARD_IMPORT)
#    if defined(QT_QTWIZARD_EXPORT)
#      undef QT_QTWIZARD_EXPORT
#    endif
#    define QT_QTWIZARD_EXPORT __declspec(dllimport)
#  elif defined(QT_QTWIZARD_EXPORT)
#    undef QT_QTWIZARD_EXPORT
#    define QT_QTWIZARD_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTWIZARD_EXPORT
#endif

class QtWizardDefaultProperty
{
public:
    QByteArray className;
    QByteArray property;
    QByteArray changedSignal;

    inline QtWizardDefaultProperty() {}
    inline QtWizardDefaultProperty(const char *className, const char *property,
                                   const char *changedSignal)
        : className(className), property(property), changedSignal(changedSignal) {}
};

class QtWizardField
{
public:
    inline QtWizardField() {}
    QtWizardField(QtWizardPage *page, const QString &spec, QObject *object, const char *property,
                  const char *changedSignal);

    void resolve(QtWizard *wizard);
    void findProperty(const QtWizardDefaultProperty *properties, int propertyCount);

    QtWizardPage *page;
    QString name;
    bool mandatory;
    QObject *object;
    QByteArray property;
    QMetaProperty metaProperty;
    QByteArray changedSignal;
    QVariant initialValue;
};

class QT_QTWIZARD_EXPORT QtWizard : public QDialog
{
    Q_OBJECT
    Q_ENUMS(WizardStyle WizardOption)
    Q_FLAGS(WizardOptions)
    Q_PROPERTY(WizardStyle wizardStyle READ wizardStyle WRITE setWizardStyle)
    Q_PROPERTY(WizardOptions options READ options WRITE setOptions)
    Q_PROPERTY(Qt::TextFormat titleFormat READ titleFormat WRITE setTitleFormat)
    Q_PROPERTY(Qt::TextFormat subTitleFormat READ subTitleFormat WRITE setSubTitleFormat)
    Q_PROPERTY(int startId READ startId WRITE setStartId)
    Q_PROPERTY(int currentId READ currentId NOTIFY currentIdChanged)

public:
    enum WizardButton {
        BackButton,
        NextButton,
        FinishButton,
        CancelButton,
        HelpButton,
        CustomButton1,
        CustomButton2,
        CustomButton3,
        Stretch,

        NoButton = -1,
        NStandardButtons = 5,
        NButtons = 8
    };

    enum WizardPixmap {
        WatermarkPixmap,
        LogoPixmap,
        BannerPixmap,
        BackgroundPixmap,
        NPixmaps
    };

    enum WizardStyle {
        ClassicStyle,
        ModernStyle,
        MacStyle
    };

    enum WizardOption {
        IndependentPages                = 0x00000001,
        IgnoreSubTitles                 = 0x00000002,
        ExtendedWatermarkPixmap         = 0x00000004,
        NoDefaultButton                 = 0x00000008,
        NoBackButtonOnStartPage         = 0x00000010,
        NoBackButtonOnLastPage          = 0x00000020,
        DisabledBackButtonOnLastPage    = 0x00000040,
        HaveNextButtonOnLastPage        = 0x00000080,
        HaveFinishButtonOnEarlyPages    = 0x00000100,
        NoCancelButton                  = 0x00000200,
        CancelButtonOnLeft              = 0x00000400,
        HaveHelpButton                  = 0x00000800,
        HelpButtonOnRight               = 0x00001000,
        HaveCustomButton1               = 0x00002000,
        HaveCustomButton2               = 0x00004000,
        HaveCustomButton3               = 0x00008000
    };

    Q_DECLARE_FLAGS(WizardOptions, WizardOption)

    QtWizard(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~QtWizard();

    int addPage(QtWizardPage *page);
    void setPage(int id, QtWizardPage *page);
    QtWizardPage *page(int id) const;
    bool hasVisitedPage(int id) const;
    QList<int> visitedPages() const;
    void setStartId(int id);
    int startId() const;
    QtWizardPage *currentPage() const;
    int currentId() const;

    void setField(const QString &name, const QVariant &value);
    QVariant field(const QString &name) const;

    void setWizardStyle(WizardStyle style);
    WizardStyle wizardStyle() const;

    void setOption(WizardOption option, bool on = true);
    bool testOption(WizardOption option) const;
    void setOptions(WizardOptions options);
    WizardOptions options() const;

    void setButtonText(WizardButton which, const QString &text);
    QString buttonText(WizardButton which) const;
    void setButtonLayout(const QList<WizardButton> &layout);
    void setButton(WizardButton which, QAbstractButton *button);
    QAbstractButton *button(WizardButton which) const;

    void setTitleFormat(Qt::TextFormat format);
    Qt::TextFormat titleFormat() const;
    void setSubTitleFormat(Qt::TextFormat format);
    Qt::TextFormat subTitleFormat() const;
    void setPixmap(WizardPixmap which, const QPixmap &pixmap);
    QPixmap pixmap(WizardPixmap which) const;

    void setDefaultProperty(const char *className, const char *property,
                            const char *changedSignal);

    void setVisible(bool visible);
    QSize sizeHint() const;

Q_SIGNALS:
    void currentIdChanged(int id);
    void helpRequested();
    void customButtonClicked(int which);

public Q_SLOTS:
    void back();
    void next();
    void restart();

protected:
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void done(int result);
    virtual void initializePage(int id);
    virtual void cleanupPage(int id);
    virtual bool validateCurrentPage();
    virtual int nextId() const;

private Q_SLOTS:
    void emitCustomButtonClicked();
    void updateButtonsEnabledState();

private:
    typedef QMap<int, QtWizardPage *> PageMap;

    enum Direction {
        Backward,
        Forward
    };

    class LayoutInfo
    {
    public:
        inline LayoutInfo()
            : topLevelMargin(-1), childMargin(-1), spacing(-1), wizStyle(ClassicStyle),
              header(false), watermark(false), title(false), subTitle(false), extension(false) {}

        int topLevelMargin;
        int childMargin;
        int spacing;
        int buttonSpacing;
        WizardStyle wizStyle;
        bool header;
        bool watermark;
        bool title;
        bool subTitle;
        bool extension;

        bool operator==(const LayoutInfo &other);
        inline bool operator!=(const LayoutInfo &other) { return !operator==(other); }
    };

#ifdef Q_WS_MAC
    static QPixmap findDefaultBackgroundPixmap();
#endif
    void reset();
    void cleanupPagesNotInHistory();
    void addField(const QtWizardField &field);
    void switchToPage(int newId, Direction direction);
    LayoutInfo layoutInfoForCurrentPage();
    void recreateLayout(const LayoutInfo &info);
    void updateLayout();
    bool ensureButton(WizardButton which) const;
    void connectButton(WizardButton which) const;
    void updateButtonsVisibleState();
    void updateDefaultButton();
    void updateButtonTexts();
    void updateButtonLayout();
    void setButtonLayout(const WizardButton *array, int size);
    bool buttonLayoutContains(WizardButton which);
    void updatePixmap(WizardPixmap which);
    void disableUpdates();
    void enableUpdates();

    PageMap pageMap;
    QVector<QtWizardField> fields;
    QMap<QString, int> fieldIndexMap;
    QVector<QtWizardDefaultProperty> defaultPropertyTable;
    QList<int> history;
    QSet<int> initialized; // ### remove
    int start;
    int current;
    bool canContinue;
    bool canFinish;
    LayoutInfo layoutInfo;
    int disableUpdatesCount;
    bool currentPageExplicitlyIncomplete;
    bool currentPageExplicitlyFinal;

    WizardStyle wizStyle;
    WizardOptions opts;
    uint buttonHasCustomText;
    bool buttonsHaveCustomLayout;
    QList<WizardButton> buttonsCustomLayout;
    Qt::TextFormat titleFmt;
    Qt::TextFormat subTitleFmt;
    mutable QPixmap defaultPixmaps[NPixmaps];

    union {
        // keep in sync with WizardButton
        mutable struct {
            QAbstractButton *back;
            QAbstractButton *next;
            QAbstractButton *finish;
            QAbstractButton *cancel;
            QAbstractButton *help;
        } btn;
        mutable QAbstractButton *btns[NButtons];
    };
    QWidget *antiFlickerWidget;
    QWidget *placeholderWidget1;
    QWidget *placeholderWidget2;
    QtWizardHeader *headerWidget;
    QLabel *watermarkLabel;
    QFrame *pageFrame;
    QLabel *titleLabel;
    QLabel *subTitleLabel;
    QtWizardRuler *bottomRuler;

    QVBoxLayout *pageVBoxLayout;
    QHBoxLayout *buttonLayout;
    QGridLayout *mainLayout;

    friend class QtWizardField;
    friend class QtWizardHeader;
    friend class QtWizardPage;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtWizard::WizardOptions)

class QT_QTWIZARD_EXPORT QtWizardPage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString subTitle READ subTitle WRITE setSubTitle)

public:
    QtWizardPage(QWidget *parent = 0);

    void setTitle(const QString &title);
    QString title() const;
    void setSubTitle(const QString &subTitle);
    QString subTitle() const;
    void setPixmap(QtWizard::WizardPixmap which, const QPixmap &pixmap);
    QPixmap pixmap(QtWizard::WizardPixmap which) const;
    void setFinal(bool final);
    bool isFinal() const;

    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool validatePage();
    virtual bool isComplete() const;
    virtual int nextId() const;

Q_SIGNALS:
    void completeChanged();

protected:
    void setField(const QString &name, const QVariant &value);
    QVariant field(const QString &name) const;
    void registerField(const QString &name, QWidget *widget, const char *property = 0,
                       const char *changedSignal = 0);
    QtWizard *wizard() const;

private Q_SLOTS:
    void maybeEmitCompleteChanged();
    void updateCachedCompleteState();

private:
    bool cachedIsComplete() const;

    enum TriState { Tri_Unknown = -1, Tri_False, Tri_True };

    QtWizard *myWizard;
    QString myTitle;
    QString mySubTitle;
    QPixmap myPixmaps[QtWizard::NPixmaps];
    QVector<QtWizardField> pendingFields;
    mutable TriState completeState;
    bool explicitlyFinal;

    friend class QtWizard;
};

#endif
