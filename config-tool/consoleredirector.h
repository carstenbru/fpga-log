/*
 * This class is based on code from http://www.qtforum.org/article/678/redirecting-cout-cerr-to-qdebug.html
 * accessed on 24.06.2014 22:32
 * Thanks to members "stele", "Ignitor" and  "jamesst20" of qtforum.org!
 */

#ifndef CONSOLEREDIRECTOR_H
#define CONSOLEREDIRECTOR_H

#include <iostream>
#include <streambuf>
#include <string>

#include <QTextEdit>

class ConsoleRedirector: public std::basic_streambuf<char>
{
public:
    ConsoleRedirector(std::ostream &stream, QTextEdit* textEdit, std::string htmlColor);
    ~ConsoleRedirector() { stream.rdbuf(oldBuf); }

protected:
    //This is called when a std::endl has been inserted into the stream
    virtual int_type overflow(int_type v);
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
    std::ostream &stream;
    std::streambuf *oldBuf;
    QTextEdit* logWindow;

    std::string htmlColor;
};

#endif // CONSOLEREDIRECTOR_H
