#include "config.h"
#include "platform.h"

#include <QCoreApplication>
#include <QTextStream>

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString script_file("build.js");
    QString script_plat("win");
    QString script_cc("vc11");

    int cnt = (argc > 1) ? 0 : -1;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            if (++i >= argc) break;
            script_file = argv[i];
            cnt |= 0x1;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            if (++i >= argc) break;
            script_plat = argv[i];
            cnt |= 0x2;
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            if (++i >= argc) break;
            script_cc = argv[i];
            cnt |= 0x4;
        }
    }

    QString in;
    if (cnt > 0) in = "y";

    forever
    {
        if (!in.isEmpty())
        {
            if (in == "y") break;
            QTextStream(stdout) << "\nPlease enter your configuration: ";
            if (in == "f")
            {
                QTextStream(stdout) << "(ini/js/...)\n";
                QTextStream(stdin) >> script_file;
            }
            else if (in == "p")
            {
                QTextStream(stdout) << "(linux/win/...)\n";
                QTextStream(stdin) >> script_plat;
            }
            else if (in == "c")
            {
                QTextStream(stdout) << "(gcc/vc8/vc9/...)\n";
                QTextStream(stdin) >> script_cc;
            }
            else if (in == "n")
            {
                QTextStream(stdout) << "(filename linux gcc)\n";
                QTextStream(stdin) >> script_file >> script_plat >> script_cc;
            }
            else
            {
                QTextStream(stdout) << "Error configuration...\n";
            }
        }

        QTextStream(stdout) << "\n"
            << "Script File: " << script_file << "\n"
            << "Target Plat: " << script_plat << "\n"
            << "Target CC:   " << script_cc << "\n\n"
            << "OK to start it ? (y/n/f/p/c) ";

        QTextStream(stdin) >> in;
    }

    Config config(script_file, script_plat, script_cc);
    if (Platform(config).make())
    {
        QTextStream(stdout) << "\n"
            << "Make Successes!" << "\n";
    }
    else
    {
        QTextStream(stdout) << "\n"
            << "Make Failure!" << " ->: "
            << config.getMessage() << "\n";
        system("pause");
    }

    return 0;
}
