/*
 * This class is based on code from http://www.qtforum.org/article/678/redirecting-cout-cerr-to-qdebug.html
 * accessed on 24.06.2014 22:32
 * Thanks to members "stele", "Ignitor" and  "jamesst20" of qtforum.org!
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
    }
    return v;
}

std::streamsize ConsoleRedirector::xsputn(const char *p, std::streamsize n) {
    stringstream ss;
    ss << "<font color=\"" << htmlColor << "\">" << p << "</font>";
    logWindow->insertHtml(QString(ss.str().c_str()));
    return n;
}
