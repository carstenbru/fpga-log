/**
 * @file consoleredirector.cpp
 * @brief ConsoleRedirector class
 *
 * This class is based on code from http://www.qtforum.org/article/678/redirecting-cout-cerr-to-qdebug.html
 * accessed on 24.06.2014 22:32
 * Thanks to members "stele", "Ignitor" and  "jamesst20" of qtforum.org!
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "consoleredirector.h"
#include <sstream>

using namespace std;

ConsoleRedirector::ConsoleRedirector(std::ostream &stream, QTextEdit* textEdit, string htmlColor) :
    stream(stream),
    htmlColor(htmlColor)
{
    logWindow = textEdit;
    oldBuf = stream.rdbuf();
    stream.rdbuf(this);
}

ConsoleRedirector::int_type ConsoleRedirector::overflow(int_type v) {
    if (v == '\n') {
        logWindow->append("");
        oldBuf->sputc('\n');
    }
    return v;
}

std::streamsize ConsoleRedirector::xsputn(const char *p, std::streamsize n) {
    oldBuf->sputn(p, n); //send data to original stream

    stringstream ss;
    QString s(p);
    s.replace("<", "&lt;");
    s.replace("\n", "<br>");
    s.replace(" ", "&nbsp;");

    ss << "<font color=\"" << htmlColor << "\">" << s.toStdString() << "</font>";
    logWindow->insertHtml(QString(ss.str().c_str()));
    logWindow->moveCursor(QTextCursor::End);
    return n;
}
