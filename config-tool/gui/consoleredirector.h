/**
 * @file consoleredirector.h
 * @brief ConsoleRedirector class
 *
 * This class is based on code from http://www.qtforum.org/article/678/redirecting-cout-cerr-to-qdebug.html
 * accessed on 24.06.2014 22:32
 * Thanks to members "stele", "Ignitor" and  "jamesst20" of qtforum.org!
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef CONSOLEREDIRECTOR_H
#define CONSOLEREDIRECTOR_H

#include <iostream>
#include <streambuf>
#include <string>

#include <QTextEdit>

/**
 * @brief class for redirecting standard outputs streams (cout, cerr) to a QTextEdit
 *
 * The data is also fed to the original stream destination.
 */
class ConsoleRedirector: public std::basic_streambuf<char>
{
public:
    /**
     * @brief constructor
     *
     * @param stream stream to redirect
     * @param textEdit new destination QTextEdit
     * @param htmlColor color in which the stream should appear in the QTextEdit (HTML representation)
     */
    ConsoleRedirector(std::ostream &stream, QTextEdit* textEdit, std::string htmlColor);
    ~ConsoleRedirector() { stream.rdbuf(oldBuf); }

protected:
    //This is called when a std::endl has been inserted into the stream
    /**
     * @brief inherited overflow function of std::basic_streambuf<char>
     *
     * This is called when a std::endl has been inserted into the stream.
     *
     * @param v new character in the stream
     * @return parameter v
     */
    virtual int_type overflow(int_type v);

    /**
     * @brief inherited character write function if std::basic_streambuf<char>
     *
     * @param p pointer to the new steam data
     * @param n size of new data
     * @return parameter n (amount of read data)
     */
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
    std::ostream &stream; /**< stream to redirect */
    std::streambuf *oldBuf; /**< old output buffer to forward messages */
    QTextEdit* logWindow; /**< output window to redirect to */

    std::string htmlColor; /**< color of the output as HTML color */
};

#endif // CONSOLEREDIRECTOR_H
