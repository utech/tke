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
#include <QtCore/qalgorithms.h>
#include <QtCore/qvarlengtharray.h>
#include <QtGui/qapplication.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qdesktopwidget.h>
#include <QtGui/qframe.h>
#include <QtGui/qlabel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qstyle.h>
#ifdef Q_WS_MAC
#include <QtGui/qmacstyle_mac.h>
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#endif

#include "qtwizard.h"

#include <string.h>

// These fudge terms were needed a few places to obtain pixel-perfect results
const int GapBetweenLogoAndRightEdge = 5;
const int ModernHeaderTopMargin = 2;
const int ClassicHMargin = 4;
const int MacButtonHMargin = 13;

static void changeSpacerSize(QLayout *layout, int index, int width, int height)
{
    QSpacerItem *spacer = layout->itemAt(index)->spacerItem();
    Q_ASSERT(spacer);
    spacer->changeSize(width, height);
}

static QWidget *iWantTheFocus(QWidget *ancestor)
{
    const int MaxIterations = 100;

    QWidget *candidate = ancestor;
    for (int i = 0; i < MaxIterations; ++i) {
        candidate = candidate->nextInFocusChain();
        if (!candidate)
            break;

        if (candidate->focusPolicy() & Qt::TabFocus) {
            if (candidate != ancestor && ancestor->isAncestorOf(candidate))
                return candidate;
        }
    }
    return 0;
}

bool QtWizard::LayoutInfo::operator==(const LayoutInfo &other)
{
    return topLevelMargin == other.topLevelMargin
           && childMargin == other.childMargin
           && spacing == other.spacing
           && buttonSpacing == other.buttonSpacing
           && wizStyle == other.wizStyle
           && header == other.header
           && watermark == other.watermark
           && title == other.title
           && subTitle == other.subTitle
           && extension == other.extension;
}

class QtWizardHeader : public QWidget
{
public:
    enum RulerType { Ruler };

    inline QtWizardHeader(RulerType /* ruler */, QWidget *parent = 0)
        : QWidget(parent) { setFixedHeight(2); }
    QtWizardHeader(QWidget *parent = 0);

    void setup(const QtWizard::LayoutInfo &info, const QString &title, const QString &subTitle,
               const QPixmap &logo, const QPixmap &banner, Qt::TextFormat titleFormat,
               Qt::TextFormat subTitleFormat);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLabel *titleLabel;
    QLabel *subTitleLabel;
    QLabel *logoLabel;
    QGridLayout *layout;
    QPixmap bannerPixmap;
};

QtWizardHeader::QtWizardHeader(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setBackgroundRole(QPalette::Base);

    titleLabel = new QLabel(this);
    titleLabel->setBackgroundRole(QPalette::Base);

    subTitleLabel = new QLabel(this);
    subTitleLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    subTitleLabel->setWordWrap(true);

    logoLabel = new QLabel(this);

    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);

    layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->setRowMinimumHeight(3, 1);
    layout->setRowStretch(4, 1);

    layout->setColumnStretch(2, 1);
    layout->setColumnMinimumWidth(4, 2 * GapBetweenLogoAndRightEdge);
    layout->setColumnMinimumWidth(6, GapBetweenLogoAndRightEdge);

    layout->addWidget(titleLabel, 2, 1, 1, 2);
    layout->addWidget(subTitleLabel, 4, 2);
    layout->addWidget(logoLabel, 1, 5, 5, 1);
}

void QtWizardHeader::setup(const QtWizard::LayoutInfo &info, const QString &title,
                           const QString &subTitle, const QPixmap &logo, const QPixmap &banner,
                           Qt::TextFormat titleFormat, Qt::TextFormat subTitleFormat)
{
    bool modern = (info.wizStyle == QtWizard::ModernStyle);

    layout->setRowMinimumHeight(0, modern ? ModernHeaderTopMargin : 0);
    layout->setRowMinimumHeight(1, modern ? info.topLevelMargin - ModernHeaderTopMargin - 1 : 0);
    layout->setRowMinimumHeight(6, (modern ? 3 : GapBetweenLogoAndRightEdge) + 2);

    int minColumnWidth0 = modern ? 2 * info.topLevelMargin : 0;
    int minColumnWidth1 = modern ? 2 * info.topLevelMargin + 1
                                 : info.topLevelMargin + ClassicHMargin;
    layout->setColumnMinimumWidth(0, minColumnWidth0);
    layout->setColumnMinimumWidth(1, minColumnWidth1);

    titleLabel->setTextFormat(titleFormat);
    titleLabel->setText(title);
    logoLabel->setPixmap(logo);

    subTitleLabel->setTextFormat(subTitleFormat);
    subTitleLabel->setText(QLatin1String("Pq\nPq"));
    int desiredSubTitleHeight = subTitleLabel->sizeHint().height();
    subTitleLabel->setText(subTitle);

    if (modern) {
        bannerPixmap = banner;
    } else {
        bannerPixmap = QPixmap();
    }

    if (bannerPixmap.isNull()) {
        /*
            There is no widthForHeight() function, so we simulate it with a loop.
        */
        int candidateSubTitleWidth = qMin(512, 2 * qApp->desktop()->width() / 3);
        int delta = candidateSubTitleWidth >> 1;
        while (delta > 0) {
            if (subTitleLabel->heightForWidth(candidateSubTitleWidth - delta)
                        <= desiredSubTitleHeight)
                candidateSubTitleWidth -= delta;
            delta >>= 1;
        }

        subTitleLabel->setMinimumSize(candidateSubTitleWidth, desiredSubTitleHeight);

        QSize size = layout->totalMinimumSize();
#if QT_VERSION < 0x040200
        // work around bug in QGridLayout
        size += QSize(minColumnWidth0 + (3 * GapBetweenLogoAndRightEdge), 9);
#endif

        setMinimumSize(size);
        setMaximumSize(QWIDGETSIZE_MAX, size.height());
    } else {
        subTitleLabel->setMinimumSize(0, 0);
        setFixedSize(banner.size() + QSize(0, 2));
    }
    updateGeometry();
}

void QtWizardHeader::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, bannerPixmap);

    int x = width() - 2;
    int y = height() - 2;
    const QPalette &pal = palette();
    painter.setPen(pal.mid().color());
    painter.drawLine(0, y, x, y);
    painter.setPen(pal.base().color());
    painter.drawPoint(x + 1, y);
    painter.drawLine(0, y + 1, x + 1, y + 1);
}

// We save one vtable by basing QtWizardRuler on QtWizardHeader
class QtWizardRuler : public QtWizardHeader
{
public:
    inline QtWizardRuler(QWidget *parent = 0)
        : QtWizardHeader(Ruler, parent) {}
};

static bool objectInheritsXAndXIsCloserThanY(const QObject *object, const QByteArray &classX,
                                             const QByteArray &classY)
{
    const QMetaObject *metaObject = object->metaObject();
    while (metaObject) {
        if (metaObject->className() == classX)
            return true;
        if (metaObject->className() == classY)
            return false;
        metaObject = metaObject->superClass();
    }
    return false;
}

const int NFallbackDefaultProperties = 7;

const struct {
    const char *className;
    const char *property;
    const char *changedSignal;
} fallbackProperties[NFallbackDefaultProperties] = {
    // If you modify this list, make sure to update the documentation
    { "QAbstractButton", "checked", SIGNAL(toggled(bool)) },
    { "QAbstractSlider", "value", SIGNAL(valueChanged(int)) },
    { "QComboBox", "currentIndex", SIGNAL(currentIndexChanged(int)) },
    { "QDateTimeEdit", "dateTime", SIGNAL(dateTimeChanged(QDateTime)) },
    { "QLineEdit", "text", SIGNAL(textChanged(QString)) },
    { "QListWidget", "currentRow", SIGNAL(currentRowChanged(int)) },
    { "QSpinBox", "value", SIGNAL(valueChanged(int)) }
};

QtWizardField::QtWizardField(QtWizardPage *page, const QString &spec, QObject *object,
                             const char *property, const char *changedSignal)
    : page(page), name(spec), mandatory(false), object(object), property(property),
      changedSignal(changedSignal)
{
    if (name.endsWith(QLatin1Char('*'))) {
        name.chop(1);
        mandatory = true;
    }
}

void QtWizardField::resolve(QtWizard *wizard)
{
    if (property.isEmpty())
        findProperty(wizard->defaultPropertyTable.constData(),
                     wizard->defaultPropertyTable.count());

    const QMetaObject *metaObject = object->metaObject();
    int index = metaObject->indexOfProperty(property);
    if (index != -1) {
        metaProperty = metaObject->property(index);
        initialValue = metaProperty.read(object);
    }
}

void QtWizardField::findProperty(const QtWizardDefaultProperty *properties, int propertyCount)
{
    QByteArray className;

    for (int i = 0; i < propertyCount; ++i) {
        if (objectInheritsXAndXIsCloserThanY(object, properties[i].className, className)) {
            className = properties[i].className;
            property = properties[i].property;
            changedSignal = properties[i].changedSignal;
        }
    }
}

/*!
    \class QtWizard
    \brief The QtWizard class provides a framework for wizards.

    A wizard (also called an assistant on Mac OS X) is a special type
    of input dialog that consists of a sequence of pages. A wizard's
    purpose is to guide the user through a process step by step.
    Wizards are useful for complex or infrequent tasks that users may
    find difficult to learn.

    QtWizard inherits QDialog and represents a wizard. Each page is a
    QtWizardPage (a QWidget subclass). To create your own wizards, you
    can use these classes directly, or you can subclass them for more
    control.

    Topics:

    \tableofcontents

    \section1 A Trivial Example

    The following example illustrates how to create wizard pages and
    add them to a wizard. For more advanced examples, see
    \l{classwizard}{Class Wizard} and \l{licensewizard}{License
    Wizard}.

    \quotefromfile trivialwizard/trivialwizard.cpp
    \skipto createIntroPage()
    \printuntil createRegistrationPage()
    \printline {
    \dots
    \skipto /^\}/
    \printline }
    \printuntil createConclusionPage()
    \printline {
    \dots
    \skipto /^\}/
    \printline /^\}/
    \printline main(
    \printuntil /^\}/

    \section1 Wizard Look and Feel

    QtWizard supports three wizard looks:

    \list
    \o ClassicStyle is the default on X11 and with Qtopia Core.
    \o ModernStyle is the default on Windows.
    \o MacStyle is the default on Mac OS X.
    \endlist

    You can explicitly set the look to use using setWizardStyle()
    (e.g., if you want the same look on all platforms).

    \table
    \header \o ClassicStyle
            \o ModernStyle
            \o MacStyle
    \row    \o \inlineimage qtwizard-classic1.png
            \o \inlineimage qtwizard-modern1.png
            \o \inlineimage qtwizard-mac1.png
    \row    \o \inlineimage qtwizard-classic2.png
            \o \inlineimage qtwizard-modern2.png
            \o \inlineimage qtwizard-mac2.png
    \endtable

    In addition to the wizard style, there are several options that
    control the look and feel of the wizard. These can be set using
    setOption() or setOptions(). For example, HaveHelpButton makes
    QtWizard show a \gui Help button along with the other wizard
    buttons.

    You can even change the order of the wizard buttons to any
    arbitrary order using setButtonLayout(), and you can add up to
    three custom buttons (e.g., a \gui Print button) to the button
    row. This is achieved by calling setButton() or setButtonText()
    with CustomButton1, CustomButton2, or CustomButton3 to set up the
    button, and by enabling the HaveCustomButton1, HaveCustomButton2,
    or HaveCustomButton3 options. Whenever the user clicks a custom
    button, customButtonClicked() is emitted. For example:

    \quotefromfile licensewizard/licensewizard.cpp
    \skipto setButtonText
    \printuntil printButtonClicked

    \section1 Elements of a Wizard Page

    Wizards consist of a sequence of \l{QtWizardPage}s. At any time,
    only one page is shown. A page has the following attributes:

    \list
    \o A \l{QtWizardPage::}{title}.
    \o A \l{QtWizardPage::}{subTitle}.
    \o A set of pixmaps, which may or may not be honored, depending
       on the wizard's style:
        \list
        \o WatermarkPixmap (used by ClassicStyle and ModernStyle)
        \o BannerPixmap (used by ModernStyle)
        \o LogoPixmap (used by ClassicStyle and ModernStyle)
        \o BackgroundPixmap (used by MacStyle)
        \endlist
    \endlist

    The diagram belows showns how QtWizard renders these attributes,
    assuming they are all present and ModernStyle is used:

    \image qtwizard-nonmacpage.png

    When a \l{QtWizardPage::}{subTitle} is set, QtWizard displays it
    in a header, in which case it also uses the BannerPixmap and the
    LogoPixmap to decorate the header. The WatermarkPixmap is
    displayed on the left side, below the header. At the bottom,
    there is a row of buttons allowing the user to navigate through
    the pages.

    The page itself (the \l{QtWizardPage} widget) occupies the area
    between the header, the watermark, and the button row. Typically,
    the page is a QtWizardPage on which a QGridLayout is installed,
    with standard child widgets (\l{QLabel}s, \l{QLineEdit}s, etc.).

    If the wizard's style is MacStyle, the page looks radically
    different:

    \image qtwizard-macpage.png

    The watermark, banner, and logo pixmaps are ignored by the
    MacStyle. If the BackgroundPixmap is set, it is used as the
    background for the wizard; otherwise, a default "assistant" image
    is used.

    The title and subtitle are set by calling
    QtWizardPage::setTitle() and QtWizardPage::setSubTitle() on the
    individual pages. They may be plain text or HTML (see titleFormat
    and subTitleFormat). The pixmaps can be set globally for the
    entire wizard using setPixmap(), or on a per-page basis using
    QtWizardPage::setPixmap().

    \target field mechanism
    \section1 Registering and Using Fields

    In many wizards, the contents of a page may affect the default
    values of the fields of a later page. To make it easy to
    communicate between pages, QtWizard supports a "field" mechanism
    that allows you to register a field (e.g., a QLineEdit) on a page
    and to access its value from any page. It is also possible to
    specify mandatory fields (i.e., fields that must be filled before
    the user can advance to the next page).

    To register a field, call QtWizardPage::registerField() field.
    For example:

    \quotefromfile classwizard/classwizard.cpp
    \skipto ::ClassInfoPage
    \printuntil {
    \dots
    \skipto classNameLabel
    \printto groupBox =
    \skipto className
    \printuntil qobjectMacro
    \dots
    \skipto /^\}/
    \printline }

    The above code registers three fields, \c className, \c
    baseClass, and \c qobjectMacro, which are associated with three
    child widgets. The asterisk (\c *) next to \c className denotes a
    mandatory field.

    \target initialize page
    The fields of any page are accessible from any other page. For
    example:

    \skipto OutputFilesPage::initializePage
    \printuntil /^\}/

    Here, we call QtWizardPage::field() to access the contents of the
    \c className field (which was defined in the \c ClassInfoPage)
    and use it to initialize the \c OuputFilePage. The field's
    contents is returned as a QVariant.

    When we create a field using QtWizardPage::registerField(), we
    pass a unique field name and a widget. We can also provide a Qt
    property name and a "changed" signal (a signal that is emitted
    when the property changes) as third and fourth arguments;
    however, this is not necessary for the most common Qt widgets,
    such as QLineEdit, QCheckBox, and QComboBox, because QtWizard
    knows which properties to look for.

    \target mandatory fields

    If an asterisk (\c *) is appended to the name when the property
    is registered, the field is a \e{mandatory field}. When a page has
    mandatory fields, the \gui Next and/or \gui Finish buttons are
    enabled only when all mandatory fields are filled.

    To consider a field "filled", QtWizard simply checks that the
    field's current value doesn't equal the original value (the value
    it had when initializePage() was called). For QLineEdit, QtWizard
    also checks that
    \l{QLineEdit::hasAcceptableInput()}{hasAcceptableInput()} returns
    true, to honor any validator or mask.

    QtWizard's mandatory field mechanism is provided for convenience.
    An more powerful (but also more cumbersome) alternative is to
    reimplement QtWizardPage::isComplete() and to emit the
    QtWizardPage::completeChanged() signal whenever the page becomes
    complete or incomplete.

    The enabled/disabled state of the \gui Next and/or \gui Finish
    buttons is one way to perform validation on the user input.
    Another way is to reimplement validateCurrentPage() (or
    QtWizardPage::validatePage()) to perform some last-minute
    validation (and show an error message if the user has entered
    incomplete or invalid information). If the function returns true,
    the next page is shown (or the wizard finishes); otherwise, the
    current page stays up.

    \section1 Creating Linear Wizards

    Most wizards have a linear structure, with page 1 followed by
    page 2 and so on until the last page. The \l{classwizard}{Class
    Wizard} example is such a wizard. With QtWizard, linear wizards
    are created by instantiating the \l{QtWizardPage}s and inserting
    them using addPage(). By default, the pages are shown in the
    order in which they were added. For example:

    \quotefromfile classwizard/classwizard.cpp
    \skipto ::ClassWizard
    \printuntil addPage(new Conclu
    \dots
    \skipto /^\}/
    \printline }

    When a page is about to be shown, QtWizard calls initializePage()
    (which in turn calls QtWizardPage::initializePage()) to fill the
    page with default values. By default, this function does nothing,
    but it can be reimplemented to initialize the page's contents
    based on other pages' fields (see the \l{initialize page}{example
    above}).

    If the user presses \gui Back, cleanupPage() is called (which in
    turn calls QtWizardPage::cleanupPage()). The default
    implementation resets the page's fields to their original values
    (the values they had before initializePage() was called). If you
    want the \gui Back button to be non-destructive and keep the
    values entered by the user, simply enable the IndependentPages
    option.

    \section1 Creating Non-Linear Wizards

    Some wizards are more complex in that they allow different
    traversal paths based on the information provided by the user.
    The \l{licensewizard}{License Wizard} example illustrates this.
    It provides five wizard pages; depending on which options are
    selected, the user can reach different pages.

    \image licensewizard-flow.png

    In complex wizards, pages are identified by IDs. These IDs are
    typically defined using an enum. For example:

    \quotefromfile licensewizard/licensewizard.h
    \skipto : public QtWizard
    \printuntil {
    \dots
    \skipto enum {
    \printuntil };
    \dots
    \skipto /^\};/
    \printline }

    The pages are inserted using setPage(), which takes an ID and a
    instance of QtWizardPage (or of a subclass):

    \quotefromfile licensewizard/licensewizard.cpp
    \skipto ::LicenseWizard
    \printuntil setPage(Page_Conclusion
    \dots
    \skipto /^\}/
    \printline }

    By default, the pages are shown in increasing ID order. To
    provide a dynamic order that depends on the options chosen by the
    user, we must reimplement QtWizardPage::nextId(). For example:

    \skipto IntroPage::nextId
    \printuntil /^\}/

    \skipto EvaluatePage::nextId
    \printuntil /^\}/

    \skipto RegisterPage::nextId
    \printuntil /^\}/

    \skipto DetailsPage::nextId
    \printuntil /^\}/

    \skipto ConclusionPage::nextId
    \printuntil /^\}/

    It would also be possible to put all the logic in one place, in a
    QtWizardPage::nextId() reimplementation. For example:

    \code
        int LicenseWizard::nextId() const
        {
            switch (currentId()) {
            case Page_Intro:
                if (field("intro.evaluate").toBool()) {
                    return Page_Evaluate;
                } else {
                    return Page_Register;
                }
            case Page_Evaluate:
                return Page_Conclusion;
            case Page_Register:
                if (field("register.upgradeKey").toString().isEmpty()) {
                    return Page_Details;
                } else {
                    return Page_Conclusion;
                }
            case Page_Details:
                return Page_Conclusion;
            case Page_Conclusion:
            default:
                return -1;
            }
        }
    \endcode

    To start at another page than the page with the lowest ID, call
    setStartId().

    To test whether a page has been visited or not, call
    hasVisitedPage(). For example:

    \skipto ConclusionPage::initializePage
    \printuntil /^\}/

    \sa QtWizardPage, {Class Wizard Example}, {License Wizard Example}
*/

/*!
    \enum QtWizard::WizardButton

    This enum specifies the buttons in a wizard.

    \value BackButton  The \gui Back button (\gui {Go Back} on Mac OS X)
    \value NextButton  The \gui Next button (\gui Continue on Mac OS X)
    \value FinishButton  The \gui Finish button (\gui Done on Mac OS X)
    \value CancelButton  The \gui Cancel button (see also NoCancelButton)
    \value HelpButton    The \gui Help button (see also HaveHelpButton)
    \value CustomButton1  The first user-defined button (see also HaveCustomButton1)
    \value CustomButton2  The second user-defined button (see also HaveCustomButton2)
    \value CustomButton3  The third user-defined button (see also HaveCustomButton3)

    The following value is only useful when calling setButtonLayout():

    \value Stretch  A horizontal stretch in the button layout

    \omitvalue NoButton
    \omitvalue NStandardButtons
    \omitvalue NButtons

    \sa setButton(), setButtonText(), setButtonLayout(), customButtonClicked()
*/

/*!
    \enum QtWizard::WizardPixmap

    This enum specifies the pixmaps that can be associated with a page.

    \value WatermarkPixmap  The tall pixmap on the left side of a ClassicStyle or ModernStyle page
    \value LogoPixmap  The small pixmap on the right side of a ClassicStyle or ModernStyle page header
    \value BannerPixmap  The pixmap that occupies the background of a ModernStyle page header
    \value BackgroundPixmap  The pixmap that occupies the background of a MacStyle wizard

    \omitvalue NPixmaps

    \sa setPixmap(), QtWizardPage::setPixmap(), {Elements of a Wizard Page}
*/

/*!
    \enum QtWizard::WizardStyle

    This enum specifies the different looks supported by QtWizard.

    \value ClassicStyle  Classic Windows look (default on X11 and Qtopia)
    \value ModernStyle  Modern Windows look (default on Windows)
    \value MacStyle  Mac OS X look (default on Mac OS X)

    \sa setWizardStyle(), WizardOption, {Wizard Look and Feel}
*/

/*!
    \enum QtWizard::WizardOption

    This enum specifies various options that affect the look and feel
    of a wizard.

    \value IndependentPages  The pages are independent of each other
                             (i.e., they don't derive values from each
                             other).
    \value IgnoreSubTitles  Don't show any subtitles, even if they are set.
    \value ExtendedWatermarkPixmap  Extend any WatermarkPixmap all the
                                    way down to the window's edge.
    \value NoDefaultButton  Don't make the \gui Next or \gui Finish button the
                            dialog's \l{QPushButton::setDefault()}{default button}.
    \value NoBackButtonOnStartPage  Don't show the \gui Back button on the start page.
    \value NoBackButtonOnLastPage   Don't show the \gui Back button on the last page.
    \value DisabledBackButtonOnLastPage  Disable the \gui Back button on the last page.
    \value HaveNextButtonOnLastPage  Show the (disabled) \gui Next button on the last page.
    \value HaveFinishButtonOnEarlyPages  Show the (disabled) \gui Finish button on non-final pages.
    \value NoCancelButton  Don't show the \gui Cancel button.
    \value CancelButtonOnLeft  Put the \gui Cancel button on the left of \gui Back (rather than on
                               the right of \gui Finish or \gui Next).
    \value HaveHelpButton  Show the \gui Help button.
    \value HelpButtonOnRight  Put the \gui Help button on the far right of the button layout
                              (rather than on the far left).
    \value HaveCustomButton1  Show the first user-defined button (CustomButton1).
    \value HaveCustomButton2  Show the second user-defined button (CustomButton2).
    \value HaveCustomButton3  Show the third user-defined button (CustomButton3).

    \sa setOptions(), setOption(), testOption()
*/

/*!
    Constructs a wizard with the given \a parent and window \a flags.

    \sa parent(), windowFlags()
*/
QtWizard::QtWizard(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags), start(INT_MIN), current(-1), canContinue(false), canFinish(false),
      disableUpdatesCount(0), currentPageExplicitlyIncomplete(false),
      currentPageExplicitlyFinal(false),
#if defined(Q_WS_MAC)
      wizStyle(MacStyle), opts(NoDefaultButton | NoCancelButton),
#elif defined(Q_WS_WIN)
      wizStyle(ModernStyle), opts(HelpButtonOnRight),
#else
      wizStyle(ClassicStyle), opts(0),
#endif
      buttonHasCustomText(0), buttonsHaveCustomLayout(false), titleFmt(Qt::AutoText),
      subTitleFmt(Qt::AutoText)
{
    antiFlickerWidget = new QWidget(this);
    placeholderWidget1 = 0;
    placeholderWidget2 = 0;

    for (int i = 0; i < NButtons; ++i)
        btns[i] = 0;

    // create these buttons right away; create the other buttons as necessary
    ensureButton(BackButton);
    ensureButton(NextButton);
    ensureButton(FinishButton);

    headerWidget = 0;
    watermarkLabel = 0;

    pageFrame = new QFrame(antiFlickerWidget);
    pageFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    titleLabel = 0;
    subTitleLabel = 0;
    bottomRuler = 0;

    pageVBoxLayout = new QVBoxLayout(pageFrame);
    pageVBoxLayout->setSpacing(0);
    pageVBoxLayout->addSpacing(0);
    pageVBoxLayout->addStretch(1);

    buttonLayout = new QHBoxLayout;
    buttonLayout->addSpacing(0);
    buttonLayout->addSpacing(0);

    mainLayout = new QGridLayout(antiFlickerWidget);
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);

    updateButtonTexts();
    updateButtonLayout();

    for (int i = 0; i < NFallbackDefaultProperties; ++i)
        defaultPropertyTable.append(QtWizardDefaultProperty(fallbackProperties[i].className,
                                                            fallbackProperties[i].property,
                                                            fallbackProperties[i].changedSignal));
}

/*!
    Destroys the wizard and its pages, releasing any allocated resources.
*/
QtWizard::~QtWizard()
{
    delete buttonLayout;
}

/*!
    Adds the given \a page to the wizard, and returns the page's ID.

    The ID is guaranteed to be larger than any other ID in the
    QtWizard so far.

    \sa setPage(), page()
*/
int QtWizard::addPage(QtWizardPage *page)
{
    int id = 0;
    if (!pageMap.isEmpty())
        id = (pageMap.constEnd() - 1).key() + 1;
    setPage(id, page);
    return id;
}

/*!
    Adds the given \a page to the wizard with the given \a id.

    \sa addPage(), page()
*/
void QtWizard::setPage(int id, QtWizardPage *page)
{
    if (!page) {
        qWarning("QtWizard::setPage: Cannot insert null page");
        return;
    }

    if (pageMap.contains(id)) {
        qWarning("QtWizard::setPage: Page with duplicate ID %d ignored", id);
        return;
    }

    page->setParent(pageFrame);

    QVector<QtWizardField> &pendingFields = page->pendingFields;
    for (int i = 0; i < pendingFields.count(); ++i)
        addField(pendingFields.at(i));
    pendingFields.clear();

    connect(page, SIGNAL(completeChanged()), this, SLOT(updateButtonsEnabledState()));

    pageMap.insert(id, page);
    page->myWizard = this;

    int n = pageVBoxLayout->count();

    // disable layout to prevent layout updates while adding
    bool pageVBoxLayoutEnabled = pageVBoxLayout->isEnabled();
    pageVBoxLayout->setEnabled(false);

    pageVBoxLayout->insertWidget(n - 1, page);

    /*
        If the page can stretch, let it stretch; otherwise, we stretch
        "infinitely" more than the page itself.
    */
    int stretch = 32768 * int(pageVBoxLayout->itemAt(n - 1)->expandingDirections() & Qt::Vertical);
    pageVBoxLayout->setStretchFactor(page, stretch);

    // hide new page and reset layout to old status
    page->hide();
    pageVBoxLayout->setEnabled(pageVBoxLayoutEnabled);
}

/*!
    Returns the page with the given \a id, or 0 if there is no such
    page.

    \sa addPage(), setPage()
*/
QtWizardPage *QtWizard::page(int id) const
{
    return pageMap.value(id);
}

/*!
    Returns true if the page history contains page \a id; otherwise,
    returns false.

    Pressing \gui Back marks the current page as "unvisited" again.

    \sa visitedPages()
*/
bool QtWizard::hasVisitedPage(int id) const
{
    return history.contains(id);
}

/*!
    Returns the list of visited pages, in the order in which they
    were visited.

    Pressing \gui Back marks the current page as "unvisited" again.

    \sa hasVisitedPage()
*/
QList<int> QtWizard::visitedPages() const
{
    return history;
}

/*!
    \property QtWizard::startId
    \brief the ID of the first page

    If this property isn't explicitly set, this property defaults to
    the lowest page ID in this wizard, or -1 if no page has been
    inserted yet.

    \sa restart(), nextId()
*/
void QtWizard::setStartId(int id)
{
    if (!pageMap.contains(id)) {
        qWarning("QtWizard::setStartId: Invalid page ID %d", id);
        return;
    }
    start = id;
}

int QtWizard::startId() const
{
    if (start != INT_MIN)
        return start;
    if (!pageMap.isEmpty())
        return pageMap.constBegin().key();
    return -1;
}

/*!
    Returns a pointer to the current page, or 0 if there is no current
    page (e.g., before the wizard is shown).

    This is equivalent to calling page(currentId()).

    \sa page(), currentId(), restart()
*/
QtWizardPage *QtWizard::currentPage() const
{
    return page(current);
}

/*!
    \property QtWizard::currentId
    \brief the ID of the current page

    This property cannot be set directly. To change the current page,
    call next(), back(), or restart().

    \sa currentIdChanged(), currentPage()
*/
int QtWizard::currentId() const
{
    return current;
}

/*!
    Sets the value of the field called \a name to \a value.

    This function can be used to set fields on any page of the wizard.

    \sa QtWizardPage::registerField(), QtWizardPage::setField(), field()
*/
void QtWizard::setField(const QString &name, const QVariant &value)
{
    int index = fieldIndexMap.value(name, -1);
    if (index != -1) {
        const QtWizardField &field = fields.at(index);
        if (!field.metaProperty.write(field.object, value))
            qWarning("QtWizard::setField: Couldn't write to property");
        return;
    }

    qWarning("QtWizard::setField: No such field '%s'", qPrintable(name));
}

/*!
    Returns the value of the field called \a name.

    This function can be used to access fields on any page of the wizard.

    \sa QtWizardPage::registerField(), QtWizardPage::field(), setField()
*/
QVariant QtWizard::field(const QString &name) const
{
    int index = fieldIndexMap.value(name, -1);
    if (index != -1) {
        const QtWizardField &field = fields.at(index);
        return field.metaProperty.read(field.object);
    }

    qWarning("QtWizard::field: No such field '%s'", qPrintable(name));
    return QVariant();
}

/*!
    \property QtWizard::wizardStyle
    \brief the look and feel of the wizard

    By default, QtWizard uses the ClassicStyle on X11 and QWS
    (Qtopia Core), the ModernStyle on Windows, and the MacStyle on
    Mac OS X.

    \sa {Wizard Look and Feel}, options
*/
void QtWizard::setWizardStyle(WizardStyle style)
{
    if (style != wizStyle) {
        wizStyle = style;

        disableUpdates();
        updateButtonTexts();
        updateLayout();
        updateGeometry();
        enableUpdates();
    }
}

QtWizard::WizardStyle QtWizard::wizardStyle() const
{
    return wizStyle;
}

/*!
    Sets the given \a option to be enabled if \a on is true;
    otherwise, clears the given \a option.

    \sa options, testOption(), setWizardStyle()
*/
void QtWizard::setOption(WizardOption option, bool on)
{
    if (!(opts & option) != !on)
        setOptions(opts ^ option);
}

/*!
    Returns true if the given \a option is enabled; otherwise, returns
    false.

    \sa options, setOption(), setWizardStyle()
*/
bool QtWizard::testOption(WizardOption option) const
{
    return (opts & option) != 0;
}

/*!
    \property QtWizard::options
    \brief the various options that affect the look and feel of the wizard

    By default, the following options are set (depending on the platform):

    \list
    \o Windows: HelpButtonOnRight.
    \o Mac OS X: NoDefaultButton and NoCancelButton.
    \o X11 and QWS (Qtopia Core): none.
    \endlist

    \sa wizardStyle
*/
void QtWizard::setOptions(WizardOptions options)
{
    WizardOptions changed = (options ^ opts);
    if (!changed)
        return;

    disableUpdates();

    opts = options;
    if ((changed & IndependentPages) && !(opts & IndependentPages))
        cleanupPagesNotInHistory();
    if (changed & DisabledBackButtonOnLastPage)
        updateButtonsEnabledState();
    if (changed & (NoBackButtonOnStartPage | NoBackButtonOnLastPage | HaveNextButtonOnLastPage
                   | HaveFinishButtonOnEarlyPages))
        updateButtonsVisibleState();
    if (changed & NoDefaultButton)
        updateDefaultButton();
    if (changed & (HaveHelpButton | HelpButtonOnRight | NoCancelButton | CancelButtonOnLeft
                   | HaveCustomButton1 | HaveCustomButton2 | HaveCustomButton3))
        updateButtonLayout();
    if (changed & (IgnoreSubTitles | ExtendedWatermarkPixmap))
        updateLayout();

    enableUpdates();
}

QtWizard::WizardOptions QtWizard::options() const
{
    return opts;
}

/*!
    Sets the text on button \a which to be \a text.

    By default, the text on buttons depends on the wizardStyle. For
    example, on Mac OS X, the \gui Next button is called \gui
    Continue.

    To add extra buttons to the wizard (e.g., a \gui Print button),
    one way is to call setButtonText() with CustomButton1,
    CustomButton2, or CustomButton3 to set their text, and make the
    buttons visible using the HaveCustomButton1, HaveCustomButton2,
    and/or HaveCustomButton3 options.

    \sa setButton(), button(), setButtonLayout(), setOptions()
*/
void QtWizard::setButtonText(WizardButton which, const QString &text)
{
    if (!ensureButton(which))
        return;

    btns[which]->setText(text);
    buttonHasCustomText |= (1 << uint(which));
}

/*!
    Returns the text on button \a which.

    By default, the text on buttons depends on the wizardStyle. For
    example, on Mac OS X, the \gui Next button is called \gui
    Continue.

    \sa button(), setButton()
*/
QString QtWizard::buttonText(WizardButton which) const
{
    if (!ensureButton(which))
        return QString();
    return btns[which]->text();
}

/*!
    Sets the order in which buttons are displayed to \a layout, where
    \a layout is a list of \l{WizardButton}s.

    The default layout depends on the options (e.g., whether
    HelpButtonOnRight) that are set. You can call this function if
    you need more control over the buttons' layout than what \l
    options already provides.

    You can specify horizontal stretches in the layout using \l
    Stretch.

    Example:

    \code
        MyWizard::MyWizard(QWidget *parent)
            : QtWizard(parent)
        {
            ...
            QList<QtWizard::WizardButton> layout;
            layout << QtWizard::Stretch << QtWizard::BackButton << QtWizard::CloseButton
                   << QtWizard::NextButton << QtWizard::FinishButton;
            setButtonLayout(layout);
            ...
        }
    \endcode

    \sa setButton(), setButtonText(), setOptions()
*/
void QtWizard::setButtonLayout(const QList<WizardButton> &layout)
{
    for (int i = 0; i < layout.count(); ++i) {
        WizardButton button1 = layout.at(i);

        if (button1 == NoButton || button1 == Stretch)
            continue;
        if (!ensureButton(button1))
            return;

        for (int j = 0; j < i; ++j) {
            WizardButton button2 = layout.at(j);
            if (button2 == button1) {
                qWarning("QtWizard::setButtonLayout: Duplicate button in layout");
                return;
            }
        }
    }

    buttonsHaveCustomLayout = true;
    buttonsCustomLayout = layout;
    updateButtonLayout();
}

/*!
    Sets the button corresponding to role \a which to \a button.

    To add extra buttons to the wizard (e.g., a \gui Print button),
    one way is to call setButton() with CustomButton1 to
    CustomButton3, and make the buttons visible using the
    HaveCustomButton1 to HaveCustomButton3 options.

    \sa setButtonText(), setButtonLayout(), options
*/
void QtWizard::setButton(WizardButton which, QAbstractButton *button)
{
    if (uint(which) >= NButtons)
        return;

    delete btns[which];
    btns[which] = button;
    buttonHasCustomText |= (1 << uint(which));

    connectButton(which);

    updateButtonLayout();
}

/*!
    Returns the button corresponding to role \a which.

    \sa setButton(), setButtonText()
*/
QAbstractButton *QtWizard::button(WizardButton which) const
{
    if (!ensureButton(which))
        return 0;
    return btns[which];
}

/*!
    \property QtWizard::titleFormat
    \brief the text format used by page titles

    The default format is Qt::AutoText.

    \sa QtWizardPage::title, subTitleFormat
*/
void QtWizard::setTitleFormat(Qt::TextFormat format)
{
    titleFmt = format;
    updateLayout();
}

Qt::TextFormat QtWizard::titleFormat() const
{
    return titleFmt;
}

/*!
    \property QtWizard::subTitleFormat
    \brief the text format used by page subtitles

    The default format is Qt::AutoText.

    \sa QtWizardPage::title, titleFormat
*/
void QtWizard::setSubTitleFormat(Qt::TextFormat format)
{
    subTitleFmt = format;
    updateLayout();
}

Qt::TextFormat QtWizard::subTitleFormat() const
{
    return subTitleFmt;
}

/*!
    Sets the pixmap for role \a which to \a pixmap.

    The pixmaps are used by QtWizard when displaying a page. Which
    pixmaps are actually used depend on the \l{Wizard Look and
    Feel}{wizard style}.

    Pixmaps can also be set for a specific page using
    QtWizardPage::setPixmap().

    \sa QtWizardPage::setPixmap(), {Elements of a Wizard Page}
*/
void QtWizard::setPixmap(WizardPixmap which, const QPixmap &pixmap)
{
    Q_ASSERT(uint(which) < NPixmaps);
    defaultPixmaps[which] = pixmap;
    updatePixmap(which);
}

/*!
    Returns the pixmap set for role \a which.

    By default, the only pixmap that is set is the BackgroundPixmap on
    Mac OS X.

    \sa QtWizardPage::pixmap(), {Elements of a Wizard Page}
*/
QPixmap QtWizard::pixmap(WizardPixmap which) const
{
    Q_ASSERT(uint(which) < NPixmaps);
#ifdef Q_WS_MAC
    if (which == BackgroundPixmap && defaultPixmaps[BackgroundPixmap].isNull())
        defaultPixmaps[BackgroundPixmap] = findDefaultBackgroundPixmap();
#endif
    return defaultPixmaps[which];
}

/*!
    Sets the default property for \a className to be \a property,
    and the associated change signal to be \a changedSignal.

    The default property is used when an instance of \a className (or
    of one of its subclasses) is passed to
    QtWizardPage::registerField() and no property is specified.

    QtWizard knows the most common Qt widgets. For these (or their
    subclasses), you don't need to specify a \a property or a \a
    changedSignal. The table below lists these widgets:

    \table
    \header \o Widget          \o Property                            \o Change Notification Signal
    \row    \o QAbstractButton \o bool \l{QAbstractButton::}{checked} \o \l{QAbstractButton::}{toggled()}
    \row    \o QAbstractSlider \o int \l{QAbstractSlider::}{value}    \o \l{QAbstractSlider::}{valueChanged()}
    \row    \o QComboBox       \o int \l{QComboBox::}{currentIndex}   \o \l{QComboBox::}{currentIndexChanged()}
    \row    \o QDateTimeEdit   \o QDateTime \l{QDateTimeEdit::}{dateTime} \o \l{QDateTimeEdit::}{dateTimeChanged()}
    \row    \o QLineEdit       \o QString \l{QLineEdit::}{text}       \o \l{QLineEdit::}{textChanged()}
    \row    \o QListWidget     \o int \l{QListWidget::}{currentRow}   \o \l{QListWidget::}{currentRowChanged()}
    \row    \o QSpinBox        \o int \l{QSpinBox::}{value}           \o \l{QSpinBox::}{valueChanged()}
    \endtable

    \sa QtWizardPage::registerField()
*/
void QtWizard::setDefaultProperty(const char *className, const char *property,
                                  const char *changedSignal)
{
    defaultPropertyTable.append(QtWizardDefaultProperty(className, property, changedSignal));
}

/*!
    \reimp
*/
void QtWizard::setVisible(bool visible)
{
    if (visible) {
        if (current == -1)
            restart();
    }
    QDialog::setVisible(visible);
}

/*!
    \reimp
*/
QSize QtWizard::sizeHint() const
{
    QSize result = mainLayout->totalSizeHint();
    QSize extra(500, 360);
    if (wizStyle == MacStyle && current != -1) {
        QSize pixmap(currentPage()->pixmap(BackgroundPixmap).size());
        extra.setWidth(616);
        if (!pixmap.isNull()) {
            extra.setHeight(pixmap.height());

            /*
                The width isn't always reliable as a size hint, as
                some wizard backgrounds just cover the leftmost area.
                Use a rule of thumb to determine if the width is
                reliable or not.
            */
            if (pixmap.width() >= pixmap.height())
                extra.setWidth(pixmap.width());
        }
    }
    return result.expandedTo(extra);
}

/*!
    \fn void QtWizard::currentIdChanged(int id)

    This signal is emitted when the current page changes, with the new
    current \a id.

    \sa currentId(), currentPage()
*/

/*!
    \fn void QtWizard::helpRequested()

    This signal is emitted when the user clicks the \gui Help button.

    By default, no \gui Help button is shown. Call
    setOption(HaveHelpButton, true) to have one.

    Example:

    \quotefromfile licensewizard/licensewizard.cpp
    \skipto ::LicenseWizard
    \printuntil {
    \dots
    \skipto HaveHelpButton
    \printline HaveHelpButton
    \skipto helpRequested
    \printuntil ;
    \dots
    \skipto /^\}/
    \printline }
    \skipto ::showHelp
    \printuntil break;
    \dots
    \skipto default:
    \printuntil }
    \skipto QMessageBox::information
    \printuntil ;
    \skipto /^\}/
    \printline }

    \sa customButtonClicked()
*/

/*!
    \fn void QtWizard::customButtonClicked(int which)

    This signal is emitted when the user clicks a custom button. \a
    which can be CustomButton1, CustomButton2, or CustomButton3.

    By default, no custom button is shown. Call setOption() with
    HaveCustomButton1, HaveCustomButton2, or HaveCustomButton3 to have
    one, and use setButtonText() or setButton() to configure it.

    \sa helpRequested()
*/

/*!
    Goes back to the previous page.

    This is equivalent to pressing the \gui Back button.

    \sa next(), accept(), reject(), restart()
*/
void QtWizard::back()
{
    int n = history.count() - 2;
    if (n < 0)
        return;
    switchToPage(history.at(n), Backward);
}

/*!
    Advances to the next page.

    This is equivalent to pressing the \gui Next button.

    \sa nextId(), back(), accept(), reject(), restart()
*/
void QtWizard::next()
{
    if (current == -1)
        return;

    if (validateCurrentPage()) {
        int next = nextId();
        if (history.contains(next)) {
            qWarning("QtWizard::next: Page %d already met", next);
            return;
        }
        if (!pageMap.contains(next)) {
            qWarning("QtWizard::next: No such page %d", next);
            return;
        }
        switchToPage(next, Forward);
    }
}

/*!
    Restarts the wizard at the start page.

    \sa startId()
*/
void QtWizard::restart()
{
    reset();
    if (!pageMap.isEmpty())
        switchToPage(startId(), Forward);
}

/*!
    \reimp
*/
bool QtWizard::event(QEvent *event)
{
    if (event->type() == QEvent::StyleChange)
        updateLayout();
    return QDialog::event(event);
}

/*!
    \reimp
*/
void QtWizard::resizeEvent(QResizeEvent * /* event */)
{
    antiFlickerWidget->resize(size());
}

/*!
    \reimp
*/
void QtWizard::paintEvent(QPaintEvent * /* event */)
{
    if (wizStyle == MacStyle && currentPage()) {
        QPixmap backgroundPixmap = currentPage()->pixmap(BackgroundPixmap);
        if (backgroundPixmap.isNull())
            return;

        QPainter painter(this);
        painter.drawPixmap(0, (height() - backgroundPixmap.height()) / 2, backgroundPixmap);
    }
}

/*!
    \reimp
*/
void QtWizard::done(int result)
{
    // canceling leaves the wizard in a known state
    if (result == Rejected) {
        reset();
    } else {
        if (!validateCurrentPage())
            return;
    }
    QDialog::done(result);
}

/*!
    This virtual function is called by QtWizard to prepare page \a id
    just before it is shown. (However, if the \l
    QtWizard::IndependentPages option is set, this function is only
    called the first time the page is shown.)

    By reimplementing this function, you can ensure that the page's
    fields are properly initialized based on fields from previous
    pages.

    The default implementation calls QtWizardPage::initializePage() on
    page(\a id).

    \sa QtWizardPage::initializePage(), cleanupPage()
*/
void QtWizard::initializePage(int id)
{
    QtWizardPage *page = this->page(id);
    if (page)
        page->initializePage();
}

/*!
    This virtual function is called by QtWizard when the user clicks
    \gui Back (unless the QtWizard::IndependentPages option is set).

    The default implementation calls QtWizardPage::cleanupPage() on
    page(\a id).

    \sa QtWizardPage::cleanupPage(), initializePage()
*/
void QtWizard::cleanupPage(int id)
{
    QtWizardPage *page = this->page(id);
    if (page)
        page->cleanupPage();
}

/*!
    This virtual function is called by QtWizard when the user clicks
    \gui Next or \gui Finish to perform some last-minute validation.
    If it returns true, the next page is shown (or the wizard
    finishes); otherwise, the current page stays up.

    The default implementation calls QtWizardPage::validatePage() on
    the currentPage().

    When possible, it is usually better style to disable the \gui
    Next or \gui Finish button (by specifying \l{mandatory fields} or
    by reimplementing QtWizardPage::isComplete()) than to reimplement
    validateCurrentPage().

    \sa QtWizardPage::validatePage(), currentPage()
*/
bool QtWizard::validateCurrentPage()
{
    QtWizardPage *page = currentPage();
    if (!page)
        return true;

    return page->validatePage();
}

/*!
    This virtual function is called by QtWizard to find out which page
    to show when the user clicks the \gui Next button.

    The default implementation calls QtWizardPage::nextId() on the
    currentPage().

    \sa QtWizardPage::nextId(), currentPage()
*/
int QtWizard::nextId() const
{
    const QtWizardPage *page = currentPage();
    if (!page)
        return -1;

    return page->nextId();
}

void QtWizard::emitCustomButtonClicked()
{
    QObject *button = sender();
    for (int i = NStandardButtons; i < NButtons; ++i) {
        if (btns[i] == button) {
            emit customButtonClicked(WizardButton(i));
            break;
        }
    }
}

void QtWizard::updateButtonsEnabledState()
{
    const QtWizardPage *page = currentPage();
    bool complete = page && page->isComplete();
    btn.back->setEnabled(history.count() > 1
                         && (!canFinish || !(opts & DisabledBackButtonOnLastPage)));
    btn.next->setEnabled(canContinue && complete);
    btn.finish->setEnabled(canFinish && complete);
}

#ifdef Q_WS_MAC
QPixmap QtWizard::findDefaultBackgroundPixmap()
{
    // ### Simplify this code if this ever goes into Qt
    CFURLRef url;
    const uint ExpectedFileSize = 62932;
    const int ExpectedImageWidth = 242;
    const int ExpectedImageHeight = 414;
    if (LSFindApplicationForInfo(kLSUnknownCreator, CFSTR("com.apple.KeyboardSetupAssistant"),
                                 0, 0, &url) == noErr) {
        CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, url);
        CFRelease(url);
        if (bundle) {
            url = CFBundleCopyResourceURL(bundle, CFSTR("Background"), CFSTR("tif"), 0);
            if (url) {
                CFRelease(bundle);
                FSRef fsref;
                CFURLGetFSRef(url, &fsref);
                CFRelease(url);
                FSSpec fsspec;
                FSCatalogInfo catalog;
                memset(&catalog, 0, sizeof(FSCatalogInfo));
                FSGetCatalogInfo(&fsref, kFSCatInfoGettableInfo, &catalog, 0, &fsspec, 0);
                if (catalog.dataLogicalSize == ExpectedFileSize) {
                    GraphicsImportComponent importer;
                    GetGraphicsImporterForFile(&fsspec, &importer);
                    CGImageRef image = 0;
                    GraphicsImportCreateCGImage(importer, &image,
                                                kGraphicsImportCreateCGImageUsingCurrentSettings);
                    if (image) {
                        int width = CGImageGetWidth(image);
                        int height = CGImageGetHeight(image);
                        if (width == ExpectedImageWidth && height == ExpectedImageHeight) {
                            QPixmap pixmap(ExpectedImageWidth, ExpectedImageHeight);
                            pixmap.fill(Qt::transparent);
                            CGContextRef context = qt_mac_cg_context(&pixmap);
                            HIRect rect = CGRectMake(0., 0., width, height);
                            HIViewDrawCGImage(context, &rect, image);
                            CFRelease(context);
                            CFRelease(image);
                            return pixmap;
                        } else {
                            CFRelease(image);
                        }
                    }
                }
            }
        }
    }
    return QPixmap();

}
#endif

void QtWizard::reset()
{
    if (current != -1) {
        cleanupPagesNotInHistory();
        for (int i = history.count() - 1; i >= 0; --i)
            cleanupPage(history.at(i));
        history.clear();
        initialized.clear();

        current = -1;
        emit currentIdChanged(-1);
    }
}

void QtWizard::cleanupPagesNotInHistory()
{
    QSet<int> original = initialized;

    QSet<int>::const_iterator i = original.constBegin();
    QSet<int>::const_iterator end = original.constEnd();
    for (; i != end; ++i) {
        if (!history.contains(*i)) {
            cleanupPage(*i);
            initialized.remove(*i);
        }
    }
}

void QtWizard::addField(const QtWizardField &field)
{
    QtWizardField myField = field;
    myField.resolve(this);

    if (fieldIndexMap.contains(myField.name)) {
        qWarning("QtWizard::addField: Duplicate field '%s'", qPrintable(myField.name));
        return;
    }

    fieldIndexMap.insert(myField.name, fields.count());
    fields += myField;
    if (myField.mandatory && !myField.changedSignal.isEmpty())
        connect(myField.object, myField.changedSignal,
                myField.page, SLOT(maybeEmitCompleteChanged()));
}

void QtWizard::switchToPage(int newId, Direction direction)
{
    disableUpdates();

    int oldId = current;
    if (direction == Backward) {
        if (!(opts & IndependentPages)) {
            cleanupPage(oldId);
            initialized.remove(oldId);
        }
        Q_ASSERT(history.last() == oldId);
        history.removeLast();
        Q_ASSERT(history.last() == newId);
    }

    QtWizardPage *oldPage = currentPage();

    if (oldPage)
        oldPage->hide();
    current = newId;
    currentPageExplicitlyIncomplete = false;
    currentPageExplicitlyFinal = false;

    QtWizardPage *newPage = currentPage();

    if (direction == Forward) {
        if (!initialized.contains(current)) {
            initialized.insert(current);
            initializePage(current);
        }
        history.append(current);
    }

    newPage->show();

    canContinue = (nextId() != -1);
    canFinish = newPage->isFinal();

    updateButtonsVisibleState();
    updateButtonsEnabledState();
    updateDefaultButton();
    updateLayout();

    QAbstractButton *nextOrFinishButton = btns[canContinue ? NextButton : FinishButton];
    QWidget *candidate;

    /*
        If there is no default button and the Next or Finish button
        is enabled, give focus directly to it as a convenience to the
        user. This is the normal case on Mac OS X.

        Otherwise, give the focus to the new page's first child that
        can handle it. If there is no such child, give the focus to
        Next or Finish.
    */
    if ((opts & NoDefaultButton) && nextOrFinishButton->isEnabled()) {
        candidate = nextOrFinishButton;
    } else {
        candidate = iWantTheFocus(newPage);
        if (!candidate)
            candidate = nextOrFinishButton;
    }
    candidate->setFocus();

    if (wizStyle == MacStyle)
        updateGeometry();

    enableUpdates();

    emit currentIdChanged(current);
}

// keep in sync with WizardButton
static const char * const defaultButtonTexts[2][QtWizard::NStandardButtons] = {
    {
        QT_TR_NOOP("< &Back"),
        QT_TR_NOOP("&Next >"),
        QT_TR_NOOP("&Finish"),
        QT_TR_NOOP("Cancel"),
        QT_TR_NOOP("&Help")
    }, {
        QT_TR_NOOP("Go Back"),
        QT_TR_NOOP("Continue"),
        QT_TR_NOOP("Done"),
        QT_TR_NOOP("Quit"),
        QT_TR_NOOP("Help")
    }
};

// keep in sync with WizardButton
static const char * const buttonSlots[QtWizard::NStandardButtons] = {
    SLOT(back()), SLOT(next()), SLOT(accept()), SLOT(reject()), SIGNAL(helpRequested())
};

QtWizard::LayoutInfo QtWizard::layoutInfoForCurrentPage()
{
    QtWizardPage *page = currentPage();

    LayoutInfo info;

    info.topLevelMargin = style()->pixelMetric(QStyle::PM_DefaultTopLevelMargin);
    info.childMargin = style()->pixelMetric(QStyle::PM_DefaultChildMargin);
    info.spacing = style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    info.buttonSpacing = info.spacing;
#ifdef Q_WS_MAC
    if (qobject_cast<QMacStyle *>(style()))
        info.buttonSpacing = 0;
#endif

    info.wizStyle = wizStyle;

    QPixmap backgroundPixmap = page->pixmap(BackgroundPixmap);
    QPixmap watermarkPixmap = page->pixmap(WatermarkPixmap);
    QString titleText = page->title();
    QString subTitleText = page->subTitle();

    info.header = (info.wizStyle == ClassicStyle || info.wizStyle == ModernStyle)
                  && !(opts & IgnoreSubTitles) && !subTitleText.isEmpty();
    info.watermark = (info.wizStyle != MacStyle) && !watermarkPixmap.isNull();
    info.title = !info.header && !titleText.isEmpty();
    info.subTitle = !(opts & IgnoreSubTitles) && !info.header && !subTitleText.isEmpty();
    info.extension = info.watermark && (opts & ExtendedWatermarkPixmap);

    return info;
}

void QtWizard::recreateLayout(const LayoutInfo &info)
{
    /*
        Start by undoing the main layout.
    */
    for (int i = mainLayout->count() - 1; i >= 0; --i) {
        QLayoutItem *item = mainLayout->takeAt(i);
        if (item->layout()) {
            item->layout()->setParent(0);
        } else {
            delete item;
        }
    }
    for (int i = mainLayout->columnCount() - 1; i >= 0; --i)
        mainLayout->setColumnMinimumWidth(i, 0);
    for (int i = mainLayout->rowCount() - 1; i >= 0; --i)
        mainLayout->setRowMinimumHeight(i, 0);

    /*
        Now, recreate it.
    */

    bool mac = (info.wizStyle == MacStyle);
    bool classic = (info.wizStyle == ClassicStyle);
    bool modern = (info.wizStyle == ModernStyle);
    int deltaMargin = info.topLevelMargin - info.childMargin;
    int deltaSpacing = info.topLevelMargin - info.spacing;

    int row = 0;
    int numColumns;
    if (mac) {
        numColumns = 3;
    } else if (info.watermark) {
        numColumns = 2;
    } else {
        numColumns = 1;
    }
    int pageColumn = qMin(1, numColumns - 1);

    if (mac) {
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        buttonLayout->setMargin(0);
        pageVBoxLayout->setMargin(10);
    } else {
        if (modern) {
            mainLayout->setMargin(0);
            mainLayout->setSpacing(0);
            pageVBoxLayout->setMargin(deltaMargin);
            buttonLayout->setMargin(info.topLevelMargin);
        } else {
            mainLayout->setMargin(info.topLevelMargin);
            mainLayout->setSpacing(info.spacing);
            pageVBoxLayout->setMargin(0);
            buttonLayout->setMargin(0);
        }
    }
    buttonLayout->setSpacing(info.buttonSpacing);
    changeSpacerSize(buttonLayout, 0, mac ? MacButtonHMargin : 0, 0);
    changeSpacerSize(buttonLayout, buttonLayout->count() - 1, mac ? MacButtonHMargin : 0, 0);

    if (info.header) {
        if (!headerWidget)
            headerWidget = new QtWizardHeader(antiFlickerWidget);
        headerWidget->setAutoFillBackground(modern);
        mainLayout->addWidget(headerWidget, row++, 0, 1, numColumns);
    }
    if (headerWidget)
        headerWidget->setVisible(info.header);

    int watermarkStartRow = row;

    if (mac)
        mainLayout->setRowMinimumHeight(row++, info.topLevelMargin + 1);

    if (info.title) {
        if (!titleLabel) {
            titleLabel = new QLabel(antiFlickerWidget);
            titleLabel->setBackgroundRole(QPalette::Base);
            titleLabel->setWordWrap(true);
        }

        QFont titleFont = font();
        titleFont.setPointSize(titleFont.pointSize() + (mac ? 1 : 4));
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setIndent(mac ? 2 : classic ? info.childMargin : info.topLevelMargin);
        if (modern) {
            if (!placeholderWidget1) {
                placeholderWidget1 = new QWidget(antiFlickerWidget);
                placeholderWidget1->setBackgroundRole(QPalette::Base);
            }
            placeholderWidget1->setFixedHeight(info.topLevelMargin + 2);
            mainLayout->addWidget(placeholderWidget1, row++, pageColumn);
        }
        mainLayout->addWidget(titleLabel, row++, pageColumn);
        if (modern) {
            if (!placeholderWidget2) {
                placeholderWidget2 = new QWidget(antiFlickerWidget);
                placeholderWidget2->setBackgroundRole(QPalette::Base);
            }
            placeholderWidget2->setFixedHeight(5);
            mainLayout->addWidget(placeholderWidget2, row++, pageColumn);
        }
        if (mac)
            mainLayout->setRowMinimumHeight(row++, info.topLevelMargin - 4);
    }
    if (placeholderWidget1)
        placeholderWidget1->setVisible(info.title && modern);
    if (placeholderWidget2)
        placeholderWidget2->setVisible(info.title && modern);

    if (info.subTitle) {
        if (!subTitleLabel) {
            subTitleLabel = new QLabel(pageFrame);
            subTitleLabel->setWordWrap(true);

            // ideally, the same margin should be used on the right side as well
            subTitleLabel->setIndent(info.childMargin);

            pageVBoxLayout->insertWidget(1, subTitleLabel);
        }
    }

    changeSpacerSize(pageVBoxLayout, 0, 0, info.subTitle ? info.childMargin : 0);

    int hMargin = mac ? 1 : 0;
    int vMargin = hMargin;

    pageFrame->setFrameStyle(mac ? (QFrame::Box | QFrame::Raised) : QFrame::NoFrame);
    pageFrame->setLineWidth(0);
    pageFrame->setMidLineWidth(hMargin);

    if (info.header) {
        if (modern) {
            hMargin = info.topLevelMargin;
            vMargin = deltaMargin;
        } else if (classic) {
            hMargin = deltaMargin + ClassicHMargin;
            vMargin = 0;
        }
    }

    pageFrame->setContentsMargins(hMargin, vMargin, hMargin, vMargin);

    if (info.watermark && !watermarkLabel) {
        watermarkLabel = new QLabel(antiFlickerWidget);
        watermarkLabel->setBackgroundRole(QPalette::Base);
        watermarkLabel->setMinimumHeight(1);
        watermarkLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        watermarkLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    }

    bool wasSemiTransparent = pageFrame->testAttribute(Qt::WA_SetPalette);
    if (mac) {
        if (!wasSemiTransparent) {
            QPalette pal = pageFrame->palette();
            pal.setBrush(QPalette::Window, QColor(255, 255, 255, 153));
            pageFrame->setPalette(pal);
            pageFrame->setAutoFillBackground(true);
        }
    } else {
        if (wasSemiTransparent)
            pageFrame->setPalette(QPalette());

        bool baseBackground = (modern && !info.header);
        pageFrame->setBackgroundRole(baseBackground ? QPalette::Base : QPalette::Window);
        if (titleLabel)
            titleLabel->setAutoFillBackground(baseBackground);
        pageFrame->setAutoFillBackground(baseBackground);
        if (watermarkLabel)
            watermarkLabel->setAutoFillBackground(baseBackground);
        if (placeholderWidget1)
            placeholderWidget1->setAutoFillBackground(baseBackground);
        if (placeholderWidget2)
            placeholderWidget2->setAutoFillBackground(baseBackground);
    }

    mainLayout->addWidget(pageFrame, row++, pageColumn);

    int watermarkEndRow = row;
    if (classic)
        mainLayout->setRowMinimumHeight(row++, deltaSpacing);

    if (mac)
        mainLayout->setRowMinimumHeight(row, 2 + 2 * deltaSpacing);

    int buttonStartColumn = info.extension ? 1 : 0;
    int buttonNumColumns = info.extension ? 1 : numColumns;

    if (classic || modern) {
        if (!bottomRuler)
            bottomRuler = new QtWizardRuler(antiFlickerWidget);
        mainLayout->addWidget(bottomRuler, row++, buttonStartColumn, 1, buttonNumColumns);
    }

    if (classic)
        mainLayout->setRowMinimumHeight(row++, deltaSpacing);

    if (mac)
        mainLayout->setRowMinimumHeight(row++, info.childMargin);
    mainLayout->addLayout(buttonLayout, row++, buttonStartColumn, 1, buttonNumColumns);
    if (mac)
        mainLayout->setRowMinimumHeight(row++, info.childMargin);

    if (info.watermark) {
        if (info.extension)
            watermarkEndRow = row;
        mainLayout->addWidget(watermarkLabel, watermarkStartRow, 0,
                              watermarkEndRow - watermarkStartRow, 1);
    }

    mainLayout->setColumnMinimumWidth(0, mac && !info.watermark ? 181 : 0);
    if (mac)
        mainLayout->setColumnMinimumWidth(2, 21);

    if (headerWidget)
        headerWidget->setVisible(info.header);
    if (titleLabel)
        titleLabel->setVisible(info.title);
    if (subTitleLabel)
        subTitleLabel->setVisible(info.subTitle);
    if (bottomRuler)
        bottomRuler->setVisible(classic || modern);
    if (watermarkLabel)
        watermarkLabel->setVisible(info.watermark);

    layoutInfo = info;
}

void QtWizard::updateLayout()
{
    QtWizardPage *page = currentPage();
    if (!page)
        return;

    disableUpdates();

    LayoutInfo info = layoutInfoForCurrentPage();
    if (layoutInfo != info)
        recreateLayout(info);

    QString titleText = page->title();
    QString subTitleText = page->subTitle();
    if (info.header)
        headerWidget->setup(info, titleText, subTitleText, page->pixmap(LogoPixmap),
                            page->pixmap(BannerPixmap), titleFmt, subTitleFmt);
    if (info.watermark)
        watermarkLabel->setPixmap(page->pixmap(WatermarkPixmap));
    if (info.title) {
        titleLabel->setTextFormat(titleFmt);
        titleLabel->setText(titleText);
    }
    if (info.subTitle) {
        subTitleLabel->setTextFormat(subTitleFmt);
        subTitleLabel->setText(subTitleText);
    }

    setMinimumSize(mainLayout->totalMinimumSize());
    setMaximumSize(mainLayout->totalMaximumSize());
    if (info.header && headerWidget->maximumWidth() != QWIDGETSIZE_MAX)
        setFixedWidth(headerWidget->maximumWidth());
    if (info.watermark)
        setFixedHeight(mainLayout->totalSizeHint().height());

    enableUpdates();
}

bool QtWizard::ensureButton(WizardButton which) const
{
    if (uint(which) >= NButtons)
        return false;

    if (!btns[which]) {
        QPushButton *pushButton = new QPushButton(antiFlickerWidget);
#ifdef Q_WS_MAC
        pushButton->setAutoDefault(false);
#endif
        pushButton->hide();
        btns[which] = pushButton;
        if (which < NStandardButtons)
            pushButton->setText(tr(defaultButtonTexts[wizStyle == MacStyle][which]));
        connectButton(which);
    }
    return true;
}

void QtWizard::connectButton(WizardButton which) const
{
    if (which < NStandardButtons) {
        connect(btns[which], SIGNAL(clicked()), this, buttonSlots[which]);
    } else {
        connect(btns[which], SIGNAL(clicked()), this, SLOT(emitCustomButtonClicked()));
    }
}

void QtWizard::updateButtonsVisibleState()
{
    btn.back->setVisible(buttonLayoutContains(BackButton)
                         && (history.count() > 1 || !(opts & NoBackButtonOnStartPage))
                         && (canContinue || !(opts & NoBackButtonOnLastPage)));
    btn.next->setVisible(buttonLayoutContains(NextButton)
                         && (canContinue || (opts & HaveNextButtonOnLastPage)));
    btn.finish->setVisible(buttonLayoutContains(FinishButton)
                           && (canFinish || (opts & HaveFinishButtonOnEarlyPages)));
}

void QtWizard::updateDefaultButton()
{
    bool useDefault = !(opts & NoDefaultButton);
    if (QPushButton *nextPush = qobject_cast<QPushButton *>(btn.next))
        nextPush->setDefault(canContinue && useDefault);
    if (QPushButton *finishPush = qobject_cast<QPushButton *>(btn.finish))
        finishPush->setDefault(!canContinue && useDefault);
}

void QtWizard::updateButtonTexts()
{
    for (int i = 0; i < NStandardButtons; ++i) {
        if (btns[i] && !(buttonHasCustomText & (1 << i)))
            btns[i]->setText(tr(defaultButtonTexts[wizStyle == MacStyle][i]));
    }
}

void QtWizard::updateButtonLayout()
{
    if (buttonsHaveCustomLayout) {
        QVarLengthArray<WizardButton> array(buttonsCustomLayout.count());
        for (int i = 0; i < buttonsCustomLayout.count(); ++i)
            array[i] = buttonsCustomLayout.at(i);
        setButtonLayout(array.constData(), array.count());
    } else {
        // Positions: Help Stretch Custom1 Custom2 Custom3 Cancel Back Next Finish Cancel Help

        const int ArraySize = 11;
        WizardButton array[ArraySize];
        memset(array, -1, sizeof(array));
        Q_ASSERT(array[0] == NoButton);

        if (opts & HaveHelpButton) {
            int i = (opts & HelpButtonOnRight) ? 10 : 0;
            array[i] = HelpButton;
        }
        array[1] = Stretch;
        if (opts & HaveCustomButton1)
            array[2] = CustomButton1;
        if (opts & HaveCustomButton2)
            array[3] = CustomButton2;
        if (opts & HaveCustomButton3)
            array[4] = CustomButton3;

        if (!(opts & NoCancelButton)) {
            int i = (opts & CancelButtonOnLeft) ? 5 : 9;
            array[i] = CancelButton;
        }
        array[6] = BackButton;
        array[7] = NextButton;
        array[8] = FinishButton;

        setButtonLayout(array, ArraySize);
    }
}

void QtWizard::setButtonLayout(const WizardButton *array, int size)
{
    QWidget *prev = pageFrame;

    // we deliberately skip the first and the last item, which are spacers
    for (int i = buttonLayout->count() - 2; i >= 1; --i) {
        QLayoutItem *item = buttonLayout->takeAt(i);
        if (QWidget *widget = item->widget())
            widget->hide();
        delete item;
    }

    int pos = 1;

    for (int i = 0; i < size; ++i) {
        WizardButton which = array[i];
        if (which == Stretch) {
            buttonLayout->insertStretch(pos++, 1);
        } else if (which != NoButton) {
            ensureButton(which);
            buttonLayout->insertWidget(pos++, btns[which]);

            // Back, Next, and Finish are handled in updateButtonsVisibleState()
            if (which != BackButton && which != NextButton && which != FinishButton)
                btns[which]->show();

            if (prev)
                setTabOrder(prev, btns[which]);
            prev = btns[which];
        }
    }

    updateButtonsVisibleState();
}

bool QtWizard::buttonLayoutContains(WizardButton which)
{
    return !buttonsHaveCustomLayout || buttonsCustomLayout.contains(which);
}

void QtWizard::updatePixmap(WizardPixmap which)
{
    if (which == BackgroundPixmap) {
        if (wizStyle == MacStyle) {
            update();
            updateGeometry();
        }
    } else {
        updateLayout();
    }
}

void QtWizard::disableUpdates()
{
    if (disableUpdatesCount++ == 0) {
        QDialog::setUpdatesEnabled(false);
        antiFlickerWidget->hide();
    }
}

void QtWizard::enableUpdates()
{
    if (--disableUpdatesCount == 0) {
        antiFlickerWidget->show();
        QDialog::setUpdatesEnabled(true);
    }
}

/*!
    \class QtWizardPage
    \brief The QtWizardPage class is the base class for wizard pages.

    QtWizard represents a wizard. Each page is a QtWizardPage. When
    you create your own wizards, you can use QtWizardPage directly,
    or you can subclass it for more control.

    A page has the following attributes, which are rendered by
    QtWizard: a \l title, a \l subTitle, and a \l{setPixmap()}{set of
    pixmaps}. See \l{Elements of a Wizard Page} for details. Once a
    page is added to the wizard (using QtWizard::addPage() or
    QtWizard::setPage()), wizard() returns a pointer to the
    associated QtWizard object.

    Page provides five virtual functions that can be reimplemented to
    provide custom behavior:

    \list
    \o initializePage() is called to initialize the page's contents
       when the user clicks the wizard's \gui Next button. If you
       want to derive the page's default from what the user entered
       on previous pages, this is the function to reimplement.
    \o cleanupPage() is called to reset the page's contents when the
       user clicks the wizard's \gui Back button.
    \o validatePage() validates the page when the user clicks \gui
       Next or \gui Finish. It is often used to show an error message
       if the user has entered incomplete or invalid information.
    \o nextId() returns the ID of the next page. It is useful when
       \l{creating non-linear wizards}, which allow different
       traversal paths based on the information provided by the user.
    \o isComplete() is called to determine whether the \gui Next
       and/or \gui Finish button should be enabled or disabled. If
       you reimplement isComplete(), also make sure that
       completeChanged() is emitted whenever the complete state
       changes.
    \endlist

    Normally, the \gui Next button and the \gui Finish button of a
    wizard are mutually exclusive. If isFinal() returns true, \gui
    Finish is available; otherwise, \gui Next is available. By
    default, isFinal() is true only when nextId() returns -1. If
    you want to show \gui Next and \gui Final simultaneously for a
    page (letting the user perform an "early finish"), call
    setFinal(true) on that page. For wizards that support early
    finishes, you might also want to set the
    \l{QtWizard::}{HaveNextButtonOnLastPage} and
    \l{QtWizard::}{HaveFinishButtonOnEarlyPages} options on the
    wizard.

    In many wizards, the contents of a page may affect the default
    values of the fields of a later page. To make it easy to
    communicate between pages, QtWizard supports a \l{Registering and
    Using Fields}{"field" mechanism} that allows you to register a
    field (e.g., a QLineEdit) on a page and to access its value from
    any page. Fields are global to the entire wizard and make it easy
    for any single page to access information stored by another page,
    without having to put all the logic in QtWizard or having the
    pages know explicitly about each other. Fields are registered
    using registerField() and can be accessed at any time using
    field() and setField().

    \sa QtWizard, {Class Wizard Example}, {License Wizard Example}
*/

/*!
    Constructs a wizard page with the given \a parent.

    When the page is inserted into a wizard using QtWizard::addPage()
    or QtWizard::setPage(), the parent is automatically set to be the
    wizard.

    \sa wizard()
*/
QtWizardPage::QtWizardPage(QWidget *parent)
    : QWidget(parent), myWizard(0), completeState(Tri_Unknown), explicitlyFinal(false)
{
    connect(this, SIGNAL(completeChanged()), this, SLOT(updateCachedCompleteState()));
}

/*!
    \property QtWizardPage::title
    \brief the title of the page

    The title is shown by the QtWizard, above the actual page. All
    pages should have a title.

    The title may be plain text or HTML, depending on the value of the
    \l{QtWizard::titleFormat} property.

    \sa subTitle, {Elements of a Wizard Page}
*/
void QtWizardPage::setTitle(const QString &title)
{
    myTitle = title;
    if (myWizard && myWizard->currentPage() == this)
        myWizard->updateLayout();
}

QString QtWizardPage::title() const
{
    return myTitle;
}

/*!
    \property QtWizardPage::subTitle
    \brief the subtitle of the page

    The subtitle is shown by the QtWizard, between the title and the
    actual page. Subtitles are optional. In
    \l{QtWizard::ClassicStyle}{ClassicStyle} and
    \l{QtWizard::ModernStyle}{ModernStyle}, using subtitles is
    necessary to make the header appear. In
    \l{QtWizard::MacStyle}{MacStyle}, the subtitle is shown as a text
    label just above the actual page.

    The subtitle may be plain text or HTML, depending on the value of
    the \l{QtWizard::subTitleFormat} property.

    \sa title, QtWizard::IgnoreSubTitles, {Elements of a Wizard Page}
*/
void QtWizardPage::setSubTitle(const QString &subTitle)
{
    mySubTitle = subTitle;
    if (myWizard && myWizard->currentPage() == this)
        myWizard->updateLayout();
}

QString QtWizardPage::subTitle() const
{
    return mySubTitle;
}

/*!
    Sets the pixmap for role \a which to \a pixmap.

    The pixmaps are used by QtWizard when displaying a page. Which
    pixmaps are actually used depend on the \l{Wizard Look and
    Feel}{wizard style}.

    Pixmaps can also be set for the entire wizard using
    QtWizard::setPixmap(), in which case they apply for all pages that
    don't specify a pixmap.

    \sa QtWizard::setPixmap(), {Elements of a Wizard Page}
*/
void QtWizardPage::setPixmap(QtWizard::WizardPixmap which, const QPixmap &pixmap)
{
    Q_ASSERT(uint(which) < QtWizard::NPixmaps);
    myPixmaps[which] = pixmap;

    if (myWizard && myWizard->currentPage() == this)
        myWizard->updatePixmap(which);
}

/*!
    Returns the pixmap set for role \a which.

    Pixmaps can also be set for the entire wizard using
    QtWizard::setPixmap(), in which case they apply for all pages that
    don't specify a pixmap.

    \sa QtWizard::pixmap(), {Elements of a Wizard Page}
*/
QPixmap QtWizardPage::pixmap(QtWizard::WizardPixmap which) const
{
    Q_ASSERT(uint(which) < QtWizard::NPixmaps);

    const QPixmap &pixmap = myPixmaps[which];
    if (!pixmap.isNull())
        return pixmap;

    if (wizard())
        return wizard()->pixmap(which);

    return pixmap;
}

/*!
    This virtual function is called by QtWizard::initializePage() to
    prepare the page just before it is shown. (However, if the \l
    QtWizard::IndependentPages option is set, this function is only
    called the first time the page is shown.)

    By reimplementing this function, you can ensure that the page's
    fields are properly initialized based on fields from previous
    pages. For example:

    \quotefromfile classwizard/classwizard.cpp
    \skipto OutputFilesPage::initializePage
    \printuntil }

    The default implementation does nothing.

    \sa QtWizard::initializePage(), cleanupPage(), QtWizard::IndependentPages
*/
void QtWizardPage::initializePage()
{
}

/*!
    This virtual function is called by QtWizard::cleanupPage() when
    the user clicks \gui Back (unless the QtWizard::IndependentPages
    option is set).

    The default implementation resets the page's fields to their
    original values (the values they had before initializePage() was
    called).

    \sa QtWizard::cleanupPage(), initializePage(), QtWizard::IndependentPages
*/
void QtWizardPage::cleanupPage()
{
    if (myWizard) {
        QVector<QtWizardField> &fields = myWizard->fields;
        for (int i = 0; i < fields.count(); ++i) {
            const QtWizardField &field = fields.at(i);
            if (field.page == this)
                field.metaProperty.write(field.object, field.initialValue);
        }
    }
}

/*!
    This virtual function is called by QtWizard::validateCurrentPage()
    when the user clicks \gui Next or \gui Finish to perform some
    last-minute validation. If it returns true, the next page is shown
    (or the wizard finishes); otherwise, the current page stays up.

    The default implementation returns true.

    When possible, it is usually better style to disable the \gui
    Next or \gui Finish button (by specifying \l{mandatory fields} or
    reimplementing isComplete()) than to reimplement validatePage().

    \sa QtWizard::validateCurrentPage(), isComplete()
*/
bool QtWizardPage::validatePage()
{
    return true;
}

/*!
    This virtual function is called by QtWizard to determine whether
    the \gui Next or \gui Finish button should be enabled or
    disabled.

    The default implementation returns true if all \l{mandatory
    fields} are filled; otherwise, it returns false.

    If you reimplement this function, make sure to emit
    completeChanged() whenever the value of isComplete() changes, to
    ensure that QtWizard updates the enabled or disabled state of its
    buttons.

    \sa completeChanged(), isFinal()
*/
bool QtWizardPage::isComplete() const
{
    if (!myWizard)
        return true;

    for (int i = myWizard->fields.count() - 1; i >= 0; --i) {
        const QtWizardField &field = myWizard->fields.at(i);
        if (field.page == this && field.mandatory) {
            QVariant value = field.metaProperty.read(field.object);
            if (value == field.initialValue)
                return false;

            if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(field.object)) {
                if (!lineEdit->hasAcceptableInput())
                    return false;
            }
        }
    }
    return true;
}

/*!
    Explicitly sets this page to be final if \a final is true.

    After calling setFinal(true), isFinal() returns true and the \gui
    Finish button is visible (and enabled if isComplete() returns
    true).

    After calling setFinal(false), isFinal() returns true if
    nextId() returns -1; otherwise, it returns false.

    \sa isComplete(), QtWizard::HaveFinishButtonOnEarlyPages
*/
void QtWizardPage::setFinal(bool final)
{
    explicitlyFinal = final;
}

/*!
    This function is called by QtWizard to determine whether the \gui
    Finish button should be shown for this page or not.

    By default, it returns true if there is no next page
    (i.e., nextId() returns -1); otherwise, it returns false.

    By explicitly calling setFinal(true), you can let the user perform an
    "early finish".

    \sa isComplete(), QtWizard::HaveFinishButtonOnEarlyPages
*/
bool QtWizardPage::isFinal() const
{
    return explicitlyFinal || nextId() == -1;
}

/*!
    This virtual function is called by QtWizard::nextId() to find
    out which page to show when the user clicks the \gui Next button.

    By default, this function returns the page with the following ID
    in the QtWizard, or -1 if there is no such page.

    By reimplementing this function, you can specify a dynamic page
    order. For example:

    \quotefromfile licensewizard/licensewizard.cpp
    \skipto IntroPage::nextId
    \printuntil /^\}/

    \sa QtWizard::nextId()
*/
int QtWizardPage::nextId() const
{
    if (!myWizard)
        return -1;

    bool foundCurrentPage = false;

    QtWizard::PageMap::const_iterator i = myWizard->pageMap.constBegin();
    QtWizard::PageMap::const_iterator end = myWizard->pageMap.constEnd();
    for (; i != end; ++i) {
        if (i.value() == this) {
            foundCurrentPage = true;
        } else if (foundCurrentPage) {
            return i.key();
        }
    }
    return -1;
}

/*!
    \fn void QtWizardPage::completeChanged()

    This signal is emitted whenever the complete state of the page
    (i.e., the value of isComplete()) changes.

    If you reimplement isComplete(), make sure to emit
    completeChanged() whenever the value of isComplete() changes, to
    ensure that QtWizard updates the enabled or disabled state of its
    buttons.

    \sa isComplete()
*/

/*!
    Sets the value of the field called \a name to \a value.

    This function can be used to set fields on any page of the wizard.
    It is equivalent to calling
    wizard()->\l{QtWizard::setField()}{setField(\a name, \a value)}.

    \sa QtWizard::setField(), field(), registerField()
*/
void QtWizardPage::setField(const QString &name, const QVariant &value)
{
    if (!myWizard)
        return;
    myWizard->setField(name, value);
}

/*!
    Returns the value of the field called \a name.

    This function can be used to access fields on any page of the
    wizard. It is equivalent to calling
    wizard()->\l{QtWizard::field()}{field(\a name)}.

    Example:

    \quotefromfile classwizard/classwizard.cpp
    \skipto OutputFilesPage::initializePage
    \printuntil }

    \sa QtWizard::field(), setField(), registerField()
*/
QVariant QtWizardPage::field(const QString &name) const
{
    if (!myWizard)
        return QVariant();
    return myWizard->field(name);
}

/*!
    Creates a field called \a name associated with the given \a
    property of the given \a widget. From then on, that property
    becomes accessible using field() and setField().

    Fields are global to the entire wizard and make it easy for any
    single page to access information stored by another page, without
    having to put all the logic in QtWizard or having the pages know
    explicitly about each other.

    If \a name ends with an asterisk (\c *), the field is a mandatory
    field. When a page has mandatory fields, the \gui Next and/or
    \gui Finish buttons are enabled only when all mandatory fields
    are filled. This requires a \a changedSignal to be specified, to
    tell QtWizard to recheck the value stored by the mandatory field.

    QtWizard knows the most common Qt widgets. For these (or their
    subclasses), you don't need to specify a \a property or a \a
    changedSignal. The table below lists these widgets:

    \table
    \header \o Widget          \o Property                            \o Change Notification Signal
    \row    \o QAbstractButton \o bool \l{QAbstractButton::}{checked} \o \l{QAbstractButton::}{toggled()}
    \row    \o QAbstractSlider \o int \l{QAbstractSlider::}{value}    \o \l{QAbstractSlider::}{valueChanged()}
    \row    \o QComboBox       \o int \l{QComboBox::}{currentIndex}   \o \l{QComboBox::}{currentIndexChanged()}
    \row    \o QDateTimeEdit   \o QDateTime \l{QDateTimeEdit::}{dateTime} \o \l{QDateTimeEdit::}{dateTimeChanged()}
    \row    \o QLineEdit       \o QString \l{QLineEdit::}{text}       \o \l{QLineEdit::}{textChanged()}
    \row    \o QListWidget     \o int \l{QListWidget::}{currentRow}   \o \l{QListWidget::}{currentRowChanged()}
    \row    \o QSpinBox        \o int \l{QSpinBox::}{value}           \o \l{QSpinBox::}{valueChanged()}
    \endtable

    You can use QtWizard::setDefaultProperty() to add entries to this
    table or to override existing entries.

    To consider a field "filled", QtWizard simply checks that their
    current value doesn't equal their original value (the value they
    had before initializePage() was called). For QLineEdit, it also
    checks that
    \l{QLineEdit::hasAcceptableInput()}{hasAcceptableInput()} returns
    true, to honor any validator or mask.

    QtWizard's mandatory field mechanism is provided for convenience.
    It can be bypassed by reimplementing QtWizardPage::isComplete().

    \sa field(), setField(), QtWizard::setDefaultProperty()
*/
void QtWizardPage::registerField(const QString &name, QWidget *widget, const char *property,
                                 const char *changedSignal)
{
    QtWizardField field(this, name, widget, property, changedSignal);
    if (myWizard) {
        myWizard->addField(field);
    } else {
        pendingFields += field;
    }
}

/*!
    Returns the wizard associated with this page, or 0 if this page
    hasn't been inserted into a QtWizard yet.

    \sa QtWizard::addPage(), QtWizard::setPage()
*/
QtWizard *QtWizardPage::wizard() const
{
    return myWizard;
}

void QtWizardPage::maybeEmitCompleteChanged()
{
    TriState newState = isComplete() ? Tri_True : Tri_False;
    if (newState != completeState)
        emit completeChanged();
}

void QtWizardPage::updateCachedCompleteState()
{
    completeState = isComplete() ? Tri_True : Tri_False;
}

bool QtWizardPage::cachedIsComplete() const
{
    if (completeState == Tri_Unknown)
        completeState = isComplete() ? Tri_True : Tri_False;
    return completeState == Tri_True;
}
