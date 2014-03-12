
#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter(QTextEdit *parent, QStringList tablesList)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    
    keywordFormat.setForeground(Qt::red);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
	keywordPatterns << "\\babs\\b"<<"\\babsolute\\b"<<"\\baccess\\b"<<"\\bacos\\b"<<"\\badd\\b"<<"\\badd_months\\b"
	<<"\\badddate\\b"<<"\\badmin\\b"<<"\\bafter\\b"<<"\\baggregate\\b"<<"\\ball\\b"<<"\\ballocate\\b"<<"\\balter\\b"<<"\\band\\b"
	<<"\\bany\\b"<<"\\bapp_name\\b"<<"\\bare\\b"<<"\\barray\\b"<<"\\bas\\b"<<"\\basc\\b"<<"\\bascii\\b"<<"\\basin\\b"<<"\\bassertion\\b"
	<<"\\bat\\b"<<"\\batan\\b"<<"\\batn2\\b"<<"\\baudit\\b"<<"\\bauthid\\b"<<"\\bauthorization\\b"<<"\\bautonomous_transaction\\b"<<"\\bavg\\b"
	<<"\\bbefore\\b"<<"\\bbegin\\b"<<"\\bbenchmark\\b"<<"\\bbetween\\b"<<"\\bbfilename\\b"<<"\\bbin\\b"<<"\\bbinary\\b"<<"\\bbinary_checksum\\b"
	<<"\\bbinary_integer\\b"<<"\\bbit\\b"<<"\\bbit_count bit_and\\b"<<"\\bbit_or blob\\b"<<"\\bbody\\b"<<"\\bboolean\\b"<<"\\bboth\\b"<<"\\bbreadth\\b"
	<<"\\bbulk\\b"<<"\\bby\\b"<<"\\bcall\\b"<<"\\bcascade\\b"<<"\\bcascaded\\b"<<"\\bcase\\b"<<"\\bcast\\b"<<"\\bcatalog\\b"<<"\\bceil\\b"<<"\\bceiling\\b"
	<<"\\bchar\\b"<<"\\bchar_base\\b"<<"\\bcharacter\\b"<<"\\bcharindex\\b"<<"\\bcheck\\b"<<"\\bchecksum\\b"<<"\\bclose\\b"<<"\\bcluster\\b"
	<<"\\bcol_length\\b"<<"\\bcol_name\\b"<<"\\bcollate\\b"<<"\\bcollation\\b"<<"\\bcolumn\\b"<<"\\bcommit\\b"<<"\\bcompletion\\b"
	<<"\\bcompress\\b"<<"\\bconcat\\b"<<"\\bconnect\\b"<<"\\bcontains\\b"<<"\\bcontinue\\b"<<"\\bcos\\b"<<"\\bcot\\b"<<"\\bcount\\b"
	<<"\\bcreate\\b"<<"\\bcross\\b"<<"\\bcube\\b"<<"\\bcurrent\\b"<<"\\bcursor\\b"<<"\\bcycle\\b"<<"\\bdata\\b"<<"\\bdatalength\\b"
	<<"\\bdate\\b"<<"\\bdeclare\\b"<<"\\bdecimal\\b"<<"\\bdefault\\b"<<"\\bdelete\\b"<<"\\bdescribe\\b"<<"\\bdescriptor\\b"<<"\\bdestroy\\b"
	<<"\\bdestructor\\b"<<"\\bdistinct\\b"<<"\\bdo\\b"<<"\\bdomain\\b"<<"\\bdouble\\b"<<"\\bdrop\\b"<<"\\bdump\\b"<<"\\bdynamic\\b"
	<<"\\beach\\b"<<"\\belse\\b"<<"\\belsif\\b"<<"\\bend\\b"<<"\\bequals\\b"<<"\\bescape\\b"<<"\\bexists\\b"<<"\\bfetch\\b"<<"\\bfloor\\b"
	<<"\\bfor\\b"<<"\\bforeign\\b"<<"\\bformat\\b"<<"\\bfrom\\b"<<"\\bget\\b"<<"\\bgrant\\b"<<"\\bgroup\\b"<<"\\bgrouping\\b"<<"\\ having\\b"
	<<"\\bheap\\b"<<"\\bimmediate\\b"<<"\\bin\\b"<<"\\bincrement\\b"<<"\\bindex\\b"<<"\\bindex_col\\b"<<"\\binner\\b"<<"\\binout\\b"
	<<"\\binput\\b"<<"\\binsert\\b"<<"\\binstr\\b"<<"\\binstrb\\b"<<"\\binteger\\b"<<"\\binterface\\b"<<"\\bintersect\\b"<<"\\binterval\\b"<<"\\binto\\b"
	<<"\\bis\\b"<<"\\bjoin\\b"<<"\\bkey\\b"<<"\\blarge\\b"<<"\\blead\\b"<<"\\bleft\\b"<<"\\blen\\b"<<"\\blength\\b"<<"\\bless\\b"<<"\\blevel\\b"
	<<"\\blike\\b"<<"\\blimit\\b"<<"\\blog\\b"<<"\\bmatch\\b"<<"\\bno\\b"<<"\\bnot\\b"<<"\\bnull\\b"<<"\\bnumber\\b"<<"\\bof\\b"
	<<"\\bor\\b"<<"\\border\\b"<<"\\bout\\b"<<"\\bouter\\b"<<"\\boutput\\b"<<"\\bprepare\\b"<<"\\bpreserve\\b"<<"\\bprimary\\b"
	<<"\\brestrict\\b"<<"\\brevoke\\b"<<"\\bright\\b"<<"\\brollback\\b"<<"\\bschema\\b"<<"\\bselect\\b"<<"\\bsession\\b"<<"\\bset\\b"
	<<"\\bsum\\b"<<"\\bsynonym\\b"<<"\\btable\\b"<<"\\btan\\b"<<"\\bterminate\\b"<<"\\bthan\\b"<<"\\bthen\\b"<<"\\btime\\b"<<"\\btimestamp\\b"
	<<"\\btransaction\\b"<<"\\btrigger\\b"<<"\\btrim\\b"<<"\\btruncate\\b"<<"\\bunion\\b"<<"\\bunique\\b"<<"\\bupdate\\b"<<"\\bvalues\\b"
	<<"\\bvarchar\\b"<<"\\bview\\b"<<"\\bwhere\\b"<<"\\bwith\\b"<<"\\bwithout\\b"<<"\\bon\\b";
	
    foreach (QString pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
	//-Підсвічування аблиць
	keywordFormat.setForeground(Qt::darkGreen);
    keywordFormat.setFontWeight(QFont::Bold);
	
    foreach (QString pattern, tablesList) {
        rule.pattern = QRegExp("\\b"+pattern+"\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
	//Кінець налаштування підсвічування таблиць
	
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

	quotationFormat.setForeground(Qt::green);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);	
	
    multiLineCommentFormat.setForeground(Qt::blue);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (HighlightingRule rule, highlightingRules) {
    	rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        int endIndex = text.indexOf(commentEndExpression, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression,
                                                startIndex + commentLength);
    }
}
